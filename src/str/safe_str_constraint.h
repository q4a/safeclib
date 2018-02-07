/*------------------------------------------------------------------
 * safe_str_constraint.h
 *
 * October 2008, Bo Berry
 *
 * Copyright (c) 2008-2011 Cisco Systems
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *------------------------------------------------------------------
 */

#ifndef __SAFE_STR_CONSTRAINT_H__
#define __SAFE_STR_CONSTRAINT_H__

#ifdef FOR_DOXYGEN
#include "safe_str_lib.h"
#else
#include "safeclib_private.h"
#endif

/* Without a handler we can skip the err-msg string processing */
EXTERN bool has_safe_str_constraint_handler;
/*
 * Function used by the libraries to invoke the registered
 * runtime-constraint handler. Always needed.
 */
EXTERN void invoke_safe_str_constraint_handler(
                           const char *msg,
                           void *ptr,
                           errno_t error);

/* has_safe_str_constraint_handler check mandatory on DEBUG */
#ifndef DEBUG
# define HAS_SAFE_STR_CONSTRAINT_HANDLER has_safe_str_constraint_handler
#else
# define HAS_SAFE_STR_CONSTRAINT_HANDLER 1
#endif
/*
 * Safe C Lib internal string routine to consolidate error handling.
 * With SAFECLIB_STR_NULL_SLACK clear the dest buffer to eliminate
 * partial copy.
 */
static inline void
handle_error(char *dest, rsize_t dmax,
             const char *err_msg, errno_t err_code)
{
#ifdef SAFECLIB_STR_NULL_SLACK
    /* null string to eliminate partial copy */
    if (dmax > 0x20)
        memset(dest, 0, dmax);
    else {
        while (dmax) { *dest = '\0'; dmax--; dest++; }
    }
#else
    (void)dmax;
    *dest = '\0';
#endif

    invoke_safe_str_constraint_handler(err_msg, dest, err_code);
    return;
}

#ifndef SAFECLIB_DISABLE_WCHAR
/*
 * Safe C Lib internal string routine to consolidate error handling.
 * With SAFECLIB_STR_NULL_SLACK clear the dest wide buffer to eliminate
 * partial copy.
 */
static inline void
handle_werror(wchar_t *dest, rsize_t dmax,
              const char *err_msg, errno_t err_code)
{
#ifdef SAFECLIB_STR_NULL_SLACK
    /* null string to eliminate partial copy */
    if (dmax > 0x20)
        memset(dest, 0, dmax*sizeof(wchar_t));
    else
        while (dmax) { *dest = L'\0'; dmax--; dest++; }
#else
    (void)dmax;
    *dest = '\0';
#endif

    invoke_safe_str_constraint_handler(err_msg, dest, err_code);
    return;
}
#endif

#endif   /* __SAFE_STR_CONSTRAINT_H__ */
