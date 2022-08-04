#include "pch.h"

#include "money_manager.h"

#include <time.h>
#include <fstream>
#include <filesystem>

#include <stdexcept>

#ifdef _WIN32
#include <io.h>
#include < fcntl.h >
#endif // _WIN32

constexpr auto save_file_name = "save.sav";

namespace fs = std::filesystem;

constexpr auto sf_second_line_regex = "((?:[\\d]*[.])?[\\d]+) ((?:[\\d]*[.])?[\\d]+)";
const std::regex sf_second_line_regex_obj(sf_second_line_regex);

// line of an event. we do not use the precise regex for dates here, as it will be tested in string_to_date()
constexpr auto sf_event_line_regex = "([\\w]+) ([\\d\\-]+) ([\\d\\-]+) ((?:[\\d]*[.])?[\\d]+) ((?:[\\d]*[.])?[\\d]+) ([0-4])";
const std::regex sf_event_line_regex_obj(sf_event_line_regex);

const std::regex integer_regex_obj(integer_regex);

bool money_manager::load_from_file(std::string path, HANDLE hFile)
{
	if (!system_loaded)
	{
		fs::path save_path(path.empty() ? save_file_name : path);

		std::ifstream sf;

		if ((HANDLE)-1 == hFile)
		{
			if (!fs::exists(save_path)) // the file does not exist now.
			{
				// create the file
				std::ofstream(save_path, std::ios::out);
			}

			sf.open(save_path, std::ios::in);
		}
		else
		{
			int file_descriptor = _open_osfhandle((intptr_t)hFile, _O_RDONLY | _O_TEXT);

			if (file_descriptor != -1)
			{
				FILE* file = _fdopen(file_descriptor, "r");

				if (file != NULL)
				{
					sf = std::ifstream(file);
					sf.setf(std::ios::in);
				}
			}
		}
		
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

void money_manager::save_back_to_file(std::string path, HANDLE hFile)
{
	fs::path save_path(path.empty() ? save_file_name : path);

	if (system_loaded) // do not write the default constructed system to file
	{
		std::ofstream sf;

		if ((HANDLE)-1 == hFile)
		{
			// overwrite the file
			sf.open(save_path, std::ios::out | std::ios::trunc);
		}
		else
		{
			int file_descriptor = _open_osfhandle((intptr_t)hFile, _O_TEXT);

			if (file_descriptor != -1)
			{
				// overwrite the file
				FILE* file = _fdopen(file_descriptor, "a");

				if (file != NULL)
				{
					sf = std::ofstream(file);
					sf.setf(std::ios::out | std::ios::trunc);
				}
			}
		}

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
		system_loaded = true;
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
