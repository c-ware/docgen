# test_project.md

### NAME
test_project - this is a test project

### SYNOPSIS
These are the arguments to the program.

```c
/* this is a constant */
#define MY_CONSTANT 0

/* this is a constant */
#define MY_OTHER_CONSTANT 0
```

```c
/* this is a structure */
struct MyStructure {
    int foo;                 /* the foo field */
    struct Foobar *bar;      /* the bar field */
    struct {
        int baz;             /* the baz field */
        struct Foobar *tuna; /* the tuna field */
    } MySubStructure;
};

/* this is a structure */
struct MyStructure {
    int foo;                 /* the foo field */
    struct Foobar *bar;      /* the bar field */
    struct {
        int baz;             /* the baz field */
        struct Foobar *tuna; /* the tuna field */
    } MySubStructure;
};
```

```c
struct pollfd test_function(const char *foo, unsigned int bar);
struct pollfd test_function(const char *foo, unsigned int bar);
```

```c
#define test_macro(foo);
```

### DESCRIPTION
This is a description.
</br>
This is another line
</br>
This is also another line.
</br>
| a | b |
| - | - |
| foo | bar |
| baz | spam |

### SEE ALSO
foo(bar), baz(tuna)
