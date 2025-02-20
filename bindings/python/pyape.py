import os
import ctypes

# This module is used to interface with the C++ APECore library.
# version: 0.4.1

# Get current path
current_path = os.path.dirname(os.path.abspath(__file__))
dll_path = os.path.join(current_path, "ApeCore.dll")

# Load the DLL
lib = ctypes.CDLL(dll_path)

# --------------------------- Define the structures ---------------------------

# Define OutputBuffer struct
class OutputBuffer(ctypes.Structure):
    _fields_ = [
        ("pixels", ctypes.POINTER(ctypes.c_uint8)),
        ("width", ctypes.c_int),
        ("height", ctypes.c_int),
        ("offsetX", ctypes.c_int),
        ("offsetY", ctypes.c_int),
        ("channels", ctypes.c_int)
    ]

class colors(ctypes.Structure):
    _fields_ = [
        ("r", ctypes.c_uint8),
        ("g", ctypes.c_uint8),
        ("b", ctypes.c_uint8),
        ("a", ctypes.c_uint8)
    ]

# Define PixelBlock struct
class PixelBlock(ctypes.Structure):
    _fields_ = [
        ("offset", ctypes.c_uint8),
        ("colorCount", ctypes.c_uint8),
        ("colors", ctypes.POINTER(colors))
    ]

# Define PixelSet struct
class PixelSet(ctypes.Structure):
    _fields_ = [
        ("blockCount", ctypes.c_uint8),
        ("blocks", ctypes.POINTER(PixelBlock))
    ]

# Define Frame struct
class Frame(ctypes.Structure):
    _fields_ = [
        ("frameSize", ctypes.c_uint32),
        ("height", ctypes.c_uint16),
        ("width", ctypes.c_uint16),
        ("x", ctypes.c_uint16),
        ("y", ctypes.c_uint16),
        ("unk", ctypes.c_uint16),
        ("pixelSets", ctypes.POINTER(PixelSet))
    ]

# --------------------------- Define the function signatures ---------------------------

# Create and destroy functions
lib.create_ape_instance.argtypes = []
lib.create_ape_instance.restype = ctypes.c_void_p

lib.destroy_ape_instance.argtypes = [ctypes.c_void_p]
lib.destroy_ape_instance.restype = None

# Load image function
# args: ape_instance, image_path, color profile, palette_path
# color profile: 0 = RGBA pixel stream; 1 = BGRA pixel stream
# returns: 1 if successful, 0 if failed
lib.load_image.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int, ctypes.c_char_p]
lib.load_image.restype = ctypes.c_int

# Valid graphic format check
# args: image_path
lib.validate_graphic_file.argtypes = [ctypes.c_char_p]
lib.validate_graphic_file.restype = ctypes.c_int

# Validate palette
# args: palette_path
lib.validate_palette_file.argtypes = [ctypes.c_char_p]
lib.validate_palette_file.restype = ctypes.c_int

# Has background frame
# args: image_path
lib.has_background_frame.argtypes = [ctypes.c_char_p]
lib.has_background_frame.restype = ctypes.c_int

# Get frame count
# args: ape_instance
lib.get_frame_count.argtypes = [ctypes.c_void_p]
lib.get_frame_count.restype = ctypes.c_int

# Get frame buffer (array of pointers)
# args: ape_instance
# returns: array of pointers to OutputBuffer
lib.get_frame_buffer.argtypes = [ctypes.c_void_p]
lib.get_frame_buffer.restype = ctypes.POINTER(ctypes.POINTER(OutputBuffer))

lib.free_frame_buffer.argtypes = [ctypes.POINTER(ctypes.POINTER(OutputBuffer))]
lib.free_frame_buffer.restype = None

# Get list of frames
# args: ape_instance
# returns: array of Frame
lib.get_frames.argtypes = [ctypes.c_void_p]
lib.get_frames.restype = ctypes.POINTER(Frame)

# Export to PNG
# args: ape_instance, output_name, frame_index
lib.frame_to_png.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]
lib.frame_to_png.restype = ctypes.c_int

# --------------------------- Helper Functions ---------------------------

def get_frame_pixels(frame):
    """Fetch the frame's pixel stream safely."""
    size = ctypes.c_int()
    pixel_stream_ptr = lib.get_frame_stream(frame, ctypes.byref(size))
    if not pixel_stream_ptr or size.value == 0:
        return None

    return bytearray(ctypes.cast(pixel_stream_ptr, ctypes.POINTER(ctypes.c_uint8 * size.value)).contents)

def get_frames(ape_instance):
    """Get list of frames."""
    frame_ptr = lib.get_frames(ape_instance)
    if not frame_ptr:
        return None
    
    # convert to list
    frames = [frame_ptr[i] for i in range(lib.get_frame_count(ape_instance))]
    return frames

def frames_to_bytearray(frames):
    """Convert frames to a bytearray."""
    output = bytearray()
    for frame in frames:
        output.extend(get_frame_pixels(frame))
    return output
