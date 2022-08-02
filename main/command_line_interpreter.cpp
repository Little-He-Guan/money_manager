#include "command_line_interpreter.h"
#include "../money_manager/money_manager.h"
#include "../money_manager/simulation.h"

#include <regex>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

void command_line_interpreter::start_cli()
{
	bool load_successful = mgr.start();

	io.enable_coloring();

	io.write_command_output(
		std::string("Welcome to money manager version ") + money_manager::version_str + ".\n" +
		(load_successful ? "Successfully loaded from previous save\n" + mgr.sys.to_string_short() : "You do not have a financial system now. Use command init to create one.")
	);
}

void command_line_interpreter::exit_cli()
{
	// currently nothing
}

void command_line_interpreter::serve()
{
	const auto& cmd = io.get_current_cmd();
	do
	{
		io.read_command();
		handle_command();

	} while (cmd.name != "exit");
}

constexpr auto amount_regex = "([\\d]*\\.)?([\\d]+)";

void command_line_interpreter::handle_command()
{
	static const std::regex amount_regex_obj(amount_regex);

	static const auto& cmd = io.get_current_cmd();

	if (cmd.name == "exit")
	{
		io.write_command_output(command_interpreter::apply_color("Saving contents... Please do not turn off your computer.", command_interpreter::f_yellow));
		mgr.shut_down();
		io.write_output("Bye bye ~\n");
	}
	else if (cmd.name == "help")
	{
		system("documentation.pdf");
	}
	else if (cmd.name == "log")
	{
		if (!fs::exists(log_file_path))
		{
			io.write_command_output(command_interpreter::apply_color("No log exists for now.", command_interpreter::f_red));
		}
		else
		{
			std::ifstream lf(log_file_path, std::ios::in);
			if (!lf.is_open())
			{
				throw std::runtime_error("Unexpected error: cannot open log file");
			}
			// first read all lines in this vector, and later read backward from the end.
			std::vector<std::string> file_lines;
			for (std::string line; std::getline(lf, line);)
			{
				file_lines.push_back(line);
			}

			io.set_color(command_interpreter::f_yellow);
			if (cmd.arguments.size() > 1) // incorrect number of arguments
			{
				io.write_command_output(command_interpreter::apply_color("Incorrect number of arguments.", command_interpreter::f_red));
			}
			else if (cmd.arguments.size() == 1) // with number of lines
			{
				if (!std::regex_match(cmd.arguments[0], integer_regex_obj))
				{
					io.write_command_output(command_interpreter::apply_color("Expected to have a number for arg1, but an incorrect one is supplied.", command_interpreter::f_red));
				}
				else
				{
					int num_l = std::atoi(cmd.arguments[0].c_str());

					for (int i = 1; i <= num_l && i <= file_lines.size(); ++i)
					{
						io.write_output(file_lines[file_lines.size() - i] + "\n");
					}
				}
			}
			else // read all of the file
			{
				for (int i = 1; i <= file_lines.size(); ++i)
				{
					io.write_output(file_lines[file_lines.size() - i] + "\n");
				}
			}

			io.write_output("\n");

			io.reset_color();

			lf.close();
		}
	}
	else if (cmd.name == "state")
	{
		if (mgr.system_loaded)
		{
			io.write_command_output(mgr.sys.to_string());
		}
		else
		{
			io.write_command_output(command_interpreter::apply_color("You have no system currently. Use command init to create one.", command_interpreter::f_red));
		}
	}
	else if (cmd.name == "init")
	{
		if (mgr.system_loaded)
		{
			io.write_command_output(command_interpreter::apply_color("Cannot init a system when there is already one.", command_interpreter::f_red));
		}
		else
		{
			if (cmd.arguments.size() < 1 || cmd.arguments.size() > 2) // incorrect number of arguments
			{
				io.write_command_output(command_interpreter::apply_color("Incorrect number of arguments.", command_interpreter::f_red));
			}
			else
			{
				if (!std::regex_match(cmd.arguments[0], amount_regex_obj)) // check arg1
				{
					io.write_command_output(command_interpreter::apply_color("Expected to have a non-negative number for arg1, but an incorrect one is supplied.", command_interpreter::f_red));
				}
				else
				{
					double amount = std::atof(cmd.arguments[0].c_str());
					double exp = mgr.default_expectation;

					if (cmd.arguments.size() == 2) // expectation is not default
					{
						if (!std::regex_match(cmd.arguments[1], amount_regex_obj)) // check arg2
						{
							io.write_command_output(command_interpreter::apply_color("Expected to have a non-negative number for arg2, but an incorrect one is supplied.", command_interpreter::f_red));
						}
						else
						{
							exp = std::atof(cmd.arguments[1].c_str());
							if (exp > amount)
							{
								io.write_command_output(command_interpreter::apply_color("Expectation cannot be bigger than cash initially", command_interpreter::f_red));
								return;
							}
						}
					}

					date cur_date = money_manager::get_current_date();
					if (cur_date != zero_date)
					{
						mgr.sys.reset(amount, exp, money_manager::get_current_date());
					}
					else
					{
						throw std::runtime_error("Unexpected error: cannot get current date");
					}

					mgr.system_loaded = true;
					io.write_command_output(command_interpreter::apply_color("System initialized successfully!", command_interpreter::f_green));
				}
			}
		}
	}
	else if (cmd.name == "add")
	{
		if (!mgr.system_loaded)
		{
			io.write_command_output(command_interpreter::apply_color("No system exists. Use init to create one first.", command_interpreter::f_red));
		}
		else if (cmd.options.size() != 1)
		{
			io.write_command_output(command_interpreter::apply_color("Incorrect number of options! Please try again.", command_interpreter::f_red));
		}
		else if (cmd.arguments.size() < 2 || cmd.arguments.size() > 5)
		{
			io.write_command_output(command_interpreter::apply_color("Incorrect number of arguments! Please try again.", command_interpreter::f_red));
		}
		else // correct number of options/arguments 
		{
			const auto& opt = cmd.options[0];

			if (!std::regex_match(cmd.arguments[1], amount_regex_obj))	// check argument amount
			{
				io.write_command_output(command_interpreter::apply_color("Expect a non-negative number for argument 2, but an incorrect one is supplied.", command_interpreter::f_red));
			}
			else // arg2 is correct
			{
				double amount = std::atof(cmd.arguments[1].c_str());

				if (opt == "-a" || opt == "--accidental-income")
				{
					mgr.sys.add_accidental_income(amount, cmd.arguments[0]);
					io.write_command_output(command_interpreter::apply_color("Accidental income " + cmd.arguments[0] + " added successfully.", command_interpreter::f_green));
				}
				else // check other arguments
				{
					if (cmd.arguments.size() == 2) // need at least 3 arguments now
					{
						io.write_command_output(command_interpreter::apply_color("Incorrect number of arguments! Please try again.", command_interpreter::f_red));
					}
					else
					{
						financial_event::effective_duration type;

						int arg3 = std::atoi(cmd.arguments[2].c_str());
						if (arg3 < 0 || arg3 > 4) // arg3 is incorrect
						{
							io.write_command_output(command_interpreter::apply_color("Expect a number in the range [0,4] for argument 3, but an incorrect one is supplied.", command_interpreter::f_red));
						}
						else // arg3 is correct
						{
							type = static_cast<financial_event::effective_duration>(arg3);

							date start_date, simulation_end;
							if (cmd.arguments.size() >= 4) // arg4 is not default
							{
								start_date = string_to_date(cmd.arguments[3]);
								if (zero_date == start_date)
								{
									io.write_command_output(command_interpreter::apply_color("Expect a date for argument 4, but an incorrect one is supplied.", command_interpreter::f_red));
									return;
								}
								else if (start_date < mgr.sys.get_date())
								{
									io.write_command_output("Must provide a date later or equal to today for argument 4");
									return;
								}

								if (cmd.arguments.size() == 5) // arg5 is not default
								{
									simulation_end = string_to_date(cmd.arguments[3]);
									if (zero_date == simulation_end)
									{
										io.write_command_output(command_interpreter::apply_color("Expect a date for argument 5, but an incorrect one is supplied.", command_interpreter::f_red));
										return;
									}
									else
									{
										auto sim_duration = simulation_end - mgr.sys.get_date();
										if (sim_duration < 365 || sim_duration > 365 * 5 + 1)
										{
											io.write_command_output(command_interpreter::apply_color("Simulation duration must be between 1 year to 5 years.", command_interpreter::f_red));
											return;
										}
									}
								}
								else
								{
									simulation_end = mgr.sys.get_date();
									simulation_end.advance(mgr.default_simulation_duration);
								}
							}
							else
							{
								start_date = mgr.sys.get_date();
								simulation_end = mgr.sys.get_date();
								simulation_end.advance(mgr.default_simulation_duration);
							}

							if (opt == "-f" || opt == "--fixed-income")
							{
								if (mgr.sys.emplace_fixed_income(cmd.arguments[0], amount, start_date, type))
								{
									io.write_command_output(command_interpreter::apply_color("Fixed income " + cmd.arguments[0] + " added successfully.", command_interpreter::f_green));
								}
								else
								{
									io.write_command_output(command_interpreter::apply_color("Cannot add fixed income " + cmd.arguments[0] + " because the name is already used.", command_interpreter::f_red));
								}
							}
							else // a proposal is to be added
							{
								// start a simulation
								simulation sim(mgr.sys, simulation_end);

								if (opt == "-p" || opt == "--periodic-proposal")
								{
									sim.emplace_p_proposal(cmd.arguments[0], amount, start_date, type);
									sim.start_simulation();

									if (!sim.aborted) // safe state guaranteed
									{
										auto avg_money = std::to_string(sim.sim_avg_amount);
										io.write_command_output(
											"The program decides that the proposal can be added, and the average amount of money you can spend each day at most until " + sim.end_date.to_string() + " would be " +
											avg_money + ".\n Proceed to add the proposal (Y/N)?");

										std::string reply;
										while (true)
										{
											reply = io.read_raw_input();
											if (reply == "y" || reply == "Y")
											{
												if (!mgr.sys.emplace_p_proposal(cmd.arguments[0], amount, start_date, type))
												{
													io.write_command_output(command_interpreter::apply_color("Cannot add the periodic proposal as the name is already used.", command_interpreter::f_red));
												}
												else
												{
													io.write_command_output(command_interpreter::apply_color("Periodic proposal " + cmd.arguments[0] + " added successfully.", command_interpreter::f_green));
												}
												break;
											}
											else if (reply == "n" || reply == "N")
											{
												// do nothing
												break;
											}
											else
											{
												io.write_command_output("Please type one of y,Y,n,N.");
											}
										}
									}
									else // danger
									{
										io.write_output(
											"The program cannot allow the proposal to be added, because it calculated that at " +
											sim.aborted_date.to_string() + " the system would not be in a safe state with cash of only " +
											std::to_string(sim.aborted_cash) + " if the proposal was added.");
									}
								}
								else if (opt == "-o" || opt == "--one-time-proposal")
								{
									sim.emplace_ot_proposal(cmd.arguments[0], amount, start_date, type);
									sim.start_simulation();

									if (!sim.aborted) // safe state guaranteed
									{
										auto avg_money = std::to_string(sim.sim_avg_amount);
										io.write_command_output(
											"The program decides that the proposal can be added, and the average amount of money you can spend each day at most until " + sim.end_date.to_string() +
											" would be " + avg_money + ".\n Proceed to add the proposal (Y/N)?");

										std::string reply;
										while (true)
										{
											reply = io.read_raw_input();
											if (reply == "y" || reply == "Y")
											{
												if (!mgr.sys.emplace_ot_proposal(cmd.arguments[0], amount, start_date, type))
												{
													io.write_command_output(command_interpreter::apply_color("Cannot add the one-time proposal as the name is already used.", command_interpreter::f_red));
												}
												else
												{
													io.write_command_output(command_interpreter::apply_color("One-time proposal " + cmd.arguments[0] + " added successfully.", command_interpreter::f_green));
												}
												break;
											}
											else if (reply == "n" || reply == "N")
											{
												// do nothing
												break;
											}
											else
											{
												io.write_command_output("Please type one of y,Y,n,N.");
											}
										}
									}
									else // danger
									{
										io.write_output(
											"The program cannot allow the proposal to be added, because it calculated that at " +
											sim.aborted_date.to_string() + " the system would not be in a safe state with cash of only " +
											std::to_string(sim.aborted_cash) + " if the proposal was added.");
									}
								}
								else
								{
									io.write_command_output(command_interpreter::apply_color("Incorrect option supplied! Please try again.", command_interpreter::f_red));
								}
							}
						}
					}
				}
			}
		}
	}
	else if (cmd.name == "set")
	{
		if (!mgr.system_loaded)
		{
			io.write_command_output(command_interpreter::apply_color("No system exists. Use init to create one first.", command_interpreter::f_red));
		}
		else if (cmd.options.size() < 1 || cmd.options.size() > 2)
		{
			io.write_command_output(command_interpreter::apply_color("Incorrect number of options! Please try again.", command_interpreter::f_red));
		}
		else if (cmd.arguments.size() < 1 || cmd.arguments.size() > 3)
		{
			io.write_command_output(command_interpreter::apply_color("Incorrect number of arguments! Please try again.", command_interpreter::f_red));
		}
		else
		{
			const auto& opt = cmd.options;

			// ture: set amount; false: set actual
			bool amount_or_actual = true;
			if (opt.size() == 2) // --actual may be supplied; usage 1
			{
				if (opt[1] == "--actual")
				{
					amount_or_actual = false;
				}
				else
				{
					io.write_command_output(command_interpreter::apply_color("Incorrect option supplied.\n", command_interpreter::f_red));
					return;
				}
			}

			date simulation_end(mgr.sys.get_date());

			/*
			* Checks the arguments for usage 1 and returns the translated argument 2 and 3
			* Also sets the simulation_end
			* @returns translated arg 2, or -1 if incorrect argument(s) are supplied 
			* (and if the arguments are correct, rtval != -1)
			*/
			auto check_arg_usage_1 = [this](date& sim_end) -> double
			{
				double res = -1;

				if (!std::regex_match(cmd.arguments[1], amount_regex_obj))	// check argument amount
				{
					io.write_command_output(command_interpreter::apply_color("Expect a non-negative number for argument 2, but an incorrect one is supplied.", command_interpreter::f_red));
				}
				else // arg 2 correct
				{
					res = std::atof(cmd.arguments[1].c_str());

					if (cmd.arguments.size() == 3) // a simulation duration is provided
					{
						if (!std::regex_match(cmd.arguments[2], integer_regex_obj))
						{
							io.write_command_output(command_interpreter::apply_color("Expect an integer for argument 3, but an incorrect one is supplied.", command_interpreter::f_red));
							res = -1;
						}
						else
						{
							int days = std::atoi(cmd.arguments[2].c_str());
							if (days < 365 || days > 365 * 5 + 1)
							{
								io.write_command_output(command_interpreter::apply_color("Simulation duration must be between 1 year and 5 years.", command_interpreter::f_red));
								res = -1;
							}
							else // arg 3 supplied and correct
							{
								sim_end += days;
							}
						}
					}
					else
					{
						sim_end += mgr.default_simulation_duration;
					}
				}

				return res;
			};
			/*
			* Checks the arguments for usage 2 and returns the translated argument 1
			* @returns translated arg 1, or -1 if an incorrect argument is supplied
			*/
			auto check_arg_usage_2 = [this]() -> double
			{
				double res = -1;

				if (cmd.arguments.size() != 1)
				{
					io.write_command_output(command_interpreter::apply_color("Incorrect number of arguments! Please try again.", command_interpreter::f_red));
				}
				else
				{
					if (std::regex_match(cmd.arguments[0], amount_regex_obj)) // integers can also match against this regex
					{
						res = std::atof(cmd.arguments[0].c_str());
					}
				}

				return res;
			};

			if (opt[0] == "-o" || opt[0] == "--one-time-proposal")
			{
				double amount = check_arg_usage_1(simulation_end);
				if (-1 == amount) return;

				simulation sim(mgr.sys, simulation_end);

				auto pe = mgr.sys.edit_ot_proposal(cmd.arguments[0]);

				if (pe != nullptr)
				{
					if (amount_or_actual)
					{
						if (amount > pe->amount) // possibility of endangering the system
						{
							sim.edit_ot_proposal(cmd.arguments[0])->amount = amount;
							sim.start_simulation();

							if (sim.aborted)
							{
								io.write_command_output("The program cannot allow the amount to be changed, because it calculated that at " +
									sim.aborted_date.to_string() + " the system would not be in a safe state with cash of only " +
									std::to_string(sim.aborted_cash) + " if the changed was passed.");
							}
							else
							{
								io.write_command_output(
									"The program decides that the changed can be passed, and the average amount of money you can spend each day until " + sim.end_date.to_string() + " would be " +
									std::to_string(sim.sim_avg_amount) + ".\n Proceed with the change (Y/N)?");

								std::string reply;
								while (true)
								{
									reply = io.read_raw_input();
									if (reply == "y" || reply == "Y")
									{
										pe->amount = amount;
										io.write_command_output(command_interpreter::apply_color(("Amount set successfully"), command_interpreter::f_green));
										break;
									}
									else if (reply == "n" || reply == "N")
									{
										// do nothing
										break;
									}
									else
									{
										io.write_output("Please type one of y,Y,n,N.\n");
									}
								}
							}
						}
						else // do the change.
						{
							pe->amount = amount;
							io.write_command_output(command_interpreter::apply_color(("Amount set successfully"), command_interpreter::f_green));
						}
					}
					else // set actual
					{
						if (mgr.sys.get_date() + 1 != pe->end) // cannot modify actual today
						{
							io.write_command_output(command_interpreter::apply_color(("You cannot know the actual today"), command_interpreter::f_red));
						}
						else
						{
							pe->actual = amount;
							io.write_command_output(command_interpreter::apply_color(("Actual set successfully"), command_interpreter::f_green));
						}
					}
				}
				else
				{
					io.write_command_output(command_interpreter::apply_color(("Could not find the event"), command_interpreter::f_red));
				}
			}
			else if (opt[0] == "-p" || opt[0] == "--periodic-proposal")
			{
				double amount = check_arg_usage_1(simulation_end);
				if (-1 == amount) return;

				simulation sim(mgr.sys, simulation_end);

				auto pe = mgr.sys.edit_p_proposal(cmd.arguments[0]);

				if (pe != nullptr)
				{
					if (amount_or_actual)
					{			
							if (amount > pe->amount) // possibility of endangering the system
							{
								sim.edit_p_proposal(cmd.arguments[0])->amount = amount;
								sim.start_simulation();

								if (sim.aborted)
								{
									io.write_output("The program cannot allow the amount to be changed, because it calculated that at " +
										sim.aborted_date.to_string() + " the system would not be in a safe state with cash of only " +
										std::to_string(sim.aborted_cash) + " if the changed was passed.");
								}
								else
								{
									io.write_command_output(
										"The program decides that the changed can be passed, and the average amount of money you can spend each day at most until " + sim.end_date.to_string() + " would be " +
										std::to_string(sim.sim_avg_amount) + ".\n Proceed with the change (Y/N)?");

									std::string reply;
									while (true)
									{
										reply = io.read_raw_input();
										if (reply == "y" || reply == "Y")
										{
											pe->amount = amount;
											io.write_command_output(command_interpreter::apply_color(("Amount set successfully"), command_interpreter::f_green));
											break;
										}
										else if (reply == "n" || reply == "N")
										{
											// do nothing
											break;
										}
										else
										{
											io.write_output("Please type one of y,Y,n,N.\n");
										}
									}
								}
							}
							else // do the change.
							{
								pe->amount = amount;
								io.write_command_output(command_interpreter::apply_color(("Amount set successfully"), command_interpreter::f_green));
							}			
					}
					else // set actual
					{
						if (mgr.sys.get_date() + 1 != pe->end) // cannot modify actual today
						{
							io.write_command_output(command_interpreter::apply_color(("You cannot know the actual today"), command_interpreter::f_red));
						}
						else
						{
							pe->actual = amount;
							io.write_command_output(command_interpreter::apply_color(("Actual set successfully"), command_interpreter::f_green));
						}
					}
				}
				else
				{
					io.write_command_output(command_interpreter::apply_color(("Could not find the event"), command_interpreter::f_red));
				}
			}
			else if (opt[0] == "-f" || opt[0] == "--fixed-income")
			{
				double amount = check_arg_usage_1(simulation_end);
				if (-1 == amount) return;

				simulation sim(mgr.sys, simulation_end);

				auto pe = mgr.sys.edit_fixed_income(cmd.arguments[0]);

				if (pe != nullptr)
				{
					if (amount_or_actual)
					{
						if (amount < pe->amount) // possibility of endangering the system
						{
							sim.edit_fixed_income(cmd.arguments[0])->amount = amount;
							sim.start_simulation();

							if (sim.aborted)
							{
								io.write_command_output("The program cannot allow the amount to be changed, because it calculated that at " +
									sim.aborted_date.to_string() + " the system would not be in a safe state with cash of only " +
									std::to_string(sim.aborted_cash) + " if the changed was passed.");
							}
							else
							{
								io.write_command_output(
									"The program decides that the changed can be passed, and the average amount of money you can spend each day at most until " + sim.end_date.to_string() + " would be " +
									std::to_string(sim.sim_avg_amount) + ".\n Proceed with the change (Y/N)?");

								std::string reply;
								while (true)
								{
									reply = io.read_raw_input();
									if (reply == "y" || reply == "Y")
									{
										pe->amount = amount;
										io.write_command_output(command_interpreter::apply_color(("Amount set successfully"), command_interpreter::f_green));
										break;
									}
									else if (reply == "n" || reply == "N")
									{
										// do nothing
										break;
									}
									else
									{
										io.write_output("Please type one of y,Y,n,N.\n");
									}
								}
							}
						}
						else // do the change.
						{
							pe->amount = amount;
							io.write_command_output(command_interpreter::apply_color(("Amount set successfully"), command_interpreter::f_green));
						}
					}
					else // set actual
					{
						if (mgr.sys.get_date() + 1 != pe->end) // cannot modify actual today
						{
							io.write_command_output(command_interpreter::apply_color(("You cannot know the actual today"), command_interpreter::f_red));
						}
						else
						{
							pe->actual = amount;
							io.write_command_output(command_interpreter::apply_color(("Actual set successfully"), command_interpreter::f_green));
						}
					}				
				}
				else
				{
					io.write_command_output(command_interpreter::apply_color(("Could not find the event"), command_interpreter::f_red));
				}			
			}	
			else if (opt[0] == "--expectation")
			{
				double exp = check_arg_usage_2();
				if (exp == -1) return;

				if (exp <= mgr.sys.get_cash() && exp >= 0.0)
				{
					mgr.sys.set_expectation(exp);
					io.write_command_output(command_interpreter::apply_color(("Expectation set successfully"), command_interpreter::f_green));
				}
				else
				{
					io.write_command_output(command_interpreter::apply_color("Expectation must be smaller than current cash and >= 0", command_interpreter::f_red));
				}
			}
			else if (opt[0] == "--default-simulation-duration")
			{
				int dur = static_cast<int>(check_arg_usage_2());
				if (dur == -1) return;

				if (dur <= 365 * 5 + 1 && dur >= 365)
				{
					mgr.default_simulation_duration = dur;
					io.write_command_output(command_interpreter::apply_color(("Default simulation duration set successfully"), command_interpreter::f_green));
				}
				else
				{
					io.write_command_output(command_interpreter::apply_color("Default simulation duration must be between 365 to 5*365+1", command_interpreter::f_red));
				}
			}
			else if (opt[0] == "--default-prediction-duration")
			{
				int dur = static_cast<int>(check_arg_usage_2());
				if (dur == -1) return;

				if (dur <= 365 * 2 && dur >= 30)
				{
					mgr.default_prediction_duration = dur;
					io.write_command_output(command_interpreter::apply_color(("Default prediction duration set successfully"), command_interpreter::f_green));
				}
				else
				{
					io.write_command_output(command_interpreter::apply_color("Default prediction duration must be between 30 to 365*2", command_interpreter::f_red));
				}
			}
			else 
			{
				io.write_command_output(command_interpreter::apply_color("Incorrect argument supplied", command_interpreter::f_red));				
			}		
		}
	}
	else if (cmd.name == "find")
	{
		if (!mgr.system_loaded)
		{
			io.write_command_output(command_interpreter::apply_color("No system exists. Use init to create one first.", command_interpreter::f_red));
		}
		else if (cmd.arguments.size() != 1)
		{
			io.write_command_output(command_interpreter::apply_color("Incorrect number of arguments! Please try again.", command_interpreter::f_red));
		}
		else if (cmd.options.size() != 1)
		{
			io.write_command_output(command_interpreter::apply_color("Incorrect number of options! Please try again.", command_interpreter::f_red));
		}
		else
		{
			const auto& opt = cmd.options[0];

			io.set_color(command_interpreter::f_yellow);
			if (opt == "-p" || opt == "--periodic-proposal")
			{
				auto pe = mgr.sys.find_p_proposal(cmd.arguments[0]);
				if (pe != nullptr)
				{
					io.write_output(std::string("Periodic Proposal: \n") + pe->to_string());
					io.write_output("\n");
				}
				else
				{
					io.write_command_output(command_interpreter::apply_color("Event not found.", command_interpreter::f_red));
				}
			}
			else if (opt == "-o" || opt == "--one-time-proposal")
			{
				auto pe = mgr.sys.find_ot_proposal(cmd.arguments[0]);
				if (pe != nullptr)
				{
					io.write_output(std::string("One-time Proposal: \n") + pe->to_string());
					io.write_output("\n");
				}
				else
				{
					io.write_command_output(command_interpreter::apply_color("Event not found.", command_interpreter::f_red));
				}
			}
			else if (opt == "-f" || opt == "--fixed-income")
			{
				auto pe = mgr.sys.find_fixed_income(cmd.arguments[0]);
				if (pe != nullptr)
				{
					io.write_output(std::string("Fixed income: \n") + pe->to_string());
					io.write_output("\n");
				}
				else
				{
					io.write_command_output(command_interpreter::apply_color("Event not found.", command_interpreter::f_red));
				}
			}

			io.reset_color();
		}
	}
	else if (cmd.name == "remove")
	{
		if (!mgr.system_loaded)
		{
			io.write_command_output(command_interpreter::apply_color("No system exists. Use init to create one first.", command_interpreter::f_red));
		}
		else if (cmd.arguments.size() != 1)
		{
			io.write_command_output(command_interpreter::apply_color("Incorrect number of arguments! Please try again.", command_interpreter::f_red));
		}
		else if (cmd.options.size() != 1)
		{
			io.write_command_output(command_interpreter::apply_color("Incorrect number of options! Please try again.", command_interpreter::f_red));
		}
		else
		{
			const auto& opt = cmd.options[0];

			if (opt == "-p" || opt == "--periodic-proposal")
			{
				bool b = mgr.sys.remove_p_proposal(cmd.arguments[0]);
				if (b)
				{
					io.write_command_output(command_interpreter::apply_color(std::string("Periodic Proposal: ") + cmd.arguments[0] + " removed successfully.", command_interpreter::f_green));
				}
				else
				{
					io.write_command_output(command_interpreter::apply_color("Event not found.", command_interpreter::f_red));
				}
			}
			else if (opt == "-o" || opt == "--one-time-proposal")
			{
				bool b = mgr.sys.remove_ot_proposal(cmd.arguments[0]);
				if (b)
				{
					io.write_command_output(command_interpreter::apply_color(std::string("One-time Proposal: ") + cmd.arguments[0] + " removed successfully.", command_interpreter::f_green));
				}
				else
				{
					io.write_command_output(command_interpreter::apply_color("Event not found.", command_interpreter::f_red));
				}
			}
			else if (opt == "-f" || opt == "--fixed-income")
			{
				bool b = mgr.sys.remove_fixed_income(cmd.arguments[0]);
				if (b)
				{
					io.write_command_output(command_interpreter::apply_color(std::string("Fixed income: ") + cmd.arguments[0] + " removed successfully.", command_interpreter::f_green));
				}
				else
				{
					io.write_command_output(command_interpreter::apply_color("Event not found.", command_interpreter::f_red));
				}
			}
		}
	}
	else if (cmd.name == "list")
	{
		if (!mgr.system_loaded)
		{
			io.write_command_output(command_interpreter::apply_color("No system exists. Use init to create one first.", command_interpreter::f_red));
		}
		else
		{
			if (cmd.arguments.size() != 0)
			{
				io.write_command_output(command_interpreter::apply_color("Incorrect number of arguments! Please try again.", command_interpreter::f_red));
			}
			else if (cmd.options.size() != 1)
			{
				io.write_command_output(command_interpreter::apply_color("Incorrect number of options! Please try again.", command_interpreter::f_red));
			}
			else
			{
				io.set_color(command_interpreter::f_yellow);

				const auto& opt = cmd.options[0];
				if (opt == "-p" || opt == "--periodic-proposal")
				{
					if (mgr.sys.get_p_proposals().empty())
					{
						io.write_command_output(command_interpreter::apply_color("No periodic proposals exist.", command_interpreter::f_red));
					}
					else
					{
						io.write_output(std::string("Periodic proposals: \n"));
						for (const auto& [n, e] : mgr.sys.get_p_proposals())
						{
							io.write_output(e.to_string());
							io.write_output("\n");
						}
					}
				}
				else if (opt == "-o" || opt == "--one-time-proposal")
				{
					if (mgr.sys.get_ot_proposals().empty())
					{
						io.write_command_output(command_interpreter::apply_color("No one-time proposals exist.", command_interpreter::f_red));
					}
					else
					{
						io.write_output(std::string("One-time proposals: \n"));
						for (const auto& [n, e] : mgr.sys.get_ot_proposals())
						{
							io.write_output(e.to_string());
							io.write_output("\n");
						}
					}
				}
				else if (opt == "-f" || opt == "--fixed-income")
				{
					if (mgr.sys.get_fixed_incomes().empty())
					{
						io.write_command_output(command_interpreter::apply_color("No fixed incomes exist.", command_interpreter::f_red));
					}
					else
					{
						io.write_output(std::string("Fixed incomes: \n"));
						for (const auto& [n, e] : mgr.sys.get_fixed_incomes())
						{
							io.write_output(e.to_string());
							io.write_output("\n");
						}
					}
				}
				else if (opt == "-a" || opt == "--all")
				{
					// p_p
					if (mgr.sys.get_p_proposals().empty())
					{
						io.write_output(command_interpreter::apply_color("No periodic proposals exist.\n", command_interpreter::f_red));
						// the previous line will reset the color
						io.set_color(command_interpreter::f_yellow);
					}
					else
					{
						io.write_output(std::string("Periodic proposals: \n"));
						for (const auto& [n, e] : mgr.sys.get_p_proposals())
						{
							io.write_output(e.to_string());
							io.write_output("\n");
						}
					}

					// ot_p 
					if (mgr.sys.get_ot_proposals().empty())
					{
						io.write_output(command_interpreter::apply_color("No one-time proposals exist.\n", command_interpreter::f_red));
						// rhe previous line will reset the color
						io.set_color(command_interpreter::f_yellow);
					}
					else
					{
						io.write_output(std::string("One-time proposals: \n"));
						for (const auto& [n, e] : mgr.sys.get_ot_proposals())
						{
							io.write_output(e.to_string());
							io.write_output("\n");
						}
					}

					// incomes
					if (mgr.sys.get_fixed_incomes().empty())
					{
						io.write_output(command_interpreter::apply_color("No fixed incomes exist.\n", command_interpreter::f_red));
						// rhe previous line will reset the color
						io.set_color(command_interpreter::f_yellow);
					}
					else
					{
						io.write_output(std::string("Fixed incomes: \n"));
						for (const auto& [n, e] : mgr.sys.get_fixed_incomes())
						{
							io.write_output(e.to_string());
							io.write_output("\n");
						}
					}
				}

				io.write_output("\n");
				io.reset_color();
			}
		}
	}
	else if (cmd.name == "simulate")
	{
		date sim_end;

		if (!mgr.system_loaded)
		{
			io.write_command_output(command_interpreter::apply_color("No system exists. Use init to create one first.", command_interpreter::f_red));
		}
		else if (!cmd.options.empty())
		{
			io.write_command_output(command_interpreter::apply_color("Incorrect number of options! Please try again.", command_interpreter::f_red));
		}
		else if (cmd.arguments.size() > 1)
		{
			io.write_command_output(command_interpreter::apply_color("Incorrect number of arguments! Please try again.", command_interpreter::f_red));
		}
		else if (cmd.arguments.size() == 0) // use default value
		{
			sim_end = mgr.sys.get_date() + mgr.default_simulation_duration;
		}
		else // use argument
		{
			sim_end = string_to_date(cmd.arguments[0]);
			if (zero_date == sim_end)
			{
				io.write_command_output(command_interpreter::apply_color("Expect to have a date for arg1, but an incorrect one is supplied.", command_interpreter::f_red));
				return;
			}
			else // make sure the duration is within our expectation
			{
				int days = sim_end - mgr.sys.get_date();
				if (days < 365 || days > 365 * 5 + 1) // out of range
				{
					io.write_command_output(command_interpreter::apply_color("The simulation duration must be between 1 year to 5 years.", command_interpreter::f_red));
					return;
				}
			}
		}

		simulation sim(mgr.sys, sim_end);
		sim.start_simulation();

		if (sim.aborted)
		{
			io.write_command_output(
				"The program calculated that at " +
				sim.aborted_date.to_string() + " the system wwill not be in a safe state with cash of only " +
				std::to_string(sim.aborted_cash));
		}
		else
		{
			io.write_command_output(
				"The program guarantees that the system will remain in a safe state until " + sim_end.to_string()
			);
		}
	}
	else if (cmd.name == "predict")
	{
		date pre_end;

		if (!mgr.system_loaded)
		{
			io.write_command_output(command_interpreter::apply_color("No system exists. Use init to create one first.", command_interpreter::f_red));
		}
		else if (!cmd.options.empty())
		{
			io.write_command_output(command_interpreter::apply_color("Incorrect number of options! Please try again.", command_interpreter::f_red));
		}
		else if (cmd.arguments.size() > 1)
		{
			io.write_command_output(command_interpreter::apply_color("Incorrect number of arguments! Please try again.", command_interpreter::f_red));
		}
		else if (cmd.arguments.size() == 0) // use default value
		{
			pre_end = mgr.sys.get_date() + mgr.default_prediction_duration;
		}
		else
		{
			pre_end = string_to_date(cmd.arguments[0]);
			if (zero_date == pre_end)
			{
				io.write_command_output(command_interpreter::apply_color("Expect to have a date for arg1, but an incorrect one is supplied.", command_interpreter::f_red));
				return;
			}
			else // make sure the duration is within our expectation
			{
				int days = pre_end - mgr.sys.get_date();
				if (days < 30 || days > 365) // out of range
				{
					io.write_command_output(command_interpreter::apply_color("The prediction duration must be between 1 month to 1 year.", command_interpreter::f_red));
					return;
				}
			}
		}

		double pre_num = mgr.sys.predict(pre_end);
		if (pre_num >= 0)
		{
			io.write_command_output("The program predicts that the approximate average amount of money you can use each day is " +
				std::to_string(pre_num) + " until " + pre_end.to_string());
		}
		else
		{
			io.write_command_output("The program approximately predicts that you can use no money and have to earn more until " + pre_end.to_string() + ".");
		}
	}
	else if (cmd.name.empty())
	{
		// very rarely the program will read an empty string that comes from God knows where (And how does it come at all??). 
		// Don't know why. Pretend as if it doesn't happen.
	}
	else
	{
		io.write_command_output(command_interpreter::apply_color(cmd.name + " is not a command.\n", command_interpreter::f_red));
	}
}