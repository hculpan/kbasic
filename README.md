# kbasic
An old-school intepreter, largely compliant with TRS-80 basic.  It was written for fun,
but also to run a specific game, Wall Street &amp; Corporate Control.

This project is being developed in C++ using SDL2 for the interface.  The build system
being employed is CMake to generate a Makefile.  This project also uses the SDL2_ttf
library.  I am building using a C++ 17 compatible compiler, though I don't believe I am
using any features of this standard.  I suspect (though have not confirmed) that this
project could be built using a much earlier C++ standard.

I have tried to make both the source code and build as platform-neutral as I can.
However, I've had to add a number of customizations to the CMakeLists.txt for the 
mac OS platform.  I have not tried to build this under any other platform (e.g.,
Windows or Linux), so I do not know what changes to the build file would be
required.  My hope, though, is that no C++ changes will be required, as I use
not platform-specific features in my code.

See the Manual.md for information on the interpreter and KBasic syntax.

# To Build
1) Execute cmake to get a Makefile
2) Build using make

If on Mac, this should produce kbasic.app in the root directory of the project.