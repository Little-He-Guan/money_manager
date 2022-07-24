#include "test_unit.h"

#include "../money_manager/command_interpreter.h"

#include <sstream>
#include <iostream>

DEFINE_TEST_CASE(test_command_interpreter_read_command)

	std::istringstream is(std::string("cmdname\nget --help-for-her\nlog -o\nset -v amount 0.0")); std::ostringstream os;
	command_interpreter interpreter(is, os);

	const auto& name = interpreter.get_current_cmd().name;
	const auto& options = interpreter.get_current_cmd().options;
	const auto& arguments = interpreter.get_current_cmd().arguments;

	// with only name
	{
		interpreter.read_command();

		ASSERT_EQUALS(std::string("cmdname"), name, "expected to read name correctly")
		ASSERT_TRUE(options.empty(), "expected to read nothing")
		ASSERT_TRUE(arguments.empty(), "expected to read nothing")
	}

	// with options
	{
		// full form
		interpreter.read_command();

		ASSERT_EQUALS(std::string("get"), name, "expected to read name correctly")
		ASSERT_TRUE(arguments.empty(), "expected to read nothing")
		ASSERT_EQUALS(1, options.size(), "expected to read 1 option")
		ASSERT_EQUALS("--help-for-her", options[0], "expected to read the option correctly")

		// abbreviated
		interpreter.read_command();

		ASSERT_EQUALS(std::string("log"), name, "expected to read name correctly")
		ASSERT_TRUE(arguments.empty(), "expected to read nothing")
		ASSERT_EQUALS(1, options.size(), "expected to read 1 option")
		ASSERT_EQUALS("-o", options[0], "expected to read the option correctly")
	}

	// with arguments
	{
		// full form
		interpreter.read_command();

		ASSERT_EQUALS(std::string("set"), name, "expected to read name correctly")
		ASSERT_EQUALS(1, options.size(), "expected to read 1 option")
		ASSERT_EQUALS("-v", options[0], "expected to read the option correctly")
		ASSERT_EQUALS(2, arguments.size(), "expected to read 2 arguments")
		ASSERT_EQUALS("amount", arguments[0], "expected to read the argument correctly")
		ASSERT_EQUALS("0.0", arguments[1], "expected to read the argument correctly")
	}

ENDDEF_TEST_CASE

void test_command_interpreter()
{
	ghl::test_unit unit
	{
		{
			&test_command_interpreter_read_command
		},
		"tests for class command_interpreter"
	};

	unit.execute();

	std::cout << unit.get_msg() << "\n";
}