#include "pch.h"

#include "system.h"

const std::filesystem::path log_file_path = std::filesystem::temp_directory_path().parent_path().parent_path() / "money_manager" / log_file_name;

my_fire_and_forget(*financial_system::record_handler_UWP)(financial_system::event_type, const void*, double) = nullptr;

financial_system& financial_system::operator=(const financial_system& right)
{
	ot_proposals = right.ot_proposals;
	p_proposals = right.p_proposals;
	incomes = right.incomes;

	current_date = right.current_date;
	current_cash = right.current_cash;
	expectation = right.expectation;

	return *this;
}

void financial_system::add_accidental_income(double value, const std::string& name)
{
	current_cash += value;
	record_event(event_type::accidental_income, &name, value);
}

const financial_event* financial_system::find_ot_proposal(const std::string& name) const
{
	auto iter = ot_proposals.find(name);
	return iter != ot_proposals.end() ? &iter->second : nullptr;
}

const periodic_event* financial_system::find_p_proposal(const std::string& name) const
{
	auto iter = p_proposals.find(name);
	return iter != p_proposals.end() ? &iter->second : nullptr;
}

const periodic_event* financial_system::find_fixed_income(const std::string& name) const
{
	auto iter = incomes.find(name);
	return iter != incomes.end() ? &iter->second : nullptr;
}

financial_event* financial_system::edit_ot_proposal(const std::string& name)
{
	auto iter = ot_proposals.find(name);
	return iter != ot_proposals.end() ? &iter->second : nullptr;
}
periodic_event* financial_system::edit_p_proposal(const std::string& name)
{
	auto iter = p_proposals.find(name);
	return iter != p_proposals.end() ? &iter->second : nullptr;
}
periodic_event* financial_system::edit_fixed_income(const std::string& name)
{
	auto iter = incomes.find(name);
	return iter != incomes.end() ? &iter->second : nullptr;
}

void financial_system::reset(double new_cash, double new_exp, date new_date)
{
	incomes.clear();
	ot_proposals.clear();
	p_proposals.clear();

	set_cash(new_cash);
	set_expectation(new_exp);
	current_date = new_date;
}

void financial_system::advance_one_day()
{
	current_date.advance(1);

	// as removals may be done, use iterators directly
	for (auto iter = ot_proposals.begin(); iter != ot_proposals.end();)
	{
		auto& e = iter->second;
		bool completed = e.update(current_date);

		if (completed)
		{
			current_cash -= e.actual;
			// record the event
			record_event(event_type::one_time_proposal, &e, e.actual);

			// remove the proposal from the the system
			// and update the iter
			iter = ot_proposals.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	for (auto& [n, e] : p_proposals)
	{
		// actual may be updated for period events. Keep a copy of it
		double prev_act = e.actual;
		if (e.update(current_date))
		{
			current_cash -= prev_act;
			record_event(event_type::periodic_proposal, &e, prev_act);
		}
	}
	for (auto& [n, e] : incomes)
	{
		// actual may be updated for period events. Keep a copy of it
		double prev_act = e.actual;
		if (e.update(current_date))
		{
			current_cash += prev_act;
			record_event(event_type::fixed_income, &e, prev_act);
		}
	}
}

double financial_system::predict(date end)
{
	double res = current_cash - expectation;
	double num_days = static_cast<double>(end - current_date);

	// step 1: counting periodic events
	{
		for (const auto& [n, e] : p_proposals)
		{
			double times = num_days / (double)financial_event::get_approx_length_in_days(e.type);
			res -= e.amount * times;
		}

		for (const auto& [n, e] : incomes)
		{
			double times = num_days / (double)financial_event::get_approx_length_in_days(e.type);
			res += e.amount * times;
		}
	}

	// step 2: counting one-time proposals (only the ones that will be completed within the time period)
	{
		for (const auto& [n, e] : ot_proposals)
		{
			if (e.end <= end)
			{
				double remaining_days = static_cast<double>(e.end - current_date);
				double times = remaining_days / (double)financial_event::get_approx_length_in_days(e.type);
				res -= e.amount * times;
			}
		}
	}

	return res / num_days;
}

std::string financial_system::to_string() const
{
	int num_possible_actuals = 0;

	for (const auto& [n, e] : p_proposals)
	{
		if (e.end == current_date + 1 && !e.actual_provided())
		{
			++num_possible_actuals;
		}
	}

	for (const auto& [n, e] : incomes)
	{
		if (e.end == current_date + 1 && !e.actual_provided())
		{
			++num_possible_actuals;
		}
	}

	for (const auto& [n, e] : ot_proposals)
	{
		if (e.end == current_date + 1 && !e.actual_provided())
		{
			++num_possible_actuals;
		}
	}

	return 
		std::string("Today is ") + current_date.to_string() + 
		".\nYou have currently " + std::to_string(current_cash) + " and your expectation is " + std::to_string(expectation) + 
		".\nThe system is " + (in_safe_state() ? "currently in a safe state" : "not in a safe state. Consider lower expenses") +
		(incomes.empty() ? ".\nYou have no fixed incomes, and the money cannot increase." : "") +
		(num_possible_actuals != 0 ? ".\nYou have " + std::to_string(num_possible_actuals) + " events that can be applied an acutal now." : "");
}

std::string financial_system::to_string_short() const
{
	return
		std::string("Current cash: ") + std::to_string(current_cash) +
		".\nThe system is " + (in_safe_state() ? "currently in a safe state" : "not in a safe state.");
}

void financial_system::record_event(event_type type, const void* p_event, double amount)
{
	if (record_handler_UWP != nullptr)
	{
		record_handler_UWP(type, p_event, amount);
	}
	else
	{
		std::ofstream lf(log_file_path, std::ios::out | std::ios::app | std::ios::ate);

		if (!lf.is_open())
		{
			throw std::runtime_error("Unexpected error: cannot open log file");
		}

		std::string type_str;
		switch (type)
		{
		case financial_system::event_type::accidental_income:
			type_str = "Accidental income ";
			break;
		case financial_system::event_type::fixed_income:
			type_str = "Fixed income ";
			break;
		case financial_system::event_type::periodic_proposal:
			type_str = "Periodic proposal ";
			break;
		case financial_system::event_type::one_time_proposal:
			type_str = "One-time proposal ";
			break;
		}

		if (type == financial_system::event_type::accidental_income)
		{
			lf << type_str << *reinterpret_cast<const std::string*>(p_event) << " at " << current_date.to_string() << " +" << amount << std::endl;
		}
		else
		{
			const auto& e = *reinterpret_cast<const financial_event*>(p_event);
			if (type == financial_system::event_type::fixed_income) // for incomes it's +
			{
				lf << type_str << *e.name << " at " << current_date.to_string() << " +" << amount << std::endl;
			}
			else // for proposals it's -
			{
				lf << type_str << *e.name << " at " << current_date.to_string() << " -" << amount << std::endl;
			}
		}

		lf.close();
	}
}

void init_directory()
{
	static auto path = std::filesystem::temp_directory_path().parent_path() / "money_manager";
	if (!std::filesystem::exists(path))
	{
		std::filesystem::create_directory(path);
	}
}
