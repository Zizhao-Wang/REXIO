// ZipfGenerator.h

#ifndef RANDOM_DATA_GENERATOR_H
#define RANDOM_DATA_GENERATOR_H

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <fstream>
#include <cstdint>

class ZipfGenerator {
public:
    ZipfGenerator(int N, double s);
    uint64_t next();
    ~ZipfGenerator();

private:
    void calculateProbabilities();

    

    int N;
    double s;
    int dataSize;
    std::vector<double> probabilities;
    std::discrete_distribution<uint64_t> distribution;
    std::mt19937 generator;
    std::ofstream outFile; // File stream for output
};

#endif
