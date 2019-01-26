/*
 * Created 190126 lynnl
 */

#ifndef XNU_PRINTF_PREC_PANIC_H
#define XNU_PRINTF_PREC_PANIC_H

#include <libkern/libkern.h>

#ifndef __kext_makefile__
#define __TS__          "????/??/?? ??:??:??+????"
#define KEXTNAME_S      "xnu_printf_prec_panic"
#endif

#define LOG(fmt, ...)        printf(KEXTNAME_S ": " fmt "\n", ##__VA_ARGS__)
#define LOG_ERR(fmt, ...)    LOG("[ERR] " fmt, ##__VA_ARGS__)
#ifdef DEBUG
#define LOG_DBG(fmt, ...)    LOG("[DBG] " fmt, ##__VA_ARGS__)
#else
#define LOG_DBG(fmt, ...)    LOG_OFF(fmt, ##__VA_ARGS__)
#endif

#endif      /* XNU_PRINTF_PREC_PANIC_H */

