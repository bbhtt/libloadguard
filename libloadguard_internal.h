#pragma once

#include <stddef.h>
#include <stdint.h>

int          match_path (const char *pattern,
                         const char *filename);
void         load_blocked_list (const char *process_name,
                                const char *config_name);

char        *la_objsearch (const char  *name,
                           uintptr_t   *cookie,
                           unsigned int flag);
unsigned int la_version (unsigned int version);
