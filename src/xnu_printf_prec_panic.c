/*
 * Created 190126 lynnl
 */

#include <sys/systm.h>
#include <mach/mach_types.h>
#include <kern/thread.h>
#include <kern/task.h>
#include "xnu_printf_prec_panic.h"

/**
 * Terminate a thread(not exported KPI)
 * @return  0 if terminated  errno o.w.
 *          KERN_INVALID_ARGUMENT   if thread is THREAD_NULL
 *          KERN_FAILURE            if not terminate by itself
 *          KERN_TERMINATED         if the thread already terminated
 * see: xnu/osfmk/kern/thread_act.c#thread_terminate
 */
extern kern_return_t thread_terminate(thread_t);

static thread_t panic_thd = THREAD_NULL;
static volatile int8_t cond_keepalive = 1;

/**
 * Do printf() precision print panic test
 */
static void printf_prec_panic_test(void)
{
    int sz = 11;
    char *p = _MALLOC(sz, M_TEMP, M_NOWAIT);
    int n;

    if (p == NULL) {
        LOG_ERR("_MALLOC() fail  OOM");
        return;
    }

    n = snprintf(p, sz, "%#010x", (uint32_t) p);
    kassertf(n == sz-1, "n is %d", n);
    kassertf(p[sz-1] == '\0', "p[%d] is %#x", sz-1, p[sz-1]);
    /* Manually clear the EOS to test printf() precision print */
    p[sz-1] = '!';

    LOG_DBG("%.*s", sz-1, p);
    _FREE(p, M_TEMP);
}

#define NS_PER_MS           1000000

/*
 * XXX:
 *  if you intended to keep this thread running even if
 *  kernel extension is unloaded  you should never access any kext var/func
 *  instead all data should be allocated dynamically
 *  o.w. the kernel will panic(memory access violation)
 *
 *  if you intended to stop this thread when kext unloading
 *      and this thread access kext var/func
 *  you should make sure this thread stop before kext unload
 */
static void thread_runloop(void *arg __unused, wait_result_t wres __unused)
{
    static struct timespec ts = {0, 10 * NS_PER_MS};

    while (cond_keepalive) {
        printf_prec_panic_test();
        (void) msleep((void *) &cond_keepalive, NULL, PPAUSE, NULL, &ts);
    }

    kassert(current_thread() == panic_thd);
    LOG("conditional variable changed");
    LOG("kernel thread %p:%#llx is dying", panic_thd, thread_tid(panic_thd));

    /* for the extra refcnt from kernel_thread_start() */
    thread_deallocate(current_thread());

    /*
     * signal blocked kext unload procedure to continue
     *  there still negligible chance between thread_terminate and kextunload
     */
    cond_keepalive = -1;

    /* thread done */
    (void) thread_terminate(current_thread());

    /* see: xnu/bsd/net/dlil.c#ifnet_start_thread_fn */
    __builtin_unreachable();
}

kern_return_t xnu_printf_prec_panic_start(
        kmod_info_t *ki __unused,
        void *d __unused)
{
    kern_return_t e;
    e = kernel_thread_start(thread_runloop, NULL, &panic_thd);
    if (e != KERN_SUCCESS) {
        LOG_ERR("kernel_thread_start() fail  errno: %d", e);
    } else {
        LOG("kernel thread %p:%#llx started", panic_thd, thread_tid(panic_thd));
    }
    return e;
}

kern_return_t xnu_printf_prec_panic_stop(
        kmod_info_t *ki __unused,
        void *d __unused)
{
    cond_keepalive = 0;
    wakeup_one((void *) &cond_keepalive);
    /* Busy wait the thread to finish */
    while (cond_keepalive != -1) continue;

    LOG("kext unloaded");
    return KERN_SUCCESS;
}

#ifdef __kext_makefile__
extern kern_return_t _start(kmod_info_t *, void *);
extern kern_return_t _stop(kmod_info_t *, void *);

/* Will expand name if it's a macro */
#define KMOD_EXPLICIT_DECL2(name, ver, start, stop) \
    __attribute__((visibility("default")))          \
        KMOD_EXPLICIT_DECL(name, ver, start, stop)

KMOD_EXPLICIT_DECL2(BUNDLEID, KEXTBUILD_S, _start, _stop)

/* If you intended to write a kext library  NULLify _realmain and _antimain */
__private_extern__ kmod_start_func_t *_realmain = xnu_printf_prec_panic_start;
__private_extern__ kmod_stop_func_t *_antimain = xnu_printf_prec_panic_stop;

__private_extern__ int _kext_apple_cc = __APPLE_CC__;
#endif

