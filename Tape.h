#pragma once
#include <fstream>
#include <string>

class Tape {
public:
	virtual bool ReadElement(unsigned int&) = 0;

	virtual void WriteElement(unsigned int) = 0;

	virtual void Rewind() = 0;

	virtual void Shift() = 0;
};

struct ConfigurationData {
	int read_delay_ms = 0;
	int write_delay_ms = 0;
	int rewind_delay_ms = 0;
	int shift_tape_delay_ms = 0;
};

class FileTape : public Tape {
public:
	FileTape(std::ifstream&, const std::string&);

	~FileTape();

	void WriteElement(unsigned int) override;

	bool ReadElement(unsigned int&) override;

	void Rewind() override;

	void Shift() override;

	bool ReadBinaryElement(unsigned int&, std::ifstream&);

	void WriteBinaryElement(unsigned int, std::ofstream&);

	void SetConfiguration(std::istream&);

	ConfigurationData GetConfiguration();
private:
	// file emulating input tape
	std::ifstream& infile_;
	// file emulating output tape
	std::ofstream outfile_;
	
	// This group of variables are used to track
	// the position of the tape under the magnetic head
	// for input tape
	std::streampos head_current_position_input_;
	std::streampos after_operation_position_input_;
	// for output tape
	std::streampos head_current_position_output_;
	std::streampos after_operation_position_output_;
	// for temp tape
	std::streampos head_current_position_temp_input_;
	std::streampos after_operation_position_temp_input_;
	std::streampos head_current_position_temp_output_;
	std::streampos after_operation_position_temp_output_;

	ConfigurationData config_data_;

	void MoveTapeForward(int);
	void ProcessConfigData(std::istream&);
};