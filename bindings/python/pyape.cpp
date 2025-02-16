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

    __declspec(dllexport) OutputBuffer* get_frame_buffer(ApeCore* ape) 
    {
        if (!ape || ape->apeBuffer().empty()) return nullptr;
        return &ape->apeBuffer()[0];
    }

    __declspec(dllexport) int get_frame_buffer_size(ApeCore* ape) 
    {
        if (!ape) return 0;
        return static_cast<int>(ape->apeBuffer().size());
    }

    __declspec(dllexport) OutputBuffer* get_frame(ApeCore* ape, int index) 
    {
        if (!ape || index < 0 || index >= ape->apeBuffer().size()) return nullptr;
        return &ape->apeBuffer()[index]; 
    }

    __declspec(dllexport) int get_frame_buffer_width(ApeCore* ape, int index) 
    {
        if (!ape || index < 0 || index >= ape->apeBuffer().size()) return 0;
        return ape->apeBuffer()[index].width;
    }

    __declspec(dllexport) int get_frame_buffer_height(ApeCore* ape, int index) 
    {
        if (!ape || index < 0 || index >= ape->apeBuffer().size()) return 0;
        return ape->apeBuffer()[index].height;
    }

    __declspec(dllexport) int frame_to_png(ApeCore* ape, const char* fileName, int index) 
    {
        if (!ape || index < 0 || index >= ape->apeBuffer().size()) return -1;
        return ape->exportToPNG(std::string(fileName), ape->apeBuffer()[index]);
    }
}
