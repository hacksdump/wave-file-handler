template <typename T>
void FileHandler::writeBytes(T data, size_t numberOfBytes) {
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
void FileHandler::modifyBytes(T data, unsigned int bytePosition, size_t numberOfBytes) {
    unsigned int currentFilePosition = this->fileOutStream.tellp();
    this->fileOutStream.seekp(bytePosition, ios::beg);
    this->writeBytes(data, numberOfBytes);
    this->fileOutStream.seekp(currentFilePosition, ios::beg);
}