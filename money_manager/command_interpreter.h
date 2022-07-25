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

public:
	// Some systems (e.g. Windows) require some preparation to make colors appear by escaping sequences
	// This enables coloring for standard input/output
	// and sets standard input color to light blue (Windows only)
	void enable_coloring();

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
	// writes the output, with additionally two \n\n s so that two lines are escaped.
	void write_command_output(const std::string& opt);

	const command& get_current_cmd() const { return current_command; }

private:
	command current_command;

	std::istream &istream;
	std::ostream &ostream;

public:
	enum color
	{	
		// f: foreground; b: background
		// bf: bright foreground color
		// bb: bright background color
		all_default,
		f_black,
		f_red,
		f_green,
		f_yellow,
		f_blue,
		f_magenta,
		f_cyan,
		f_white,
		b_black,
		b_red,
		b_green,
		b_yellow,
		b_blue,
		b_magenta,
		b_cyan,
		b_white,
		bf_black,
		bf_red,
		bf_green,
		bf_yellow,
		bf_blue,
		bf_magenta,
		bf_cyan,
		bf_white,
		bb_black,
		bb_red,
		bb_green,
		bb_yellow,
		bb_blue,
		bb_magenta,
		bb_cyan,
		bb_white,
	};

private:
	static constexpr const char* console_colors_begin[] = 
	{ 
		"\033[0m", 
		"\033[30m",
		"\033[31m",
		"\033[32m",
		"\033[33m",
		"\033[34m",
		"\033[35m",
		"\033[36m",
		"\033[37m",
		"\033[40m",
		"\033[41m",
		"\033[42m",
		"\033[43m",
		"\033[44m",
		"\033[45m",
		"\033[46m",
		"\033[47m",
		"\033[90m",
		"\033[91m",
		"\033[92m",
		"\033[93m",
		"\033[94m",
		"\033[95m",
		"\033[96m",
		"\033[97m",
		"\033[100m",
		"\033[101m",
		"\033[102m",
		"\033[103m",
		"\033[104m",
		"\033[105m",
		"\033[106m",
		"\033[107m",
	};
	// restore to default
	static constexpr auto console_colors_end = "\033[0m";

public:
	// encloses str to have color indicated by escape sequences
	// @returns the result string
	static std::string apply_color(const std::string& str, color c = color::all_default)
	{
		if (c == all_default)
		{
			return str;
		}
		else
		{
			return std::string(console_colors_begin[c]) + str + console_colors_end;
		}		
	}

	// encloses str to have color indicated by escape sequences
	// @returns the result string
	static std::string apply_color(const char* str, color c = color::all_default)
	{
		std::string res(str);
		if (c == all_default)
		{
			// do nothing
		}
		else
		{
			res.insert(0, console_colors_begin[c]);
			res += console_colors_end;
		}
		return res;
	}

	// sets the text color
	void set_color(color c);
	// resets the text color to default
	void reset_color();
};