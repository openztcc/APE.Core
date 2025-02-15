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
