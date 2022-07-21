#include "simulation.h"

void simulation::start_simulation()
{
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
}
