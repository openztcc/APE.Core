#ifndef APECORE_H
#define APECORE_H

#include <fstream>
#include <vector>

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

class ApeCore
{
    public: 
        ApeCore();
        virtual ~ApeCore();

        void load(std::string fileName);
        void save(std::string fileName);

    private:
        void read_pal();
        void write_pal();
        bool isFatz();

        std::ifstream input;
        std::ofstream output;
        Header header;
        std::vector<Frame> frames;
        std::vector<std::vector<PixelBlock>> pixelBlocks;
        std::vector<uint8_t> pal;
        bool hasBackground;
};


#endif // APECORE_H