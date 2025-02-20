// APE.CORE .NET Bindings
// by Eric Galvan (Goosifer.IO)
// https://github.com/openztcc/APE.Core
// Licensed under MIT (see LICENSE)
//
// Python bindings for APE.CORE
// Version 0.4.0

#include "../ApeCore.h"

#define VERSION "0.4.0"

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

extern "C" {
    EXPORT ApeCore* create_ape_instance() {
        return new ApeCore();
    }

    EXPORT void destroy_ape_instance(ApeCore* ape) {
        if (ape) {
            // Ensure OutputBuffer pointers are cleared before deletion
            for (int i = 0; i < ape->getFrameCount(); i++) {
                delete ape->apeBuffer()[i];  // Free each frame buffer
            }
            delete ape;
        }
    }

    EXPORT int load_image(ApeCore* ape, const char* fileName, int colorProfile, const char* palName) {
        if (!ape) return 0;
        return ape->load(std::string(fileName), colorProfile, std::string(palName)) ? 1 : 0;
    }

    EXPORT int get_frame_count(ApeCore* ape) {
        return (ape) ? ape->getFrameCount() : 0;
    }

    EXPORT OutputBuffer** get_frame_buffer(ApeCore* ape) {
        if (!ape || ape->getFrameCount() == 0) return nullptr;
        return ape->apeBuffer();
    }

    EXPORT void free_frame_buffer(OutputBuffer** buffer) {
        if (buffer) {
            delete[] buffer;
        }
    }

    EXPORT Color* get_colors(ApeCore* ape) {
        if (!ape || ape->getColors().empty()) 
            return nullptr;
        return ape->getColors().data();
    }

    EXPORT Frame* get_frames(ApeCore* ape) {
        if (!ape || ape->getFrames().empty()) 
            return nullptr;
        return ape->getFrames().data();
    }

    EXPORT int frame_to_png(ApeCore* ape, const char* fileName, int index) {
        if (!ape || index < 0 || index >= ape->getFrameCount()) return 0;
        return ape->exportToPNG(std::string(fileName), *ape->apeBuffer()[index]) ? 1 : 0;
    }

    EXPORT int validate_graphic_file(const char* fileName) {
        return ApeCore::validateGraphicFile(std::string(fileName));
    }

    EXPORT int validate_palette_file(const char* fileName) {
        return ApeCore::validatePaletteFile(std::string(fileName));
    }

    EXPORT int has_background_frame(const char* fileName) {
        return ApeCore::hasBackgroundFrame(std::string(fileName));
    }

    EXPORT Header get_header(const char* fileName) {
        return ApeCore::getHeader(std::string(fileName));
    }
}
