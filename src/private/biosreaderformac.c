/*
 * This file is part of the dmidecode project.
 *
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

#ifdef BR_MAC_PLATFORM

#include "biosreaderformac.h"
#include "CoreFoundation/CFNumber.h"

void mac_service_gauger_initialize()
{
	io_iterator_t anIterator;

	//IOMainPort(MACH_PORT_NULL, &masterPort);			// Aligning the pointer
}

/*
 *********************************************************************************************************
 * A method to retrieve the relevant information from the mentioned service.
 * Please read
 * 1. https://developer.apple.com/library/archive/documentation/DeviceDrivers/Conceptual/AccessingHardware/AH_Finding_Devices/AH_Finding_Devices.html
 * 2. https://developer.apple.com/library/archive/documentation/DeviceDrivers/Conceptual/AccessingHardware/AH_Device_Access_IOKit/AH_Device_Access_IOKit.html
 *
 * @param serviceClassString     The particular subclass of IOService to be probed
 * @param keyString              Name of the field (or property) whose value is to be retrieved
 * @param dataType               The datatype of field (or property) which needs be obtained
 * @return const char*           Character array containing the useful letters
 *
 * @see biosreaderformac::MacPropertyDataTypes
 **********************************************************************************************************
 */

const char* get_mac_service_field_value(const char* serviceClassString, const char* keyString, enum MacPropertyDataTypes dataType)
{
	io_service_t service = MACH_PORT_NULL;
	mach_port_t aPort;
	IOMainPort(MACH_PORT_NULL, &aPort);

	service = IOServiceGetMatchingService(aPort, IOServiceMatching(serviceClassString));

	if (service == MACH_PORT_NULL)
	{
		fprintf(stderr, "%s service is unreachable. Verify the name.\n", serviceClassString);
		return "strike";
	}

	CFMutableDictionaryRef propertiesDict = NULL;		// The dictionary of variety of fields and corresponding useful value data

	if (kIOReturnSuccess != IORegistryEntryCreateCFProperties(service,
				&propertiesDict, kCFAllocatorDefault, kNilOptions))
	{
		fprintf(stderr, "No properties can be extracted from the %s IOService.\n", serviceClassString);
		return "strike";
	}

	CFStringRef fetchingKey = CFStringCreateWithCString(NULL, keyString, CFStringGetSystemEncoding());		// Key to be fetched from plethora of fields contained within the property dictionary

	// The pointer that shall, well point to, the value
	const void* valuePointer;

	if(!CFDictionaryGetValueIfPresent(propertiesDict, fetchingKey, &valuePointer))
	{
		fprintf(stderr, "Value doesn't exist. Re-check the intention.\n");
		return "strike";
	}

	// Now the value is gotta be encapsulated within variety of types
	// Thus we need to gauge the type of valuePointer
	// Type to mean any instance of a property list type;
	// currently, CFString, CFData, CFNumber, CFBoolean, CFDate, CFArray, and CFDictionary.
	// And if that is not enough, we may have to deal with NSString and whatnot

	char* fieldValueBuffer = NULL;

	// assuming we will be getting string type of return
	if(dataType == StringType)
	{
		fieldValueBuffer = (char *) CFStringGetCStringPtr((CFStringRef) valuePointer, CFStringGetSystemEncoding());
	}
	// A shoutout to Apple forums
	// https://developer.apple.com/forums//thread/717250
	else if(dataType == NumberType)
	{
		int64_t value = 0;

		if(CFNumberGetValue((CFNumberRef) valuePointer, CFNumberGetType((CFNumberRef) valuePointer), &value))
		{
			fieldValueBuffer = malloc(sizeof(char) * 11);
			sprintf(fieldValueBuffer, "%lld", value);
		}
		else
		{
			printf("No suitable conversion found.");
		}
	}
	else
	{
		fieldValueBuffer = NULL;
	}

	if(fieldValueBuffer == NULL)
	{
		return "strike";
	}

	// Copy the information in some format
	// First clear the previous crap
	if(returnString != NULL)
	{
		free(returnString);
	}

	returnString = malloc(sizeof(char) * sizeof(fieldValueBuffer));

	// Now do the actual copying
	strcpy(returnString, fieldValueBuffer);

	if(dataType == NumberType)
	{
		if(fieldValueBuffer)
		{
			free(fieldValueBuffer);
		}
	}

	// Release and free resources for subsequent routines
	CFIndex count = CFDictionaryGetCount(propertiesDict);
	if (propertiesDict != NULL && count > 0)
	{
		//CFRelease(propertiesDict);
	}

	if(fetchingKey != NULL)
	{
		CFRelease(fetchingKey);
	}

	IOObjectRelease(service);

	return returnString;
}

void mac_service_gauger_decommision()
{
	if(returnString != NULL)
	{
		free(returnString);
	}
}

#endif

