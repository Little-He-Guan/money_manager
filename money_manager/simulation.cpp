#include "pch.h"

#include "simulation.h"

simulation& simulation::operator=(const simulation& right)
{
	financial_system::operator=(right);

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

	while (current_date < end_date)
	{
		advance_one_day();
		if (!in_safe_state())
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
