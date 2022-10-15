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
#include <sys/mman.h>
#include <unistd.h>
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

int checksum(const u8* buf, size_t len)
{
	u8 sum = 0;
	size_t a;

	for (a = 0; a < len; a++)
		sum += buf[a];
	return (sum == 0);
}

/*
 * Reads all of file from given offset, up to max_len bytes.
 * A buffer of at most max_len bytes is allocated by this function, and
 * needs to be freed by the caller.
 * This provides a similar usage model to mem_chunk()
 *
 * Returns a pointer to the allocated buffer, or NULL on error, and
 * sets max_len to the length actually read.
 */
void* read_file(off_t base, size_t* max_len, const char* filename)
{
	struct stat statbuf;
	int fd;
	u8* p;

	/*
	 * Don't print error message on missing file, as we will try to read
	 * files that may or may not be present.
	 */
	if ((fd = open(filename, O_RDONLY)) == -1)
	{
		if (errno != ENOENT)
			perror(filename);
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

err_free:
	free(p);
	p = NULL;

out:
	if (close(fd) == -1)
		perror(filename);

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
