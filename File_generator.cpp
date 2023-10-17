#include <cstdlib>
#include <random>
#include <fstream>
#include <limits>

#include "File_generator.h"

using namespace std::literals;

void GenerateInputFile(const std::string& filename) {
	int num_count = 200'000'000;

	std::ofstream ff(filename);
	std::random_device rd;
	std::mt19937 gen(rd());

	int MAX_VAL = std::numeric_limits<unsigned int>::max();
	int MIN_VAL = std::numeric_limits<unsigned int>::min();
	std::uniform_int_distribution<unsigned int> uniformDistribution(MIN_VAL, MAX_VAL);


	for (int i = 0; i < num_count; i++) {

		unsigned int rand_num = uniformDistribution(gen);

		if (i < num_count) {
			ff << rand_num << "\n"s;
		}
		else {
			ff << rand_num;
		}
	}
	ff.close();
}