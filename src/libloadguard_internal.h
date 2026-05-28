#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool        match_path (const char *pattern,
                        const char *filename);
void        load_blocked_list (const char *process_name,
                               const char *config_name);
size_t      get_blocked_pattern_count (void);
const char *get_blocked_pattern (size_t idx);

char        *la_objsearch (const char  *name,
                           uintptr_t   *cookie,
                           unsigned int flag);
unsigned int la_version (unsigned int version);
