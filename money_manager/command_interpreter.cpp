#include "command_interpreter.h"

#include <sstream>
#include <iostream>

const std::regex command_interpreter::command::option_regex_obj(command_interpreter::command::option_regex);

command_interpreter::command::command(const std::string& input)
{
	std::istringstream sinput(input);

	std::getline(sinput, name, ' ');

	for (std::string token; std::getline(sinput, token); )
	{
		if (std::regex_match(token, command::option_regex_obj)) // it is an option
		{
			options.push_back(token);
		}
		else
		{
			arguments.push_back(token);
		}
	}
}

void command_interpreter::command::as(const std::string& input)
{
	name.clear();
	options.clear();
	arguments.clear();

	std::istringstream sinput(input);

	std::getline(sinput, name, ' ');

	for (std::string token; std::getline(sinput, token, ' '); )
	{
		if (std::regex_match(token, command::option_regex_obj)) // it is an option
		{
			// avoids the copy
			options.push_back(token);
		}
		else
		{
			arguments.push_back(token);
		}
	}
}

command_interpreter::command_interpreter() : istream(std::cin), ostream(std::cout) 
{
}

void command_interpreter::read_command()
{
	std::string input;
	std::getline(istream, input);
	current_command.as(input);
}

std::string command_interpreter::read_raw_input()
{
	std::string res;
	istream >> res;
	return res;
}

void command_interpreter::write_output(const std::string& opt)
{
	ostream << opt;
}
