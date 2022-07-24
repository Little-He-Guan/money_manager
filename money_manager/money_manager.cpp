#define _CRT_SECURE_NO_WARNINGS

#include "money_manager.h"
#include "simulation.h"

#include <time.h>
#include <fstream>
#include <filesystem>

#include <stdexcept>

constexpr auto save_file_name = "save.sav";

namespace fs = std::filesystem;

const fs::path save_path(save_file_name);

constexpr auto sf_second_line_regex = "((?:[\\d]*[.])?[\\d]+) ((?:[\\d]*[.])?[\\d]+)";
const std::regex sf_second_line_regex_obj(sf_second_line_regex);

// line of an event. we do not use the precise regex for dates here, as it will be tested in string_to_date()
constexpr auto sf_event_line_regex = "([\\w]+) ([\\d\\-]+) ([\\d\\-]+) ((?:[\\d]*[.])?[\\d]+) ([0-4])";
const std::regex sf_event_line_regex_obj(sf_event_line_regex);

// non-negative integer
constexpr auto integer_regex = "[\\d]+"; 
const std::regex integer_regex_obj(integer_regex);

bool money_manager::load_from_file()
{
	if (!fs::exists(save_path)) // the file does not exist now.
	{
		// create the file
		std::ofstream(save_path, std::ios::out);
	}

	std::ifstream sf(save_path);
	if (!sf.is_open())
	{
		throw std::runtime_error("Unexpected error: cannot open save file");
	}
	else
	{
		if (sf.peek() == std::ifstream::traits_type::eof()) // the file is empty
		{
			return false;
		}
		else
		{
			date saved_date;
			double cash, expectation;

			int num_ot_p, num_p_p, num_i;

			std::string name;
			date start, end;
			double amount;
			financial_event::effective_duration type;

			std::string line;

			// first line: date
			{
				std::getline(sf, line);
				saved_date = string_to_date(line);

				if (zero_date == saved_date)
				{
					throw std::runtime_error("Unexpected error: save file corrupted");
				}
			}

			// second line: cash and expectation
			{
				std::getline(sf, line);

				std::smatch match;
				if (!std::regex_match(line, match, sf_second_line_regex_obj))
				{
					throw std::runtime_error("Unexpected error: save file corrupted");
				}
				else
				{
					cash = std::atof(match[1].str().c_str());
					expectation = std::atof(match[2].str().c_str());
				}
			}

			// third line: number of ot-proposals
			{
				std::getline(sf, line);

				if (!std::regex_match(line, integer_regex_obj))
				{
					throw std::runtime_error("Unexpected error: save file corrupted");
				}
				else
				{
					num_ot_p = std::atoi(line.c_str());
				}
			}

			sys.reset(cash, expectation, saved_date);

			// lines of ot-proposals
			{
				for (int i = 0; i != num_ot_p; ++i)
				{
					std::getline(sf, line);

					std::smatch match;
					if (!std::regex_match(line, match, sf_event_line_regex_obj))
					{
						throw std::runtime_error("Unexpected error: save file corrupted");
					}
					else
					{
						name = match[1].str();
						start = string_to_date(match[2].str()); end = string_to_date(match[3].str());
						if (zero_date == start || zero_date == end)
						{
							throw std::runtime_error("Unexpected error: save file corrupted");
						}
						amount = std::atof(match[4].str().c_str());
						type = (financial_event::effective_duration)std::atoi(match[5].str().c_str());

						sys.emplace_ot_proposal(name, amount, start, end, type);
					}
				}
			}

			// number of p_proposals
			{
				std::getline(sf, line);

				if (!std::regex_match(line, integer_regex_obj))
				{
					throw std::runtime_error("Unexpected error: save file corrupted");
				}
				else
				{
					num_p_p = std::atoi(line.c_str());
				}
			}

			// lines of p-proposals
			{
				for (int i = 0; i != num_p_p; ++i)
				{
					std::getline(sf, line);

					std::smatch match;
					if (!std::regex_match(line, match, sf_event_line_regex_obj))
					{
						throw std::runtime_error("Unexpected error: save file corrupted");
					}
					else
					{
						name = match[1].str();
						start = string_to_date(match[2].str()); end = string_to_date(match[3].str());
						if (zero_date == start || zero_date == end)
						{
							throw std::runtime_error("Unexpected error: save file corrupted");
						}
						amount = std::atof(match[4].str().c_str());
						type = (financial_event::effective_duration)std::atoi(match[5].str().c_str());

						sys.emplace_p_proposal(name, amount, start, end, type);
					}
				}
			}

			// number of f_incomes
			{
				std::getline(sf, line);

				if (!std::regex_match(line, integer_regex_obj))
				{
					throw std::runtime_error("Unexpected error: save file corrupted");
				}
				else
				{
					num_i = std::atoi(line.c_str());
				}
			}

			// lines of p-proposals
			{
				for (int i = 0; i != num_i; ++i)
				{
					std::getline(sf, line);

					std::smatch match;
					if (!std::regex_match(line, match, sf_event_line_regex_obj))
					{
						throw std::runtime_error("Unexpected error: save file corrupted");
					}
					else
					{
						name = match[1].str();
						start = string_to_date(match[2].str()); end = string_to_date(match[3].str());
						if (zero_date == start || zero_date == end)
						{
							throw std::runtime_error("Unexpected error: save file corrupted");
						}
						amount = std::atof(match[4].str().c_str());
						type = (financial_event::effective_duration)std::atoi(match[5].str().c_str());

						sys.emplace_fixed_income(name, amount, start, end, type);
					}
				}
			}

			sf.close();
			return true;
		}	
	}

	return false;
}

