/*
 *   ----------------------------
 *  |  config.h
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

#ifndef CONFIG_H
#define CONFIG_H

/* Default memory device file */
#if defined(__HAIKU__)
#define DEFAULT_MEM_DEV "/dev/misc/mem"
#else
#ifdef __sun
#define DEFAULT_MEM_DEV "/dev/xsvc"
#else
#define DEFAULT_MEM_DEV "/dev/mem"
#endif
#endif

/* Use memory alignment workaround or not */
#if defined(__ia64__) || defined(__LP64__)
#define ALIGNMENT_WORKAROUND
#endif

/* Avoid unaligned memcpy on /dev/mem */
#ifdef __aarch64__
#define USE_SLOW_MEMCPY
#endif

#endif
