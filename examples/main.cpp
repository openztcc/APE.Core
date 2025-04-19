#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/ApeCore.h"
#include <string>
#include <iostream>
#include <chrono>

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

    // Start test 
    double total_time = 0.0;
    auto start = std::chrono::high_resolution_clock::now();

    if (argc > 2) {
        // load custom palette
        if (core.load(input_file, 0, palette_file)) {
            // std::cout << "loaded" << std::endl;
        } else {
            std::cout << "Failed to decode " << input_file << std::endl;
            return 1;
        }
    } 

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> runtime = end - start;
    total_time += runtime.count();
    std::cout << "Time taken to decode " << core.getFrameCount() << " frames: " << total_time << " seconds" << std::endl;
    std::cout << "Average time per frame: " << total_time / core.getFrameCount() << " seconds" << std::endl;

    int numBuffers = core.getFrameCount();
    // std::cout << "numBuffers: " << numBuffers << std::endl;
    std::cout << "Writing PNGs" << std::endl;
    int successCount = 0;
    for (int i = 0; i < numBuffers; i++) {
        int error = core.exportToPNG(output_name + std::to_string(i) + ".png", *core.apeBuffer()[i]);
        if (error < 0) {
            std::cout << "Failed to write frame " << i << " to png" << std::endl;
        } else {
            successCount++;
        }
    }
    std::cout << "Finished writing " << successCount << "frames to PNG with " << numBuffers - successCount << " errors." << std::endl;

    // if (ApeCore::validateGraphicFile(input_file)) {
    //     std::cout << "Valid ZT graphic file" << std::endl;
    // } else {
    //     std::cout << "Invalid ZT graphic file" << std::endl;
    // }

    // if (ApeCore::validatePaletteFile(palette_file)) {
    //     std::cout << "Valid ZT palette file" << std::endl;
    // } else {
    //     std::cout << "Invalid ZT palette file" << std::endl;
    // }
    
    return 0;
}