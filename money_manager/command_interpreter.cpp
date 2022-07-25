#include "command_interpreter.h"

#include <sstream>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

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

void command_interpreter::enable_coloring()
{
	// enable virtual terminal for console to show colors for escape sequences
#ifdef _WIN32
	DWORD dw_mode;
	HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE h_in = GetStdHandle(STD_INPUT_HANDLE);

	GetConsoleMode(h_out, &dw_mode);
	SetConsoleMode(h_out, dw_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	GetConsoleMode(h_in, &dw_mode);
	SetConsoleMode(h_in, dw_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

void command_interpreter::read_command()
{
	std::string input;

	// set bg color to very dark blue for inputs
	ostream << "\033[48;5;17m";
	std::getline(istream, input);
	ostream << console_colors_end;

	current_command.as(input);
}

std::string command_interpreter::read_raw_input()
{
	std::string res;
	// set bg color to very dark blue for inputs
	ostream << "\033[48;5;17m";
	istream >> res;
	ostream << console_colors_end;
	return res;
}

void command_interpreter::write_output(const std::string& opt)
{
	ostream << opt;
}

void command_interpreter::write_command_output(const std::string& opt)
{
	ostream << opt << std::endl << std::endl;
}

void command_interpreter::set_color(color c)
{
	ostream << console_colors_begin[c];
}

void command_interpreter::reset_color()
{
	ostream << console_colors_end;
}