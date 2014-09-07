/*
  exception: Basic C++-like exception handling for ANSI C
  Copyright (c) 2014 Aggelos Kolaitis <neoaggelos@gmail.com>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/**
 *  \file exception.h
 *
 *  \brief Basic C++-like exception handling for ANSI C
 *
 *  USAGE:
 *  ------
 *
 *  #include <exception.h>
 *
 *  try {
 *    code that might raise an exception using throw() or raise_exception()
 *  } catch(type, e) {
 *    do something with the caught exception
 *
 *    endcatch;
 *  }
 *
 *  In C++ you would write:
 *
 *  try {
 *    code that might raise an exception using throw
 *  } catch(type e) {
 *    do something with the caught exception
 *  }
 *
 *  FEATURES:
 *  ---------
 *  + It is written in ANSI C
 *  + Uses well-known C++ syntax (try { } catch() { } blocks)
 *  + Allows throwing any type of exception. The type of exception caught
 *    must be declared in catch [e.g. catch(int, e) or catch(char*, e)]
 *  - No nested try { } catch { } blocks supported yet
 *  - No multiple catch { } blocks for different types of exceptions allowed
 *  - You must add 'endcatch;' at the end of catch { } blocks
 * 
 * -- Aggelos Kolaitis <neoaggelos@gmail.com>
 */

#ifndef _EXCEPTION_H
#define _EXCEPTION_H

/* Protect from C++ code that includes this header */
#ifdef __cplusplus
#error "exception.h must not be included in C++ code"
#endif /* __cplusplus */

/* Header files */
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

/* Static variables -- needed for exception handling */
static jmp_buf _exc_buf;
static int _exc_can_throw = 0;
static void *_exc_what = (void*)0;

/* Macro magic */
#define try \
  _exc_can_throw=1; \
  if (!setjmp(_exc_buf))

#define throw(e) \
  if (_exc_can_throw) { \
    _exc_can_throw=0;   \
    _exc_what = (void*)e; \
    longjmp(_exc_buf, 1); \
  }

#define catch(type, var) \
  else { \
    type var = (type)_exc_what;

#define endcatch \
  }

/* Abstract type for exception handling */
struct exception_t {
  const char* name;
  const char* reason;
  void *userdata;
};

/* Create a new exception */
static struct exception_t *new_exception(const char*, const char*, void*);

/* Create a new exception and throw it */
static void raise_exception(const char*, const char*, void*);

/* Print out the exception name and reason */
static void print_exception(struct exception_t*);


/*** implementation section for exception functions ***/
static struct exception_t*
new_exception(const char* name, const char* reason, void* userdata)
{
  struct exception_t *e = malloc(sizeof(struct exception_t));

  e->name = name;
  e->reason = reason;
  e->userdata = userdata;

  return e;
}

static void
raise_exception(const char* name, const char* reason, void *userdata)
{
  throw(new_exception(name, reason, userdata));
}

static void
print_exception(struct exception_t *e)
{
  printf("**** Exception caught: '%s': %s\n", e->name, e->reason);
}
/*** end of implementation section ***/

#endif /* _EXCEPTION_H */
