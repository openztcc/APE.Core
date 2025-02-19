#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../ApeCore.h"
#include <string>
#include <iostream>

int main(int argc, char const *argv[])
{
    // Validate arguments
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <filename> <palette> <optional: output name>" << std::endl;
        return 1;
    } else if (argc > 4) {
        std::cout << "Too many arguments. Usage: " << argv[0] << " <filename> <palette> <optional: output name>" << std::endl;
        return 1;
    }

    ApeCore core;
    std::string input_file = argv[1];
    std::string palette_file = argv[2];
    std::string output_name = argv[3];
    std::cout << "Opening " << input_file << std::endl;
    std::cout << "Using palette " << palette_file << std::endl;

    if (argc > 2) {
        // load custom palette
        if (core.load(input_file, 0, palette_file)) {
            std::cout << "loaded" << std::endl;
        } else {
            std::cout << "failed to load" << std::endl;
        }
    } 


    int numBuffers = core.getFrameCount();
    std::cout << "numBuffers: " << numBuffers << std::endl;
    std::cout << "Writing PNGs" << std::endl;
    for (int i = 0; i < numBuffers; i++) {
        core.exportToPNG(output_name + std::to_string(i) + ".png", *core.apeBuffer()[i]);
    }

    if (ApeCore::validateGraphicFile(input_file)) {
        std::cout << "Valid graphic file" << std::endl;
    } else {
        std::cout << "Invalid graphic file" << std::endl;
    }

    if (ApeCore::validatePaletteFile(palette_file)) {
        std::cout << "Valid palette file" << std::endl;
    } else {
        std::cout << "Invalid palette file" << std::endl;
    }
    
    return 0;
}