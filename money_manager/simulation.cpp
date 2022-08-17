#include "pch.h"

#include "simulation.h"

simulation& simulation::operator=(const simulation& right)
{
	financial_system::operator=(right);

	sim_results = right.sim_results;

	end_date = right.end_date;
	aborted_date = right.aborted_date;
	aborted_cash = right.aborted_cash;
	sim_avg_amount = right.sim_avg_amount;
	completed = right.completed;
	aborted = right.aborted;

	return *this;
}

void simulation::start_simulation()
{
	double initial_cash = current_cash;
	date initial_date = current_date;

	sim_results.clear();

	while (current_date < end_date)
	{
		bool b_safe_state = in_safe_state();

		sim_results.push_back({ b_safe_state, (int)get_cash() });
		advance_one_day();

		if (!b_safe_state)
		{
			aborted = true;
			aborted_cash = current_cash;
			aborted_date = current_date;
			break;
		}
	}

	completed = true;

	if (!aborted)
	{
		sim_avg_amount = (current_cash - expectation) / static_cast<double>(current_date - initial_date);
	}
}
