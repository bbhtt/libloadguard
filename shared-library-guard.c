static char** blockedlist_patterns;


static int match_path(char* pattern, char* filename) {
  if (pattern[0] == '/') {
    return 0 == strcmp(pattern, filename);
  }
  else {
    for (char* path = filename; path != NULL; path = strchr(path, '/')) {
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
    pattern[PATH_MAX-1] = 0;
  else
    pattern[pattern_pos] = 0;
  return pos;
}

static
void
load_blocked_list(void) {
  static const char blockedlist_file_name[] = "/etc/freedesktop-sdk.ld.so.blockedlist";
  blockedlist_patterns = NULL;
  size_t found_patterns = 0;
  if (__access (blockedlist_file_name, R_OK) == 0) {
    char current_path[PATH_MAX];
    ssize_t res = readlink("/proc/self/exe", current_path, PATH_MAX-1);
    if ((res < 0) || (res >= PATH_MAX)) {
    } else {
      size_t file_size = 0;
      void *file = _dl_sysdep_read_whole_file (blockedlist_file_name, &file_size, PROT_READ);
      if (file != MAP_FAILED) {
      char* file_data = (char*)file;
      size_t pos = 0;
      while (pos < file_size) {
        char pattern[PATH_MAX];
	pos = read_pattern(file_data, pos, file_size, pattern);
	if (pos >= file_size)
	  break ;
	if (match_path(pattern, current_path) == 0) {
	  pos = read_pattern(file_data, pos, file_size, pattern);
	  char *new_pattern = (char*)malloc(strlen(pattern)+1);
	  strcpy(pattern, new_pattern);
	  if (blockedlist_patterns == NULL) {
	    blockedlist_patterns = (char**)malloc(2*sizeof(char*));
	  } else {
	    blockedlist_patterns = realloc(blockedlist_patterns, (found_patterns+2)*sizeof(char*));
	  }
	  blockedlist_patterns[found_patterns] = new_pattern;
	  ++found_patterns;
	  blockedlist_patterns[found_patterns] = NULL;
	  } else {
	    pos = read_pattern(file_data, pos, file_size, pattern);
	  }
	}
      }
    }
  }
}


unsigned int la_version(unsigned int version) {
  load_blocked_list();
  if (blocked_list_patterns == NULL) {
    return 0;
  } else {
    return version;
  }
}
