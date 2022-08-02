/*
* If the program starts in CLI, the functions here will be used to interpret and execute the user commands
*/
#pragma once

#include "../money_manager/command_interpreter.h"

/*
* Runs the program in CLI
*/
struct command_line_interpreter
{
	command_line_interpreter() = delete;
	explicit command_line_interpreter(class money_manager& in_mgr) : io(), mgr(in_mgr) {}
	~command_line_interpreter() = default;

public:
	/*
	* Starts the program in CLI
	*/
	void start_cli();
	/*
	* Exits the program that is running in CLI
	*/
	void exit_cli();

public:
	/*
	* Serves the user until he executes the exit command
	* @throws std::runtime_error if unexpected error occurs
	*/
	void serve();

	/*
	* Handles the current command.
	*/
	void handle_command();

public:
	command_interpreter io;
	class money_manager& mgr;
};
