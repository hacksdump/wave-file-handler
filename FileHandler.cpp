//
// Created by hmaurya on 20/02/20.
//

#include "FileHandler.h"

void FileHandler::initializeFile(string newFilename) {
    if (this->fileOutStream.is_open()) {
        this->fileOutStream.close();
    }
    this->filename = std::move(newFilename);
    this->fileOutStream.open(this->filename, ios_base::binary);
}

void FileHandler::writeByte(char toWrite) {
    int buf_size = 1;
    char* buffer = new char[buf_size];
    buffer[0] = toWrite;
    fileOutStream.write(buffer, buf_size);
    delete [] buffer;
}

void FileHandler::writeString(const string& toWrite) {
    fileOutStream << toWrite;
}

int FileHandler::currentSize() {
    unsigned int currentFilePosition = this->fileOutStream.tellp();
    this->fileOutStream.seekp(0, ios::end);
    int fileSize = this->fileOutStream.tellp();
    this->fileOutStream.seekp(currentFilePosition, ios::beg);
    return fileSize;
}

unsigned int FileHandler::currentWriteHeadPosition() {
    return this->fileOutStream.tellp();
}

void FileHandler::close() {
    this->fileOutStream.close();
}