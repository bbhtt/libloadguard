/* libloadguard tester
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

#include <assert.h>
#include "libloadguard.c"
#define PATTERN_START 3


int main(int argc, const char * argv[]) {
  load_blocked_list(argv[1], argv[2]);
  if (argc <= PATTERN_START) {
    assert(blocked_list_patterns == NULL);
  } else {
    int i = 0;
    assert(blocked_list_patterns != NULL);
    for (; i < argc - PATTERN_START; i++) {
      const char* pattern = blocked_list_patterns[i];
      assert(pattern != NULL);
      assert(0 == strcmp(pattern, argv[i + PATTERN_START]));
    }
    assert(blocked_list_patterns[i] == NULL);
  }
}
