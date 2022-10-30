/*
 *   ----------------------------
 *  |  dmidecode.h
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

#ifndef DMIDECODE_H
#define DMIDECODE_H

#include "types.h"


struct motherboard_components
{
	char* baseboard;
	char* managementdevice; // electronics monitor (temp, fan speed and whatnot) voltage probe, see mb_management_elements
	char* portconnector; // multiple slots usb, lan, and whatnot
	char* pcieslots; // multiple slots for GPU etc mounting
	char* integratedgraphicsdevice;
	char* ethernet;
	char* randomaccessmemory; // multiple slots
	char* cpucache; // multiple slots
};

struct random_access_memory
{
	char* formfactor;
	char* ramtype;
	char* bank; // what is this and is this really necesary?
	char* manufacturer;

	char* serialnumber;
	char* partnumber;
	char* assettag;

	char* configuredmemoryspeed;
	char* operatingvoltage;
};

struct cpu_cache
{
	char* designation;
	char* status;
	char* operationmode;
	char* sramtype;
	char* speed;
	char* errorcorrection;
	char* associativity;
	char* installedsize;
};

struct mb_management_elements
{
	char* temperatureprobe;
	char* voltageprobe;
	char* data;
};

struct mb_language_modules
{
	char* currentactivemodule;
	char* supportedlanguagemodules;
};

struct central_processing_unit
{
	char* designation;
	char* processingfamily;
	char* manufacturer;
	char* cpuid; // in the context of motherboard components
	char* signature;
	char* cpuflags;
	char* completeprocessingunitidentifier;
	char* operatingvoltage;
	char* externalclock;
	char* maximumspeed;
	char* currentspeed;

	char* serialnumber;
	char* partnumber;
	char* assettag;

	char* corecount;
	char* threadcount;
	char* restofthecharacterstics;
};


struct bios_information
{
	int bIsFilled;
	const char* vendor;
	const char* version;
	const char* biosreleasedate;
	char* bioscharacteristics;
};

struct bios_information extern biosinformation;

enum bios_reader_information_classification
{
	// Pure Information
	pi_manufacturer = 0,
	pi_onboardinformation, // system boot
	pi_localization,

	// Software side
	ss_bios,

	// Physicaldevice side
	ps_motherboard,
	ps_chassis,  // almost synonymous to cabinet (or Chastity if I may)
	ps_heaver, // cooling system for CPU area
	ps_processor
};

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
struct u64;

int is_printable(const u8* data, int len);
const char* dmi_string(const struct dmi_header* dm, u8 s);
void dmi_print_memory_size(const char* addr, u64 code, int shift);
void dmi_print_cpuid(void (*print_cb)(const char* name, const char* format, ...),
	const char* label, enum cpuid_type sig, const u8* p);
static int smbios3_decode(u8* buf, const char* devmem, u32 flags);

#ifdef __cplusplus
extern "C"{
#endif
void* electronics_spit(enum bios_reader_information_classification informationCategory);
#ifdef __cplusplus
}
#endif

// Should the electronics be displayed in console
#define bDisplayOutput 1

#ifdef BR_WINDOWS_PLATFORM
int get_windows_platform(void);
RawSMBIOSData* get_raw_smbios_table(void);
int count_smbios_structures(const void* buff, u32 len);
#endif

#endif
