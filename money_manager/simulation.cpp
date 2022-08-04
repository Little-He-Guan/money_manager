#include "pch.h"

#include "simulation.h"

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
