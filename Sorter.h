#pragma once
#include <vector>
#include "Tape.h"

// using approximately 100 Mb of RAM
const int BUFFER_SIZE = 25'000'000;

struct Comparator {
	bool operator()(const std::pair<int, unsigned int>& lhs, const std::pair<int, unsigned int>& rhs) {
		return lhs.second > rhs.second;
	}
};

class TapeSorter {
public:
	TapeSorter(FileTape&);

	void SortTapeContent();

	int GetNumTempFiles();

private:
	FileTape& file_tape_;
	int num_files_ = 0;
	
	bool ReadFromFile();
	void WriteElementsToTempFile(std::vector<unsigned int>& temp_data);

	void ExternalSort();
	void QuickSort(std::vector<unsigned int>& A,  int p, int r);
	int Partition(std::vector<unsigned int>& A,  int p, int r);
	void swap_elements(unsigned int& a, unsigned int& b);
};

