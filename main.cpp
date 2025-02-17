#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ApeCore.h"
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
    std::string OUTPUT_NAME = argv[2];
    std::cout << "Opening " << input_file << std::endl;
    std::cout << "Using palette " << argv[2] << std::endl;

    if (argc > 2) {
        // load custom palette
        std::string PALETTE_FILE = argv[2];
        if (core.load(INPUT_FILE, 0, PALETTE_FILE)) {
            std::cout << "loaded" << std::endl;
        } else {
            std::cout << "failed to load" << std::endl;
        }
    } else {
        // load default palette stated in graphic
        if (core.load(input_file)) {
            std::cout << "loaded" << std::endl;
        } else {
            std::cout << "failed to load" << std::endl;
        }
    }

    if (!OUTPUT_NAME.empty()) {
        core.save(OUTPUT_NAME);
        std::cout << "Saved " << OUTPUT_NAME << std::endl;  
    }

    if (core.load(INPUT_FILE)) {
        std::cout << "loaded" << std::endl;
    } else {
        std::cout << "failed to load" << std::endl;
    }

    int numBuffers = core.getFrameCount();
    for (int i = 0; i < numBuffers; i++) {
        core.exportToPNG(OUTPUT_NAME + std::to_string(i) + ".png", *core.apeBuffer()[i]);
    }
    
    return 0;
}