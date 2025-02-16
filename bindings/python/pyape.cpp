// APE.CORE .NET Bindings
// by Eric Galvan (Goosifer.IO)
// https://github.com/openztcc/APE.Core
// Licensed under MIT (see LICENSE)
//
// Python bindings for APE.CORE
// Version 0.2.0

#include "../../ApeCore.h"
#define VERSION "0.2.0"

extern "C" {
    __declspec(dllexport) ApeCore* create_ape_instance() 
    {
        return new ApeCore();
    }

    __declspec(dllexport) void destroy_ape_instance(ApeCore* ape) 
    {
        delete ape;
    }

    __declspec(dllexport) int load_image(ApeCore* ape, const char* fileName, int colorProfile = 0, const char* palName = "")
    {
        if (ape) {
            ape->load(std::string(fileName), colorProfile, std::string(palName));
            return 1;
        }
        return 0;
    }

    __declspec(dllexport) OutputBuffer** get_frame_buffer(ApeCore* ape) 
    {
        int frame_count = ape->getFrameCount();
        if (!ape || frame_count == 0) return nullptr;
        OutputBuffer** buffer = new OutputBuffer*[frame_count];
        for (int i = 0; i < frame_count; i++) {
            buffer[i] = ape->apeBuffer()[i];
        }
        return buffer;
    }

    __declspec(dllexport) int get_frame_count(ApeCore* ape) 
    {
        if (!ape) return 0;
        return ape->getFrameCount();
    }

    __declspec(dllexport) OutputBuffer* get_frame(ApeCore* ape, int index) 
    {
        int max_size = ape->getFrameCount();
        if (!ape || index < 0 || index >= max_size) return nullptr;
        return ape->apeBuffer()[index];
    }

    __declspec(dllexport) int get_frame_buffer_width(OutputBuffer* buffer) 
    {
        if (!buffer) return 0;
        return buffer->width;
    }

    __declspec(dllexport) int get_frame_buffer_height(OutputBuffer* buffer) 
    {
        if (!buffer) return 0;
        return buffer->height;
    }

    __declspec(dllexport) uint8_t* get_frame_stream(OutputBuffer* buffer) 
    {
        if (!buffer) return nullptr;
        return buffer->pixels;
    }

    __declspec(dllexport) int frame_to_png(ApeCore* ape, const char* fileName, int index) 
    {
        if (!ape || index < 0 || index >= ape->getFrameCount()) return 0;
        return ape->exportToPNG(std::string(fileName), *ape->apeBuffer()[index]);
    }
}
