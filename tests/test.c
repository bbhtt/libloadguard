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
test_match_null (void)
{
  CHECK_NO_MATCH (NULL, "/foo/bar");
  CHECK_NO_MATCH ("/foo/bar", NULL);
  CHECK_NO_MATCH (NULL, NULL);
}

static void
test_match_absolute (void)
{
  CHECK_MATCH ("/some/absolute/path", "/some/absolute/path");
  CHECK_NO_MATCH ("/some/absolute/path", "/root/some/absolute/path");
  CHECK_NO_MATCH ("/some/absolute/path", "/some/absolute/path/extra");
  CHECK_NO_MATCH ("/some/absolute/path", "/some/absolute");
}

static void
test_match_wildcard (void)
{
  CHECK_MATCH ("/foo/*/baz.so", "/foo/bar/baz.so");
  CHECK_NO_MATCH ("/foo/*/baz.so", "/foo/bar/qux/baz.so");
  CHECK_MATCH ("/foo/ba?.so",   "/foo/bar.so");
  CHECK_NO_MATCH ("/foo/ba?.so",   "/foo/ba.so");
  CHECK_NO_MATCH ("/foo/ba?.so",   "/foo/barr.so");
  CHECK_MATCH ("/foo/bar/*.so", "/foo/bar/libc.so");
  CHECK_NO_MATCH ("/foo/bar/*.so", "/foo/bar/.hidden.so");
}

static void
test_match_extmatch (void)
{
  CHECK_MATCH ("*(/*)/relative/path",     "/root/some/relative/path");
  CHECK_MATCH ("*(/*)/relative/path",     "/relative/path");
  CHECK_NO_MATCH ("*(/*)/relative/path",
                  "/relative/path/not/finished");
  CHECK_NO_MATCH ("*(/*)/relative/path",
                  "/root/.some/relative/path");
  CHECK_MATCH ("*(/*|/.*)/relative/path", "/root/.some/relative/path");
  CHECK_MATCH ("*(/*|/.*)/relative/path", "/root/some/relative/path");
  CHECK_NO_MATCH ("relative/path",
                  "/root/some/relative/path/not/finished");
}

static void
test_match_period (void)
{
  CHECK_NO_MATCH ("/foo/*/baz.so",  "/foo/.hidden/baz.so");
  CHECK_MATCH ("/foo/.*/baz.so", "/foo/.hidden/baz.so");
  CHECK_NO_MATCH ("*.so",           ".hidden.so");
}

static void
test_match_extmatch_process (void)
{
  CHECK_MATCH ("*(/*)tester", "tester");
  CHECK_MATCH ("*(/*)tester", "/foo/bar/tester");
  CHECK_NO_MATCH ("*(/*)tester", "/foo/bar/tester2");
  CHECK_NO_MATCH ("*(/*)tester", "atester");
}

static void
test_empty_file (void)
{
  char *path = write_tmp ("tester /foo/bar/baz.so\n");

  load_blocked_list ("tester", path);
  CHECK (get_blocked_pattern_count () == 1);
  unlink (path);
  free (path);

  path = write_tmp ("");
  load_blocked_list ("tester", path);
  CHECK (get_blocked_pattern_count () == 0);
  CHECK (get_blocked_pattern (0) == NULL);
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
  CHECK (get_blocked_pattern (1) == NULL);
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
  CHECK (get_blocked_pattern (2) == NULL);
  unlink (path);
  free (path);
}

static void
test_absolute_process_path (void)
{
  char *path = write_tmp ("*(/*)tester /foo/bar/baz.so\n");

  load_blocked_list ("/foo/bar/tester", path);
  CHECK (get_blocked_pattern_count () == 1);
  CHECK (get_blocked_pattern (0) != NULL &&
         strcmp (get_blocked_pattern (0), "/foo/bar/baz.so") == 0);
  unlink (path);
  free (path);
}

static void
test_orphaned_token (void)
{
  char *path = write_tmp ("test\n");

  load_blocked_list ("tester", path);
  CHECK (get_blocked_pattern_count () == 0);
  unlink (path);
  free (path);
}

static void
test_nonexistent_config (void)
{
  load_blocked_list ("tester", "/tmp/libloadguard_no_such_file");
  CHECK (get_blocked_pattern_count () == 0);
}

static void
test_reload_clears (void)
{
  char *path = write_tmp ("tester /foo/bar/baz.so\n");

  load_blocked_list ("tester", path);
  CHECK (get_blocked_pattern_count () == 1);
  load_blocked_list ("tester", path);
  CHECK (get_blocked_pattern_count () == 1);
  unlink (path);
  free (path);

  char *empty = write_tmp ("");
  load_blocked_list ("tester", empty);
  CHECK (get_blocked_pattern_count () == 0);
  unlink (empty);
  free (empty);
}

static void
test_comment_lines (void)
{
  char *path = write_tmp (
    "# this is a comment\n"
    "tester /foo/bar/baz.so\n"
    "# another comment\n"
    "tester2 /ignored.so\n");

  load_blocked_list ("tester", path);
  CHECK (get_blocked_pattern_count () == 1);
  CHECK (get_blocked_pattern (0) != NULL &&
         strcmp (get_blocked_pattern (0), "/foo/bar/baz.so") == 0);
  unlink (path);
  free (path);
}

static void
test_blank_lines (void)
{
  char *path = write_tmp (
    "\n"
    "tester /foo/bar/baz.so\n"
    "\n"
    "tester2 /ignored.so\n"
    "\n");

  load_blocked_list ("tester", path);
  CHECK (get_blocked_pattern_count () == 1);
  CHECK (get_blocked_pattern (0) != NULL &&
         strcmp (get_blocked_pattern (0), "/foo/bar/baz.so") == 0);
  unlink (path);
  free (path);
}

static void
test_no_trailing_newline (void)
{
  char *path = write_tmp ("tester /foo/bar/baz.so");

  load_blocked_list ("tester", path);
  CHECK (get_blocked_pattern_count () == 1);
  CHECK (get_blocked_pattern (0) != NULL &&
         strcmp (get_blocked_pattern (0), "/foo/bar/baz.so") == 0);
  unlink (path);
  free (path);
}

static void
test_escaped_space_in_pattern (void)
{
  char *path = write_tmp ("tester /foo/bar\\ baz.so\n");

  load_blocked_list ("tester", path);
  CHECK (get_blocked_pattern_count () == 1);
  CHECK (get_blocked_pattern (0) != NULL &&
         strcmp (get_blocked_pattern (0), "/foo/bar baz.so") == 0);
  unlink (path);
  free (path);
}

int
main (void)
{
  RUN (test_match_null);
  RUN (test_match_absolute);
  RUN (test_match_wildcard);
  RUN (test_match_extmatch);
  RUN (test_match_period);
  RUN (test_match_extmatch_process);

  RUN (test_empty_file);
  RUN (test_single_match);
  RUN (test_wrong_process);
  RUN (test_complex);
  RUN (test_absolute_process_path);
  RUN (test_orphaned_token);
  RUN (test_nonexistent_config);
  RUN (test_reload_clears);
  RUN (test_comment_lines);
  RUN (test_blank_lines);
  RUN (test_no_trailing_newline);
  RUN (test_escaped_space_in_pattern);

  fprintf (stderr, "%d checks, %d failures\n", checks, failures);
  return failures > 0 ? 1 : 0;
}
