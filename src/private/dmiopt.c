/*
 * Command line handling of dmidecode
 * This file is part of the dmidecode project.
 *
 *   Copyright (C) 2005-2008 Jean Delvare <jdelvare@suse.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef BR_LINUX_PLATFORM
#include <strings.h>
#include <getopt.h>
#endif // BR_LINUX_PLATFORM

#include "config.h"
#include "types.h"
#include "util.h"
#include "dmidecode.h"
#include "dmiopt.h"

 /* Options are global */
 //struct opt opt;

 /*
  * Handling of option --type
  */

struct type_keyword
{
	const char* keyword;
	const u8* type;
};

static const u8 opt_type_bios[] = { 0, 13, 255 };
static const u8 opt_type_system[] = { 1, 12, 15, 23, 32, 255 };
static const u8 opt_type_baseboard[] = { 2, 10, 41, 255 };
static const u8 opt_type_chassis[] = { 3, 255 };
static const u8 opt_type_processor[] = { 4, 255 };
static const u8 opt_type_memory[] = { 5, 6, 16, 17, 255 };
static const u8 opt_type_cache[] = { 7, 255 };
static const u8 opt_type_connector[] = { 8, 255 };
static const u8 opt_type_slot[] = { 9, 255 };

static const struct type_keyword opt_type_keyword[] = {
	{ "bios", opt_type_bios },
	{ "system", opt_type_system },
	{ "baseboard", opt_type_baseboard },
	{ "chassis", opt_type_chassis },
	{ "processor", opt_type_processor },
	{ "memory", opt_type_memory },
	{ "cache", opt_type_cache },
	{ "connector", opt_type_connector },
	{ "slot", opt_type_slot },
};

static void print_opt_type_list(void)
{
	unsigned int i;

	fprintf(stderr, "Valid type keywords are:\n");
	for (i = 0; i < ARRAY_SIZE(opt_type_keyword); i++)
	{
		fprintf(stderr, "  %s\n", opt_type_keyword[i].keyword);
	}
}

static u8* parse_opt_type(u8* p, const char* arg)
{
	unsigned int i;

	/* Allocate memory on first call only */
	if (p == NULL)
	{
		p = (u8*)calloc(256, sizeof(u8));
		if (p == NULL)
		{
			perror("calloc");
			return NULL;
		}
	}

	/* First try as a keyword */
	for (i = 0; i < ARRAY_SIZE(opt_type_keyword); i++)
	{
		if (!strcasecmp(arg, opt_type_keyword[i].keyword))
		{
			int j = 0;
			while (opt_type_keyword[i].type[j] != 255)
				p[opt_type_keyword[i].type[j++]] = 1;
			goto found;
		}
	}

	/* Else try as a number */
	while (*arg != '\0')
	{
		unsigned long val;
		char* next;

		val = strtoul(arg, &next, 0);
		if (next == arg || (*next != '\0' && *next != ',' && *next != ' '))
		{
			fprintf(stderr, "Invalid type keyword: %s\n", arg);
			print_opt_type_list();
			goto exit_free;
		}
		if (val > 0xff)
		{
			fprintf(stderr, "Invalid type number: %lu\n", val);
			goto exit_free;
		}

		p[val] = 1;
		arg = next;
		while (*arg == ',' || *arg == ' ')
			arg++;
	}

found:
	return p;

exit_free:
	free(p);
	return NULL;
}

/*
 * Handling of option --string
 */

 /* This lookup table could admittedly be reworked for improved performance.
	Due to the low count of items in there at the moment, it did not seem
	worth the additional code complexity though. */
static const struct string_keyword opt_string_keyword[] = {
	{ "bios-vendor", 0, 0x04 },
	{ "bios-version", 0, 0x05 },
	{ "bios-release-date", 0, 0x08 },
	{ "bios-revision", 0, 0x15 },		/* 0x14 and 0x15 */
	{ "firmware-revision", 0, 0x17 },	/* 0x16 and 0x17 */
	{ "system-manufacturer", 1, 0x04 },
	{ "system-product-name", 1, 0x05 },
	{ "system-version", 1, 0x06 },
	{ "system-serial-number", 1, 0x07 },
	{ "system-uuid", 1, 0x08 },             /* dmi_system_uuid() */
	{ "system-sku-number", 1, 0x19 },
	{ "system-family", 1, 0x1a },
	{ "baseboard-manufacturer", 2, 0x04 },
	{ "baseboard-product-name", 2, 0x05 },
	{ "baseboard-version", 2, 0x06 },
	{ "baseboard-serial-number", 2, 0x07 },
	{ "baseboard-asset-tag", 2, 0x08 },
	{ "chassis-manufacturer", 3, 0x04 },
	{ "chassis-type", 3, 0x05 },            /* dmi_chassis_type() */
	{ "chassis-version", 3, 0x06 },
	{ "chassis-serial-number", 3, 0x07 },
	{ "chassis-asset-tag", 3, 0x08 },
	{ "processor-family", 4, 0x06 },        /* dmi_processor_family() */
	{ "processor-manufacturer", 4, 0x07 },
	{ "processor-version", 4, 0x10 },
	{ "processor-frequency", 4, 0x16 },     /* dmi_processor_frequency() */
};

/* This is a template, 3rd field is set at runtime. */
static struct string_keyword opt_oem_string_keyword =
{ NULL, 11, 0x00 };

static void print_opt_string_list(void)
{
	unsigned int i;

	fprintf(stderr, "Valid string keywords are:\n");
	for (i = 0; i < ARRAY_SIZE(opt_string_keyword); i++)
	{
		fprintf(stderr, "  %s\n", opt_string_keyword[i].keyword);
	}
}

static int parse_opt_string(const char* arg)
{
	unsigned int i;

	if (opt.string)
	{
		fprintf(stderr, "Only one string can be specified\n");
		return -1;
	}

	for (i = 0; i < ARRAY_SIZE(opt_string_keyword); i++)
	{
		if (!strcasecmp(arg, opt_string_keyword[i].keyword))
		{
			opt.string = &opt_string_keyword[i];
			return 0;
		}
	}

	fprintf(stderr, "Invalid string keyword: %s\n", arg);
	print_opt_string_list();
	return -1;
}

static int parse_opt_oem_string(const char* arg)
{
	unsigned long val;
	char* next;

	if (opt.string)
	{
		fprintf(stderr, "Only one string can be specified\n");
		return -1;
	}

	/* Return the number of OEM strings */
	if (strcmp(arg, "count") == 0)
		goto done;

	val = strtoul(arg, &next, 10);
	if (next == arg || *next != '\0' || val == 0x00 || val > 0xff)
	{
		fprintf(stderr, "Invalid OEM string number: %s\n", arg);
		return -1;
	}

	opt_oem_string_keyword.offset = val;
done:
	opt.string = &opt_oem_string_keyword;
	return 0;
}

static u32 parse_opt_handle(const char* arg)
{
	u32 val;
	char* next;

	val = strtoul(arg, &next, 0);
	if (next == arg || *next != '\0' || val > 0xffff)
	{
		fprintf(stderr, "Invalid handle number: %s\n", arg);
		return ~0;
	}
	return val;
}