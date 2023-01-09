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
void *read_file(off_t base, size_t *len, const char *filename, int* file_access);
void *mem_chunk(off_t base, size_t len, const char *devmem);
int write_dump(size_t base, size_t len, const void *data, const char *dumpfile, int add);
u64 u64_range(u64 start, u64 end);


// By the generocity of post https://stackoverflow.com/a/2170743


// Define br_safe_sprintf
#ifdef BR_MAC_PLATFORM
	#define br_safe_sprintf(str, len, ...) \
		snprintf(str, len, __VA_ARGS__)
#endif

#if (BR_LINUX_PLATFORM)
	#define br_safe_sprintf(str, len, ...) \
		sprintf(str, __VA_ARGS__)
#endif

#if defined (BR_WINDOWS_PLATFORM)
	#define br_safe_sprintf(str, len, ...) \
		sprintf_s(str, len, __VA_ARGS__)
#endif

// Define br_safe_strcpy
#if defined (BR_LINUX_PLATFORM)
	#define br_safe_strcpy(dest, dest_size, src)\
		strcpy(dest, src);
#endif

#if defined (BR_WINDOWS_PLATFORM)
	#define br_safe_strcpy(dest, dest_size, src)\
		strcpy_s(dest, dest_size, src);
#endif

#ifdef BR_MAC_PLATFORM
	#define br_safe_strcpy(dest, dest_size, src)\
		strlcpy(dest, src, dest_size)
#endif
