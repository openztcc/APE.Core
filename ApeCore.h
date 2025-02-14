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

// -------------------------------- APE Graphic Structures

struct Header
{
    uint32_t speed; // animation speed in ms
    uint32_t palNameSize;
    std::vector<char> palName;
    uint32_t frameCount;
};

struct PixelBlock {
    uint8_t offset; // How many transparent pixels before drawing
    uint8_t colorCount; // How many colors in this block
    std::vector<uint8_t> colors; // Color indexes
};

struct PixelSet {
    uint8_t blockCount; // How many pixel blocks
    std::vector<PixelBlock> blocks; // The pixel blocks
};

struct Frame
{
    uint32_t frameSize; // in bytes
    uint16_t height;
    uint16_t width;
    uint16_t x; // x offset
    uint16_t y; // y offset
    uint16_t unk; // unknown bytes
    std::vector<PixelSet> pixelSets; // The pixel sets  
};

// -------------------------------- PAL Colors

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

// -------------------------------- Standard Pixel Output

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
        void readPal(std::string fileName);
        void writePal(std::string fileName);
        bool isFatz(std::ifstream &input);

        std::ifstream input;
        OutputBuffer output;
        Header header;
        std::vector<Frame> frames;
        std::vector<std::vector<PixelBlock>> pixelBlocks;
        std::vector<Color> colors;
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
    colors = std::vector<Color>();
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

    // ------------------------------- read header
    // Note: if bg frame exists, not counted in frameCount
    std::cout << "Header" << std::endl;

    // check if fatz
    if (ApeCore::isFatz(input)) {
        // skip 4 bytes
        input.seekg(4, std::ios::cur);
        // read 9th byte
        input.read((char*)&hasBackground, 1);
        std::cout << "\tType: is fatz" << std::endl;
        std::cout << "\thasBackground: " << hasBackground << std::endl;
    } else {
        std::cout << "\tType: not fatz" << std::endl;
    }

    input.read((char*)&header.speed, 4);
    input.read((char*)&header.palNameSize, 4);
    header.palName.resize(header.palNameSize);
    input.read(header.palName.data(), header.palNameSize);
    input.read((char*)&header.frameCount, 4);
    frames.resize(header.frameCount);

    // print header
    std::cout << "\tspeed: " << header.speed << " ms" << std::endl;
    std::cout << "\tpalNameSize: " << header.palNameSize << " bytes" << std::endl;
    std::cout << "\tpalName: " << header.palName.data() << std::endl;
    std::cout << "\tframeCount: " << header.frameCount << std::endl;
    std::cout << "\tframes: " << frames.size() << std::endl;

    // ------------------------------- read frames
    for (int i = 0; i < header.frameCount; i++) {
        Frame frame;
        input.read((char*)&frame.frameSize, 4);
        input.read((char*)&frame.height, 2);
        input.read((char*)&frame.width, 2);
        input.read((char*)&frame.x, 2);
        input.read((char*)&frame.y, 2);
        input.read((char*)&frame.unk, 2); // always 0?

        // read pixel sets
        for (int j = 0; j < frame.height; j++) {
            PixelSet pixelSet;
            input.read((char*)&pixelSet.blockCount, 1); // how many pixel blocks
            pixelSet.blocks.resize(pixelSet.blockCount); // resize to block count
            for (int k = 0; k < pixelSet.blockCount; k++) { // read each block
                PixelBlock block;
                input.read((char*)&block.offset, 1); // offset
                input.read((char*)&block.colorCount, 1); // color count
                block.colors.resize(block.colorCount); // resize to color count
                input.read((char*)block.colors.data(), block.colorCount); // colors
                pixelSet.blocks[k] = block; // store block
            }
            frame.pixelSets.push_back(pixelSet); // store pixel set
        }

        // store frame
        frames[i] = frame;

        // print frame
        std::cout << "Frame " << i << std::endl;
        std::cout << "\tframeSize: " << frame.frameSize << " bytes" << std::endl;
        std::cout << "\theight: " << frame.height << " px" << std::endl;
        std::cout << "\twidth: " << frame.width << " px" << std::endl;
        std::cout << "\tx: " << frame.x << std::endl;
        std::cout << "\ty: " << frame.y << std::endl;
        std::cout << "\tunk: " << frame.unk << std::endl;
        std::cout << "\tpixelSets: " << frame.pixelSets.size() << std::endl;
        for (int j = 0; j < frame.pixelSets.size(); j++) {
            std::cout << "\t\tpixelSet " << j << std::endl;
            std::cout << "\t\t\tblockCount: " << frame.pixelSets[j].blockCount << std::endl;
            for (int k = 0; k < frame.pixelSets[j].blocks.size(); k++) {
                std::cout << "\t\t\tblock " << k << std::endl;
                std::cout << "\t\t\t\toffset: " << (int)frame.pixelSets[j].blocks[k].offset << std::endl;
                std::cout << "\t\t\t\tcolorCount: " << (int)frame.pixelSets[j].blocks[k].colorCount << std::endl;
                std::cout << "\t\t\t\tcolors: ";
                for (int l = 0; l < frame.pixelSets[j].blocks[k].colorCount; l++) {
                    std::cout << (int)frame.pixelSets[j].blocks[k].colors[l] << " ";
                }
                std::cout << std::endl;
            }
        }
    }

    input.close();

    return 1;
}


#endif // APECORE_H