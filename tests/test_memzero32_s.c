/*------------------------------------------------------------------
 * test_memzero32_s
 * File 'extmem/memzero32_s.c'
 * Lines executed:100.00% of 12
 *
 *
 *=
 *------------------------------------------------------------------
 */

#include "test_private.h"
#include "safe_mem_lib.h"

#define LEN   ( 512 )

int main()
{
    errno_t rc;
    uint32_t i;
    uint32_t len;

    uint32_t mem1[LEN];
    int errs = 0;

/*--------------------------------------------------*/

    rc = memzero32_s(NULL, LEN);
    ERR(ESNULLP)
/*--------------------------------------------------*/

    rc = memzero32_s(mem1, 0);
    ERR(ESZEROL)
/*--------------------------------------------------*/

    rc = memzero32_s(mem1, RSIZE_MAX_MEM32+1);
    ERR(ESLEMAX)
/*--------------------------------------------------*/

    for (i=0; i<LEN; i++) { mem1[i] = 33; }

    len = 1;
    rc = memzero32_s(mem1, len);
    ERR(EOK)
    /* verify mem1 was zeroed */
    for (i=0; i<len; i++) {
        if (mem1[i] != 0) {
            debug_printf("%d - %d m1=%d \n",
                 __LINE__, i, mem1[i]);
            errs++;
        }
    }

/*--------------------------------------------------*/

    for (i=0; i<LEN; i++) { mem1[i] = 33; }

    len = 2;
    rc = memzero32_s(mem1, len);
    ERR(EOK)
    /* verify mem1 was zeroed */
    for (i=0; i<len; i++) {
        if (mem1[i] != 0) {
            debug_printf("%d - %d m1=%d \n",
                 __LINE__, i, mem1[i]);
            errs++;
        }
    }

/*--------------------------------------------------*/

    for (i=0; i<LEN; i++) { mem1[i] = 33; }

    len = 3;
    rc = memzero32_s(mem1, len);
    ERR(EOK)
    /* verify mem1 was zeroed */
    for (i=0; i<len; i++) {
        if (mem1[i] != 0) {
            debug_printf("%d - %d m1=%d \n",
                 __LINE__, i, mem1[i]);
            errs++;
        }
    }

/*--------------------------------------------------*/

    for (i=0; i<LEN; i++) { mem1[i] = 33; }

    len = LEN;
    rc = memzero32_s(mem1, len);
    ERR(EOK)
    /* verify mem1 was zeroed */
    for (i=0; i<len; i++) {
        if (mem1[i] != 0) {
            debug_printf("%d - %d m1=%d \n",
                 __LINE__, i, mem1[i] );
            errs++;
        }
    }

/*--------------------------------------------------*/

    return (errs);
}