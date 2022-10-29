/*
 *   ----------------------------
 *  |  types.h
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

#ifndef TYPES_H
#define TYPES_H

#include "config.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned int u32;

/*
 * You may use the following defines to adjust the type definitions
 * depending on the architecture:
 * - Define BIGENDIAN on big-endian systems.
 * - Define ALIGNMENT_WORKAROUND if your system doesn't support
 *   non-aligned memory access. In this case, we use a slower, but safer,
 *   memory access method. This should be done automatically in config.h
 *   for architectures which need it.
 *
 *   consult https://github.com/ravimohan1991/BiosReader/wiki/Demystifying-the-RAW-BIOS-information
 *   before being too cocky about alignment.
 */

#ifdef BR_BIG_ENDIAN
typedef struct
{
	u32 h;
	u32 l;
} u64;
#elif BR_LITTLE_ENDIAN
typedef struct
{
	u32 l;
	u32 h;
} u64;
#endif

#if defined(BR_BIG_ENDIAN)
static inline u64 U64(u32 low, u32 high)
{
	u64 self;

	self.l = low;
	self.h = high;

	return self;
}
#endif

/*
 * Per SMBIOS v2.8.0 and later, all structures assume a little-endian
 * ordering convention.
 * https://github.com/ravimohan1991/BiosReader/wiki/Demystifying-the-RAW-BIOS-information
 */
#if defined(BR_BIG_ENDIAN) // Conversion from le to be
#define WORD(x) (u16)((x)[0] + ((x)[1] << 8))
#define DWORD(x) (u32)((x)[0] + ((x)[1] << 8) + ((x)[2] << 16) + ((x)[3] << 24))
#define QWORD(x) (U64(DWORD(x), DWORD(x + 4)))
#elif BR_LITTLE_ENDIAN
#define WORD(x) (u16)(*(const u16 *)(x))
#define DWORD(x) (u32)(*(const u32 *)(x)) // dereference of pointer to const u32
#define QWORD(x) (*(const u64 *)(x)) // dereference of pointer to const u64
#endif

#endif
