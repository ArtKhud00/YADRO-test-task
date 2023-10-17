#include "Tape.h"
#include "json.h"

#include <vector>
#include <thread>
#include <chrono>
#include <iostream>

using namespace std::literals;

FileTape::FileTape(std::ifstream& in_file, const std::string& out_filename)
	: infile_(in_file) {
	outfile_.open(out_filename, std::ios::out);
	head_current_position_input_ = infile_.tellg();
	after_operation_position_input_ = head_current_position_input_;
	head_current_position_output_ = outfile_.tellp();
	after_operation_position_output_ = head_current_position_output_;
}

FileTape::~FileTape() {
	infile_.close();
	outfile_.close();
}

void FileTape::Rewind() {
	infile_.seekg(0, std::ios::beg);
}

void FileTape::MoveTapeForward(int flag) {
	switch (flag)
	{
	case 1: // move input tape
		head_current_position_input_ = after_operation_position_input_;
		infile_.seekg(head_current_position_input_);
		break;
	case 2: // move output tape
		head_current_position_output_ = after_operation_position_output_;
		outfile_.seekp(head_current_position_output_);
		break;
	case 3: // move temp input tape
		head_current_position_temp_input_ = after_operation_position_temp_input_;
		break;
	case 4: // move temp output tape
		head_current_position_temp_output_ = after_operation_position_temp_output_;
		break;
	default: // the tape remains in its old position, that is, it does not move
		return;
	}
	std::chrono::milliseconds duration(config_data_.shift_tape_delay_ms);
	std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

void FileTape::Shift() {
	if (!infile_.eof()) {
		infile_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		after_operation_position_input_ = infile_.tellg();
		MoveTapeForward(1);
	}
}

bool FileTape::ReadElement(unsigned int& element) {
	if (infile_.good() && !infile_.eof()) {
		std::chrono::milliseconds duration(config_data_.read_delay_ms);
		std::this_thread::sleep_for(duration);
		head_current_position_input_ = infile_.tellg();
		infile_ >> element;
		after_operation_position_input_ = infile_.tellg();
		MoveTapeForward(1);
		return true;
	}
	return false;
}

void FileTape::WriteElement(unsigned int element) {
	if (outfile_.good()) {
		std::chrono::milliseconds duration(config_data_.write_delay_ms);
		std::this_thread::sleep_for(duration);

		outfile_ << element << "\n"s;
		after_operation_position_output_ = outfile_.tellp();
		// Move tape after writing element
		MoveTapeForward(2);
	}
	else {
		std::cerr << "Error writing element to output file"s << std::endl;
	}
}

bool FileTape::ReadBinaryElement(unsigned int& element, std::ifstream& in_file) {
	if (in_file.good()) {
		std::chrono::milliseconds duration(config_data_.read_delay_ms);
		std::this_thread::sleep_for(duration);
		//in_file.seekg(head_current_position_temp_input_);
		bool result = static_cast<bool>(in_file.read(reinterpret_cast<char*>(&element), sizeof(element)));
		after_operation_position_temp_input_ = in_file.tellg();
		MoveTapeForward(3);
		return result;
	}
	return false;
}

void FileTape::WriteBinaryElement(unsigned int element, std::ofstream& out_file) {
	if (out_file.good()) {
		std::chrono::milliseconds duration(config_data_.write_delay_ms);
		std::this_thread::sleep_for(duration);
		//out_file.seekp(head_current_position_temp_output_);
		out_file.write(reinterpret_cast<const char*>(&element), sizeof(element));
		after_operation_position_temp_output_ = out_file.tellp();
		MoveTapeForward(4);
	}
	else {
		std::cerr << "Error writing element to temp binary file"s << std::endl;
	}
}

void FileTape::SetConfiguration(std::istream& input) {
	ProcessConfigData(input);
}


ConfigurationData FileTape::GetConfiguration() {
	return config_data_;
}

void FileTape::ProcessConfigData(std::istream& input) {
	if (input.good() && !input.eof()) {
		json::Document doc = json::Load(input);
		json::Node root = doc.GetRoot();
		json::Dict delays = root.AsDict();
		if (delays.count("delays_info"s) > 0) {
			json::Node delay_info = delays.at("delays_info"s);
			json::Dict records = delay_info.AsDict();
			if (records.count("read_delay") > 0) {
				config_data_.read_delay_ms = records.at("read_delay").AsInt();
			}
			if (records.count("write_delay") > 0) {
				config_data_.write_delay_ms = records.at("write_delay").AsInt();
			}
			if (records.count("rewind_delay") > 0) {
				config_data_.rewind_delay_ms = records.at("rewind_delay").AsInt();
			}
			if (records.count("shift_delay") > 0) {
				config_data_.shift_tape_delay_ms = records.at("shift_delay").AsInt();
			}
		}
	}
}