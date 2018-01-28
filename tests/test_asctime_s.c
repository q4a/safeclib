/*------------------------------------------------------------------
 * test_asctime_s
 * File 'os/asctime_s.c'
 * Lines executed:87.93% of 58
 *
 *------------------------------------------------------------------
 */

#include "test_private.h"
#include "safe_lib.h"

#ifdef HAVE_ASCTIME_S
# define HAVE_NATIVE 1
#else
# define HAVE_NATIVE 0
#endif
#include "test_msvcrt.h"

#define LEN   ( 128 )

static char   str1[LEN];

int test_asctime_s (void)
{
    errno_t rc;
    int errs = 0;
    int old;
    int ind;
    int len;
    struct tm *tm;
    const time_t timet = 20000000;
    char *str2;

    tm = gmtime(&timet);

/*--------------------------------------------------*/
    /* even static we might use the native forceinline asctime_s */
#if defined(_WIN32) && (HAVE_NATIVE)
    use_msvcrt = true;
#endif
    if (use_msvcrt)
        printf("Using msvcrt...\n");

    rc = asctime_s(NULL, 0, tm);
    if ( use_msvcrt && rc == ESNULLP ) {
        printf("safec.dll overriding msvcrt.dll\n");
        use_msvcrt = false;
    }
    ERR_MSVC(ESNULLP,EINVAL);

    rc = asctime_s(str1, LEN, NULL);
    ERR_MSVC(ESNULLP,EINVAL);

/*--------------------------------------------------*/

    rc = asctime_s(str1, 25, tm);
    ERR_MSVC(ESLEMIN,EINVAL);

    rc = asctime_s(str1, RSIZE_MAX_STR+1, tm);
    ERR_MSVC(ESLEMAX,0);

/*--------------------------------------------------*/

#define TM_RANGE(memb, mmin, mmax)      \
    old = tm->tm_##memb;                \
    tm->tm_##memb = mmin-1;             \
    rc = asctime_s(str1, LEN, tm);      \
    ERR_MSVC(ESLEMIN, EINVAL);          \
                                        \
    tm->tm_##memb = mmax+1;             \
    rc = asctime_s(str1, LEN, tm);      \
    ERR_MSVC(ESLEMAX, EINVAL);          \
                                        \
    tm->tm_##memb = mmin;               \
    rc = asctime_s(str1, LEN, tm);      \
    ERR(EOK);                           \
    tm->tm_##memb = mmax;               \
    rc = asctime_s(str1, LEN, tm);      \
    tm->tm_##memb = old;                \
    ERR(EOK)


#ifndef __MINGW32__
    TM_RANGE(sec,  0, 60);
#endif
    TM_RANGE(min,  0, 59);
    TM_RANGE(hour, 0, 23);
    TM_RANGE(mday, 1, 31);
    TM_RANGE(mon,  0, 11);
    if (!use_msvcrt) {
    TM_RANGE(year, 0, 8099);
    TM_RANGE(wday, 0, 6);
    TM_RANGE(yday, 0, 365);
    TM_RANGE(isdst,0, 1);
    }

    /* stack buffer branch */
    tm = gmtime(&timet);
    tm->tm_year = 0;
    rc = asctime_s(str1, 40, tm);
    ERR(EOK);
    len = strlen(str1);
    CHECK_SLACK(&str1[len], 40-len);

    /* darwin asctime_r overflows >= 8100 */
    for (ind=8090; ind<=8200; ind++) {
        tm->tm_year = ind;
        rc = asctime_s(str1, 40, tm);
        if (rc == -1) {
            ERR(EOK);
            debug_printf("year %d => %d ", ind, rc);
        }
    }

/*--------------------------------------------------*/

    tm = gmtime(&timet);
    rc = asctime_s(str1, 40, tm);
    str2 = asctime(tm);
    EXPSTR(str1, str2);
    len = strlen(str1);
    CHECK_SLACK(&str1[len], 40-len);

/*--------------------------------------------------*/

    return (errs);
}

#ifndef __KERNEL__
/* simple hack to get this to work for both userspace and Linux kernel,
   until a better solution can be created. */
int main (void)
{
    return (test_asctime_s());
}
#endif