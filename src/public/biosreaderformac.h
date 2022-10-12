/*
 * This file is part of the dmidecode project.
 * The name is bit'o misnomer becoz for Mac we essentially tackle the IOServices
 * which include much more than AppleSMBios as of now!
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


#if defined BR_MAC_PLATFORM

#pragma once

#include <Carbon/Carbon.h>
#include <IOKit/hid/IOHIDKeys.h> // For HUMAN INTERFACE DEVICES
#include <IOKit/serial/IOSerialKeys.h>
#include <MacTypes.h>
#include <CoreFoundation/CFBase.h>
//#include <Foundation/Foundation.h>

#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


// Type to mean any instance of a property list type;
// CFString, CFData, CFNumber, CFBoolean, CFDate, CFArray, and CFDictionary.
// And if that is not enough, we may have to deal with NSString and whatnot
enum MacPropertyDataTypes
{
	NumberType = 0,
	StringType,
	BooleanType,
	DataType,
	ArrayType,
	NSString
};

//static mach_port_t masterPort;						// Port for service querying
static char* returnString = NULL;

void mac_service_gauger_initialize();
const char* get_mac_service_field_value(const char* serviceClassString, const char* keyString, enum MacPropertyDataTypes dataType);
void mac_service_gauger_decommision();

#endif
