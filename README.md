# STG Example

STG (Spineless Tagless G-machine) is implementation of lazy functional language as described in:

> Simon Peyton Jones. Implementing Lazy Functional Languages on Stock Hardware: The Spineless Tagless G-machine. Journal of Functional Programming 1992

[haskell.hs](./haskell.hs) is example program written in Haskell (except there is no monad IO, i am sure you can figure it out).

[stg.hs](./stg.hs) is the program translated to STG as explained in book (`.hs` extension is for better editing).

[stgi.hs](./stgi.hs) is the program, but in format, supported by <https://github.com/quchen/stgi>. The STG machine explained there too in details, so check it out!

[stg.c](./stg.c) is the program, translated (compiled) from STG to C as explained in book.

To run:

```sh
gcc stg.c && ./a.out
```

You should get the following results in the end:

```text
...
Result = 91
Steps = 281
Stack A size = 0
Stack B size = 0
Heap size = 1008
Objects allocated = 47
```

Tested on Linux (gcc version 14.2.1)
