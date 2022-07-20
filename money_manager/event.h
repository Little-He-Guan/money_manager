#pragma once

#include "date.h"

#include <string>

/*
* Represents a finacial event -- a proposal, or an income (see documentation for more info)
*/
class financial_event
{
public:
	enum effective_duration
	{
		daily,
		weekly,
		monthly,
		seasonal,
		annual
	};

public:
	// must provide sufficient info
	financial_event() = delete;
	financial_event(double a, date s, effective_duration t, const std::string& n):
		name(n), amount(a), start(s), type(t)
	{
		calculate_end();
	}
	financial_event(double a, date s, date e, effective_duration t, const std::string& n) :
		name(n), amount(a), start(s), end(e), type(t) {}

	// no two events (of the same kind) can have the same name!
	financial_event(const financial_event&) = delete;
	financial_event& operator=(const financial_event&) = delete;

	financial_event(financial_event&& other) noexcept :
		name(other.name),
		amount(other.amount),
		type(other.type),
		start(other.start), end(other.end)
	{}
	// once created, its name cannot be rebound!
	financial_event& operator=(financial_event&&) = delete;

	virtual ~financial_event() = default;

public:
	/*
	* updates the state of the event when the time has arrived to d.
	* If it is an one-time event, the function will do nothing
	* If it is a periodic event, the start and end will be updated to include d in its new duration
	* 
	* Note: the system must update the events day by day!
	* which means that, as all completed one-time events are removed from the system, and as all events are not completed when created,
	* that d <= end is always satisfied
	* 
	* @returns true iff the end has been reached
	*/
	virtual bool update(date d) { if (d == end) return true; else return false; }

protected:
	// calculates end according to start and type
	void calculate_end();

public:
	const std::string& name;

	// effective duration is [start, end)
	date start, end;

	// if cost > 0, it is an income; if < 0, it is a proposal.
	double amount;

	effective_duration type;
};

// an event that happens periodically
class periodic_event : public financial_event
{
public:
	periodic_event(double a, date s, effective_duration t, const std::string& n) : financial_event(a, s, t, n) {}
	periodic_event(double a, date s, date e, effective_duration t, const std::string& n) : financial_event(a, s, e, t, n) {}

	periodic_event(periodic_event&& other) noexcept : financial_event(std::move(other)) {}

	~periodic_event() = default;

public:
	/*
	* For periodic events, update will only update the start and end to match the next round if d = end 
	* (d can only <= end as the specification of the method in the superclass requires that updates are done day-by-day)
	* 
	* @returns true iff the end has been reached
	*/
	bool update(date d) override;
};