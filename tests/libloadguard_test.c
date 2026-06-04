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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libloadguard_internal.h"

static int failures = 0;
static int checks   = 0;

#define CHECK(cond) \
        do { \
            ++checks; \
            if (!(cond)) \
              { \
                fprintf (stderr, \
                         "FAIL %s:%d: %s\n", \
                         __FILE__, \
                         __LINE__, \
                         #cond); \
                ++failures; \
              } \
          } while (0)

#define CHECK_MATCH(p, f)    CHECK ( match_path ((p), (f)))
#define CHECK_NO_MATCH(p, f) CHECK (!match_path ((p), (f)))

#define RUN(fn) \
        do { \
            int before = failures; \
            fn (); \
            fprintf (stderr, \
                     "%s: %s\n", \
                     #fn, \
                     failures == before ? "ok" : "FAILED"); \
          } while (0)

static void
test_absolute_match (void)
{
  CHECK_MATCH ("/some/absolute/path", "/some/absolute/path");
}

static void
test_absolute_non_match (void)
{
  CHECK_NO_MATCH ("/some/absolute/path", "/root/some/absolute/path");
}

static void
test_relative_match (void)
{
  CHECK_MATCH ("*(/*)/relative/path", "/root/some/relative/path");
  CHECK_NO_MATCH ("relative/path",
                  "/root/some/relative/path/not/finished");
}

static void
test_relative_match_hidden (void)
{
  CHECK_NO_MATCH ("*(/*)/relative/path", "/root/.some/relative/path");
  CHECK_MATCH ("*(/*|/.*)/relative/path", "/root/.some/relative/path");
}

static char *
write_tmp (const char *contents)
{
  char *path = strdup ("/tmp/libloadguard_test_XXXXXX");
  int fd = mkstemp (path);

  if (fd == -1)
    {
      perror ("mkstemp");
      exit (1);
    }
  size_t len = strlen (contents);
  if (len > 0 && (size_t) write (fd, contents, len) != len)
    {
      perror ("write");
      exit (1);
    }
  close (fd);
  return path;
}

static void
test_empty_file (void)
{
  char *path = write_tmp ("");

  load_blocked_list ("tester", path);
  CHECK (get_blocked_pattern_count () == 0);
  unlink (path);
  free (path);
}

static void
test_single_match (void)
{
  char *path = write_tmp ("*(/*)tester /foo/bar/baz.so\n");

  load_blocked_list ("tester", path);
  CHECK (get_blocked_pattern_count () == 1);
  CHECK (get_blocked_pattern (0) != NULL &&
         strcmp (get_blocked_pattern (0), "/foo/bar/baz.so") == 0);
  unlink (path);
  free (path);
}

static void
test_wrong_process (void)
{
  char *path = write_tmp ("*(/*)tester /foo/bar/baz.so\n");

  load_blocked_list ("tester2", path);
  CHECK (get_blocked_pattern_count () == 0);
  unlink (path);
  free (path);
}

static void
test_complex (void)
{
  char *path = write_tmp (
    "tester /foo/bar/baz.so\n"
    "tester2 w/foo/bar/baz.so\n"
    "tester /meep.so\n"
    "tester3 foo/bar\n");

  load_blocked_list ("tester", path);
  CHECK (get_blocked_pattern_count () == 2);
  CHECK (get_blocked_pattern (0) != NULL &&
         strcmp (get_blocked_pattern (0), "/foo/bar/baz.so") == 0);
  CHECK (get_blocked_pattern (1) != NULL &&
         strcmp (get_blocked_pattern (1), "/meep.so") == 0);
  unlink (path);
  free (path);
}

static void
test_absolute_path (void)
{
  char *path = write_tmp ("*(/*)tester /foo/bar/baz.so\n");

  load_blocked_list ("/foo/bar/tester", path);
  CHECK (get_blocked_pattern_count () == 1);
  CHECK (get_blocked_pattern (0) != NULL &&
         strcmp (get_blocked_pattern (0), "/foo/bar/baz.so") == 0);
  unlink (path);
  free (path);
}

int
main (void)
{
  RUN (test_absolute_match);
  RUN (test_absolute_non_match);
  RUN (test_relative_match);
  RUN (test_relative_match_hidden);
  RUN (test_empty_file);
  RUN (test_single_match);
  RUN (test_wrong_process);
  RUN (test_complex);
  RUN (test_absolute_path);

  fprintf (stderr, "%d checks, %d failures\n", checks, failures);
  return failures > 0 ? 1 : 0;
}
