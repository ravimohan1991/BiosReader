/*
 *   ----------------------------
 *  |  util.h
 *   ----------------------------
 *   This file is part of BiosReader.
 *
 *   BiosReader is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   BiosReader is distributed in the hope and belief that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with BiosReader.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <sys/types.h>

#include "types.h"

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

int checksum(const u8 *buf, size_t len);
void *read_file(off_t base, size_t *len, const char *filename);
void *mem_chunk(off_t base, size_t len, const char *devmem);
int write_dump(size_t base, size_t len, const void *data, const char *dumpfile, int add);
u64 u64_range(u64 start, u64 end);
