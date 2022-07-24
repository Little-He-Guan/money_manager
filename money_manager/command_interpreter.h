#pragma once

#include "date.h"

#include <regex>
#include <string>
#include <vector>

/*
* Interacts with I/O stream (by default, std::cin/cout) and can interpret user input into commands.
*/
class command_interpreter
{
public:
	struct command
	{
	public:
		command() = default;
		explicit command(const std::string& input);
		~command() = default;

	public:
		// interprets input and make this its interpretation
		void as(const std::string& input);

	public:
		std::string name;
		std::vector<std::string> options;
		std::vector<std::string> arguments;

	public:
		static constexpr auto option_regex = "(\\-\\-[A-Za-z\\-]+)|(\\-[A-Za-z])";
		static const std::regex option_regex_obj;
	};

public:
	command_interpreter();
	command_interpreter(std::istream& is, std::ostream& os) : istream(is), ostream(os) {}
	~command_interpreter() = default;

	/*
	* Reads the user input from standard input and interpret it as current_command
	*/
	void read_command();
	/*
	* Some commands may require additional user input to be handled
	* Call this to read one line of user input without interpreting
	* 
	* @returns raw user input
	*/
	std::string read_raw_input();

	// writes output to output stream
	void write_output(const std::string& opt);

	const command& get_current_cmd() const { return current_command; }

private:
	command current_command;

	std::istream &istream;
	std::ostream &ostream;
};