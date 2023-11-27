/* shared-library-guard
 * Copyright (C) 2019 Seppo Yli-Olli
 * Copyright (C) 2019 Codethink Ltd.
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

#define _GNU_SOURCE
#include <link.h>
#include <string.h>
#include <stdbool.h>
#include <linux/limits.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fnmatch.h>
#include "config.h"


static char** blocked_list_patterns = NULL;
static int debug_mode = 0;
extern char* program_invocation_name;
static char* debug_env = "SHARED_LIBRARY_GUARD_DEBUG";
static char* config_env = "SHARED_LIBRARY_GUARD_CONFIG";


static int match_path(const char* pattern, const char* filename) {
  if (debug_mode) {
    fprintf(stderr, "pattern %s, filename %s\n", pattern, filename);
  }
  int ret = fnmatch(pattern, filename, FNM_PATHNAME|FNM_PERIOD|FNM_EXTMATCH);
  if (ret == 0) {
    return true;
  } else {
    if (ret != FNM_NOMATCH) {
      fprintf(stderr, "fnmatch pattern %s, filename %s: error %d\n", pattern,
              filename, ret);
    }
    return false;
  }
}

static
size_t
read_pattern(char* file, size_t *pos, size_t last_pos, char pattern[PATH_MAX]) {
  size_t pattern_pos = 0;
  for (; *pos < last_pos; ++(*pos)) {
    switch (file[*pos]) {
      case '\\':
        ++(*pos);
        if (*pos >= last_pos) {
          goto ret;
        }
      default:
        if (pattern_pos < PATH_MAX) {
          pattern[pattern_pos] = file[*pos];
        }
        ++pattern_pos;
        break ;
      case ' ':
      case '\n':
        goto ret;
    }
  }
  ret:
  if (pattern_pos >= PATH_MAX)
    pattern_pos = PATH_MAX-1;
  pattern[pattern_pos] = 0;
  ++(*pos);
  return pattern_pos;
}


static
char *
read_whole_file(const char* name, size_t *file_size) {
  int fd;
  void * map;
  if ((fd = open(name, O_RDONLY)) != -1) {
    struct stat buf;
    if ((fstat(fd, &buf)) != -1 ) {
      *file_size = buf.st_size;
      map = mmap(NULL, *file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    }
    close(fd);
  }
  return (char*)map;
}

static
void
load_blocked_list(const char* process_name, const char* config_name) {
  blocked_list_patterns = NULL;
  size_t found_patterns = 0;
  size_t file_size = 0;
  char* file_data = read_whole_file(config_name, &file_size);
  if(file_data == NULL)
    return;
  size_t pos = 0;
  while (pos < file_size) {
    char pattern[PATH_MAX];
    read_pattern(file_data, &pos, file_size, pattern);
    if (pos >= file_size)
      break ;
    if (match_path(pattern, process_name)) {
      char *new_pattern;
      size_t num_bytes = read_pattern(file_data, &pos, file_size, pattern);
      new_pattern = (char*)malloc(num_bytes);
      memcpy(new_pattern, pattern, num_bytes);
      if (blocked_list_patterns == NULL) {
        blocked_list_patterns = (char**)malloc(2*sizeof(char*));
      } else {
        blocked_list_patterns = realloc(blocked_list_patterns, (found_patterns+2)*sizeof(char*));
      }
      blocked_list_patterns[found_patterns] = new_pattern;
      ++found_patterns;
      blocked_list_patterns[found_patterns] = NULL;
    } else {
      read_pattern(file_data, &pos, file_size, pattern);
    }
  }
  munmap(file_data, file_size);
}

static
int
should_block(const char* library_name) {
  if (debug_mode) {
    fprintf(stderr, "Trying to load library %s\n", library_name);
  }
  if (blocked_list_patterns == NULL) {
    return false;
  } else
    for (size_t i = 0; blocked_list_patterns[i] != NULL; ++i)
      {
      if (match_path(blocked_list_patterns[i], library_name))
        {
          fprintf(stderr, "Blocked library %s\n", library_name);
          return true;
      }
    }
  return false;
}

char
*la_objsearch(const char *name, uintptr_t *cookie, unsigned int flag) {
  char* real_name = realpath(name, NULL);
  if (should_block(real_name?real_name:name)) {
    name = NULL;
  }
  free(real_name);
  return (char*)name;
}

unsigned int
la_version(unsigned int version) {
  char real_path[PATH_MAX+1];
  ssize_t real_path_size;
  char *config_path = getenv(config_env);
  char *debug_value = getenv(debug_env);
  if (debug_value) {
    debug_mode = 1;
  }
  real_path_size = readlink("/proc/self/exe", real_path, PATH_MAX);
  if (real_path_size == -1) {
    strcpy(program_invocation_name, real_path);
  } else {
    real_path[real_path_size] = '\0';
  }
  if (config_path == NULL) {
    config_path = SHARED_LIBRARY_GUARD_CONFIG;
  }
  if (debug_mode) {
    fprintf(stderr, "Using the configuration file %s\n", config_path);
  }
  load_blocked_list(real_path, config_path);
  if (blocked_list_patterns == NULL) {
    if (debug_value && match_path(debug_value, real_path)) {
      fprintf(stderr, "shared-library-guard active for %s\n", real_path);
      return version;
    } else {
      if (debug_mode) {
        fprintf(stderr, "shared-library-guard inactivate for %s\n", real_path);
      }
      return 0;
    }
  } else {
    fprintf(stderr, "shared-library-guard active for %s\n", real_path);
    return version;
  }
}
