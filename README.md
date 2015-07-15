# ltest

This is my attempt at writing a roguelike, inspired by (and borrowing some ideas and code from) [Liberal Crime Squad](http://sourceforge.net/p/lcsgame) and the gameplay of, well, more traditional roguelikes, such as NetHack.

### Compiling

Right now, compiling **ltest** only takes two commands:

    $ cmake .
    $ make

As a result, the executable file **ltest** will appear in the directory. Right now, it doesn't depend on any data files (and I hope to keep it that way), so it can be launched from (or copied into) any directory.

### Controls

This is *very, very far* from a complete game. Right now, it is just a small demo that generates a map, puts enemies in random spaces and makes them spin around until they see you.

The movement controls are taken from NetHack (HJKL = ←↓↑→, yubn = NW/NE/SW/SE), doors open automatically or on pressing O(open), C closes doors and F allows you to attack a nearby enemy (only if they're not alerted in any way).

To exit, hit the standard Ctrl+C combination.

### Libraries

This application uses the following libraries:

* [ncursesw](https://www.gnu.org/software/ncurses/) for a console UI. \[X11\]

