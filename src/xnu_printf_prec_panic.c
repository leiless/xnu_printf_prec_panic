/*
 * Created 190126 lynnl
 */

#include <sys/systm.h>
#include <mach/mach_types.h>
#include <kern/thread.h>
#include "xnu_printf_prec_panic.h"

/**
 * Terminate a thread(not exported KPI)
 * @return  0 if terminated  errno o.w.
 *          KERN_INVALID_ARGUMENT   if thread is THREAD_NULL
 *          KERN_FAILURE            if try stop kernel_task by other thread
 *          KERN_TERMINATED         if the thread already terminated
 * see: xnu/osfmk/kern/thread_act.c#thread_terminate
 */
extern kern_return_t thread_terminate(thread_t);

static void thread_runloop(void *arg __unused, wait_result_t wres __unused)
{
    static struct timespec ts = {5, 0};
    static uint64_t i = 0;

    while (1) {
        ++i;
        LOG_DBG("wakeup #%llu", i);
        (void) msleep(NULL, NULL, PPAUSE, NULL, &ts);
    }
}

static thread_t panic_thread = THREAD_NULL;

kern_return_t xnu_printf_prec_panic_start(
        kmod_info_t *ki __unused,
        void *d __unused)
{
    kern_return_t e;
    e = kernel_thread_start(thread_runloop, NULL, &panic_thread);
    if (e != KERN_SUCCESS) {
        LOG_ERR("kernel_thread_start() fail  errno: %d", e);
    } else {
        LOG("kernel thread %p started", panic_thread);
    }
    return e;
}

kern_return_t xnu_printf_prec_panic_stop(
        kmod_info_t *ki __unused,
        void *d __unused)
{
    kern_return_t e;
    e = thread_terminate(panic_thread);
    if (e != KERN_SUCCESS) {
        LOG_ERR("thread_terminate() fail  errno: %d", e);
        goto out_exit;
    }

    thread_deallocate(panic_thread);
    LOG("kernel thread %p destroyed", panic_thread);
out_exit:
    return e;
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

