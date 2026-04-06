**Gasoline Metronome aims to become a desktop metronome for Windows, MacOS, Linux and the web (WebAssembly).**

I started developing it mainly for learning purposes.
Also, I failed to find any desktop metronome that has the features I would like & actually works.

## Building

I'm currently developing on Windows, so that's the only build script for the moment.
It does what is called a "unity build", which means everything is rebuilt from scratch every time.

If you have Microsoft Visual Studio 2022 with C/C++ developer toolchain installed,
you should be able to just run [win\_debug\_build.bat](win_debug_build.bat) & find
the executable in build/debug/windows.

It is also recommended to have [ripgrep](https://github.com/BurntSushi/ripgrep) installed - it is used in the script
to search for the @NO\_COMMIT tags in the comments.
These tags are just a reminder to not commit some change I may forget about
while working on something else (usually some loose printf).

I have not tried using the Visual Studio debugger, so I do not know if it works by default.
I highly recommend using Epic's [RadDebugger](https://github.com/EpicGamesExt/raddebugger), though.

## Style

+ Use tabs for indentation, spaces for alignment.

+ { always on newlines, except for single line if statements.

+ snake\_case. MY\_MACRO. My\_Struct.

+ C99, but [no VLAs](https://www.dependablec.org/#VLAs%20are%20broken).

+ Otherwise just look around the codebase.
