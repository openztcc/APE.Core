#ifndef APECORE_H
#define APECORE_H

// APE.CORE - ZT1 Graphics Parser
// by Eric Galvan (Goosifer.IO)
// https://github.com/openztcc/APE.Core
// Licensed under MIT (see LICENSE)
//
// This library parses ZT1 game sprites and animations. Please feel
// free to use in your own projects and tools.
//
// Credits to Jeff Bostoen for his fantastic documentation on the ZT1 file formats:
// https://github.com/jbostoen/ZTStudio/wiki/ZT1-Graphics-Explained
//
// Version 0.4.0

#include <fstream>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cstring>
#include "./include/stb_image_write.h"

#define INPUT_FILE "SE"
#define MAGIC "FATZ"
#define MAGIC_ALT "ZTAF"
#define APE_CORE_VERSION "0.4.0"

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

struct PixelBlock 
{
    uint8_t offset; // How many transparent pixels before drawing
    uint8_t colorCount; // How many colors in this block
    std::vector<uint8_t> colors; // Color indexes in pal
};

struct PixelSet 
{
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

struct Color 
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

// -------------------------------- Standard Pixel Output

struct OutputBuffer 
{
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

        int load(std::string fileName, int colorProfile = 0, std::string ioPal = "");
        int save(std::string fileName);
        int exportToPNG(std::string fileName, OutputBuffer output);
        int getFrameCount();
        OutputBuffer** apeBuffer();
        std::string getPalLocation();

    private:
        int readPal(std::string fileName);
        void writePal(std::string fileName);
        bool isFatz(std::ifstream &input);
        int writeBuffer();

