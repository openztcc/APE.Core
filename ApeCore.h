#ifndef APECORE_H
#define APECORE_H

#include <fstream>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cstring>

#define INPUT_FILE "N"
#define PAL_FILE "N.pal"
#define MAGIC "FATZ"

// if FATZ is first 4 bytes, additional 5 bytes ahead
// The ninth byte is a boolean value that specifies if there is an 
// a background frame 

struct Header
{
    uint32_t speed;
    uint32_t palNameSize;
    std::vector<char> palName;
    uint32_t frameCount;
};

struct Frame
{
    uint32_t frameSize; // in bytes
    uint16_t height;
    uint16_t width;
    uint16_t x;
    uint16_t y;
    uint16_t unk;
};

struct PixelBlock {

};

struct OutputBuffer {
    uint8_t* pixels; // continuous array of pixels: i.e. {0,0,0,255,255,255,255,...}
    int width;
    int height;
    int channels;
};

class ApeCore
{
    public: 
        ApeCore();
        virtual ~ApeCore();

        int load(std::string fileName);
        void save(std::string fileName);

    private:
        void read_pal();
        void write_pal();
        bool isFatz(std::ifstream &input);

        std::ifstream input;
        OutputBuffer output;
        Header header;
        std::vector<Frame> frames;
        std::vector<std::vector<PixelBlock>> pixelBlocks;
        std::vector<uint8_t> pal;
        bool hasBackground;
};

ApeCore::ApeCore()
{
    hasBackground = false;
    output.pixels = nullptr;
    output.width = 0;
    output.height = 0;
    output.channels = 0;
    header.speed = 0;
    header.palNameSize = 0;
    header.frameCount = 0;
    header.palName = std::vector<char>();
    frames = std::vector<Frame>();
    pixelBlocks = std::vector<std::vector<PixelBlock>>();
    pal = std::vector<uint8_t>();
    input = std::ifstream();
    input.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    
}

ApeCore::~ApeCore()
{
}

bool ApeCore::isFatz(std::ifstream &input)
{
    // save pos
    std::streampos originalPos = input.tellg();  // Save current position

    char magic[5] = {0};
    input.read(magic, 4);
    
    // read at least 4 bytes
    if (input.gcount() < 4) 
    {
        input.clear();  
        input.seekg(originalPos);
        return false;
    }

    // restore pos if not FATZ
    if (strcmp(magic, MAGIC) != 0) {
        input.clear();
        input.seekg(originalPos);
        return false;
    }

    return true;
}

int ApeCore::load(std::string fileName)
{
    input.open(fileName, std::ios::binary);
    if (!input.is_open()) {
        return -1;
    }

    // check if fatz
    if (ApeCore::isFatz(input)) {
        // skip 4 bytes
        input.seekg(4, std::ios::cur);
        // read 9th byte
        input.read((char*)&hasBackground, 1);
        std::cout << "Type: is fatz" << std::endl;
        std::cout << "hasBackground: " << hasBackground << std::endl;
    } else {
        std::cout << "Type: not fatz" << std::endl;
    }

    input.read((char*)&header.speed, 4);
    input.read((char*)&header.palNameSize, 4);
    header.palName.resize(header.palNameSize);
    input.read(header.palName.data(), header.palNameSize);
    input.read((char*)&header.frameCount, 4);
    frames.resize(header.frameCount);

    // print header
    std::cout << "speed: " << header.speed << std::endl;
    std::cout << "palNameSize: " << header.palNameSize << " bytes" << std::endl;
    std::cout << "palName: " << header.palName.data() << std::endl;
    std::cout << "frameCount: " << header.frameCount << std::endl;
    std::cout << "frames: " << frames.size() << std::endl;

    input.close();

    return 1;
}


#endif // APECORE_H