// APE.CORE .NET Bindings
// by Eric Galvan (Goosifer.IO)
// https://github.com/openztcc/APE.Core
// Licensed under MIT (see LICENSE)
//
// .NET bindings for APE.CORE

#ifndef APECORE_WRAPPER_H
#define APECORE_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
    #define APE_EXPORT __declspec(dllexport)
#else
    #define APE_EXPORT
#endif

#include "./ApeCore.h"

typedef void* ApeCoreHandle;

APE_EXPORT ApeCoreHandle ApeCore_Create();
APE_EXPORT void ApeCore_Destroy(ApeCoreHandle handle);
APE_EXPORT int ApeCore_Load(ApeCoreHandle handle, const char* fileName, int colorModel);
APE_EXPORT int ApeCore_GetFrameCount(ApeCoreHandle handle);
APE_EXPORT OutputBuffer ApeCore_GetFrame(ApeCoreHandle handle, int index);

#ifdef __cplusplus
}
#endif

#endif // APECORE_WRAPPER_H
