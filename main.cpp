#include "ApeCore.h"

int main(int argc, char const *argv[])
{
    ApeCore core;
    if (core.load(INPUT_FILE)) {
        std::cout << "loaded" << std::endl;
    } else {
        std::cout << "failed to load" << std::endl;
    }
    
    return 0;
}