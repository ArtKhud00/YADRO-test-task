#include "Tape.h"
#include "Sorter.h"
#include "File_generator.h"
#include "Tests.h"

#include <iostream>
#include <chrono>

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
	stream << "Usage: tape [<input-file> <output-file> | <input-file> <output-file> <json-config-file>]"sv;
}

int main(int argc, char* argv[]) {
	if (argc < 3 || argc > 4) {
		PrintUsage();
		return 1;
	}
	
	const std::string input_file(argv[1]);
	const std::string output_file(argv[2]);
	if (input_file == output_file) {
		std::cerr << "Input and output files should be different"sv;
		return 1;
	}

	if (argc == 3) {
		TestJSON();
		TestTapeOperations();
		TestTapeSorting();
		std::cout << "Tests passed"s << std::endl;
		// open specially in binary mode in order to avoid problem with LF/CRLF
		std::ifstream file_input(input_file, std::ios::binary);
		FileTape tape(file_input, output_file);
		TapeSorter sorter(tape);
		auto start_time = std::chrono::high_resolution_clock::now();
		sorter.SortTapeContent();
		auto end_time = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
		std::cout << "time: "s << duration.count() << " ms"s << std::endl;
	}
	if (argc == 4) {
		const std::string config_file(argv[3]);
		TestJSON();
		TestTapeOperations();
		TestTapeSorting();
		std::cout << "Tests passed"s << std::endl;
		std::ifstream file_input(input_file,std::ios::binary);
		std::ifstream file_config(config_file);
		FileTape tape(file_input, output_file);
		tape.SetConfiguration(file_config);
		TapeSorter sorter(tape);
		auto start_time = std::chrono::high_resolution_clock::now();
		sorter.SortTapeContent();
		auto end_time = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
		std::cout << "time: "s << duration.count() << " ms"s << std::endl;
	}
	return 0;
}