void money_manager::save_back_to_file()
{
	if (system_loaded) // do not write the default constructed system to file
	{
		// overwrite the file
		std::ofstream sf(save_path, std::ios::out | std::ios::trunc);

		sf << sys.get_date().to_string() << std::endl;
		sf << sys.get_cash() << " " << sys.get_expectation() << std::endl;

		sf << sys.get_ot_proposals().size() << std::endl;
		for (const auto& [n, e] : sys.get_ot_proposals())
		{
			sf << *e.name << " " << e.start.to_string() << " " << e.end.to_string() << " " << std::to_string(e.amount) << " " << (int)e.type << std::endl;
		}

		sf << sys.get_p_proposals().size() << std::endl;
		for (const auto& [n, e] : sys.get_p_proposals())
		{
			sf << *e.name << " " << e.start.to_string() << " " << e.end.to_string() << " " << std::to_string(e.amount) << " " << (int)e.type << std::endl;
		}

		sf << sys.get_fixed_incomes().size() << std::endl;
		for (const auto& [n, e] : sys.get_fixed_incomes())
		{
			sf << *e.name << " " << e.start.to_string() << " " << e.end.to_string() << " " << std::to_string(e.amount) << " " << (int)e.type << std::endl;
		}

		sf.close();	}

}

void money_manager::start()
{
	bool load_successful = load_from_file();

	// update the system state.
	if (load_successful)
	{
		system_loaded = true;
		date cur_date = get_current_date();
		if (zero_date != cur_date)
		{
			sys.advance_until(get_current_date());
		}
		else
		{
			throw std::runtime_error("Unexpected error: cannot get current date");
		}
	}

	io.write_output(
		std::string("Welcome to money manager version 0.99.\n") +
		(load_successful ? "Successfully loaded from previous save\n" + sys.to_string_short() : "You do not have a financial system now. Use command init to create one.") +
		"\n"
	);
}

void money_manager::serve()
{
	const auto& cmd = io.get_current_cmd();
	do
	{
		io.read_command();
		handle_command();

	} while (cmd.name != "exit");
}

void money_manager::shut_down()
{
	save_back_to_file();
}

constexpr auto amount_regex = "([\\d]*\\.)?([\\d]+)";
const std::regex amount_regex_obj(amount_regex);

