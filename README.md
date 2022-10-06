BiosReader
==========

INTRODUCTION
------------

BiosReader is a cross-platform application (und library) which reports information about your system's vital electronics as described in your system BIOS according to the SMBIOS/DMI ([wiki link](https://en.wikipedia.org/wiki/Desktop_Management_Interface)) standard (whatever this is). 

This information typically includes system manufacturer, model name, serial number, BIOS version, asset tag as well as a lot of other details of varying level of
interest and reliability depending on the manufacturer. 

This will often include usage status for the CPU sockets, expansion slots (e.g. AGP, PCI, ISA) and memory module slots, and the list of I/O ports (e.g. serial,
parallel, USB).

DMI data can be used to enable or disable specific portions of kernel code depending on the specific hardware. Thus, one use of BiosReader is for kernel
developers to detect system "signatures" and add them to the kernel source code when needed without breaking compatibility.

CAUTION:
-------
Beware that DMI data have proven to be too unreliable to be blindly trusted.
BiosReader does not bother your hardware, it only reports what the BIOS told it to.

Please note that this project is extention and, supposabily, heavy modification of [demidecode](https://www.nongnu.org/dmidecode/). Therefore you can identify, and, possibly mingle with the code, if familiar already.


INSTALLATION
------------

Install [CMake](https://cmake.org/) and rest is cakewalk.

THANKS
------
DEVELOPER AND MAINTAINER
Jean Delvare <jdelvare@suse.de>

ORIGINAL AUTHORS
Alan Cox <alan@redhat.com>
Jean Delvare <jdelvare@suse.de>

Rest of the contributers are named [here](http://git.savannah.gnu.org/cgit/dmidecode.git/plain/AUTHORS).
