/* shared-library-guard tester
 * Copyright (C) 2026 bbhtt
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
#include "shared-library-guard.c"

int main(int argc, char* argv[]) {
  if (argc != 4)
    return 1;
  if (0 == strcmp(argv[1], "true")) {
    return match_path(argv[2], argv[3])?0:1;
  } else if (0 == strcmp(argv[1], "false")) {
    return match_path(argv[2], argv[3])?1:0;
  } else {
    return 1;
  }
}
