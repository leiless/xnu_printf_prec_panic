## XNU kernel precision printf() C-string panic validation program

### Prelude

This repository used as a bug validation program to verify XNU printf() panic bug with C-string precision modifier, i.e. `%.[precision]s`

This bug can be reproduce steadily in macOS which uses [log(1)](x-man-page://1/log) as its primary logging system

New [log(1)](x-man-page://1/log) logging system first adopted in macOS Sierra(10.12), currently this kernel panic bug can be reproduced in macOS [10.12, 10.14.2(18D42)]

### Compile & test

```shell
# Use `release' target for kext release build
$ make [release]

$ mkdir -p /var/tmp/t
$ sudo cp -R xnu_printf_prec_panic.kext /var/tmp/t
$ sudo kextload -v /var/tmp/t/xnu_printf_prec_panic.kext

$ cd daemon
# Use `release' target for user-space daemon release build
$ make [release]
# Run user-space test program
$ ./daemon.out
...

# Check kext logs
$ sudo dmesg | grep xnu_printf_prec_panic
...
```

### Sample output

```
$ sw_vers
ProductName:	Mac OS X
ProductVersion:	10.13.6
BuildVersion:	17G65

$ lldb /Library/Developer/KDKs/KDK_10.13.6_17G65.kdk/System/Library/Kernels/kernel.debug
(lldb) kdp-remote 172.16.41.130
...
(lldb) bt
* thread #2, name = '0xffffff803178dca0', queue = '0x0', stop reason = signal SIGSTOP
  * frame #0: 0xffffff8019cf81b6 kernel.debug`strlcpy(dst="hypothesi", src="hypothesis??ﾭ?"..., siz=10) at subrs.c:566
    frame #1: 0xffffff8019cf8659 kernel.debug`__strlcpy_chk(dst="hypothesi", src="hypothesis??ﾭ?"..., s=10, chk_size=18446744073709551615) at subrs.c:698
    frame #2: 0xffffff801a57a76f kernel.debug`_encode_data(content=0xffffff82f773b670, arg=0xffffff8033da2900, arg_len=10, context=0xffffff82f773b4b0) at log_encode.h:64
    frame #3: 0xffffff801a579b25 kernel.debug`_os_log_encode [inlined] _os_log_encode_arg(arg=0xffffff8033da2900, arg_len=10, ctype='\x02', is_private=false, context=0xffffff82f773b4b0) at log_encode.h:208
    frame #4: 0xffffff801a579831 kernel.debug`_os_log_encode(format="xnu_printf_prec_panic: setopt()  #%llu opt: %4d data: %.*s\n", args=0xffffff82f773b530, saved_errno=0, context=0xffffff82f773b4b0) at log_encode.h:457
    frame #5: 0xffffff801a576447 kernel.debug`_os_log_to_log_internal(oslog=0xffffff801aa8eb80, type='\0', format="xnu_printf_prec_panic: setopt()  #%llu opt: %4d data: %.*s\n", args=0xffffff82f773b9d0, addr=0xffffff7f9c542dd3, dso=0xffffff7f9c542000) at log.c:315
    frame #6: 0xffffff801a572dcd kernel.debug`_os_log_with_args_internal(oslog=0xffffff801aa8eb80, type='\0', format="xnu_printf_prec_panic: setopt()  #%llu opt: %4d data: %.*s\n", args=0xffffff82f773b9d0, addr=0xffffff7f9c542dd3, dso=0x0000000000000000) at log.c:189
    frame #7: 0xffffff801a572e3b kernel.debug`os_log_with_args(oslog=0xffffff801aa8eb80, type='\0', format="xnu_printf_prec_panic: setopt()  #%llu opt: %4d data: %.*s\n", args=0xffffff82f773b9d0, addr=0xffffff7f9c542dd3) at log.c:160
    frame #8: 0xffffff8019b83115 kernel.debug`vprintf_internal(fmt="xnu_printf_prec_panic: setopt()  #%llu opt: %4d data: %.*s\n", ap_in=0xffffff82f773b9d0, caller=0xffffff7f9c542dd3) at printf.c:846
    frame #9: 0xffffff8019b82f12 kernel.debug`printf(fmt="xnu_printf_prec_panic: setopt()  #%llu opt: %4d data: %.*s\n") at printf.c:859
    frame #10: 0xffffff7f9c542dd3 xnu_printf_prec_panic`kctl_setopt(kctlref=<unavailable>, unit=<unavailable>, unitinfo=<unavailable>, opt=<unavailable>, data=<unavailable>, len=10) at kernctl.c:94 [opt]
    frame #11: 0xffffff801a30de79 kernel.debug`ctl_ctloutput(so=0xffffff80323b4dc0, sopt=0xffffff82f773be00) at kern_control.c:1206
    frame #12: 0xffffff801a3ba76c kernel.debug`sosetoptlock(so=0xffffff80323b4dc0, sopt=0xffffff82f773be00, dolock=1) at uipc_socket.c:4759
    frame #13: 0xffffff801a3cf292 kernel.debug`setsockopt(p=0xffffff8032bb36d0, uap=0xffffff802dc046d0, retval=0xffffff802dc04710) at uipc_syscalls.c:2421
    frame #14: 0xffffff801a525e3d kernel.debug`unix_syscall64(state=0xffffff8030c812a0) at systemcalls.c:382
    frame #15: 0xffffff8019d82706 kernel.debug`hndl_unix_scall64 + 22

(lldb) up 10
frame #10: 0xffffff7f9c542dd3 xnu_printf_prec_panic`kctl_setopt(kctlref=<unavailable>, unit=<unavailable>, unitinfo=<unavailable>, opt=<unavailable>, data=<unavailable>, len=10) at kernctl.c:94 [opt]
   91  	    s = (char *) data;
   92  	    i = 1 + OSIncrementAtomic64((volatile SInt64 *) &cnt);
   93  	    /* Assume data is C string */
-> 94  	    LOG("setopt()  #%llu opt: %4d data: %.*s", i, opt, (int) len, s);
   95
   96  	    return 0;
   97  	}

(lldb) register read
General Purpose Registers:
       rax = 0xffffff8033da3000
       rbx = 0x000000000000218c
       rcx = 0xffffff8033da3001
       rdx = 0x0000000000000069
       rdi = 0xffffff82f773b550
       rsi = 0x0000000000000069
       rbp = 0xffffff82f773a9a0
       rsp = 0xffffff82f773a970
        r8 = 0x0000000000000000
        r9 = 0x0000000000000002
       r10 = 0x000000000000000a
       r11 = 0x0000000000000016
       r12 = 0xffffff82f773bfb0
       r13 = 0x0000000000000000
       r14 = 0x000000000000000a
       r15 = 0xffffff8033da2900
       rip = 0xffffff8019cf81b6  kernel.debug`strlcpy + 214 at subrs.c:566
    rflags = 0x0000000000010282
        cs = 0x0000000000000008
        fs = 0x00000000ffff0000
        gs = 0x000000009b730000

(lldb) showbootargs
debug=0x144 kext-dev-mode=1 kcsuffix=debug pmuflags=1 -v

(lldb) showversion
Darwin Kernel Version 17.7.0: Thu Jun 21 22:52:15 PDT 2018; root:xnu_debug-4570.71.2~1/DEBUG_X86_64
```

### Explanation

**TODO: tl;dr**

### *References*

[printf - C++ Reference](http://www.cplusplus.com/reference/cstdio/printf)

[printf, fprintf, sprintf,... - cppreference.com](https://en.cppreference.com/w/c/io/fprintf)

---

*Created 190128*