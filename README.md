# docgen

Docgen is a utility program for generating documentation from the source
code of a program. While docgen is mainly intended to produce manual pages (it
is very easy to see the UNIX heritage in the some parts of the internal
structure of docgen), it can also produce documentation in arbitrary formats
as long as a backend exists for it.

Docgen is written in highly portable ANSI C, and is known to compile on the
following platforms:
</br>
- BSDs
  - FreeBSD
    - FreeBSD 13.0
    - FreeBSD 3.0
  - NetBSD 9.2
  - OpenBSD 7.0
  - BSDi 5.1
- Linux
  - Arch Linux 06-2022
  - Debian 11.2.0
  - Linux Mint 20.3
  - Red Hat Linux 7.1
- Other UNIXes
  - Solaris
    - Solaris 9
    - Solaris 10
  - UnixWare
    - UnixWare 7.1.1
    - UnixWare 7.1.4
  - AIX 5.1
  - NeXTSTEP 4.0
  - OSF/1 5.1
  - IRIX
    - IRIX 5.3
    - IRIX 6.5
  - GNU/Hurd 0.9
- Windows, DOS, and related
  - Windows NT 4.0
  - OS/2 3.0
- Mac OS
  - OS X 10.6
  - OS X 11.5
- Misc. Operating systems
  - VM/ESA 2.4 (OpenVM)
  - MVS/ESA 6.0.2 (OpenMVS)
  - OpenVMS 7.3

Docgen is quite simple to use. The way that docgen knows what to document is
through the use of source code comments. Through these, docgen can document all
sorts of things in C source code, including:
  - Functions
  - Projects (like program manuals)
  - Structures
  - Constants
  - Macro functions

Each of these things has different 'tags' that are used to describe a certain
thing about them. For example, functions have the '@param' tag to describe a
parameter, and an '@type' tag to describe the type of said parameter. These
are composed in a single source code comment with a 'header' that specifies what
type of thing is being documented. For example, let us say we want to document a
function that produces the factorial of a certain integer, and it is in a file
called `math.h`.

```c
/*
 * @docgen: function
 * @brief: compute the factorial of an integer
 * @name: factorial
 *
 * @include: math.h
 *
 * @description
 * @Compute the factorial of an integer x.
 * @description
 *
 * @example
 * @#include <stdio.h>
 * @#include "math.h"
 * @
 * @int main(void) {
 * @    unsigned long number = 0;
 * @
 * @    number = factorial(5);
 * @    printf("The factorial of %i is %lu\n", 5, number);
 * @
 * @    return 0;
 * @}
 * @example
 *
 * @error: x is negative
 * 
 * @param x: the integer to compute the factorial of
 * @type: int
*/
unsigned long factorial(int x);
```










