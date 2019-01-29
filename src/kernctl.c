/*
 * Created 190126 lynnl
 *
 * see: https://developer.apple.com/library/archive/documentation/Darwin/Conceptual/NKEConceptual/control/control.html
 */

#include <sys/kern_control.h>
#include <sys/kpi_mbuf.h>
#include <sys/errno.h>
#include <libkern/OSAtomic.h>
#include "kernctl.h"

#include <sys/malloc.h>

static errno_t kctl_connect(
        kern_ctl_ref kctlref,
        struct sockaddr_ctl *sac,
        void **unitinfo)
{
    UNUSED(kctlref, unitinfo);
    LOG_DBG("connected  id: %d unit: %d", sac->sc_id, sac->sc_unit);
    return 0;
}

/*
 * If ctl_connect() nonzero
 *  it'll fall through to ctl_disconnect() automatically
 * see: xnu/bsd/kern/kern_control.c#ctl_connect
 */
static errno_t kctl_disconnect(
        kern_ctl_ref kctlref,
        u_int32_t unit,
        void *unitinfo)
{
    UNUSED(kctlref, unit, unitinfo);
    return 0;
}

static errno_t kctl_send(
        kern_ctl_ref kctlref,
        u_int32_t unit,
        void *unitinfo,
        mbuf_t m,
        int flags)
{
    UNUSED(kctlref, unit, unitinfo, flags);

    while ((m = mbuf_free(m)) != NULL)
        continue;

    return ENOTSUP;
}

static volatile uint64_t cnt = 0;

#define PANIC_QUICKLY       1

static errno_t kctl_setopt(
        kern_ctl_ref kctlref,
        u_int32_t unit,
        void *unitinfo,
        int opt,
        void *data,
        size_t len)
{
    char *s;
    uint64_t i;

    UNUSED(kctlref, unitinfo, opt);

    s = (char *) data;
    i = 1 + OSIncrementAtomic64((volatile SInt64 *) &cnt);
    /* Assume data is a C-string */
    LOG("setopt() #%llu  unit: %d data: %#x %.*s",
            i, unit, (uint32_t) s, (int) len, s);

#if PANIC_QUICKLY
    /**
     * This memleak used to increase kernel address entropy
     *  so the kernel can panic quickly
     */
    (void) _MALLOC(1, M_TEMP, M_NOWAIT);
#endif

    return 0;
}

static errno_t kctl_getopt(
        kern_ctl_ref kctlref,
        u_int32_t unit,
        void *unitinfo,
        int opt,
        void *data,
        size_t *len)
{
    UNUSED(kctlref, unit, unitinfo, opt, data, len);
    return ENOTSUP;
}

/*
 * NOTE:
 *  xnu/bsd/kern/kern_control.c
 *      #define CTL_SENDSIZE (2 * 1024)     //  data into kernel
 *      #define CTL_RECVSIZE (8 * 1024)     //  data out of kernel
 *  above denoted default send/recv buffer size
 *
 * see: xnu/bsd/kern/kern_control.c#ctl_getenqueuespace
 */
static struct kern_ctl_reg kctlreg = {
    KERN_CTL_NAME,          /* Name up to MAX_KCTL_NAME */
    0,                      /* ID(dynamically assigned) */
    0,                      /* Unit(dynamically assigned) */
    0,                      /* Flags (CTL_FLAG_*) */
    0,                      /* Sendsize(0 for default) */
    0,                      /* Recvsize(ditto) */
    kctl_connect,           /* The ctl_connect callback must be specified */
    kctl_disconnect,
    kctl_send,
    kctl_setopt,
    kctl_getopt,
};

static kern_ctl_ref kctlref;

kern_return_t install_kern_ctl(void)
{
    errno_t e = ctl_register(&kctlreg, &kctlref);
    if (e == 0) {
        LOG("kernel control %s registered", kctlreg.ctl_name);
    } else {
        LOG_ERR("ctl_register() fail  errno: %d", e);
    }
    return e ? KERN_FAILURE : KERN_SUCCESS;
}

kern_return_t uninstall_kern_ctl(void)
{
    errno_t e = ctl_deregister(kctlref);
    if (e == 0) {
        LOG("kernel control %s deregistered", kctlreg.ctl_name);
    } else {
        LOG_ERR("ctl_deregister() fail  errno: %d", e);
    }
    return e ? KERN_FAILURE : KERN_SUCCESS;
}

