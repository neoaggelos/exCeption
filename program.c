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

/*
 * program.c: Test program for exCeption
 */

#include "exception.h"
#include <stdio.h>
#include <stdlib.h>

void function(int i)
{
  if (i < 0) {
    throw(new_exception("BAD_PARAMETER", "negative values are not allowed", (void*)0));
  }
}

int main(int argc, char** argv)
{
  /*
   * Normal execution, no exception is thrown, so the second printf is never
   * reached
   */
  try {
    printf("Normal execution\n");
  } catch(struct exception_t*, e) {
    printf("No exceptions are thrown, this should never be executed\n");
    endcatch;
  }
  
  /* 
   * Throw an integer as an exception
   */
  try {
    throw(5);
  } catch(int, e) {
    printf("Exception caught (code %d)\n", e);
    endcatch;
  }

  /*
   * Throw a string as an exception
   */
  try {
    throw("Unknown error");
  } catch(const char*, e) {
    printf("Exception caught: %s\n", e);
    endcatch;
  }

  /*
   * Throw an exception of the built-in 'struct exception_t' type
   */
  try {
    throw(new_exception("BAD_CAST", "a type cast failed", (void*)0));
  } catch(struct exception_t*, e) {
    printf("Exception caught: '%s' Reason: %s. Userdata: %p\n",
        e->name, e->reason, e->userdata);
    endcatch;
  }

  /*
   * Same as before, but we throw the exception using raise_exception()
   */
  try {
    raise_exception("BAD_CAST", "a type cast failed", (void*)0);
  } catch(struct exception_t*, e) {
    printf("Exception caught: '%s' Reason: %s. Userdata: %p\n",
        e->name, e->reason, e->userdata);
    endcatch;
  }

  /*
   * Throw an exception from a function, and print it out using
   * print_exception().
   */
  try {
    function(-1);
  } catch(struct exception_t*, e) {
    print_exception(e);
    endcatch;
  }

  return 0;
}
