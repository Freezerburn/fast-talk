#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#ifdef __MACH__
#include <mach/mach_time.h> /* mach_absolute_time */
#include <mach/mach.h>      /* host_get_clock_service, mach_... */
#include <mach/clock.h>     /* clock_get_time */
#endif

#include "FST.h"

int main() {
#ifdef __MACH__
    mach_timebase_info_data_t timebase = {0, 0};
    mach_timebase_info(&timebase);
    uint64_t timebaseRatio = (uint64_t)timebase.numer / (uint64_t)timebase.denom;
    uint64_t initclock = mach_absolute_time();
#endif

    Ft_Interp *interp = FST_MkInterp();
    FtModuleInit_IntObj(interp);

    Ft_Obj *testInt = FtIntObj_Init(10);

#ifdef __MACH__
    uint64_t total = 0;
#endif

    int runs = 10000000;
    for (int i = 0; i < runs; i++) {
#ifdef __MACH__
        uint64_t clock = mach_absolute_time() - initclock;
        uint64_t nanoBefore = clock * timebaseRatio;
#endif
        Ft_Obj *toAdd = FtIntObj_Init(10);
        Ft_StaticMsg plusMsg = FtStaticMsg_Init(FtStr_Init("+"), toAdd, NULL);
        Ft_Obj *result = FtObj_Handle(interp, testInt, FtStaticMsg_CastMsg(&plusMsg));

        Ft_StaticMsg printMsg = FtStaticMsg_Init(FtStr_Init("print"), NULL);
        Ft_Obj* printResult = FtObj_Handle(interp, result, FtStaticMsg_CastMsg(&printMsg));
        FtObj_DECREF(result);
        FtObj_DECREF(printResult);

#ifdef __MACH__
        clock = mach_absolute_time() - initclock;
        uint64_t nanoAfter = clock * timebaseRatio;
        total += (nanoAfter - nanoBefore);
#endif
    }

#ifdef __MACH__
    printf("%llu nanos\n", total / runs);
#endif

    return 0;
}