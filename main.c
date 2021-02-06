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

#include "FastTalk.h"

int main() {
#ifdef __MACH__
    mach_timebase_info_data_t timebase = {0, 0};
    mach_timebase_info(&timebase);
    uint64_t timebaseRatio = (uint64_t)timebase.numer / (uint64_t)timebase.denom;
    uint64_t initclock = mach_absolute_time();
#endif

    Ft_Interp *interp = FtInterp_Init();
    FtModuleInit_IntObj(interp);

    Ft_Obj *testInt = FtIntObj_Init(10);

    int runs = 10000000;
#ifdef __MACH__
    uint64_t total = 0;
    uint64_t avgCalcTime = 0;
    for (int i = 0; i < runs; i++) {
        uint64_t clockBefore = mach_absolute_time();
        uint64_t clockAfter = mach_absolute_time();
        uint64_t nanoBefore = (clockBefore - initclock) * timebaseRatio;
        uint64_t nanoAfter = (clockAfter - initclock) * timebaseRatio;
        total += (nanoAfter - nanoBefore);
    }
    avgCalcTime = total / runs;
    printf("avg nanos to get a clock time diff: %llu\n", avgCalcTime);
    total = 0;
#endif

    for (int i = 0; i < runs; i++) {
#ifdef __MACH__
        uint64_t clock = mach_absolute_time() - initclock;
        uint64_t nanoBefore = clock * timebaseRatio;
#endif
        Ft_Obj *toAdd = FtIntObj_Init(10);

#ifdef __MACH__
        clock = mach_absolute_time() - initclock;
        uint64_t nanoAfter = clock * timebaseRatio;
        total += (nanoAfter - nanoBefore);
#endif
        FtObj_DECREF(toAdd);
    }
#ifdef __MACH__
    printf("%llu nanos (intobj init)\n", total / runs);
    total = 0;
#endif

    for (int i = 0; i < runs; i++) {
        Ft_Obj *toAdd = FtIntObj_Init(10);
#ifdef __MACH__
        uint64_t clock = mach_absolute_time() - initclock;
        uint64_t nanoBefore = clock * timebaseRatio;
#endif
        FtObj_DECREF(toAdd);
#ifdef __MACH__
        clock = mach_absolute_time() - initclock;
        uint64_t nanoAfter = clock * timebaseRatio;
        total += (nanoAfter - nanoBefore);
#endif
    }
#ifdef __MACH__
    printf("%llu nanos (intobj decref)\n", total / runs);
    total = 0;
#endif

    Ft_MsgName* addMsg = FtMsgName_Find(FtStr_InitLen("+", 1));
    for (int i = 0; i < runs; i++) {
#ifdef __MACH__
        uint64_t clockBefore = mach_absolute_time();
#endif
        Ft_Obj *toAdd = FtIntObj_Init(10);
        Ft_StaticMsg plusMsg = FtStaticMsg_Init(addMsg, toAdd, NULL);
        Ft_Obj *result = FtObj_SendMsg(interp, testInt, FtStaticMsg_CastMsg(&plusMsg));
        FtObj_DECREF(toAdd);
        FtObj_DECREF(result);

#ifdef __MACH__
        uint64_t clockAfter = mach_absolute_time();
        uint64_t nanoBefore = (clockBefore - initclock) * timebaseRatio;
        uint64_t nanoAfter = (clockAfter - initclock) * timebaseRatio;
        total += (nanoAfter - nanoBefore);
#endif
    }

#ifdef __MACH__
    printf("%llu nanos (full int create and addition cycle)\n", total / runs);
    total = 0;
#endif

    int objsToMake = 100000;
    Ft_Arr vs = FtArr_Init(sizeof(Ft_IntObj), objsToMake);
    for (int i = 0; i < objsToMake; i++) {
        Ft_Obj* v = FtIntObj_Init(10);
        FtArr_Append(&vs, v);
    }
    for (int i = 0; i < vs.len; i++) {
        if (rand() % 10 == 1) {
            Ft_Obj* o = FtArr_Get(&vs, i);
            FtObj_DECREF(o);
        }
    }
    for (int i = 0; i < runs; i++) {
#ifdef __MACH__
        uint64_t clockBefore = mach_absolute_time();
#endif
        Ft_Obj *toAdd = FtIntObj_Init(10);
        Ft_StaticMsg plusMsg = FtStaticMsg_Init(addMsg, toAdd, NULL);
        Ft_Obj *result = FtObj_SendMsg(interp, testInt, FtStaticMsg_CastMsg(&plusMsg));
        FtObj_DECREF(toAdd);
        FtObj_DECREF(result);

#ifdef __MACH__
        uint64_t clockAfter = mach_absolute_time();
        uint64_t nanoBefore = (clockBefore - initclock) * timebaseRatio;
        uint64_t nanoAfter = (clockAfter - initclock) * timebaseRatio;
        total += (nanoAfter - nanoBefore);
#endif
    }

#ifdef __MACH__
    printf("%llu nanos (full int create and addition cycle w/ garbage)\n", total / runs);
#endif

    return 0;
}