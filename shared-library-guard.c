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


static char** blockedlist_patterns = NULL;
static const char blockedlist_file_name[] = "/etc/freedesktop-sdk.ld.so.blockedlist";
extern char* program_invocation_name;


static int match_path(const char* pattern, const char* filename) {
  if (pattern[0] == '/') {
    return 0 == strcmp(pattern, filename);
  }
  else {
    for (const char* path = filename; path != NULL; path = strchr(path, '/')) {
      if (0 == strcmp(pattern, path))
        return true;
    }
    return false;
  }
}

static
size_t
read_pattern(char* file, size_t pos, size_t last_pos, char pattern[PATH_MAX]) {
  size_t pattern_pos = 0;
  for (; pos < last_pos; ++pos) {
    switch (file[pos]) {
      case '\\':
        ++pos;
        if (pos >= last_pos) {
          goto ret;
        }
      default:
        if (pattern_pos < PATH_MAX) {
          pattern[pattern_pos] = file[pos];
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
load_blocked_list(void) {
  blockedlist_patterns = NULL;
  size_t found_patterns = 0;
  char current_path[PATH_MAX];
  size_t file_size = 0;
  char* file_data = read_whole_file(blockedlist_file_name, &file_size);
  if(file_data == NULL)
    return;
  size_t pos = 0;
  while (pos < file_size) {
    char pattern[PATH_MAX];
    size_t pattern_pos = read_pattern(file_data, pos, file_size, pattern);
    pos += pattern_pos + 1;
    if (pos >= file_size)
      break ;
    if (match_path(pattern, program_invocation_name)) {
      char *new_pattern;
      pattern_pos = read_pattern(file_data, pos, file_size, pattern);
      pos += pattern_pos + 1;
      new_pattern = (char*)malloc(pattern_pos);
      memcpy(pattern, new_pattern, pattern_pos);
      if (blockedlist_patterns == NULL) {
	blockedlist_patterns = (char**)malloc(2*sizeof(char*));
      } else {
	blockedlist_patterns = realloc(blockedlist_patterns, (found_patterns+2)*sizeof(char*));
      }
      blockedlist_patterns[found_patterns] = new_pattern;
      ++found_patterns;
      blockedlist_patterns[found_patterns] = NULL;
    } else {
      pos += read_pattern(file_data, pos, file_size, pattern) + 1;
    }
  }
  munmap(file_data, file_size);
}

char
*la_objsearch(const char *name, uintptr_t *cookie, unsigned int flag) {
  if (blockedlist_patterns) {
    for (size_t i = 0; blockedlist_patterns[i] != NULL; ++i)
      {
	if (match_path(blockedlist_patterns[i], name) == 0)
	  {
	    return NULL;
	  }
      }
  }
  return (char*)name;
}

unsigned int
la_version(unsigned int version) {
  load_blocked_list();
  /* TODO: See if a bug is filed about glibc crashing if audit hook
     disables itself by returning version 0 like documentation says
     it can. */
  return version;
}
