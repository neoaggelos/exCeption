================================================================================
What is exCeption
================================================================================

exCeption is a C library that provides basic C++ exception handling in C. To
learn more about what exceptions are and how/why they are so widely used in
modern applications, see http://www.functionx.com/cpp/Lesson18.htm.

exCeption consists of a single header file, and in order to use it all you have
to do is include it in your code:

    #include "exception.h"


================================================================================
Features
================================================================================

+ You can throw any type of exception.
+ There is a built-in type for handling exceptions 'struct exception_t'.
+ Uses the well-known C++ syntax.
+ Written in ANSI C, works everywhere.

- No nested try-catch blocks allowed.
- No multiple catch blocks allowed for different types of exceptions.


================================================================================
Exception handling with exCeption
================================================================================

exCeption is a C library, and therefore should not be used in C++ code. This is
because, in order to be as close to the C++ syntax, it uses the keywords: 'try',
'throw' and 'catch', which in ANSI C are valid names for macros.

Extension handling in C++ looks like this:

    try {
        ....
    } catch(exception_type e) {
        ....
    }

With exCeption, you would write this:

    try {
        ....
    } catch(exception_type, e) {
        ....
        endcatch;
    }


-- Aggelos Kolaitis <neoaggelos@gmail.com>
