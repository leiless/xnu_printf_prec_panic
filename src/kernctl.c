/*
 * Created 190126 lynnl
 *
 * see: https://developer.apple.com/library/archive/documentation/Darwin/Conceptual/NKEConceptual/control/control.html
 */

#include <sys/kern_control.h>
#include <sys/kpi_mbuf.h>
#include "kernctl.h"

static errno_t kctl_connect(
        kern_ctl_ref kctlref,
        struct sockaddr_ctl *sac,
        void **unitinfo)
{

    return 0;
}

static errno_t kctl_disconnect(
        kern_ctl_ref kctlref,
        u_int32_t unit,
        void *unitinfo)
{

    return 0;
}

/**
 * see
 */
static errno_t kctl_send(
        kern_ctl_ref kctlref,
        u_int32_t unit,
        void *unitinfo,
        mbuf_t m,
        int flags)
{

    return 0;
}

static errno_t kctl_setopt(
        kern_ctl_ref kctlref,
        u_int32_t unit,
        void *unitinfo,
        int opt,
        void *data,
        size_t len)
{

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

    return 0;
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
    if (e) {
        LOG_ERR("ctl_register() fail  errno: %d", e);
    } else {
        LOG_DBG("kernel control %s registered", kctlreg.ctl_name);
    }
    return e ? KERN_FAILURE : KERN_SUCCESS;
}

kern_return_t uninstall_kern_ctl(void)
{
    errno_t e = ctl_deregister(kctlref);
    if (e) {
        LOG_ERR("ctl_deregister() fail  errno: %d", e);
    } else {
        LOG_DBG("kernel control %s deregistered", kctlreg.ctl_name);
    }
    return e ? KERN_FAILURE : KERN_SUCCESS;
}

