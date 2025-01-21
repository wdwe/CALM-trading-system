# Installation of libbid.a (Intel® Decimal Floating-Point Math Library)

# Important
After downloading the source code from tip 1 and building the projects, it is important to run the tests in the source code
to check the library is compatible with the processor (e.g. there will be error if it is run on Mac Arm chips).

## Debug
### tip 1
When compiling on Mac, compiler may complain about linkage error when compiling using the included libbid.a library (Intel® Decimal Floating-Point Math Library) from IBKR.

The solution is to download the source code from [here](https://www.netlib.org/misc/intel/) and recompile with the right platform.

Two modifications are needed for the successful compilation of the library.

```c
#include <stdlib.h>
```
needs to be added to each of the `bid21/64/128_pow.c` files in the LIBRARY/src

```c
# include <signal.h> // line 134 from <sys/signal.h> 
```
needs to be changed in `LIBRARY/float128/dpml_exception.c`

### tip 2
[discussion on debugging](https://groups.io/g/twsapi/topic/96140048?p=%2C%2C%2C20%2C0%2C0%2C0%3A%3Arecentpostdate%2Fsticky%2C%2C%2C20%2C2%2C180%2C96140048)
when reference option is opened, there will be segmentation fault error. i.e. don't use `[gi]cc1xxb?libbid.a`

## Logs
the `libbid.a` in this folder is `gcc011libbid.a` after running `RUNLINUX`.