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
$ ./daemon
...

# Check kext logs
$ sudo dmesg | grep xnu_printf_prec_panic
...
```

### Sample output

Tested on macOS High Sierra(10.13.6 17G65)

```
TODO
```

### Explanation

**TODO: tl;dr**

### *References*

[printf - C++ Reference](http://www.cplusplus.com/reference/cstdio/printf)

[printf, fprintf, sprintf,... - cppreference.com](https://en.cppreference.com/w/c/io/fprintf)

---

*Created 190128*