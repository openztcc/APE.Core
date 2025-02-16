# APE.Core Parser

This is an image parsing library for Zoo Tycoon 1 graphics. It's designed to be general enough to be used in any project as long as supported bindings are available. Otherwise it should work out-of-the-box in a given C++ project.

## Bindings Available

Experimental .NET bindings are available and can be found in the dotnet folder.

### How to Compile .NET Bindings

Clone this repository and cd into the dotnet folder. Then run the following command:

`g++ -shared -m64 -o ApeCoreNet.dll ApeCore.NET.cpp "-I../../ -Wl,--out-implib,bindings/libApeCoreNet.a"`

ApeCoreNet.dll should be generated in the dotnet folder and the bindings can be used in any .NET project. Check out the source file for available functions.