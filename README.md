# APE.Core Parser

This is an image parsing library for Zoo Tycoon 1 graphics. It's designed to be general enough to be used in any project as long as supported bindings are available. Otherwise it should work out-of-the-box in a given C++ project.

## Bindings Available

### .NET

Experimental .NET bindings are available and can be found in the dotnet folder.

To compile, clone this repository and cd into the dotnet folder. Then run the following command:

`g++ -shared -m64 -o ApeCoreNet.dll ApeCore.NET.cpp "-I../../ -Wl,--out-implib,bindings/libApeCoreNet.a"`

ApeCoreNet.dll should be generated in the dotnet folder and the bindings can be used in any .NET project. Check out the source file for available functions.

### Python

Python bindings are also available.

To compile, clone this repository and cd into the python folder. 

Gather the following dependencies and place them in the same folder as pyape.cpp:

- libstdc++-6.dll
- libgcc_s_seh-1.dll
- libwinpthread-1.dll

Then run the following command:

`g++ -shared -o pyape.dll pyape.cpp ../../stb_image_write_impl.cpp "-Wl,--out-implib,libpyape.a" -static-libgcc -static-libstdc++ -static -lpthread`

You can then import pyape.dll into any python project using ctypes.

Example:

```python
import ctypes

# Load the DLL
lib = ctypes.CDLL("./pyape.dll")

# --------------------------- Define the function signatures ---------------------------

# Define create function
lib.create_ape_instance.argtypes = []
lib.create_ape_instance.restype = ctypes.c_void_p  # ptr to ApeCore

# Define the destroy function
lib.destroy_ape_instance.argtypes = [ctypes.c_void_p]
lib.destroy_ape_instance.restype = None

# Define load_image function (char* argument needs proper conversion)
lib.load_image.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int, ctypes.c_char_p]
lib.load_image.restype = ctypes.c_int

# Define get_frame_buffer_size function
lib.get_frame_count.argtypes = [ctypes.c_void_p]
lib.get_frame_count.restype = ctypes.c_int

# Define frame_to_png function
lib.frame_to_png.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
lib.frame_to_png.restype = None

# Define OutputBuffer struct
class OutputBuffer(ctypes.Structure):
    _fields_ = [
        ("pixels", ctypes.POINTER(ctypes.c_uint8)),
        ("width", ctypes.c_int),
        ("height", ctypes.c_int),
        ("channels", ctypes.c_int)
    ]

# Define frame buffers
lib.get_frame_buffer.argtypes = [ctypes.c_void_p]
lib.get_frame_buffer.restype = ctypes.POINTER(ctypes.c_ubyte)

# --------------------------- Use the functions ---------------------------

# Create instance
ape = lib.create_ape_instance()
if not ape:
    raise RuntimeError("Failed to create ApeCore instance.")

# Convert Python string to C-style string
image_path = b"./SE"

# Load image
lib.load_image(ape, image_path, 0, b"restrant.pal")

# Frames to png
num_frames = lib.get_frame_count(ape)

# Get frame buffer
frame_buffer = lib.get_frame_buffer(ape)

for i in range(0, num_frames):
    lib.frame_to_png(ape, b"test" + str(i).encode() + b".png", i)
    # get frame properties
    width = ctypes.cast(frame_buffer, ctypes.POINTER(OutputBuffer)).contents.width
    height = ctypes.cast(frame_buffer, ctypes.POINTER(OutputBuffer)).contents.height
    channels = ctypes.cast(frame_buffer, ctypes.POINTER(OutputBuffer)).contents.channels
    # get frame data
    data = ctypes.cast(frame_buffer, ctypes.POINTER(OutputBuffer)).contents.pixels
    # print frame properties
    print(f"Frame {i}: {width}x{height} with {channels} channels")

print("Image loaded successfully!")

# Destroy instance (to avoid memory leaks)
lib.destroy_ape_instance(ape)

```