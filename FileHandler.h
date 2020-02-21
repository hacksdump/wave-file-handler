//
// Created by hmaurya on 20/02/20.
//

#ifndef AUDIO_FILEHANDLER_H
#define AUDIO_FILEHANDLER_H
#include <string>
#include <iostream>
#include <fstream>
using namespace std;


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
    void initializeFile(string newFilename);

    /**
     * Appends a single byte at the end of file
     * @param toWrite
     */
    void writeByte(char toWrite);

    /**
     * Write bytes from given data in little-endian format
     * @tparam T
     * @param data
     */
    template <typename T>
    void writeBytes(T data, size_t numberOfBytes);

    template <typename T>
    void modifyBytes(T data, unsigned int bytePosition, size_t numberOfBytes);

    /**
     * Writes string in big-endian format
     * @param toWrite
     */
    void writeString(const string& toWrite);

    /**
     * Get number of bytes that have been written via object of this class.
     * @return Number of bytes written
     */
    int currentSize();

    /**
     * Get current write pointer (byte number) associated with the file
     * @return
     */
    unsigned int currentWriteHeadPosition();

    void close();
};

#include "FileHandler.tpp"


#endif //AUDIO_FILEHANDLER_H
