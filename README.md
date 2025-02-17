# APE.Core Parser

This is an image parsing library for Zoo Tycoon 1 graphics. It's designed to be general enough to be used in any project as long as supported bindings are available. Otherwise it should work out-of-the-box in a given C++ project.

## Usage

For most use cases, you can simply include the ApeCore.h header file in your project and link against the ApeCore library. The library is header-only and does not require any additional linking. 

The following functions are available:

| Function | Description |
| --- | --- |
| `int ApeCore::loadImage(const char* path, int colorModel, const char* palettePath)` | Load an image from a file |
| path: path to the image file |
| colorModel: 0 for RGBA or 1 for BGRA |
| palettePath: path to the palette file |
| returns 0 on success, -1 on failure |
| `int ApeCore::getFrameCount(std::string path, OutputBuffer* buffer)` | Get the number of frames in the image |
| path: path to the image file |
| buffer: pixel buffer that contains frame data |
| `ApeCore::exportToPNG(const char* path, int frame)` | Save a frame to a PNG file |
| `std::string ApeCore::getPalLocation()` | Get the location of the palette file |
| `OutputBuffer** ApeCore::apeBuffer()` | Get all the frame buffers in the image as an array of OutputBuffer pointers |

See `main.cpp` in examples/ for an example of how to use the library.

### Compiling

Because the library makes use of `stb_image_write.h`, you will need the includes folder in your project directory. A successful compilation would look something like this:

```bash
g++ -o my_program my_program.cpp ApeCore.h ApePng.cpp -o my_program
```

## Bindings Available

### Compile Extern C

In order for the libary to work in other languages, we need to compile the library as an extern C library. 

#### Windows

It makes use of a few MinGW libraries, so you might need the following dependencies to link against:

- libstdc++-6.dll
- libgcc_s_seh-1.dll
- libwinpthread-1.dll

Compile:

```bash
g++ -shared -o pyape.dll pyape.cpp ../../ApePng.cpp "-Wl,--out-implib,libpyape.a" -static-libgcc -static-libstdc++ -static -lpthread
```

#### Linux

The libraries should be available by default on most Linux distributions. 

Compile:

```bash
g++ -shared -o pyape.so pyape.cpp ../../stb_image_write_impl.cpp -lpthread
```

### Python

Experimental Python bindings are available.

All you need are the compiled shared libraries and the pyape.py module available under bindings. Keeping the directory structure as is, you can import the module and use the functions. 

Check out the APE.KritaTools project for a working example of the Python bindings.

https://github.com/openztcc/APE.KritaTools