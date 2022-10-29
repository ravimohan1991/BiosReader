/*
 *   ----------------------------
 *  |  util.c
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

#include <sys/types.h>
#include <sys/stat.h>

#include "config.h"

#ifdef USE_MMAP
#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif /* !MAP_FAILED */
#endif /* USE MMAP */

#include <stdio.h>
#include <stdlib.h>

#ifdef BR_WINDOWS_PLATFORM
 //#include "memoryapi.h"
#endif // BR_WINDOWS_PLATFORM

#if defined (BR_LINUX_PLATFORM) || defined (BR_MAC_PLATFORM)
#ifndef TARGET_UID
#define TARGET_UID 0
#endif

#ifndef TARGET_GID
#define TARGET_GID 0
#endif

#ifndef UID_MIN
#define UID_MIN 500
#endif

#ifndef GID_MIN
#define GID_MIN 500
#endif
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <linux/capability.h>

// Decommisioned part
// #include <cap-ng.h>

#endif // BR_LINUX_PLATFORM

#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "types.h"
#include "util.h"

/* ******************************************************************************************************
 * myread: an attempt to read rSize bytes from the fileName associated with the open file descriptor,
 * fildes, into the buffer.
 * @param fildes         the open file descriptor
 * @param buffer         the buffer to write into
 * @param rSize          the number of readable bytes to be read
 * @param fileName       the file being read
 * @return int           0 for success and -1 for faliure
 * ******************************************************************************************************
 */

static int myread(int fildes, u8* buffer, size_t rSize, const char* fileName)
{
	size_t r = 1;
	size_t r2 = 0;

	while (r2 != rSize && r != 0)
	{
		// r = the total number of bytes actually read
		r = read(fildes, buffer + r2, rSize - r2);
		if (r == -1)
		{
			if (errno != EINTR)
			{
				perror(fileName);
				return -1;
			}
		}
		else
		{
			r2 += r;
		}
	}

	if (r2 != rSize)
	{
		fprintf(stderr, "%s: Unexpected end of file\n", fileName);
		return -1;
	}

	return 0;
}

/*
 * Function to verify the integrity of entry point structure by
 * ensuring that length of entry point structure "completely wraps (?)
 * and covers" the buffer in consistent fashion.
 * For more information please refer to table at
 * https://github.com/ravimohan1991/BiosReader/wiki/Demystifying-the-RAW-BIOS-information
 *
 * @param buffer              The raw form of Entry Point Structure information obtained
 * @param lenth               The length in bytes to verify with
 * @return int                1 (true) if integrity is intact, 0 (false) elsewise
 */

int checksum(const u8* buffer, size_t length)
{
	u8 sum = 0;
	size_t counter;

	for (counter = 0; counter < length; counter++)
	{
		sum += buffer[counter];
	}

	return (sum == 0);
}

/*************************************************************************************
 *
 * Reads all of file from given offset, up to max_len bytes.
 * A buffer of at most max_len bytes is allocated by this function, and
 * needs to be freed by the caller.
 * This provides a similar usage model to mem_chunk()
 *
 * Returns a pointer to the allocated buffer, or NULL on error, and
 * sets max_len to the length actually read.
 *
 *************************************************************************************
 */

int getresuid (__uid_t *__ruid, __uid_t *__euid, __uid_t *__suid);
int getresgid (__gid_t *__rgid, __gid_t *__egid, __gid_t *__sgid);
int setresuid (__uid_t __ruid, __uid_t __euid, __uid_t __suid);
int setresgid (__gid_t __rgid, __gid_t __egid, __gid_t __sgid);

