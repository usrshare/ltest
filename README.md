# ltest

This is my attempt at writing a roguelike, inspired by (and borrowing some ideas and code from) [Liberal Crime Squad](http://sourceforge.net/p/lcsgame) and the gameplay of, well, more traditional roguelikes, such as NetHack.

### Compiling

First, you have to compile the **utf8proc** library. First, it must be initialized:

    $ git submodules init
    $ git submodules update

After that, you can compile **utf8proc**.

    $ cd utf8proc
    $ make

After doing so, **ltest** will be able to statically link **utf8proc**. Right now, compiling **ltest** only takes two commands:

    $ cmake .
    $ make

As a result, the executable file **ltest** will appear in the directory. Right now, it doesn't depend on any data files (and I hope to keep it that way), so it can be launched from (or copied into) any directory.

