#pragma once
#include "json.h"
#include "Tape.h"
#include "Sorter.h"

#include <cassert>
#include <sstream>
#include <vector>

using namespace std::literals;

void TestJSON() {
    using namespace json;
    using namespace std::literals;
    Node dict_node{ Dict{{"key1"s, "value1"s}, {"key2"s, 42}} };
    assert(dict_node.IsDict());
    const Dict& dict = dict_node.AsDict();
    assert(dict.size() == 2);
    assert(dict.at("key1"s).AsString() == "value1"s);
    assert(dict.at("key2"s).AsInt() == 42);
    std::istringstream input ("{ \"key1\": \"value1\", \"key2\": 42 }"s);
    assert(Load(input).GetRoot() == dict_node);
}

void PrepareTestFile() {
    std::vector<unsigned int> data_array = { 4, 15, 796, 11, 5446, 47, 12, 0, 1, 6, 5 };
    std::fstream test_file;
    test_file.open("test_input.txt", std::ios::out);
    size_t data_size = data_array.size();
    for (size_t i = 0; i < data_size; ++i) {
        if (i < data_size - 1) {
            test_file << data_array[i] << '\n';
        }
        else {
            test_file << data_array[i];
        }
    }
    test_file.close();
}


void TestTapeOperations() {
    PrepareTestFile();
    // Test Shift and ReadElement operations
    {
        // open specially in binary mode in order to avoid problem with LF/CRLF
        std::ifstream input("test_input.txt", std::ios::binary);
        FileTape tape(input, "test_output.txt");
        tape.Shift();
		unsigned int element = 0;
        tape.ReadElement(element);
        assert(element == 15);
        tape.Shift();
        tape.Shift();
        tape.Shift();
        tape.Shift();
        tape.ReadElement(element);
        assert(element == 47);
    }
    // Test Rewind operation
    {
        // open specially in binary mode in order to avoid problem with LF/CRLF
        std::ifstream input("test_input.txt", std::ios::binary);
        input.seekg(0, std::ios::end);
        FileTape tape(input, "test_output.txt");
        tape.Rewind();
        unsigned int element = 0;
        tape.ReadElement(element);
        assert(element == 4);
    }
    // Test Write operation
    {
        // open specially in binary mode in order to avoid problem with LF/CRLF
        std::ifstream input("test_input.txt", std::ios::binary);
        FileTape tape(input, "test_output.txt");
        tape.WriteElement(14);
        tape.WriteElement(46);
    }
    {
        std::ifstream written_file("test_output.txt");
        unsigned int element = 0;
        written_file >> element;
        assert(element == 14);
        written_file >> element;
        assert(element == 46);
        written_file.close();
    }
    // Test SetConfiguration and GetConfiguration operations
    {
        // open specially in binary mode in order to avoid problem with LF/CRLF
        std::ifstream input("test_input.txt", std::ios::binary);
        FileTape tape(input, "test_output.txt");
        std::istringstream input1("{ \"delays_info\": { \"read_delay\": 45, \"write_delay\": 50, \"rewind_delay\": 1000, \"shift_delay\": 100} }"s);
        tape.SetConfiguration(input1);
        ConfigurationData config = tape.GetConfiguration();
        assert(config.read_delay_ms == 45);
        assert(config.write_delay_ms == 50);
        assert(config.rewind_delay_ms == 1000);
        assert(config.shift_tape_delay_ms == 100);
    }
}

void TestTapeSorting() {
    PrepareTestFile();
    {
        // open specially in binary mode in order to avoid problem with LF/CRLF
        std::ifstream input("test_input.txt", std::ios::binary);
        FileTape tape(input, "test_output.txt");
        TapeSorter sorter(tape);
        sorter.SortTapeContent();
    }
    {
        std::ifstream input("test_output.txt");
        std::vector<unsigned int> expected_result = {0, 1, 4, 5, 6, 11, 12, 15, 47, 796, 5446};
        std::vector<unsigned int> actual_result;
        while (!input.eof()) {
            unsigned int number = 0;
            if (input >> number) {
                actual_result.push_back(number);
            }
        }
        assert(expected_result == actual_result);
    }
}