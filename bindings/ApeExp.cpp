// APE.CORE .NET Bindings
// by Eric Galvan (Goosifer.IO)
// https://github.com/openztcc/APE.Core
// Licensed under MIT (see LICENSE)
//
// Python bindings for APE.CORE
// Version 0.2.0

#include "../ApeCore.h"

#define VERSION "0.3.0"

extern "C" {
    __declspec(dllexport) ApeCore* create_ape_instance() {
        return new ApeCore();
    }

    __declspec(dllexport) void destroy_ape_instance(ApeCore* ape) {
        if (ape) {
            // Ensure OutputBuffer pointers are cleared before deletion
            for (int i = 0; i < ape->getFrameCount(); i++) {
                delete ape->apeBuffer()[i];  // Free each frame buffer
            }
            delete ape;
        }
    }

    __declspec(dllexport) int load_image(ApeCore* ape, const char* fileName, int colorProfile, const char* palName) {
        if (!ape) return 0;
        return ape->load(std::string(fileName), colorProfile, std::string(palName)) ? 1 : 0;
    }

    __declspec(dllexport) int get_frame_count(ApeCore* ape) {
        return (ape) ? ape->getFrameCount() : 0;
    }

    __declspec(dllexport) OutputBuffer** get_frame_buffer(ApeCore* ape) {
        if (!ape || ape->getFrameCount() == 0) return nullptr;
        return ape->apeBuffer();
    }

    __declspec(dllexport) void free_frame_buffer(OutputBuffer** buffer) {
        if (buffer) {
            delete[] buffer;
        }
    }

    __declspec(dllexport) Color* get_colors(ApeCore* ape) {
        if (!ape || ape->getColors().empty()) 
            return nullptr;
        return ape->getColors().data();
    }

    __declspec(dllexport) Frame* get_frames(ApeCore* ape) {
        if (!ape || ape->getFrames().empty()) 
            return nullptr;
        return ape->getFrames().data();
    }

    __declspec(dllexport) int frame_to_png(ApeCore* ape, const char* fileName, int index) {
        if (!ape || index < 0 || index >= ape->getFrameCount()) return 0;
        return ape->exportToPNG(std::string(fileName), *ape->apeBuffer()[index]) ? 1 : 0;
    }

    __declspec (dllexport) bool validate_graphic_file(const char* fileName) {
        return ApeCore::validateGraphicFile(std::string(fileName));
    }

    __declspec (dllexport) bool validate_palette_file(const char* fileName) {
        return ApeCore::validatePaletteFile(std::string(fileName));
    }
}
