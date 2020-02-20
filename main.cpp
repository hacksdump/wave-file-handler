#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <math.h>

using namespace std;

// The default byte-write order is little-endian
// It changes only for writing strings, i.e. big-endian (the natural order) is used.

class FileHandler {
private:
    string filename;
    ofstream fileOutStream;
public:
    FileHandler() = default;

    explicit FileHandler(string filename) {
        this->filename = std::move(filename);
        this->fileOutStream.open(this->filename, ios_base::binary);
    }

    /**
     * Initialize file if not initialized with constructor
     * @param newFilename
     */
    void initializeFile(string newFilename) {
        if (this->fileOutStream.is_open()) {
            this->fileOutStream.close();
        }
        this->filename = std::move(newFilename);
        this->fileOutStream.open(this->filename, ios_base::binary);
    }

    /**
     * Appends a single byte at the end of file
     * @param toWrite
     */
    void writeByte(char toWrite) {
        int buf_size = 1;
        char* buffer = new char[buf_size];
        buffer[0] = toWrite;
        fileOutStream.write(buffer, buf_size);
        delete [] buffer;
    }

    /**
     * Write bytes from given data in little-endian format
     * @tparam T
     * @param data
     */
    template <typename T>
    void writeBytes(T data, size_t numberOfBytes) {
        char* buffer = new char[numberOfBytes];
        for (int idx = 0; idx < numberOfBytes; idx++) {
            unsigned int bitMaskForLowestByte = 0xFF;
            unsigned int shiftByBits = 8;
            buffer[idx] = data & bitMaskForLowestByte;
            data = data >> shiftByBits;
        }
        this->fileOutStream.write(buffer, numberOfBytes);
        delete [] buffer;
    }

    template <typename T>
    void modifyBytes(T data, unsigned int bytePosition, size_t numberOfBytes) {
        unsigned int currentFilePosition = this->fileOutStream.tellp();
        this->fileOutStream.seekp(bytePosition, ios::beg);
        this->writeBytes(data, numberOfBytes);
        this->fileOutStream.seekp(currentFilePosition, ios::beg);
    }

    /**
     * Writes string in big-endian format
     * @param toWrite
     */
    void writeString(const string& toWrite) {
        fileOutStream << toWrite;
    }

    /**
     * Get number of bytes that have been written via object of this class.
     * @return Number of bytes written
     */
    int currentSize() {
        unsigned int currentFilePosition = this->fileOutStream.tellp();
        this->fileOutStream.seekp(0, ios::end);
        int fileSize = this->fileOutStream.tellp();
        this->fileOutStream.seekp(currentFilePosition, ios::beg);
        return fileSize;
    }

    /**
     * Get current write pointer (byte number) associated with the file
     * @return
     */
    unsigned int currentWriteHeadPosition() {
        return this->fileOutStream.tellp();
    }

    void close() {
        this->fileOutStream.close();
    }
};

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
    void writeRiffChunkDescriptor() {
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

    /**
     * Set chunk size when all headers and data have been written
     */
    void setChunkSize() {
        size_t currentFileSize = this->rawFileHandler.currentSize();
        unsigned int sizeToWrite = currentFileSize - 8;
        this->rawFileHandler.modifyBytes(sizeToWrite, 4, 4);
    }

    /**
     * Write fmt sub-chunk header
     * Currently assuming PCM mode.
     * TODO: Make this function generic to write other formats other than PCM as well.
     */
    void writeFmtSubChunk() {
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

    /**
     * Write only the header of data sub-chunk
     * Actual data to be written by separate function
     */
    void writeDataSubChunkHeader() {
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

    void writeDataSubChunkSize() {
        size_t currentFileSize = this->rawFileHandler.currentSize();
        unsigned int sizeToWrite = currentFileSize - this->dataStartLocation;
        this->rawFileHandler.modifyBytes(sizeToWrite, this->dataSubChunkSizeLocation, 4);
    }
public:
    explicit WaveFileHandler(string filename) {
        this->bitsPerSample = 16;
        this->rawFileHandler.initializeFile(std::move(filename));
        this->writeRiffChunkDescriptor();
        this->writeFmtSubChunk();
        this->writeDataSubChunkHeader();
    }

    template <typename T>
    void writeSample(T sample_data, size_t sample_size) {
        this->rawFileHandler.writeBytes(sample_data, sample_size);
    }

    void close() {
        // Set Chunk size at the end of all writes (headers and data)
        this->setChunkSize();
        // Set data sub-chunk size
        this->writeDataSubChunkSize();
        // Finally close the file
        this->rawFileHandler.close();
    }
};

int main() {
    WaveFileHandler wave_file("sample.wav");
    int sample_rate = 44100;
    int frequency = 500;
    int duration = 5;
    float amplitude = 0.1;
    for (int i = 0; i <= sample_rate * duration; i++) {
        short value = sin(2 * M_PI * i / sample_rate * frequency) * ((1 << 15) - 1) * amplitude;
        wave_file.writeSample(value, 2);
    }
    wave_file.close();
    return 0;
}
