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
 *  } endcatch;
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
 *  + Allows nested try-catch blocks
 *  + Uses well-known C++ syntax (try { } catch() { } blocks)
 *  + Allows throwing any type of exception. The type of exception caught
 *    must be declared in catch [e.g. catch(int, e) or catch(char*, e)]
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

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

/**
 * Abstract type for exception handling
 */
struct exception_t {
  const char* name;   /* exception name               */
  const char* reason; /* exception reason             */
  void *userdata;     /* generic pointer for userdata */
};

/**
 * - raise_exception("EXCEPTION NAME", "REASON", userdata);
 *
 * Raise an exception of type 'struct exception_t*' with name, reason and
 * userdata
 *
 * \param _name      --> exception name                (const char*)
 * \param _reason    --> reason why exception occured  (const char*)
 * \param _userdata  --> additional userdata           (void*)
 */
#define raise_exception(_name, _reason, _userdata)                                    \
  do {                                                                                \
    struct exception_t* e = (struct exception_t*)malloc(sizeof(struct exception_t));  \
    e->name = _name;                                                                  \
    e->reason = _reason;                                                              \
    e->userdata = _userdata;                                                          \
    throw(e);                                                                         \
  } while(0);

/**
 * - print_exception(exception)
 *
 * Print out the exception name and reason
 *
 * \param e --> an exception of type 'struct exception_t*', thrown by raise_exception()
 */
#define print_exception(e) \
  printf("EXCEPTION CAUGHT: \n NAME: \"%s\"\n REASON: \"%s\"\n USERDATA: \"%p\"\n", e->name, e->reason, e->userdata);

/*******************************************************************************
 *                                                                             *
 * NOTHING IMPORTANT BELOW THIS, UNLESS YOU ARE A DEVELOPER OR WANT TO HAVE A  *
 * LOOK AT UGLY PREPROCESSOR CODE.                                             *
 *                                                                             *
 * THE TYPICAL USER OF THE LIBRARY SHOULD NOT HAVE TO DO ANYTHING WITH THESE.  *
 *                                                                             *
 * C DEVELOPERS, HOWEVER, MIGHT FIND THE PART BELOW QUITE INTERESTING.         *
 *                                                                             *
 *******************************************************************************/

/**
 *  The _exception_t_node struct.
 *  This struct contains information that are required for the try-catch macro
 *  mechanism below to work correctly. It stores the jmp_buf that is used to
 *  move into the catch block, as well as the thrown exception.
 *
 *  It is implemented as a linked-list to allow nested try-catch blocks.
 *  Entering a try block pushes a new _exception_t_node at the head of a list
 *  that is maintained internally (namely, _exc_head). Its information is then
 *  used by throw(), which stores the thrown exception in '_exc_head->exception'
 *  and then performs a long jump to '_exc_head->jmpbuf', which in turn leads
 *  the control flow in the catch block.
 *
 *  The 'catch' macro, from its side, uses _exc_head->exception to initialize
 *  its argument. Note that 'catch' does not free the no longer important
 *  '_exc_head'. It is retained until we use 'endcatch;' This is because
 *  internally, 'catch' itself is only reached when throw() is used in the try
 *  block. But what if no exception is thrown? Then, catch would not be reached,
 *  thus '_exc_head' would remain there, and because of that, either a memory
 *  leak would happen, or much worse things like false long jumps (you really
 *  don't want that).
 *
 *  Each macro below contains more specific information about what it actually
 *  does.
 *
 *  NOTE: '_exc_head' is a pointer to the head of the internal list of exception
 *  nodes. '_exc_node' is used as a temporary pointer for things like pushing a
 *  new node in _exc_head, popping _exc_head from the list, etc. If you have the
 *  slightest experience in implementing linked-lists, you can easily understand
 *  its usage.
 */
struct _exception_t_node {
  jmp_buf jmpbuf;
  void *exception;

  struct _exception_t_node *next;
};
static struct _exception_t_node* _exc_head = (void*)0;
static struct _exception_t_node* _exc_node;

/* Macro magic.
 * This translates the try-catch syntax into the following:
 *
 * if (!setjmp(jmpbuf)) {
 *    do something
 * } else {
 *    catch exception
 * }
 *
 * throw() just does a long jump to the saved jmpbuf with return value 1, so
 * that control goes into the catch block
 */

/**
 * Entering a new try block.
 * Create a new _exception_t_node, which will hold the jmp_buf that will be used
 * in case we throw() an exception, plus a generic pointer for the thrown
 * exception itself.
 */
#define try \
  _exc_node = (void*)0;                                                             \
  _exc_node = (struct _exception_t_node*)malloc(sizeof(struct _exception_t_node));  \
  if (_exc_head) {                                                                  \
    _exc_node->next = _exc_head;                                                    \
  } else {                                                                          \
    _exc_node->next = (void*)0;                                                     \
  }                                                                                 \
  _exc_head = _exc_node;                                                            \
  if (!setjmp(_exc_head->jmpbuf))

/**
 * Thrown an exception
 * If we are in a try block (that is, when '_exc_head' is valid, save the thrown
 * exception in _exc_head and long jump into the appropriate jmpbuf, which is
 * initialized in 'try'
 */
#define throw(e) \
  if (_exc_head) {                            \
    _exc_head->exception = (void*)e;          \
    longjmp(_exc_head->jmpbuf, 1);            \
  } else {                                    \
    printf("* Uncaught exception occured\n"); \
  }

/* Catch exception
 * The 'else' is there to complete the 'if (!setjmp(jmpbuf))' that we did
 * in 'try'. Also declare 'var' of type 'type' to store the thrown exception.
 * Don't worry about the missing '}', this is handled in 'endcatch'.
 */
#define catch(type, var) \
  else { type var = (type)_exc_head->exception;

/* Finish a catch block
 * The '}' at the beginning closes the '{' bracket from 'catch'. Furthermore,
 * we pop _exc_head from the list of exceptions. If _exc_head->next is valid,
 * then we were in a nested try-catch block. Make _exc_head point to it, so
 * that the top-level catch will work. Otherwise, set _exc_head to 0
 */
#define endcatch \
  }                               \
  _exc_node = _exc_head;          \
  if (_exc_head->next) {          \
    _exc_head = _exc_head->next;  \
  } else {                        \
    _exc_head = (void*) 0;        \
  }                               \
  free(_exc_node);                \
  _exc_node = (void*)0;

#endif /* _EXCEPTION_H */
