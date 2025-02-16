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

# Define get_frame_buffer function (array of structs)
lib.get_frame_buffer.argtypes = [ctypes.c_void_p]
lib.get_frame_buffer.restype = ctypes.POINTER(ctypes.POINTER(OutputBuffer))

# Get frame
lib.get_frame.argtypes = [ctypes.c_void_p, ctypes.c_int]
lib.get_frame.restype = ctypes.POINTER(OutputBuffer)


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
    #get current frame
    frame = frame_buffer[i].contents
    width = frame.width
    height = frame.height
    channels = frame.channels
    # get frame data
    data = ctypes.cast(frame_buffer, ctypes.POINTER(OutputBuffer)).contents.pixels
    # print frame properties
    print(f"Frame {i}: {width}x{height} with {channels} channels")

print("Image loaded successfully!")

# Destroy instance (to avoid memory leaks)
lib.destroy_ape_instance(ape)