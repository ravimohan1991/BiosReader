/*
 *   ----------------------------
 *  |  dmidecode.h
 *   ----------------------------
 *   This file is part of BiosReader.
 *
 *   BiosReader is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   BiosReader is distributed in the hope and belief that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with BiosReader.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DMIDECODE_H
#define DMIDECODE_H

#include "types.h"

#ifdef BR_WINDOWS_PLATFORM

#define WIN_UNSUPORTED        (0 << 1)
#define WIN_NT_2K_XP          (1 << 1)
#define WIN_2003_VISTA        (1 << 2)
#define WIN_10                (1 << 3)

 /*
  * Struct needed to get the SMBIOS table using GetSystemFirmwareTable API.
  */
typedef struct _RawSMBIOSData
{
	u8	Used20CallingMethod;
	u8	SMBIOSMajorVersion;
	u8	SMBIOSMinorVersion;
	u8	DmiRevision;
	u32	Length;
	u8	SMBIOSTableData[];
} RawSMBIOSData, * PRawSMBIOSData;
#endif // BR_WINDOWS_PLATFORM

struct dmi_header
{
	u8 type;
	u8 length;
	u16 handle;
	u8* data;
};

enum cpuid_type
{
	cpuid_none,
	cpuid_80386,
	cpuid_80486,
	cpuid_arm_legacy,
	cpuid_arm_soc_id,
	cpuid_x86_intel,
	cpuid_x86_amd,
};

extern enum cpuid_type cpuid_type;

int is_printable(const u8* data, int len);
const char* dmi_string(const struct dmi_header* dm, u8 s);
void dmi_print_memory_size(const char* addr, u64 code, int shift);
void dmi_print_cpuid(void (*print_cb)(const char* name, const char* format, ...),
	const char* label, enum cpuid_type sig, const u8* p);

#ifdef BR_WINDOWS_PLATFORM
int get_windows_platform(void);
RawSMBIOSData* get_raw_smbios_table(void);
int count_smbios_structures(const void* buff, u32 len);
#endif

#endif
