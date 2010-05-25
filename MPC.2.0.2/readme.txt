"The first part is the compiler itself. I used Visual Studio 6 (you can load 
the project file in newer versions of Visual Studio). The compiler can be 
built as a standalone EXE or a static library used by the IDE. Originally, 
I built compiler as a separate EXE, then I switched to using static library 
(to increase the compilation speed). So, to build MIDletPascal, first you 
need to built the static library, then copy the library .LIB file to 
the IDE directory, and then build IDE project (and link it with the static 
compiler library).

There are few subdirectories:
classgen\ - includes the sources that generate CLASS file
lex\ - definitions for flex tool which is used to generate the
lexical analyzer (tokenizer) for the compiler
main\ - main program
parser\ - the recursive descent pareser
preverifier\ - preverifes CLASS file; this is taken from Sun code that I was able to download for free; I am not sure if the licensing for this has changed
StaticLibrary\ - here is the compiled output .LIB file placed
structures\ - structures used by the compiler are defined here
util\ - additional utility files"
- original source code release email by Niksa Orlic (September 2009)