        std::ifstream input;
        std::ifstream pal;
        OutputBuffer** frameBuffers;
        Header header;
        std::vector<Frame> frames;
        std::vector<std::vector<PixelBlock>> pixelBlocks;
        std::vector<Color> colors;
        bool hasBackground;
        int colorModel;
        std::string palLocation;
    };

ApeCore::ApeCore()
{
    hasBackground = false;
    header.speed = 0;
    header.palNameSize = 0;
    header.frameCount = 0;
    header.palName = std::vector<char>();
    frames = std::vector<Frame>();
    pixelBlocks = std::vector<std::vector<PixelBlock>>();
    colors = std::vector<Color>();
    input = std::ifstream();
    input.exceptions(static_cast<std::ios_base::iostate>(
        std::ifstream::failbit | std::ifstream::badbit));
    colorModel = 0;
    palLocation = "";

    frameBuffers = new OutputBuffer*[1];
}

ApeCore::~ApeCore()
{ 
    if (input.is_open()) 
    {
        input.close();
    }

    if (pal.is_open()) 
    {
        pal.close();
    }

    // free frame buffers
    int numBuffers = getFrameCount();
    for (int i = 0; i < numBuffers; i++) 
    {
        delete[] frameBuffers[i]->pixels;
        delete frameBuffers[i];
    }

    // free colors
    colors.clear();

    // free frames
    frames.clear();

    // free pixel blocks
    for (std::vector<PixelBlock> &blocks : pixelBlocks) 
    {
        blocks.clear();
    }

    // free header
    header.palName.clear();    
}

OutputBuffer** ApeCore::apeBuffer()
{
    return frameBuffers;
}

int ApeCore::getFrameCount() 
{
    return header.frameCount;
}

std::string ApeCore::getPalLocation() 
{
    return palLocation;
}

bool ApeCore::isFatz(std::ifstream &input)
{
    // save pos
    std::streampos originalPos = input.tellg();

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

int ApeCore::readPal(std::string fileName) 
{
    std::cout << "Reading palette" << std::endl;
    pal.open(fileName, std::ios::binary);
    if (!pal.is_open()) {
        return -1;
    }

    // read first bytes to know how many colors
    uint16_t colorCount;
    pal.read((char*)&colorCount, 2);    

    // skip 2 bytes
    pal.seekg(2, std::ios::cur);

    std::cout << "\tcolorCount: " << colorCount << std::endl;

    if (colorCount > 256) {
        return -2;
    }

    // clear existing data
    colors.clear();
    colors.reserve(colorCount);

    // read 256 colors
    for (int i = 0; i < colorCount; i++) {
        Color color;
        pal.read((char*)&color.r, 1);
        pal.read((char*)&color.g, 1);
        pal.read((char*)&color.b, 1);
        pal.read((char*)&color.a, 1);
        colors.push_back(color);
        std::cout << "\tcolor " << i << ": " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << ", " << (int)color.a << std::endl;
    }

    pal.close();

    return 1;
}

int ApeCore::writeBuffer() 
{
    // convert PixelSets to output buffer

    if (frames.empty()) {
        return 0;
    }

    int numBuffers = getFrameCount();
    frameBuffers = new OutputBuffer*[numBuffers];

    for (Frame &frame : frames) 
    {
        // get current index
        int index = &frame - &frames[0];

        // get current frame buffer
        OutputBuffer output = OutputBuffer();


        // allocate pixel buffer
        output.width = static_cast<int>(frame.width);
        output.height = static_cast<int>(frame.height);
        output.channels = 4;
        output.pixels = new uint8_t[output.width * output.height * output.channels];

        // pixel rows (height)
        for (int row = 0; row < frame.height; row++) 
        {
            PixelSet  &pixelSet = frame.pixelSets[row];
            int xOffset = 0; // track x pos in row

            // pixel columns (width)
            for (PixelBlock &pixelBlock : pixelSet.blocks) 
            {
                xOffset += pixelBlock.offset; // skip transparent pixels

                for (uint8_t colorIndex : pixelBlock.colors) 
                {
                    Color &color = colors[colorIndex]; // get rgba value
                    
                    // calc pixel buffer index
                    int pixelIndex = (row * output.width + xOffset) * output.channels;
                    if (colorModel == 1) {
                        output.pixels[pixelIndex] = color.b;
                        output.pixels[pixelIndex + 1] = color.g;
                        output.pixels[pixelIndex + 2] = color.r;
                        output.pixels[pixelIndex + 3] = color.a;
                    } else {
                        output.pixels[pixelIndex] = color.r;
                        output.pixels[pixelIndex + 1] = color.g;
                        output.pixels[pixelIndex + 2] = color.b;
                        output.pixels[pixelIndex + 3] = color.a;
                    }
                    xOffset++; // next pixel
                }
            }
        }
        frameBuffers[index] = new OutputBuffer(output);
    }

    return 1;
}

// Color model 0 = RGBA
// Color model 1 = BGRA
int ApeCore::load(std::string fileName, int colorModel, std::string ioPal)
{
    this->colorModel = colorModel;

    input.open(fileName, static_cast<std::ios_base::openmode>(std::ios::binary | std::ios::in));
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

    input.read((char*)&header.speed, 4); // animation speed in ms
    input.read((char*)&header.palNameSize, 4); // size of palette name
    header.palName.resize(header.palNameSize); // resize to size
    input.read(header.palName.data(), header.palNameSize); // read palette name
    input.read((char*)&header.frameCount, 4); // number of frames
    frames.resize(header.frameCount); // resize frames to frame count

    if (ioPal.empty()) 
        palLocation = std::string(header.palName.data());
    else
        palLocation = ioPal;

    if (hasBackground) {
        header.frameCount += 1;
        frames.resize(header.frameCount);
    }

    // print header
    std::cout << "\tspeed: " << header.speed << " ms" << std::endl;
    std::cout << "\tpalNameSize: " << header.palNameSize << " bytes" << std::endl;
    std::cout << "\tpalName: " << header.palName.data() << std::endl;
    std::cout << "\tframeCount: " << header.frameCount << std::endl;
    std::cout << "\tframes: " << frames.size() << std::endl;

    // ------------------------------- read palette
    ApeCore::readPal(palLocation);

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

    // write output buffer
    if (ApeCore::writeBuffer() > 0) {
        std::cout << "Wrote output buffer" << std::endl;
    } else if (ApeCore::writeBuffer() == 0) {
        std::cout << "No frames to write" << std::endl;
    } else {
        std::cout << "Failed to write output buffer" << std::endl;
    }

    return 1;
}

int ApeCore::save(std::string fileName)
{
    std::ofstream output(fileName, static_cast<std::ios_base::openmode>(std::ios::binary | std::ios::out));
    if (!output.is_open()) {
        return -1;
    }

    if (hasBackground) 
    {
        // WRITE: FATZ
        output.write(MAGIC, 4);
    }

    // -------------------------------------- write header
    output.write((char*)&header.speed, 4); // speed in ms
    output.write((char*)&header.palNameSize, 4); // size of palette name
    output.write(header.palName.data(), header.palNameSize); // palette name
    output.write((char*)&header.frameCount, 4); // frame count

    // write frames
    // TODO: write a Buffer reader and convert to frames and pal
    for (Frame &frame : frames) {
        output.write((char*)&frame.frameSize, 4);
        output.write((char*)&frame.height, 2);
        output.write((char*)&frame.width, 2);
        output.write((char*)&frame.x, 2);
        output.write((char*)&frame.y, 2);
        output.write((char*)&frame.unk, 2);

        // write pixel sets
        for (PixelSet &pixelSet : frame.pixelSets) {
            output.write((char*)&pixelSet.blockCount, 1);
            for (PixelBlock &block : pixelSet.blocks) {
                output.write((char*)&block.offset, 1);
                output.write((char*)&block.colorCount, 1);
                output.write((char*)block.colors.data(), block.colorCount);
            }
        }
    }

    output.close();

    // write palette
    // ApeCore::writePal(palLocation);

    return 1;
}

int ApeCore::exportToPNG(std::string fileName, OutputBuffer output)
{
    if (!output.pixels) {
        std::cerr << "No pixels to write" << std::endl;
        return -1;
    }

    if (!stbi_write_png(fileName.c_str(), output.width, output.height, output.channels, output.pixels, 0)) {
        std::cerr << "Failed to write image" << std::endl;
        return -2;
    } else {
        std::cout << "Wrote image" << std::endl;
    }

    return 1;
}

#endif // APECORE_H