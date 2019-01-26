/*
 * Created 190126 lynnl
 */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>             /* memset */
#include <unistd.h>             /* close */
#include <limits.h>

#include <sys/errno.h>          /* errno */
#include <sys/kern_control.h>   /* sockaddr_ctl  ctl_info */
#include <sys/socket.h>         /* socket */
#include <sys/sys_domain.h>     /* SYSPROTO_CONTROL */
#include <sys/ioctl.h>          /* ioctl */

#define LOG(fmt, ...)           printf(fmt "\n", ##__VA_ARGS__)
#define LOG_ERR(fmt, ...)       LOG("[ERR] " fmt, ##__VA_ARGS__)
#ifdef DEBUG
#define LOG_DBG(fmt, ...)       LOG("[DBG] " fmt, ##__VA_ARGS__)
#else
#define LOG_DBG(fmt, ...)       (void) ((void) 0, ##__VA_ARGS__)
#endif

#define KERN_CTL_NAME           "cn.junkman.kext.xnu_printf_prec_panic.kernctl"

/**
 * Connect to a kernel control socket
 * @kctlname    kernel control name
 * @return      file descriptor  -1 if fail(errno will set)
 */
int connect_to_kern_ctl(const char *kctlname)
{
    int e;
    int fd;
    struct sockaddr_ctl sc;
    struct ctl_info ci;

    fd = socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);
    if (fd == -1) {
        LOG_ERR("socket(2) fail  errno: %d", errno);
        goto out_exit;
    }

    LOG_DBG("kernel control  fd: %d", fd);

    memset(&ci, 0, sizeof(struct ctl_info));
    strncpy(ci.ctl_name, kctlname, MAX_KCTL_NAME-1);
    ci.ctl_name[MAX_KCTL_NAME-1] = '\0';

    /*
     * The kernel control ID is dynamically generated
     *  we can obtain ci.sc_id via ioctl(2)
     * If ioctl(2) returns ENOENT  it means control name doesn't exist
     *  we should load kext before run the daemon
     */
    e = ioctl(fd, CTLIOCGINFO, &ci);
    if (e == -1) {
        LOG_ERR("ioctl(2) fail  errno: %d", errno);
        goto out_fail;
    }

    LOG_DBG("kernel control  name: %s id: %d", ci.ctl_name, ci.ctl_id);

    memset(&sc, 0, sizeof(struct sockaddr_ctl));
    sc.sc_len     = sizeof(struct sockaddr_ctl);
    sc.sc_family  = AF_SYSTEM;
    sc.ss_sysaddr = AF_SYS_CONTROL;
    sc.sc_id      = ci.ctl_id;
    sc.sc_unit    = 0;

    e = connect(fd, (struct sockaddr *) &sc, sizeof(struct sockaddr_ctl));
    if (e == -1) {
        LOG_ERR("connect(2) fail  fd: %d errno: %d", fd, errno);
        goto out_fail;
    }

    LOG("connected  name: %s id: %d fd: %d", ci.ctl_name, ci.ctl_id, fd);

out_exit:
    return fd;

out_fail:
    (void) close(fd);
    fd = -1;
    goto out_exit;
}

#define MAX_KERN_CTL_MSG_SIZE           2048

/**
 * Send setsockopt(2) message to kernel control socket
 * @fd          socket descriptor
 * @type        option
 * @msg         message
 * @len         message length
 * @return      0 if success  -1 o.w.(errno will set)
 */
int setsockopt_to_kern_ctl(int fd, int type, const char *msg, size_t len)
{
    int e;

    if (msg == NULL || len == 0) {
        /* Prevent potential programmatic error */
        assert(msg == NULL);
        assert(len == 0);
    }

    if (len > MAX_KERN_CTL_MSG_SIZE) {
        e = -1;
        errno = EMSGSIZE;
        LOG_ERR("message too long  fd: %d type: %d len: %zu", fd, type, len);
        goto out_exit;
    }

    e = setsockopt(fd, SYSPROTO_CONTROL, type, (void *) msg, (socklen_t) len);
    if (e == -1) {
        LOG_ERR("setsockopt(2) fail  fd: %d type: %d len: %zu", fd, type, len);
    }

out_exit:
    return e;
}

#define SOCKOPT_PSEUDO      1000

int main(void)
{
    int fd;

    fd = connect_to_kern_ctl(KERN_CTL_NAME);
    if (fd < 0) exit(EXIT_FAILURE);

    while (1) {
        if (setsockopt_to_kern_ctl(fd, SOCKOPT_PSEUDO, NULL, 0)) {
            break;
        }
        (void) sleep(1);
    }

    (void) close(fd);
    return 0;
}

