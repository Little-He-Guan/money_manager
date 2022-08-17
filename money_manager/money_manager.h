#pragma once

#include "system.h"
#include "command_interpreter.h"

#include <atomic>

#include <fstream>

#ifdef _WIN32
#include <Windows.h> // for UWP's sake
#endif // _WIN32

/*
* All the core data the program uses
*/
class money_manager final
{
public:
	money_manager() : sys() {}
	~money_manager() = default;

	/*
	* Tries to load the program (mainly the system) from the save files
	* @param path the path to the save file, optional. If empty, default file path is applied
	* @returns true iff successfully loaded
	* @throws std::runtime_error if unexpected error occurs
	*/
	bool load_from_file(std::string path = "");

	/*
	* Saves the current system to the save files
	* @param path the path to the save file, optional. If empty, default file path is applied
	*/
	void save_back_to_file(std::string path = "");

public:
	/*
	* Starts the program. Prepare all the things needed.
	* After a call to this, the program should be able to serve.
	* 
	* @returns true iff the system is loaded from file
	* @throws std::runtime_error if unexpected error occurs
	*/
	bool start();

	/*
	* Exits the program.
	* Cleans the resources and saves the system to files by calling save_back_to_file()
	*/
	void shut_down();

public:

	// @returns the current date in reality, or zero_date if an unexpected error occurs
	static date get_current_date();

public:
	financial_system sys;

	double default_expectation = 0.0;
	unsigned default_simulation_duration = 730;
	unsigned default_prediction_duration = 30;

	mutable std::atomic_bool system_loaded = false;

public:
	static constexpr auto version_str = "2.0.5";

private:

};

std::string double_to_string_high_precision(double d);

// non-negative integer
constexpr auto integer_regex = "[\\d]+";
extern const std::regex integer_regex_obj;