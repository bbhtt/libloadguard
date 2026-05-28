/* libloadguard
 * Copyright (C) 2026 bbhtt
 * Copyright (C) 2019-2026 Seppo Yli-Olli
 * Copyright (C) 2019-2026 Codethink Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <limits.h>
#include <link.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"
#include "libloadguard_internal.h"

static char **blocked_list_patterns = NULL;
static size_t blocked_patterns_count = 0;
static bool debug_mode = false;

extern char *program_invocation_name;

static const char *debug_env  = "LIBLOADGUARD_DEBUG";
static const char *config_env = "LIBLOADGUARD_CONFIG";

bool
match_path (const char *pattern,
            const char *filename)
{
  if (pattern == NULL || filename == NULL)
    return false;

  if (debug_mode)
    fprintf (stderr, "pattern=%s filename=%s\n", pattern, filename);

  int ret = fnmatch (pattern,
                     filename,
                     FNM_PATHNAME | FNM_PERIOD | FNM_EXTMATCH);

  if (ret == 0)
    return true;

  if (ret != FNM_NOMATCH)
    {
      fprintf (stderr,
               "fnmatch failed: pattern=%s filename=%s error=%d\n",
               pattern,
               filename,
               ret);
    }

  return false;
}

static size_t
read_pattern (const char *file,
              size_t     *pos,
              size_t      last_pos,
              char        pattern[PATH_MAX])
{
  while (*pos < last_pos)
    {
      char c = file[*pos];

      if (c == ' ' || c == '\t')
        {
          ++(*pos);
          continue;
        }

      if (c == '\n')
        {
          ++(*pos);
          continue;
        }

      if (c == '#')
        {
          while (*pos < last_pos && file[*pos] != '\n')
            ++(*pos);
          continue;
        }

      break;
    }

  size_t pattern_pos = 0;

  while (*pos < last_pos)
    {
      char c = file[*pos];

      switch (c)
        {
        case '\\':
          ++(*pos);

          if (*pos >= last_pos)
            goto done;

          c = file[*pos];
          break;

        case ' ':
        case '\n':
        case '\t':
          goto done;

        default:
          break;
        }

      if (pattern_pos < PATH_MAX - 1)
        pattern[pattern_pos++] = c;
      else if (debug_mode)
        fprintf (stderr, "Pattern truncated to PATH_MAX-1 bytes\n");

      ++(*pos);
    }

done:
  pattern[pattern_pos] = '\0';

  while (*pos < last_pos &&
         (file[*pos] == ' ' || file[*pos] == '\t'))
    ++(*pos);

  if (*pos < last_pos && file[*pos] == '\n')
    ++(*pos);

  return pattern_pos;
}

static char *
read_whole_file (const char *name,
                 size_t     *file_size)
{
  int fd = -1;
  struct stat st;
  void *map = NULL;

  *file_size = 0;

  fd = open (name, O_RDONLY);
  if (fd == -1)
    {
      if (debug_mode)
        fprintf (stderr, "open(%s) failed: %s\n", name,
                 strerror (errno));

      return NULL;
    }

  if (fstat (fd, &st) == -1)
    {
      if (debug_mode)
        {
          fprintf (stderr,
                   "fstat(%s) failed: %s\n",
                   name,
                   strerror (errno));
        }

      close (fd);
      return NULL;
    }

  if (st.st_size == 0)
    {
      close (fd);
      return NULL;
    }

  *file_size = (size_t) st.st_size;

  map = mmap (NULL, *file_size, PROT_READ, MAP_PRIVATE, fd, 0);

  close (fd);

  if (map == MAP_FAILED)
    {
      if (debug_mode)
        fprintf (stderr, "mmap(%s) failed: %s\n", name,
                 strerror (errno));

      return NULL;
    }

  return map;
}

static void
free_blocked_list (void)
{
  if (blocked_list_patterns == NULL)
    return;

  for (size_t i = 0; i < blocked_patterns_count; ++i)
    free (blocked_list_patterns[i]);

  free (blocked_list_patterns);

  blocked_list_patterns = NULL;
  blocked_patterns_count = 0;
}

static bool
append_blocked_pattern (const char *pattern)
{
  char *copy = strdup (pattern);

  if (copy == NULL)
    return false;

  char **tmp =
    realloc (blocked_list_patterns,
             (blocked_patterns_count + 2) * sizeof (char *));

  if (tmp == NULL)
    {
      free (copy);
      return false;
    }

  blocked_list_patterns = tmp;

  blocked_list_patterns[blocked_patterns_count++] = copy;
  blocked_list_patterns[blocked_patterns_count]   = NULL;

  return true;
}

void
load_blocked_list (const char *process_name,
                   const char *config_name)
{
  free_blocked_list ();

  size_t file_size = 0;
  char *file_data = read_whole_file (config_name, &file_size);

  if (file_data == NULL)
    return;

  size_t pos = 0;

  while (pos < file_size)
    {
      char process_pattern[PATH_MAX];
      char library_pattern[PATH_MAX];

      if (read_pattern (file_data, &pos, file_size,
                        process_pattern) == 0)
        continue;

      if (read_pattern (file_data, &pos, file_size,
                        library_pattern) == 0)
        continue;

      if (match_path (process_pattern, process_name))
        {
          if (!append_blocked_pattern (library_pattern))
            {
              fprintf (stderr, "Failed to allocate blocked pattern\n");
              break;
            }
        }
    }

  munmap (file_data, file_size);
}

size_t
get_blocked_pattern_count (void)
{
  return blocked_patterns_count;
}

const char *
get_blocked_pattern (size_t idx)
{
  if (idx >= blocked_patterns_count)
    return NULL;

  return blocked_list_patterns[idx];
}

static bool
should_block (const char *library_name)
{
  if (library_name == NULL)
    return false;

  if (debug_mode)
    fprintf (stderr, "Trying to load library %s\n", library_name);

  if (blocked_list_patterns == NULL)
    return false;

  for (size_t i = 0; blocked_list_patterns[i] != NULL; ++i)
    {
      if (match_path (blocked_list_patterns[i], library_name))
        {
          fprintf (stderr, "Blocked library %s\n", library_name);
          return true;
        }
    }

  return false;
}

char *
la_objsearch (const char  *name,
              uintptr_t   *cookie,
              unsigned int flag)
{
  (void) cookie;
  (void) flag;

  if (name == NULL)
    return NULL;

  char *real_name = realpath (name, NULL);

  const char *candidate = (real_name != NULL) ? real_name : name;

  bool blocked = should_block (candidate);

  free (real_name);

  if (blocked)
    {
      errno = EACCES;
      return NULL;
    }

  return (char *) name;
}

unsigned int
la_version (unsigned int version)
{
  char real_path[PATH_MAX + 1];
  ssize_t real_path_size;

  const char *config_path = getenv (config_env);
  const char *debug_value = getenv (debug_env);

  debug_mode = (debug_value != NULL);

  real_path_size = readlink ("/proc/self/exe", real_path, PATH_MAX);

  if (real_path_size >= 0)
    {
      real_path[real_path_size] = '\0';
    }
  else
    {
      strncpy (real_path,
               program_invocation_name != NULL ?
               program_invocation_name :
               "unknown",
               PATH_MAX);
      real_path[PATH_MAX] = '\0';
    }

  if (config_path == NULL)
    config_path = LIBLOADGUARD_CONFIG;

  if (debug_mode)
    fprintf (stderr, "Using configuration file %s\n", config_path);

  load_blocked_list (real_path, config_path);

  if (blocked_list_patterns == NULL)
    {
      if (debug_value != NULL && match_path (debug_value, real_path))
        {
          fprintf (stderr, "libloadguard active for %s\n", real_path);
          return version;
        }

      if (debug_mode)
        fprintf (stderr, "libloadguard inactive for %s\n", real_path);

      return 0;
    }

  fprintf (stderr, "libloadguard active for %s\n", real_path);

  return version;
}
