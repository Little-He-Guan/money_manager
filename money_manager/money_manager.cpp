#include "pch.h"

#include "money_manager.h"

#include <time.h>

#include <filesystem>

#include <stdexcept>

#include <sstream>

namespace fs = std::filesystem;

constexpr auto save_file_name = "save.sav";
const fs::path save_file_path = fs::temp_directory_path().parent_path().parent_path() / "money_manager" / save_file_name;

constexpr auto sf_second_line_regex = "((?:[\\d]*[.])?[\\d]+) ((?:[\\d]*[.])?[\\d]+)";
const std::regex sf_second_line_regex_obj(sf_second_line_regex);

// line of an event. we do not use the precise regex for dates here, as it will be tested in string_to_date()
constexpr auto sf_event_line_regex = "([\\w]+) ([\\d\\-]+) ([\\d\\-]+) ((?:[\\d]*[.])?[\\d]+) ((?:[\\d]*[.])?[\\d]+) ([0-4])";
const std::regex sf_event_line_regex_obj(sf_event_line_regex);

const std::regex integer_regex_obj(integer_regex);

bool money_manager::load_from_file(std::string path)
{
	if (!system_loaded)
	{
		if (!fs::exists(save_file_path))
		{
			// create the file if not exist
			std::ofstream o(save_file_path);
			o.close();
		}

		std::ifstream sf(save_file_path, std::ios::in);
		
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
				double amount, actual;
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
							actual = std::atof(match[5].str().c_str());
							type = (financial_event::effective_duration)std::atoi(match[6].str().c_str());

							sys.emplace_ot_proposal(name, amount, actual, start, end, type);
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
							actual = std::atof(match[5].str().c_str());
							type = (financial_event::effective_duration)std::atoi(match[6].str().c_str());

							sys.emplace_p_proposal(name, amount, actual, start, end, type);
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
							actual = std::atof(match[5].str().c_str());
							type = (financial_event::effective_duration)std::atoi(match[6].str().c_str());

							sys.emplace_fixed_income(name, amount, actual, start, end, type);
						}
					}
				}

				sf.close();

				return true;
			}
		}
	}
	else
	{
		return true;
	}

	return false;
}

void money_manager::save_back_to_file(std::string path)
{
	if (system_loaded) // do not write the default constructed system to file
	{
		// overwrite the file
		std::ofstream sf(save_file_path, std::ios::out | std::ios::trunc);

		// output money precisely.
		sf << std::fixed;

		sf << sys.get_date().to_string() << std::endl;
		sf << sys.get_cash() << " " << sys.get_expectation() << std::endl;

		sf << sys.get_ot_proposals().size() << std::endl;
		for (const auto& [n, e] : sys.get_ot_proposals())
		{
			sf << *e.name << " " << e.start.to_string() << " " << e.end.to_string() << " " << std::to_string(e.amount) << " " << std::to_string(e.actual) << " " << (int)e.type << std::endl;
		}

		sf << sys.get_p_proposals().size() << std::endl;
		for (const auto& [n, e] : sys.get_p_proposals())
		{
			sf << *e.name << " " << e.start.to_string() << " " << e.end.to_string() << " " << std::to_string(e.amount) << " " << std::to_string(e.actual) << " " << (int)e.type << std::endl;
		}

		sf << sys.get_fixed_incomes().size() << std::endl;
		for (const auto& [n, e] : sys.get_fixed_incomes())
		{
			sf << *e.name << " " << e.start.to_string() << " " << e.end.to_string() << " " << std::to_string(e.amount) << " " << std::to_string(e.actual) << " " << (int)e.type << std::endl;
		}

		sf.close();
	}
}

bool money_manager::start()
{
	bool load_successful = load_from_file();

	// update the system state.
	if (load_successful)
	{
		system_loaded.store(true);
		date cur_date = get_current_date();
		if (zero_date != cur_date)
		{
			sys.advance_until(cur_date);
		}
		else
		{
			throw std::runtime_error("Unexpected error: cannot get current date");
		}
	}

	return load_successful;
}

void money_manager::shut_down()
{
	save_back_to_file();
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

std::string double_to_string_high_precision(double d)
{
	std::ostringstream ss;
	ss << std::fixed;
	ss << d;
	return ss.str();
}
