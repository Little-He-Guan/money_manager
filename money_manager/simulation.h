#pragma once

#include "system.h"
#include <vector>

class simulation : public financial_system
{
public:
	// a simulation must be based on a system.
	simulation() = delete;
	// simply copies the system's state
	simulation(const financial_system& sys, date simulate_until) : 
		financial_system(sys), 
		end_date(simulate_until) {}

	~simulation() = default;

public:
	void start_simulation();

private:
	// overridden to do nothing
	void record_event(event_type type, const class financial_event* p_event, double ai_amount) override {}

private:
	// the date until (not including) which the simulation will be done 
	date end_date;

	// if the simulation is aborted because in a day the system is not in a safe state,
	// the member will record when that date is.
	date aborted_date{};
	// the cash at the day the simulation is aborted
	double aborted_cash = 0.0;
	// has the simulation completed (true)
	bool completed = false;
	// is the simulated aborted (true) or exited normally (false)
	bool aborted = false;
};