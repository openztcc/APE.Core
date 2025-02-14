#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "ApeCore.h"
#include <string>

#define OUTPUT_NAME "output"

void writeImage(std::string fileName, OutputBuffer output)
{
    if (!output.pixels) {
        std::cerr << "No pixels to write" << std::endl;
        return;
    }

    if (!stbi_write_png(fileName.c_str(), output.width, output.height, output.channels, output.pixels, 0)) {
        std::cerr << "Failed to write image" << std::endl;
    } else {
        std::cout << "Wrote image" << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    ApeCore core;
    if (core.load(INPUT_FILE)) {
        std::cout << "loaded" << std::endl;
    } else {
        std::cout << "failed to load" << std::endl;
    }

    int numBuffers = core.apeBuffer().size();
    for (int i = 0; i < numBuffers; i++) {
        writeImage(OUTPUT_NAME + std::to_string(i) + ".png", core.apeBuffer()[i]);
    }
    
    return 0;
}