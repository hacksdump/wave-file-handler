//
// Created by hmaurya on 21/02/20.
//

#include "WaveFileHandler.h"


void WaveFileHandler::writeRiffChunkDescriptor() {
    // Contains the letters "RIFF" in ASCII form
    // (0x52494646 big-endian form).
    const string chunkID = "RIFF";

    // 36 + SubChunk2Size, or more precisely:
    // 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
    // This is the size of the rest of the chunk
    // following this number.  This is the size of the
    // entire file in bytes minus 8 bytes for the
    // two fields not included in this count:
    // ChunkID and ChunkSize.
    // Initialized to 0.
    const unsigned int chunkSize = 0;

    // Contains the letters "WAVE"
    // (0x57415645 big-endian form).
    const string format = "WAVE";

    // Dumping the header to file with appropriate endianness.
    this->rawFileHandler.writeString(chunkID);
    this->rawFileHandler.writeBytes(chunkSize, sizeof(chunkSize));
    this->rawFileHandler.writeString(format);
}

void WaveFileHandler::setChunkSize() {
    size_t currentFileSize = this->rawFileHandler.currentSize();
    unsigned int sizeToWrite = currentFileSize - 8;
    this->rawFileHandler.modifyBytes(sizeToWrite, 4, 4);
}

void WaveFileHandler::writeFmtSubChunk() {
    // Bytes per sample (=2 for 16 bit resolution)
    const unsigned bytesPerSample = bitsPerSample / 8;

    // Contains the letters "fmt "
    // Notice the extra space after fmt to complete 4 bytes.
    // (0x666d7420 big-endian form).
    const string subchunk1ID = "fmt ";

    // 16 for PCM.  This is the size of the
    // rest of the Subchunk which follows this number.
    const unsigned int subchunk1Size = 16;

    // PCM = 1 (i.e. Linear quantization)
    // Values other than 1 indicate some form of compression.
    const unsigned short audioFormat = 1;

    // Mono = 1, Stereo = 2, etc.
    const unsigned short numChannels = 1;

    // Sample rate 8000, 44100, etc.
    const unsigned int sampleRate = 44100;

    // ByteRate == SampleRate * NumChannels * BytesPerSample
    const unsigned int byteRate = sampleRate * numChannels * bytesPerSample;

    // BlockAlign == NumChannels * BytesPerSample
    // The number of bytes for one sample including all channels.
    const unsigned short blockAlign = numChannels * bytesPerSample;

    // TODO: Add extra params for non-PCM sounds.

    // Finally dumping the header to file.
    this->rawFileHandler.writeString(subchunk1ID);
    this->rawFileHandler.writeBytes(subchunk1Size, sizeof(subchunk1Size));
    this->rawFileHandler.writeBytes(audioFormat, sizeof(audioFormat));
    this->rawFileHandler.writeBytes(numChannels, sizeof(numChannels));
    this->rawFileHandler.writeBytes(sampleRate, sizeof(sampleRate));
    this->rawFileHandler.writeBytes(byteRate, sizeof(byteRate));
    this->rawFileHandler.writeBytes(blockAlign, sizeof(blockAlign));
    this->rawFileHandler.writeBytes(bitsPerSample, sizeof(bitsPerSample));
}

void WaveFileHandler::writeDataSubChunkHeader() {
    // Contains the letters "data"
    // (0x64617461 big-endian form).
    const string subChunk2ID = "data";

    // Subchunk2Size == NumSamples * NumChannels * BitsPerSample/8
    // This is the number of bytes in the data.
    // You can also think of this as the size
    // of the rest of the sub-chunk following this number.
    // Initialize to 0 since there is no data initially.
    const int subChunk2Size = 0;

    this->rawFileHandler.writeString(subChunk2ID);
    this->setDataSubChunkSizeLocation(this->rawFileHandler.currentWriteHeadPosition());
    this->rawFileHandler.writeBytes(subChunk2Size, sizeof(subChunk2Size));
    this->setDataStartLocation(this->rawFileHandler.currentWriteHeadPosition());
}

void WaveFileHandler::writeDataSubChunkSize() {
    size_t currentFileSize = this->rawFileHandler.currentSize();
    unsigned int sizeToWrite = currentFileSize - this->dataStartLocation;
    this->rawFileHandler.modifyBytes(sizeToWrite, this->dataSubChunkSizeLocation, 4);
}

void WaveFileHandler::close() {
    // Set Chunk size at the end of all writes (headers and data)
    this->setChunkSize();
    // Set data sub-chunk size
    this->writeDataSubChunkSize();
    // Finally close the file
    this->rawFileHandler.close();
}