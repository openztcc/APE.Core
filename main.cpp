#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ApeCore.h"
#include <string>

#define OUTPUT_NAME "output"

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
        core.exportToPNG(OUTPUT_NAME + std::to_string(i) + ".png", core.apeBuffer()[i]);
    }
    
    return 0;
}