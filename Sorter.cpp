#include "Sorter.h"

#include <queue>
#include <iostream>

using namespace std::literals;

TapeSorter::TapeSorter(FileTape& file_tape)
	: file_tape_(file_tape) {}


int TapeSorter::GetNumTempFiles() {
	return num_files_;
}

bool TapeSorter::ReadFromFile() {
	std::vector<unsigned int> temp_buf;
	temp_buf.reserve(BUFFER_SIZE);
	bool status = true;
	while (status) {
		for (int i = 0; i < BUFFER_SIZE && status; ++i) {
			unsigned int element;
			status = file_tape_.ReadElement(element);
			if (status) {
				temp_buf.emplace_back(element);
			}
		}
		WriteElementsToTempFile(temp_buf);
		temp_buf.clear();
	}
	return true;
}

void TapeSorter::WriteElementsToTempFile(std::vector<unsigned int>& temp_data) {
	QuickSort(temp_data, 0, temp_data.size() - 1);
	std::string temp_filename = "tmp/temp_file"s;
	temp_filename += std::to_string(num_files_++) + ".bin"s;
	std::ofstream out_temp(temp_filename, std::ios::binary);
	if (!out_temp.is_open()) {
		std::cerr << "Output file is not open"s << std::endl;
		return;
	}
	for (const unsigned int data : temp_data) {
		file_tape_.WriteBinaryElement(data, out_temp);
	}
}

void TapeSorter::SortTapeContent() {
	bool isOk = ReadFromFile();
	ExternalSort();
}

void TapeSorter::ExternalSort() {
	std::string template_filename = "tmp/temp_file"s;
	int num_files = GetNumTempFiles();
	std::vector<std::ifstream> temp_files;
	temp_files.reserve(num_files);

	// open temp_files for reading
	for (int i = 0; i < num_files; ++i) {
		std::string filename = template_filename + std::to_string(i) + ".bin"s;
		temp_files.emplace_back(filename, std::ios::binary | std::ios::in);
		if (!temp_files.back().is_open()) {
			std::cerr << "Unable to open file"s + filename << std::endl;
		}
	}

	std::priority_queue<std::pair<int, unsigned int>, std::vector<std::pair<int, unsigned int>>, Comparator> pq;

	// add to queue first element of each temp file
	for (int i = 0; i < num_files; ++i) {
		unsigned int value;
		if (file_tape_.ReadBinaryElement(value, temp_files[i])) {
			pq.push({ i,value });
		}
	}
	while (!pq.empty()) {
		std::pair<int, unsigned int> min_element = pq.top();
		pq.pop();

		file_tape_.WriteElement(min_element.second);

		unsigned int next_value;

		// read an element from a file whose number 
		// corresponds to the minimum element written to the output file
		if (file_tape_.ReadBinaryElement(next_value, temp_files[min_element.first])) {
			pq.push({ min_element.first,next_value });
		}
	}

	// close each temp file
	for (std::ifstream& temp_file : temp_files) {
		temp_file.close();
	}

}


void TapeSorter::swap_elements(unsigned int& a, unsigned int& b) {
	int temp = a;
	a = b;
	b = temp;
}

int TapeSorter::Partition(std::vector<unsigned int>& A, int p, int r) {
	int ii = rand() % (r - p) + p;
	swap_elements(A[ii], A[r]);

	unsigned int pivot = A[r];
	unsigned int i = p;
	for (int j = p; j < r; j++) {
		if (A[j] <= pivot) {
			swap_elements(A[i], A[j]);
			i++;
		}
	}
	swap_elements(A[i], A[r]);
	return i;
}

void TapeSorter::QuickSort(std::vector<unsigned int>& A, int p, int r) {
	if (p < r) {
		int q = Partition(A, p, r);
		QuickSort(A, p, q - 1);
		QuickSort(A, q + 1, r);
	}
}