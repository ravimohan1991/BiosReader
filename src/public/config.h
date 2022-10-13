/*
 * Configuration
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
