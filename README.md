dcarr.h - Generic dynamic arrays in C using macros
==================================================

*Note:* This project has been superseded by *aadeque*.

https://github.com/zuiderkwast/aadeque

Historical notes
----------------

The array uses a circular buffer, thus it has amortized constant time
insertion/removal at both ends.  This implementation is known as an
array deque.

This implementation consists entirely of macros.  There are macros for
defining types and macros that take types as parameters.  In this way
it can be entirely generic and still plain C.  The drawback of this
design is that it's hard to debug this kind of code and that the
functions are always inlined.

*Aadeque*'s approach on the other hand is a combination of inline
functions and tweaking macros. This is more clear and maintainable and
more intuitive to use, while being almost as generic.

Usage
-----

Just copy dcarr.h into your project and include it.

Beeing generic means the values can be of any type.  The array type
has to be defined though, using dcarr_define_type, which expands to a
typedef struct.

Many of the macros require the elements' type as one of the parameters.
This may seem strange, but there is nothing strange about it in the
world of macros.


Old examples
------------

First define an array type using the dcarr_define_type.  This example
defines the type my_array_of_double_t to be an array of doubles:

``` C
  dcarr_define_type(array_of_double_t, double);
```

To create an array, declare a variable of your array type and
initialize it.

``` C
  array_of_double_t numbers;
  dcarr_init(numbers);
```

Then add some values using push, unshift and insert.

``` C
  dcarr_push(numbers, double, 3.1415926535);
  dcarr_unshift(numbers, double, 42.0);
  dcarr_insert(numbers, 1, double, 99.9);
```

Access random elements at a given index. Indexing starts at 0, like for
ordinary arrays.

``` C
  double d = dcarr_elem(numbers, 2);
```

It's also possible to assign to dcarr_elem.

``` C
  dcarr_elem(numbers, 2) = 123.45;
```

To remove and element and get the value, use pop and shift. (A macro
for removing at arbitrary positions is yet to be written.)

``` C
  dcarr_pop(numbers, double, value);
  dcarr_shift(numbers, double, value);
```

Print the contents of the array.

``` C
  int i;
  for (i=0; i < dcarr_len(numbers); i++)
      printf("Element at %d is %g.\n", i, dcarr_elem(i));
```

Finally, free all allocated memory.

``` C
  dcarr_destroy(numbers);
```

For more information, refer to `dcarr.h`. It is quite small.

Known bugs
----------

There is a bug in the code shrinking a memory allocation after enough
elements have deleted from an array.  The content gets corrupted.
