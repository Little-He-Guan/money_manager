#include "system.h"

void financial_system::add_accidental_income(double value)
{
	current_cash += value;
	record_event(event_type::accidental_income, nullptr, value);
}

const financial_event* financial_system::find_ot_proposal(const std::string& name)
{
	auto iter = ot_proposals.find(name);
	return iter != ot_proposals.end() ? &iter->second : nullptr;
}

const periodic_event* financial_system::find_p_proposal(const std::string& name)
{
	auto iter = p_proposals.find(name);
	return iter != p_proposals.end() ? &iter->second : nullptr;
}

const periodic_event* financial_system::find_fixed_income(const std::string& name)
{
	auto iter = incomes.find(name);
	return iter != incomes.end() ? &iter->second : nullptr;
}

void financial_system::advance_one_day()
{
	current_date.advance(1);

	// as removals may be done, use iterators directly
	for (auto iter = ot_proposals.begin(); iter != ot_proposals.end(); ++iter)
	{
		auto& e = iter->second;
		bool completed = e.update(current_date);

		if (completed)
		{
			current_cash -= e.amount;
			// record the event
			record_event(event_type::one_time_proposal, &e);

			// remove the proposal from the the system
			// and update the iter
			iter = ot_proposals.erase(iter);
		}
	}

	for (auto& [n, e] : p_proposals)
	{
		if (e.update(current_date))
		{
			current_cash -= e.amount;
			record_event(event_type::periodic_proposal, &e);
		}
	}
	for (auto& [n, e] : incomes)
	{
		if (e.update(current_date))
		{
			current_cash += e.amount;
			record_event(event_type::fixed_income, &e);
		}
	}
}

void financial_system::record_event(event_type type, const financial_event* p_event, double ai_amount)
{
}
