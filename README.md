# docgen

## This is the "legacy" version of Docgen, suitable for older-style Docgen input.

Docgen is a utility program for generating documentation from the source
code of a program. While docgen is mainly intended to produce manual pages (its
UNIX heritage can easily be seen in pretty much all documentation produced, as
the structure will mirror that of UNIX manuals), it can also produce documentation
in arbitrary formats as long as a backend exists for it.

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

Each of these things have different 'tags' that are used to describe a certain
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
 *
 * @return: the factorial of x
 * @type: unsigned long
*/
unsigned long factorial(int x);
```

When you are ready to actually generate documentation for your project,
go to the root directory of your project, create a `doc/` directory, and
then run docgen. To generate documentation for all functions for example,
you can do the following:
```sh
docgen functions path/to/file
```

The first argument to docgen is the `category` to generate documentation for.
It tells docgen what kind of comment with what kind of header to look for. All
the categories can be found by running `docgen --help`, but without having to
compile docgen yourself, it supports:
  - Functions
  - Program manuals
  - Macro functions
As for the file, it is quite literally just the path to the file that contains
the documentation you want to generate.

What we mean by 'document' here is that you can generate individual manuals or
other forms of documentation with the purpose of documenting that kind of token.
However, the list at the start of this README also included `constants` and
`structures`. While these on their own cannot have individual manual pages, they
can be *embedded* into the above forms of documentation.

Embedding these types of objects in a manual basically means that they are put
into the synopsis of the manual. Things other than constants and structures can
be embedded as well. A full list would be:
  - Functions
  - Macro functions
  - Constants
  - Structures

If you wish to know *how* to embed them, please see the manuals for further
information.
