#include "uniform_generator.h"

UniformGenerator::UniformGenerator(uint64_t minVal, uint64_t maxVal) 
    : minVal(minVal), maxVal(maxVal), distribution(minVal, maxVal) {
}

void UniformGenerator::writeDataToFile(const std::string &filename) {
    std::ofstream outFile(filename);
    for (uint64_t val : generatedData) {
        outFile << val << "\n";
    }
    outFile.close();
}

uint64_t UniformGenerator::next() {
    uint64_t value = distribution(generator);
    generatedData.push_back(value);
    return value;
}
