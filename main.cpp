#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <math.h>
#include "FileHandler.h"
#include "WaveFileHandler.h"

using namespace std;

// The default byte-write order is little-endian
// It changes only for writing strings, i.e. big-endian (the natural order) is used.



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