void* read_file(off_t base, size_t* max_len, const char* filename, int* file_access)
{
	struct stat statbuf;
	int fd;
	u8* p;

	uid_t ruid, euid, suid; /* Real, Effective, Saved user ID */
	gid_t rgid, egid, sgid; /* Real, Effective, Saved group ID */
	int uerr, gerr;

	if (getresuid(&ruid, &euid, &suid) == -1)
	{
		fprintf(stderr, "Cannot obtain user identity: %m.\n");
		*file_access = 35;
		return NULL;
	}
	if (getresgid(&rgid, &egid, &sgid) == -1)
	{
		fprintf(stderr, "Cannot obtain group identity: %m.\n");
		*file_access = 36;
		return NULL;
	}
	if (ruid != (uid_t)TARGET_UID && ruid < (uid_t)UID_MIN)
	{
		fprintf(stderr, "Invalid user.\n");
		*file_access = 37;
		return NULL;
	}
	if (rgid != (gid_t)TARGET_UID && rgid < (gid_t)GID_MIN)
	{
		fprintf(stderr, "Invalid group.\n");
		*file_access = 38;
		return NULL;
	}

	/* Switch to target user. setuid bit handles this, but doing it again does no harm. */
	if (seteuid((uid_t)TARGET_UID) == -1)
	{
		fprintf(stderr, "Insufficient user privileges.\n");
		*file_access = 39;
		return NULL;
	}

	/* Switch to target group. setgid bit handles this, but doing it again does no harm.
	 * If TARGET_UID == 0, we need no setgid bit, as root has the privilege. */
	if (setegid((gid_t)TARGET_GID) == -1)
	{
		fprintf(stderr, "Insufficient group privileges.\n");
		*file_access = 40;
		return NULL;
	}

	/* ... privileged operations ... */

	/*
	 * Open the restricted file.
	 */

	/*
	 * Don't print error message on missing file, as we will try to read
	 * files that may or may not be present.
	 */
	if ((fd = open(filename, O_RDONLY)) == -1)
	{
		if (errno != ENOENT)
		{
			*file_access = errno;
			perror(filename);
		}
		return NULL;
	}

	/*
	 * Check file size, don't allocate more than can be read.
	 */
	if (fstat(fd, &statbuf) == 0)
	{
		if (base >= statbuf.st_size)
		{
			fprintf(stderr, "%s: Can't read data beyond EOF\n",
				filename);
			p = NULL;
			goto out;
		}
		if (*max_len > (size_t)statbuf.st_size - base)
			*max_len = statbuf.st_size - base;
	}

	if ((p = malloc(*max_len)) == NULL)
	{
		perror("malloc");
		goto out;
	}

	if (lseek(fd, base, SEEK_SET) == -1)
	{
		fprintf(stderr, "%s: ", filename);
		perror("lseek");
		goto err_free;
	}

	if (myread(fd, p, *max_len, filename) == 0)
		goto out;

	/* Drop privileges. */
	gerr = 0;
	if (setresgid(rgid, rgid, rgid) == -1)
	{
		gerr = errno;
		if (!gerr)
		gerr = EINVAL;
	}
	uerr = 0;
	if (setresuid(ruid, ruid, ruid) == -1)
	{
		uerr = errno;
		if (!uerr)
		{
			uerr = EINVAL;
		}
	}
	if (uerr || gerr)
	{
		if (uerr)
		{
			fprintf(stderr, "Cannot drop user privileges: %s.\n", strerror(uerr));
		}
		if (gerr)
		{
			fprintf(stderr, "Cannot drop group privileges: %s.\n", strerror(gerr));
		}

		return NULL;
	}

	/* ... unprivileged operations ... */

err_free:
	free(p);
	p = NULL;

out:
	if (close(fd) == -1)
	{
		perror(filename);
	}

	/* Drop privileges. */
	gerr = 0;
	if (setresgid(rgid, rgid, rgid) == -1)
	{
		gerr = errno;
		if (!gerr)
		gerr = EINVAL;
	}
	uerr = 0;
	if (setresuid(ruid, ruid, ruid) == -1)
	{
		uerr = errno;
		if (!uerr)
		{
			uerr = EINVAL;
		}
	}
	if (uerr || gerr)
	{
		if (uerr)
		{
			fprintf(stderr, "Cannot drop user privileges: %s.\n", strerror(uerr));
		}
		if (gerr)
		{
			fprintf(stderr, "Cannot drop group privileges: %s.\n", strerror(gerr));
		}

		return NULL;
	}

	return p;
}

