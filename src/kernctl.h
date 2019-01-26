/*
 * Created 190126 lynnl
 */

#ifndef KERNCTL_H
#define KERNCTL_H

#include "common.h"

#define KERN_CTL_NAME       BUNDLEID_S ".kernctl"

kern_return_t install_kern_ctl(void);
kern_return_t uninstall_kern_ctl(void);

#endif      /* KERNCTL_H */
