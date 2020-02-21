template <typename T>
void WaveFileHandler::writeSample(T sample_data, size_t sample_size) {
    this->rawFileHandler.writeBytes(sample_data, sample_size);
}