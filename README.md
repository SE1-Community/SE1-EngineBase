# Serious Engine 1 - Engine Base

This is a part of the customly refactored fork of Croteam's [Serious Engine v1.10](https://github.com/Croteam-official/Serious-Engine) source code.

The main game shell and other engine modules can be found at https://github.com/DreamyCecil/SE1-GameShell

Includes the following projects:

* `DecodeReport` Used to decode crash *.rpt files
* `Depend` Used to build a list of dependency files based on a list of root files
* `Ecc` The *Entity Class Compiler*, a custom build tool used to compile *.es files
* `Engine` Serious Engine 1.10
* `Shaders` Compiled shaders
* `libogg`, `libvorbis` Third party libraries used for playing OGG-encoded ingame music (see http://www.vorbis.com/ for more information)

License
-------

Serious Engine is licensed under the GNU GPL v2 (see LICENSE file).

Some of the code included with the engine sources is not licensed under the GNU GPL v2:

* zlib (located in `Engine/zlib`) by Jean-loup Gailly and Mark Adler
* libogg/libvorbis (located in `libogg` and `libvorbis`) by Xiph.Org Foundation
