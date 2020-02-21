//
// Created by hmaurya on 21/02/20.
//

#ifndef AUDIO_WAVEFILEHANDLER_H
#define AUDIO_WAVEFILEHANDLER_H
#include "FileHandler.h"
#include <string>

using namespace std;


class WaveFileHandler {
private:
    FileHandler rawFileHandler;
    unsigned int dataSubChunkSizeLocation{};
    unsigned int dataStartLocation{};
    // Size of a single sample/frame in bits
    // 8 bits = 8, 16 bits = 16, etc.
    unsigned short bitsPerSample;

    void setDataSubChunkSizeLocation(unsigned int locationBytes) {
        dataSubChunkSizeLocation = locationBytes;
    }
    void setDataStartLocation(unsigned int locationBytes) {
        dataStartLocation = locationBytes;
    }

    /**
     * Write RIFF header
     */
    void writeRiffChunkDescriptor();

    /**
     * Set chunk size when all headers and data have been written
     */
    void setChunkSize();

    /**
     * Write fmt sub-chunk header
     * Currently assuming PCM mode.
     * TODO: Make this function generic to write other formats other than PCM as well.
     */
    void writeFmtSubChunk();

    /**
     * Write only the header of data sub-chunk
     * Actual data to be written by separate function
     */
    void writeDataSubChunkHeader();

    void writeDataSubChunkSize();
public:
    explicit WaveFileHandler(string filename) {
        this->bitsPerSample = 16;
        this->rawFileHandler.initializeFile(std::move(filename));
        this->writeRiffChunkDescriptor();
        this->writeFmtSubChunk();
        this->writeDataSubChunkHeader();
    }

    template <typename T>
    void writeSample(T sample_data, size_t sample_size);

    void close();
};

#include "WaveFileHandler.tpp"

#endif //AUDIO_WAVEFILEHANDLER_H
