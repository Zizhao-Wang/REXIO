#ifndef UNIFORM_GENERATOR_DATA_GENERATOR_H
#define UNIFORM_GENERATOR_DATA_GENERATOR_H

#include <vector>
#include <random>
#include <string>
#include <fstream>

class UniformGenerator {
public:
    UniformGenerator(uint64_t minVal, uint64_t maxVal, 
                     bool writeToFile = false, 
                     const std::string &defaultFilename = "default_data.txt");
    ~UniformGenerator();
    void closeFile();
    uint64_t next();

private:
    uint64_t minVal;
    uint64_t maxVal;
    bool writeToFile;
    std::string defaultFilename;
    std::ofstream outFile;
    std::default_random_engine generator;
    std::uniform_int_distribution<uint64_t> distribution;
};

#endif //UNIFORM_GENERATOR_DATA_GENERATOR_H
