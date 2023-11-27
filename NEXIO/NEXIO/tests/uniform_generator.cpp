#include "uniform_generator.h"

UniformGenerator::UniformGenerator(uint64_t minVal, uint64_t maxVal, bool writeToFile, const std::string &defaultFilename) 
    : minVal(minVal), maxVal(maxVal), writeToFile(writeToFile), defaultFilename(defaultFilename), distribution(minVal, maxVal) {
    if (writeToFile) {
        outFile.open(defaultFilename);
    }
}

UniformGenerator::~UniformGenerator() {
    if (writeToFile) {
        outFile.close();
    }
}

void UniformGenerator::closeFile() {
    if (writeToFile && outFile.is_open()) {
        outFile.close();
    }
}

uint64_t UniformGenerator::next() {
    uint64_t value = distribution(generator);
    if (writeToFile && outFile.is_open()) {
        outFile << value << "\n";
    }
    return value;
}
