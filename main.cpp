#include <iostream>
#include <fstream>
#include <string>
#include <utility>
using namespace std;

class FileHandler {
private:
    int bytesWritten;
    string filename;
    ofstream fileOutStream;
public:
    FileHandler() {}

    FileHandler(string filename) {
        this->filename = std::move(filename);
        this->fileOutStream.open(this->filename, ios_base::binary);
    }

    /**
     * Initialize file if not initialized with constructor
     * @param filename
     */
    void initializeFile(string filename) {
        if (this->fileOutStream.is_open()) {
            this->fileOutStream.close();
        }
        this->filename = std::move(filename);
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
        this->bytesWritten += 1;
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
            buffer[idx] = data & 0xFF;
            data = data >> 8;
        }
        this->fileOutStream.write(buffer, numberOfBytes);
        this->bytesWritten += numberOfBytes;
        delete [] buffer;
    }

    /**
     * Writes string in big-endian format
     * @param toWrite
     */
    void writeString(const string& toWrite) {
        fileOutStream << toWrite;
        this->bytesWritten += toWrite.length();
    }

    /**
     * Get number of bytes that have been written via object of this class.
     * @return Number of bytes written
     */
    int currentSize() {
        return this->bytesWritten;
    }
};

class WaveFileHandler {
private:
    FileHandler rawFileHandler;
    void writeRiffChunkDescriptor() {
        const string chunkID = "RIFF";
        const unsigned int chunkSize = 0;
        const string format = "WAVE";
        this->rawFileHandler.writeString(chunkID);
        this->rawFileHandler.writeBytes(chunkSize, sizeof(chunkSize));
        this->rawFileHandler.writeString(format);
    }
public:
    WaveFileHandler(string filename) {
        this->rawFileHandler.initializeFile(std::move(filename));
        this->writeRiffChunkDescriptor();
    }
};

int main() {
    WaveFileHandler wave_file("sample.wav");
    return 0;
}
