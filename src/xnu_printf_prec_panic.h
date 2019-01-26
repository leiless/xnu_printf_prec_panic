/*
 * Created 190126 lynnl
 */

#ifndef XNU_PRINTF_PREC_PANIC_H
#define XNU_PRINTF_PREC_PANIC_H

#include <libkern/libkern.h>
#include <kern/debug.h>

#ifndef __kext_makefile__
#define __TS__          "????/??/?? ??:??:??+????"
#define KEXTNAME_S      "xnu_printf_prec_panic"
#endif

#define LOG(fmt, ...)        printf(KEXTNAME_S ": " fmt "\n", ##__VA_ARGS__)
#define LOG_ERR(fmt, ...)    LOG("[ERR] " fmt, ##__VA_ARGS__)
#define LOG_BUG(fmt, ...)    LOG("[BUG] " fmt, ##__VA_ARGS__)
#ifdef DEBUG
#define LOG_DBG(fmt, ...)    LOG("[DBG] " fmt, ##__VA_ARGS__)
#else
#define LOG_DBG(fmt, ...)    LOG_OFF(fmt, ##__VA_ARGS__)
#endif

#define panicf(fmt, ...)                \
    panic("\n" fmt "\n%s@%s#L%d\n\n",   \
            ##__VA_ARGS__, __BASE_FILE__, __FUNCTION__, __LINE__)

#ifdef DEBUG
#define kassert(ex) (ex) ? (void) 0 : panicf("Assert `%s' failed", #ex)

#define kassertf(ex, fmt, ...) \
    (ex) ? (void) 0 : panicf("Assert `%s' failed: " fmt, #ex, ##__VA_ARGS__)
#else
#define kassert(ex) (ex) ? (void) 0 : LOG_BUG("Assert `%s' failed", #ex)

#define kassertf(ex, fmt, ...) \
    (ex) ? (void) 0 : LOG_BUG("Assert `%s' failed: " fmt, #ex, ##__VA_ARGS__)
#endif

/**
 * kern_os_* family provides zero-out memory allocation
 * see: xnu/libkern/c++/OSRuntime.cpp
 */
extern void *kern_os_malloc(size_t);
extern void kern_os_free(void *);
extern void *kern_os_realloc(void *, size_t);

#endif      /* XNU_PRINTF_PREC_PANIC_H */

