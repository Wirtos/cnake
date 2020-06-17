# Snake Curses

![Snake Curses](screenshot.png)

This is a very basic Snake game in C with Ncurses.

Command line arguments:
```
Usage: snake [-t | -H HEIGHT -W WIDTH] [-h]

Snake Curses game

Options:
	-t, --use-terminal-dimensions   Map dimensions following terminal size
	-H, --height <height>           Set height of the map
	-W, --width <width>             Set width of the map
	-h, --help                      Display this help
```

### Dependencies
- Make
- pkg-config
- GCC/Clang
- Ncurses

### Build
Build with `make`. That will leave you the `snake` executable.

### Installation
You can directly run the compiled executable, but if you want to install it
on your system, `make install` will do it. Notice that you can set the
variable `PREFIX` (default to /usr/local) to your desire.

### Uninstall
`make uninstall` (Mind the PREFIX too)