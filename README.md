## XNU kernel precision printf() C-string panic validation program

### Prelude

This repository used as a bug validation program to verify XNU printf() panic bug with C-string precision modifier, i.e. `%[.precision]s`

This bug can be reproduce steadily in macOS which uses [log(1)](x-man-page://1/log) as its primary logging system

New [log(1)](x-man-page://1/log) logging system first adopted in macOS Sierra(10.12), currently this kernel panic bug affects macOS [10.12, 10.14.3(18D42)]

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

Please check text files under `test/` directory

### Explanation

Please refers to [Tracking down a XNU printf() precision modifier panic bug](http://junkman.cn/p/19/01_xnu_printf_precision_panic.html)

### *References*

[printf - C++ Reference](http://www.cplusplus.com/reference/cstdio/printf)

[printf, fprintf, sprintf,... - cppreference.com](https://en.cppreference.com/w/c/io/fprintf)

[xnu/osfmk/kern/printf.c#printf()](http://xr.anadoxin.org/source/xref/macos-10.13.6-highsierra/xnu-4570.71.2/osfmk/kern/printf.c#853)

[xnu/osfmk/device/subrs.c#strlcpy()](http://xr.anadoxin.org/source/xref/macos-10.13.6-highsierra/xnu-4570.71.2/osfmk/device/subrs.c#548)

---

*Created 190128*

