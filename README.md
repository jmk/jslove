## JSLove ##

JSLove is a port of the [LÖVE](https://love2d.org) 2D game engine which supports JavaScript (instead of Lua). It's still a native binary that runs on the desktop.

Combining LÖVE with JavaScript also sets the stage for some interesting possibilities in the future: 

* A more direct path for deploying LÖVE games on the web. Currently, the [LÖVE WebPlayer project](https://github.com/ghoulsblade/love-webplayer) must convert Lua game code into JavaScript.

* Support for other languages that were actually intended to target JavaScript, from [Dart](http://www.dartlang.org/) to [CoffeeScript](http://coffeescript.org/).

### Status ###

This project is a very early proof-of-concept; although it can run a couple of very simple examples, many parts of the LÖVE API have not been exposed yet. OS X is the only platform supported at this time. 

### Building from source ###

JSLove has the same external dependencies as LÖVE. To build from source, you must install:

* [SDL](http://www.libsdl.org/)
* [PhysicsFS](http://icculus.org/physfs/)
* [FreeType](http://www.freetype.org/)
* [DevIL](http://openil.sourceforge.net/)

The included Xcode project assumes that these libraries have been installed via the [Homebrew package manager](http://mxcl.github.com/homebrew/) (i.e., in `/usr/local`).