static void safe_memcpy(void* dest, const void* src, size_t n)
{
#ifdef USE_SLOW_MEMCPY
	size_t i;

	for (i = 0; i < n; i++)
		*((u8*)dest + i) = *((const u8*)src + i);
#else
	memcpy(dest, src, n);
#endif
}

/*
 * Copy a physical memory chunk into a memory buffer.
 * This function allocates memory.
 */
void* mem_chunk(off_t base, size_t len, const char* devmem)
{
	void* p;
	int fd;
#ifdef USE_MMAP
	struct stat statbuf;
	off_t mmoffset;
	void* mmp;
#endif

	if ((fd = open(devmem, O_RDONLY)) == -1)
	{
		perror(devmem);
		return NULL;
	}

	if ((p = malloc(len)) == NULL)
	{
		perror("malloc");
		goto out;
	}

#ifdef USE_MMAP
	if (fstat(fd, &statbuf) == -1)
	{
		fprintf(stderr, "%s: ", devmem);
		perror("stat");
		goto err_free;
	}
#ifdef BR_LINUX_PLATFORM
	/*
	 * mmap() will fail with SIGBUS if trying to map beyond the end of
	 * the file.
	 */
	if (S_ISREG(statbuf.st_mode) && base + (off_t)len > statbuf.st_size)
	{
		fprintf(stderr, "mmap: Can't map beyond end of file %s\n",
			devmem);
		goto err_free;
	}

#ifdef _SC_PAGESIZE
	mmoffset = base % sysconf(_SC_PAGESIZE);
#else
	mmoffset = base % getpagesize();
#endif /* _SC_PAGESIZE */

	/*
		 * Please note that we don't use mmap() for performance reasons here,
		 * but to workaround problems many people encountered when trying
		 * to read from /dev/mem using regular read() calls.
		 */
	mmp = mmap(NULL, mmoffset + len, PROT_READ, MAP_SHARED, fd, base - mmoffset);

	/*
	#ifdef BR_WINDOWS_PLATFORM
		//mmp = VirtualAllocEx();
		// Well unless I see this interfering in my query, I forestall the Windows equivalence
		// until desired (funwise)
		mmp == MAP_FAILED;
	#endif // BR_WINDOWS_PLATFORM
	*/

	if (mmp == MAP_FAILED)
		goto try_read;

	safe_memcpy(p, (u8*)mmp + mmoffset, len);

	if (munmap(mmp, mmoffset + len) == -1)
	{
		fprintf(stderr, "%s: ", devmem);
		perror("munmap");
	}

	goto out;
#endif // BR_LINUX_PLATFORM
try_read:
#endif /* USE_MMAP */
	if (lseek(fd, base, SEEK_SET) == -1)
	{
		fprintf(stderr, "%s: ", devmem);
		perror("lseek");
		goto err_free;
	}

	if (myread(fd, p, len, devmem) == 0)
		goto out;

err_free:
	free(p);
	p = NULL;

out:
	if (close(fd) == -1)
		perror(devmem);

	return p;
}

int write_dump(size_t base, size_t len, const void* data, const char* dumpfile, int add)
{
	FILE* f;

	f = fopen(dumpfile, add ? "r+b" : "wb");
	if (!f)
	{
		fprintf(stderr, "%s: ", dumpfile);
		perror("fopen");
		return -1;
	}

	if (fseek(f, base, SEEK_SET) != 0)
	{
		fprintf(stderr, "%s: ", dumpfile);
		perror("fseek");
		goto err_close;
	}

	if (fwrite(data, len, 1, f) != 1)
	{
		fprintf(stderr, "%s: ", dumpfile);
		perror("fwrite");
		goto err_close;
	}

	if (fclose(f))
	{
		fprintf(stderr, "%s: ", dumpfile);
		perror("fclose");
		return -1;
	}

	return 0;

err_close:
	fclose(f);
	return -1;
}

/* Returns end - start + 1, assuming start < end */
u64 u64_range(u64 start, u64 end)
{
	u64 res;

	res.h = end.h - start.h;
	res.l = end.l - start.l;

	if (end.l < start.l)
		res.h--;
	if (++res.l == 0)
		res.h++;

	return res;
}
