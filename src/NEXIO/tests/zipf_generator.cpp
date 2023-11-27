#include "zipf_generator.h"
#include <algorithm>

void ZipfGenerator::calculateProbabilities() {
    double sum = 0;
    probabilities.resize(N);
    for (int i = 1; i <= N; ++i) {
        sum += 1.0 / std::pow(i, s);
    }
    for (int i = 1; i <= N; ++i) {
        probabilities[i - 1] = (1.0 / std::pow(i, s)) / sum;
    }
    distribution = std::discrete_distribution<uint64_t>(probabilities.begin(), probabilities.end());
}


ZipfGenerator::ZipfGenerator(int N, double s) 
: N(N), s(s), generator(std::random_device{}()), distribution(), outFile("zipf_data.txt", std::ios::out | std::ios::trunc) {
    calculateProbabilities();
}

ZipfGenerator::~ZipfGenerator(){
    if(outFile.is_open()){
        outFile.close(); // Close the file stream
    }
    
}

uint64_t ZipfGenerator::next() {
    uint64_t value = distribution(generator)+5;
    // outFile << value << '\n';
    return value;
}
