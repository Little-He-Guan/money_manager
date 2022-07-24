#pragma once

#include "system.h"
#include "command_interpreter.h"

class money_manager final
{
public:
	money_manager() : sys(), io() {}
	~money_manager() = default;

	/*
	* Tries to load the program (mainly the system) from the save files
	* @returns true if successfully loaded
	* @throws std::runtime_error if unexpected error occurs
	*/
	bool load_from_file();

	/*
	* Saves the current system to the save files
	*/
	void save_back_to_file();

public:
	/*
	* Starts the program. Prepare all the things needed.
	* After a call to this, the program should be able to serve.
	* @throws std::runtime_error if unexpected error occurs
	*/
	void start();

	/*
	* Serves the user until he executes the exit command
	* @throws std::runtime_error if unexpected error occurs
	*/
	void serve();

	/*
	* Exits the program.
	* Cleans the resources and saves the system to files by calling save_back_to_file()
	*/
	void shut_down();

private:
	/*
	* Handles the current command.
	*/
	void handle_command();

	// @returns the current date in reality, or zero_date if an unexpected error occurs
	static date get_current_date();


private:
	financial_system sys;
	command_interpreter io;

	double default_expectation = 0.0;
	unsigned default_simulation_duration = 730;
	unsigned default_prediction_duration = 30;

	bool system_loaded = false;

private:
	static constexpr auto version_str = "0.99";
};