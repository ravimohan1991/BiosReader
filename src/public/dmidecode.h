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
#include <glad.h>
#include "glfw/glfw3.h"

 /*
  *******************************************************************
  *                                                                 *
  *                  Electronics Categories                         *
  *                                                                 *
  *******************************************************************
  */

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

struct turing_machine_system_memory
{
	int bIsFilled;

	// A number
	unsigned int number_of_ram_or_system_memory_devices; // Computed near "case 16: /* 7.17 Physical Memory Array */"
	char* total_grand_capacity;
	char* mounting_location; // usually some view-able und asthetic place
};
struct turing_machine_system_memory extern turingmachinesystemmemory;

// Makes me remind class V, when I first read the word, thought it was Raam
// https://en.wikipedia.org/wiki/Rama, and in graduate school, I stumbled upon
// Biblical one https://en.wikipedia.org/wiki/Ram_(biblical_figure).
// In middle I came across, and recollected later, Ram is a male sheep too!
// Well what do I know, Ram is a verb too!!

// For an excellent viewpoint about the elements of this structure, I'd refer to
// definitions and articles https://www.crucial.in/articles/
struct random_access_memory
{
	int bIsFilled;

	char* formfactor;// https://www.crucial.in/articles/pc-builders/what-is-a-form-factor
	char* ramsize; // ye, powers of 2 stuff!! 128 MB, 256 MB, 512 MB, und 1 GB, 8 GB, and 16 GB. Well 1 GB is actually 1024 MB, so power of 2 strikes again.
	char* locator; // String number of the string that identifies the physically - labeled socket or board position where the memory device is located EXAMPLE : “SIMM 3”
	char* ramtype;
	char* banklocator; // String number of the string that identifies the physically labeled bank where the memory device is located EXAMPLE : “Bank 0” or “A
	char* manufacturer;

	char* serialnumber;
	char* partnumber;
	char* assettag;// Seems abiguous atm

	char* memoryspeed;
	char* configuredmemoryspeed;
	char* operatingvoltage;

	// A memory rank is a block or area of data that is created using some, or all, of the memory
	// chips on a module.A rank is a data block that is 64 bits wide.On systems that support
	// Error Correction Code(ECC) an additional 8 bits are added, which makes the data block 72 bits wide
	// Reference: https://www.crucial.in/support/articles-faq-memory/what-is-a-memory-rank#:~:text=A%20memory%20rank%20is%20a,data%20block%2072%20bits%20wide.
	char* rank;
};
struct random_access_memory extern* randomaccessmemory;
static unsigned int ramCounter;

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
struct mb_language_modules extern mblanguagemodules;

// Assuming 1 cpu only
struct central_processing_unit
{
	int bIsFilled;

	// SMBIOS is ambiguous about the designation, for instance printing U3E1 for FCLGA1151 socket
	// similar issue has been reported (followed by heavy criticism, if I may sao so)
	// Verdict: I shall drop the notion of socket display until SMBIOS comes up with, well, proper standard!
	char* designation;
	char* cputype; // In order to distinguish from GPU processor :) or DSP https://en.wikipedia.org/wiki/Digital_signal_processor
	char* processingfamily;
	char* manufacturer;
	char* cpuflags;
	char* version; // Kind of the most important element of this struct. eg Intel(R) Core(TM) i5-7400 CPU @ 3.00GHz (completeprocessingunitidentifier)
	char* operatingvoltage;

	char* externalclock;// For communicating with rest of the MotherBoard devices. Topic for blog-post https://electronics.stackexchange.com/questions/176466/how-do-devices-with-different-external-clock-communicate-between-each-other-on-a
	char* maximumspeed;
	char* currentspeed;

	char* serialnumber;
	char* partnumber;
	char* assettag;

	char* corescount;
	char* enabledcorescount;
	char* threadcount;
	char* characterstics;

	// The Cpu ID field contains processor - specific information that describes the processor’s features.
	// For instance E9 06 09 00 FF FB EB BF (something to be computed by cpu registers and stuff)
	char* cpuid;
	char* signature; //
};
struct central_processing_unit extern centralprocessinguint; // I know it should be unit, can't resist the temptation, besides uint means unsigned so...