void money_manager::handle_command()
{
	static const auto& cmd = io.get_current_cmd();

	if (cmd.name == "exit")
	{
		io.write_output("Saving contents... Please do not turn off your computer.\n");
		shut_down();
		io.write_output("Bye bye ~\n");
	}
	else if (cmd.name == "help")
	{
		system("documentation.pdf");
	}
	else if (cmd.name == "log") // TBD
	{
		if (!fs::exists(log_file_path))
		{
			io.write_output("No log exists for now.\n");
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

			if (cmd.arguments.size() > 1) // incorrect number of arguments
			{
				io.write_output("Incorrect number of arguments.\n");
			}
			else if (cmd.arguments.size() == 1) // with number of lines
			{
				if (!std::regex_match(cmd.arguments[0], integer_regex_obj))
				{
					io.write_output("Expected to have a number for arg1, but an incorrect one is supplied.\n");
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

			lf.close();
		}
	}
	else if (cmd.name == "state")
	{
		if (system_loaded)
		{
			io.write_output(sys.to_string() + "\n");
		}
		else
		{
			io.write_output("You have no system currently. Use command init to create one.\n");
		}
	}
	else if (cmd.name == "init")
	{
		if (system_loaded)
		{
			io.write_output("Cannot init a system when there is already one.\n");
		}
		else
		{
			if (cmd.arguments.size() < 1 || cmd.arguments.size() > 2) // incorrect number of arguments
			{
				io.write_output("Incorrect number of arguments.\n");
			}
			else
			{
				if (!std::regex_match(cmd.arguments[0], amount_regex_obj)) // check arg1
				{
					io.write_output("Expected to have a non-negative number for arg1, but an incorrect one is supplied.\n");
				}
				else
				{
					double amount = std::atof(cmd.arguments[0].c_str());
					double exp = default_expectation;

					if (cmd.arguments.size() == 2) // expectation is not default
					{
						if (!std::regex_match(cmd.arguments[1], amount_regex_obj)) // check arg2
						{
							io.write_output("Expected to have a non-negative number for arg2, but an incorrect one is supplied.\n");
						}
						else
						{
							exp = std::atof(cmd.arguments[1].c_str());
							if (exp > amount)
							{
								io.write_output("Expectation cannot be bigger than cash initially\n");
								return;
							}
						}
					}

					date cur_date = get_current_date();
					if (cur_date != zero_date)
					{
						sys.reset(amount, exp, get_current_date());
					}
					else
					{
						throw std::runtime_error("Unexpected error: cannot get current date");
					}

					system_loaded = true;
					io.write_output("System initialized successfully!\n");
				}
			}
		}
	}
	else if (cmd.name == "add")
	{
		if (!system_loaded)
		{
			io.write_output("No system exists. Use init to create one first.\n");
		}
		else if (cmd.options.size() != 1)
		{
			io.write_output("Incorrect number of options! Please try again.\n");
		}
		else if (cmd.arguments.size() < 2 || cmd.arguments.size() > 5)
		{
			io.write_output("Incorrect number of arguments! Please try again.\n");
		}
		else // correct number of options/arguments 
		{
			const auto& opt = cmd.options[0];

			if (!std::regex_match(cmd.arguments[1], amount_regex_obj))	// check argument amount
			{
				io.write_output("Expect a non-negative number for argument 2, but an incorrect one is supplied.\n");
			}
			else // arg2 is correct
			{
				double amount = std::atof(cmd.arguments[1].c_str());

				if (opt == "-a" || opt == "--accidental-income")
				{
					sys.add_accidental_income(amount, cmd.arguments[0]);
					io.write_output("Accidental income " + cmd.arguments[0] + " added successfully.\n");
				}
				else // check other arguments
				{
					if (cmd.arguments.size() == 2) // need at least 3 arguments now
					{
						io.write_output("Incorrect number of arguments! Please try again.\n");
					}
					else
					{
						financial_event::effective_duration type;

						int arg3 = std::atoi(cmd.arguments[2].c_str());
						if (arg3 < 0 || arg3 > 4) // arg3 is incorrect
						{
							io.write_output("Expect a number in the range [0,4] for argument 3, but an incorrect one is supplied.\n");
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
									io.write_output("Expect a date for argument 4, but an incorrect one is supplied.\n");
									return;
								}
								else if (start_date < sys.get_date())
								{
									io.write_output("Must provide a date later or equal to today for argument 4\n");
									return;
								}

								if (cmd.arguments.size() == 5) // arg5 is not default
								{
									simulation_end = string_to_date(cmd.arguments[3]);
									if (zero_date == simulation_end)
									{
										io.write_output("Expect a date for argument 5, but an incorrect one is supplied.\n");
										return;
									}
									else
									{
										auto sim_duration = simulation_end - sys.get_date();
										if (sim_duration < 365 || sim_duration > 365 * 5 + 1)
										{
											io.write_output("Simulation duration must be between 1 year to 5 years.\n");
											return;
										}
									}
								}
								else
								{
									simulation_end = sys.get_date();
									simulation_end.advance(default_simulation_duration);
								}
							}
							else
							{
								start_date = sys.get_date();
								simulation_end = sys.get_date();
								simulation_end.advance(default_simulation_duration);
							}

							if (opt == "-f" || opt == "--fixed-income")
							{
								if (sys.emplace_fixed_income(cmd.arguments[0], amount, start_date, type))
								{
									io.write_output("Fixed income " + cmd.arguments[0] + " added successfully.\n");
								}
								else
								{
									io.write_output("Cannot add fixed income " + cmd.arguments[0] + " because the name is already used.\n");
								}
							}
							else // a proposal is to be added
							{
								// start a simulation
								simulation sim(sys, simulation_end);

								if (opt == "-p" || opt == "--periodic-proposal")
								{
									sim.emplace_p_proposal(cmd.arguments[0], amount, start_date, type);
									sim.start_simulation();

									if (!sim.aborted) // safe state guaranteed
									{
										auto avg_money = std::to_string(sim.sim_avg_amount);
										io.write_output(
											"The program decides that the proposal can be added, and the average amount of money you can spend each day at most in one year (if the proposal was added ) would be " +
											avg_money + ".\n Proceed to add the proposal (Y/N)?\n");

										std::string reply;
										while (true)
										{
											reply = io.read_raw_input();
											if (reply == "y" || reply == "Y")
											{
												if (!sys.emplace_p_proposal(cmd.arguments[0], amount, start_date, type))
												{
													io.write_output("Cannot add the periodic proposal as the name is already used.\n");
												}
												else
												{
													io.write_output("Periodic proposal " + cmd.arguments[0] + " added successfully.\n");
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
												io.write_output("Please type one of y,Y,n,N.\n");
											}
										}
									}
									else // danger
									{
										io.write_output(
											"The program cannot allow the proposal to be added, because it calculated that at " +
											sim.aborted_date.to_string() + " the system would not be in a safe state with cash of only " +
											std::to_string(sim.aborted_cash) + " if the proposal was added.\n");
									}
								}
								else if (opt == "-o" || opt == "--one-time-proposal")
								{
									sim.emplace_ot_proposal(cmd.arguments[0], amount, start_date, type);
									sim.start_simulation();

									if (!sim.aborted) // safe state guaranteed
									{
										auto avg_money = std::to_string(sim.sim_avg_amount);
										io.write_output(
											"The program decides that the proposal can be added, and the average amount of money you can spend each day at most in one year is " +
											avg_money + ".\n Proceed to add the proposal (Y/N)?\n");

										std::string reply;
										while (true)
										{
											reply = io.read_raw_input();
											if (reply == "y" || reply == "Y")
											{
												if (!sys.emplace_ot_proposal(cmd.arguments[0], amount, start_date, type))
												{
													io.write_output("Cannot add the one-time proposal as the name is already used.\n");
												}
												else
												{
													io.write_output("One-time proposal " + cmd.arguments[0] + " added successfully.\n");
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
												io.write_output("Please type one of y,Y,n,N.\n");
											}
										}
									}
									else // danger
									{
										io.write_output(
											"The program cannot allow the proposal to be added, because it calculated that at " +
											sim.aborted_date.to_string() + " the system would not be in a safe state with cash of only " +
											std::to_string(sim.aborted_cash) + " if the proposal was added.\n");
									}
								}
								else
								{
									io.write_output("Incorrect option supplied! Please try again.\n");
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
		if (!system_loaded)
		{
			io.write_output("No system exists. Use init to create one first.\n");
		}
		else if (cmd.options.size() != 1)
		{
			io.write_output("Incorrect number of options! Please try again.\n");
		}
		else if (cmd.arguments.size() < 2 || cmd.arguments.size() > 3)
		{
			io.write_output("Incorrect number of arguments! Please try again.\n");
		}
		else
		{
			const auto& opt = cmd.options[0];

			if (!std::regex_match(cmd.arguments[1], amount_regex_obj))	// check argument amount
			{
				io.write_output("Expect a non-negative number for argument 2, but an incorrect one is supplied.\n");
			}
			else
			{
				double amount = std::atof(cmd.arguments[1].c_str());
				date simulation_end(sys.get_date());
				
				if (cmd.arguments.size() == 2) // default
				{
					simulation_end += default_simulation_duration;
				}
				else
				{
					if (!std::regex_match(cmd.arguments[2], integer_regex_obj))
					{
						io.write_output("Expect an integer for argument 3, but an incorrect one is supplied.\n");
						return;
					}
					else
					{
						int days = std::atoi(cmd.arguments[2].c_str());
						if (days < 365 || days > 365 * 5 + 1)
						{
							io.write_output("Simulation duration must be between 1 year and 5 years.\n");
							return;
						}
						else
						{
							simulation_end += days;
						}
					}
				}

				simulation sim(sys, simulation_end);

				if (opt == "-o" || opt == "--one-time-proposal")
				{
					auto pe = sys.edit_ot_proposal(cmd.arguments[0]);

					if (pe != nullptr)
					{
						if (amount > pe->amount) // possibility of endangering the system
						{
							sim.edit_ot_proposal(cmd.arguments[0])->amount = amount;
							sim.start_simulation();

							if (sim.aborted)
							{
								io.write_output("The program cannot allow the amount to be changed, because it calculated that at " +
									sim.aborted_date.to_string() + " the system would not be in a safe state with cash of only " +
									std::to_string(sim.aborted_cash) + " if the changed was passed.\n");
							}
							else
							{
								io.write_output(
									"The program decides that the changed can be passed, and the average amount of money you can spend each day at most in one year is " +
									std::to_string(sim.sim_avg_amount) + ".\n Proceed with the change (Y/N)?\n");

								std::string reply;
								while (true)
								{
									reply = io.read_raw_input();
									if (reply == "y" || reply == "Y")
									{
										pe->amount = amount;
										io.write_output("Amount set successfully\n");
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
							io.write_output("Amount set successfully\n");
						}
					}
				}
				if (opt == "-p" || opt == "--periodic-proposal")
				{
					auto pe = sys.edit_p_proposal(cmd.arguments[0]);

					if (pe != nullptr)
					{
						if (amount > pe->amount) // possibility of endangering the system
						{
							sim.edit_p_proposal(cmd.arguments[0])->amount = amount;
							sim.start_simulation();

							if (sim.aborted)
							{
								io.write_output("The program cannot allow the amount to be changed, because it calculated that at " +
									sim.aborted_date.to_string() + " the system would not be in a safe state with cash of only " +
									std::to_string(sim.aborted_cash) + " if the changed was passed.\n");
							}
							else
							{
								io.write_output(
									"The program decides that the changed can be passed, and the average amount of money you can spend each day at most in one year is " +
									std::to_string(sim.sim_avg_amount) + ".\n Proceed with the change (Y/N)?\n");

								std::string reply;
								while (true)
								{
									reply = io.read_raw_input();
									if (reply == "y" || reply == "Y")
									{
										pe->amount = amount;
										io.write_output("Amount set successfully\n");
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
							io.write_output("Amount set successfully\n");
						}
					}
				}
				if (opt == "-f" || opt == "--fixed-income")
				{
					auto pe = sys.edit_fixed_income(cmd.arguments[0]);

					if (pe != nullptr)
					{
						if (amount < pe->amount) // possibility of endangering the system
						{
							sim.edit_fixed_income(cmd.arguments[0])->amount = amount;
							sim.start_simulation();

							if (sim.aborted)
							{
								io.write_output("The program cannot allow the amount to be changed, because it calculated that at " +
									sim.aborted_date.to_string() + " the system would not be in a safe state with cash of only " +
									std::to_string(sim.aborted_cash) + " if the changed was passed.\n");
							}
							else
							{
								io.write_output(
									"The program decides that the changed can be passed, and the average amount of money you can spend each day at most in one year is " +
									std::to_string(sim.sim_avg_amount) + ".\n Proceed with the change (Y/N)?\n");

								std::string reply;
								while (true)
								{
									reply = io.read_raw_input();
									if (reply == "y" || reply == "Y")
									{
										pe->amount = amount;
										io.write_output("Amount set successfully\n");
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
							io.write_output("Amount set successfully\n");
						}
					}
				}
				else
				{
					io.write_output("Incorrect argument supplied\n");
				}
			}
		}
	}
	else if (cmd.name == "find")
	{
		if (!system_loaded)
		{
			io.write_output("No system exists. Use init to create one first.\n");
		}
		else if (cmd.arguments.size() != 1)
		{
			io.write_output("Incorrect number of arguments! Please try again.\n");
		}
		else if (cmd.options.size() != 1)
		{
			io.write_output("Incorrect number of options! Please try again.\n");
		}
		else
		{
			const auto& opt = cmd.options[0];
			if (opt == "-p" || opt == "--periodic-proposal")
			{
				auto pe = sys.find_p_proposal(cmd.arguments[0]);
				if (pe != nullptr)
				{
					io.write_output(std::string("Periodic Proposal: \n") + pe->to_string());
					io.write_output("\n");
				}
				else
				{
					io.write_output("Event not found.\n");
				}
			}
			else if (opt == "-o" || opt == "--one-time-proposal")
			{
				auto pe = sys.find_ot_proposal(cmd.arguments[0]);
				if (pe != nullptr)
				{
					io.write_output(std::string("One-time Proposal: \n") + pe->to_string());
					io.write_output("\n");
				}
				else
				{
					io.write_output("Event not found.\n");
				}
			}
			else if (opt == "-f" || opt == "--fixed-income")
			{
				auto pe = sys.find_fixed_income(cmd.arguments[0]);
				if (pe != nullptr)
				{
					io.write_output(std::string("Fixed income: \n") + pe->to_string());
					io.write_output("\n");
				}
				else
				{
					io.write_output("Event not found.\n");
				}
			}
		}
	}
	else if (cmd.name == "remove")
	{
		if (!system_loaded)
		{
			io.write_output("No system exists. Use init to create one first.\n");
		}
		else if (cmd.arguments.size() != 1)
		{
			io.write_output("Incorrect number of arguments! Please try again.\n");
		}
		else if (cmd.options.size() != 1)
		{
			io.write_output("Incorrect number of options! Please try again.\n");
		}
		else
		{
			const auto& opt = cmd.options[0];

			if (opt == "-p" || opt == "--periodic-proposal")
			{
				bool b = sys.remove_p_proposal(cmd.arguments[0]);
				if (b)
				{
					io.write_output(std::string("Periodic Proposal: ") + cmd.arguments[0] + " removed.\n");
				}
				else
				{
					io.write_output("Event not found.\n");
				}
			}
			else if (opt == "-o" || opt == "--one-time-proposal")
			{
				bool b = sys.remove_ot_proposal(cmd.arguments[0]);
				if (b)
				{
					io.write_output(std::string("One-time Proposal: ") + cmd.arguments[0] + " removed.\n");
				}
				else
				{
					io.write_output("Event not found.\n");
				}
			}
			else if (opt == "-f" || opt == "--fixed-income")
			{
				bool b = sys.remove_fixed_income(cmd.arguments[0]);
				if (b)
				{
					io.write_output(std::string("Fixed income: ") + cmd.arguments[0] + " removed.\n");
				}
				else
				{
					io.write_output("Event not found.\n");
				}
			}
		}
	}
	else if (cmd.name == "list")
	{
		if (!system_loaded)
		{
			io.write_output("No system exists. Use init to create one first.\n");
		}
		else if (cmd.arguments.size() != 0)
		{
			io.write_output("Incorrect number of arguments! Please try again.\n");
		}
		else if (cmd.options.size() != 1)
		{
			io.write_output("Incorrect number of options! Please try again.\n");
		}
		else
		{
			const auto& opt = cmd.options[0];
			if (opt == "-p" || opt == "--periodic-proposal")
			{
				if (sys.get_p_proposals().empty())
				{
					io.write_output("No periodic proposals exist.\n");
				}
				else
				{
					io.write_output(std::string("Periodic proposals: \n"));
					for (const auto& [n, e] : sys.get_p_proposals())
					{
						io.write_output(e.to_string());
						io.write_output("\n");
					}
				}
			}
			else if (opt == "-o" || opt == "--one-time-proposal")
			{
				if (sys.get_ot_proposals().empty())
				{
					io.write_output("No one-time proposals exist.\n");
				}
				else
				{
					io.write_output(std::string("One-time proposals: \n"));
					for (const auto& [n, e] : sys.get_ot_proposals())
					{
						io.write_output(e.to_string());
						io.write_output("\n");
					}					
				}
			}
			else if (opt == "-f" || opt == "--fixed-income")
			{
				if (sys.get_fixed_incomes().empty())
				{
					io.write_output("No fixed incomes exist.\n");
				}
				else
				{
					io.write_output(std::string("Fixed incomes: \n"));
					for (const auto& [n, e] : sys.get_fixed_incomes())
					{
						io.write_output(e.to_string());
						io.write_output("\n");
					}				
				}
			}
		}
	}
	else if (cmd.name == "simulate")
	{
		date sim_end;

		if (!system_loaded)
		{
			io.write_output("No system exists. Use init to create one first.\n");
		}
		else if (!cmd.options.empty())
		{
			io.write_output("Incorrect number of options! Please try again.\n");
		}
		else if (cmd.arguments.size() > 1)
		{
			io.write_output("Incorrect number of arguments! Please try again.\n");
		}
		else if (cmd.arguments.size() == 0) // use default value
		{
			sim_end = sys.get_date() + default_simulation_duration;
		}
		else // use argument
		{
			sim_end = string_to_date(cmd.arguments[0]);
			if (zero_date == sim_end)
			{
				io.write_output("Expect to have a date for arg1, but an incorrect one is supplied.\n");
				return;
			}
			else // make sure the duration is within our expectation
			{
				int days = sim_end - sys.get_date();
				if (days < 365 || days > 365 * 5 + 1) // out of range
				{
					io.write_output("The simulation duration must be between 1 year to 5 years.\n");
					return;
				}
			}
		}

		simulation sim(sys, sim_end);
		sim.start_simulation();

		if (sim.aborted)
		{
			io.write_output(
				"The program calculated that at " +
				sim.aborted_date.to_string() + " the system wwill not be in a safe state with cash of only " +
				std::to_string(sim.aborted_cash) + ".\n");
		}
		else
		{
			io.write_output(
				"The program guarantees that the system will remain in a safe state until " + sim_end.to_string() + ".\n"
			);
		}
	}
	else if (cmd.name == "predict")
	{
		date pre_end;

		if (!system_loaded)
		{
			io.write_output("No system exists. Use init to create one first.\n");
		}
		else if (!cmd.options.empty())
		{
			io.write_output("Incorrect number of options! Please try again.\n");
		}
		else if (cmd.arguments.size() > 1)
		{
			io.write_output("Incorrect number of arguments! Please try again.\n");
		}
		else if (cmd.arguments.size() == 0) // use default value
		{
			pre_end = sys.get_date() + default_prediction_duration;
		}
		else
		{
			pre_end = string_to_date(cmd.arguments[0]);
			if (zero_date == pre_end)
			{
				io.write_output("Expect to have a date for arg1, but an incorrect one is supplied.\n");
				return;
			}
			else // make sure the duration is within our expectation
			{
				int days = pre_end - sys.get_date();
				if (days < 30 || days > 365 ) // out of range
				{
					io.write_output("The prediction duration must be between 1 month to 1 year.\n");
					return;
				}
			}
		}

		double pre_num = sys.predict(pre_end);
		if (pre_num >= 0)
		{
			io.write_output("The program predicts that the approximate average amount of money you can use each day is " +
				std::to_string(pre_num) + " until " + pre_end.to_string() + ".\n");
		}
		else
		{
			io.write_output("The program approximately predicts that you can use no money and have to earn more until " + pre_end.to_string() + ".\n");
		}
	}
}

date money_manager::get_current_date()
{
	time_t cur_time = time(NULL);
	if (cur_time != (time_t)-1)
	{
		tm* ptm = gmtime(&cur_time);
		return date(ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
	}
	else // unexpected error
	{
		return zero_date;
	}
}
