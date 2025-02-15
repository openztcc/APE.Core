// APE.CORE .NET Bindings
// by Eric Galvan (Goosifer.IO)
// https://github.com/openztcc/APE.Core
// Licensed under MIT (see LICENSE)
//
// .NET bindings for APE.CORE

#include "ApeCore.NET.h"

// create a new ApeCore instance
APE_EXPORT ApeCoreHandle ApeCore_Create() 
{
    return new ApeCore();
}

// delete the ApeCore instance
APE_EXPORT void ApeCore_Destroy(ApeCoreHandle handle) 
{
    delete static_cast<ApeCore*>(handle);
}

// call load from apecore
APE_EXPORT int ApeCore_Load(ApeCoreHandle handle, const char* fileName, int colorModel)
{
    return static_cast<ApeCore*>(handle)->load(fileName, colorModel);
}

// get number of frames in the buffer
APE_EXPORT int ApeCore_GetFrameCount(ApeCoreHandle handle) 
{
    return static_cast<ApeCore*>(handle)->apeBuffer().size();
}

// get frame from buffer
APE_EXPORT OutputBuffer ApeCore_GetFrame(ApeCoreHandle handle, int index) 
{
    ApeCore* core = static_cast<ApeCore*>(handle);
    std::vector<OutputBuffer> frames = core->apeBuffer();
    
    if (index >= 0 && index < frames.size()) {
        return frames[index];
    }

    OutputBuffer empty = { nullptr, 0, 0, 0 };
    return empty;
}

// get width of frame
APE_EXPORT int ApeCore_GetFrameWidth(ApeCoreHandle handle, int index) 
{
    ApeCore* core = static_cast<ApeCore*>(handle);
    std::vector<OutputBuffer> frames = core->apeBuffer();

    if (index >= 0 && index < frames.size()) {
        return frames[index].width;
    }

    return 0;
}

// get height of frame
APE_EXPORT int ApeCore_GetFrameHeight(ApeCoreHandle handle, int index) 
{
    ApeCore* core = static_cast<ApeCore*>(handle);
    std::vector<OutputBuffer> frames = core->apeBuffer();

    if (index >= 0 && index < frames.size()) {
        return frames[index].height;
    }

    return 0;
}

// get frame buffer size
APE_EXPORT int ApeCore_GetFrameBufferSize(ApeCoreHandle handle, int index) 
{
    ApeCore* core = static_cast<ApeCore*>(handle);
    std::vector<OutputBuffer> frames = core->apeBuffer();

    if (index >= 0 && index < frames.size()) {
        return frames[index].width * frames[index].height * frames[index].channels * sizeof(int);
    }

    return 0;
}

// get frame buffer
APE_EXPORT int* ApeCore_GetFrameBuffer(ApeCoreHandle handle, int index) 
{
    ApeCore* core = static_cast<ApeCore*>(handle);
    std::vector<OutputBuffer>& frames = core->apeBuffer();

    if (index >= 0 && index < frames.size()) {
        int bufferSize = frames[index].width * frames[index].height * frames[index].channels;
        
        int* buffer = new int[bufferSize];  // Allocate memory dynamically

        std::memcpy(buffer, frames[index].pixels, bufferSize * sizeof(int));

        return buffer;  // Caller must free this memory
    }

    return nullptr;
}

// free buffer
APE_EXPORT void ApeCore_FreeFrameBuffer(int* buffer) 
{
    delete[] buffer;
}