// Assuming 1 gpu only

struct graphics_processing_unit
{
	int bIsFilled;

	char* vendor;
	char* gpuModel;
};
struct graphics_processing_unit extern graphicsprocessingunit;

struct bios_information
{
	int bIsFilled;

	char* vendor;
	char* version;
	char* biosreleasedate;
	char bioscharacteristics[9999];
	char* biosromsize;
	//struct mb_language_modules knownLanguages;
};
struct bios_information extern biosinformation;

enum bios_reader_information_classification
{
	// Pure Information
	pi_manufacturer = 0,
	pi_onboardinformation, // system boot
	pi_localization,
	pi_systemmemory, // collective system memory information
	pi_bioslanguages,

	// Software side
	ss_bios,

	// Physicaldevice side
	ps_motherboard,
	ps_chassis,  // almost synonymous to cabinet (or Chastity if I may)
	ps_heaver, // cooling system for CPU area
	ps_processor,
	ps_systemmemory, // array of memory devices (see turing_machine_system_memory::number_of_ram_or_system_memory_devices)
	ps_graphicscard
};

////////////////////////////////////////////////////////////////////////////////////////////

/*
 ***************************************************************************************************
 *
 * The chief API function or routine which can/should be called from outside the library for
 * performing queries about the electronics.
 *
 * Things to note:
 * 1. First (for instance since application start) query shall iterate over all the electronics
 *    and cache the information which shall be returned on that specific query.
 * 2. Subsequent queries shall return the cached information only.
 * 3. An overall iteration shall be performed only if application restarts or
 *    reset_electronics_structures() is invoked
 *
 * @param informationCategory                        The category of electronics being queried
 * @return void *                                    Pointer to the relevant struct category
 *
 ***************************************************************************************************
 */

void* electronics_spit(enum bios_reader_information_classification informationCategory);

/*
 ***************************************************************************************************
 *
 * Resets and, thus, frees the char primitive type memory allocations. Should be called when your
 * Application no longer needs BiosReader's services or is shutting.
 *
 ***************************************************************************************************
 */

void reset_electronics_structures();

/*
 ***************************************************************************************************
 *
 * Return the random_access_memory struct which can be accessed individually in Application
 * Could be the offset thing because I couldn't access the array via Application
 *
 ***************************************************************************************************
 */

struct random_access_memory* fetch_access_memory_members(unsigned int counter);

// Should the electronics be displayed in console with each query
#define bDisplayOutput 0

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

int get_windows_platform(void);
RawSMBIOSData* get_raw_smbios_table(void);
int count_smbios_structures(const void* buff, u32 len);

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

// Forward declarations

extern enum cpuid_type cpuid_type;
struct u64;

int is_printable(const u8* data, int len);
const char* dmi_string(const struct dmi_header* dm, u8 s);
void dmi_print_memory_size(const char* addr, u64 code, int shift);
void dmi_print_cpuid(void (*print_cb)(const char* name, const char* format, ...),
	const char* label, enum cpuid_type sig, const u8* p);
static int smbios3_decode(u8* buf, const char* devmem, u32 flags);
static void dmi_table_decode(u8* buf, u32 len, u16 num, u16 ver, u32 flags);
static void ashwamegha_run();

/////////////////////////////////////////////////////////////////////////////////////////////
// Helpers!
////////////////////////////////////////////////////////////////////////////////////////////
static void copy_to_structure_char(char** destinationPointer, const char* sourcePointer);
static void generate_multiline_buffer(char* const bufferHandle, char* const lineTextToEmbed, const char junctionCondition);
static void append_to_structure_char(char** destinationPointer, const char* sourcePointer, const char junctionCondition);
static void copy_to_already_initialized_structure_char(char** destinationPointer, const char* sourcePointer);

// Metric system for electronicssss
static const char* memoUnit[8] = {
		"bytes", "kB", "MB", "GB", "TB", "PB", "EB", "ZB"
};

#endif
