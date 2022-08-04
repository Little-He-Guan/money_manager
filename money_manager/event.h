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
		daily = 0,
		weekly = 1,
		monthly = 2,
		seasonal = 3,
		annual = 4
	};

public:
	// must provide sufficient info
	financial_event() = delete;
	financial_event(double a, date s, effective_duration t, const std::string& n):
		name(&n), amount(a), start(s), type(t)
	{
		actual = amount;
		calculate_end();
	}
	financial_event(double a, date s, date e, effective_duration t, const std::string& n) :
		name(&n), amount(a), start(s), end(e), type(t) 
	{
		actual = amount;
	}
	financial_event(double a, double ac, date s, date e, effective_duration t, const std::string& n) :
		name(&n), amount(a), start(s), end(e), type(t), actual(ac) {}

	// although no two events of the same kind should have the same name in A SYSTEM,
	// we may need to copy a system's state to run a simulation.
	financial_event(const financial_event& other) :
		name(other.name),
		amount(other.amount), actual(other.actual),
		type(other.type),
		start(other.start), end(other.end) {}
	// not useful for now
	financial_event& operator=(const financial_event&) = delete;

	financial_event(financial_event&& other) noexcept :
		name(other.name),
		amount(other.amount), actual(other.actual),
		type(other.type),
		start(other.start), end(other.end) {}
	// not useful for now
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

	// @returns true iff a different actual amount is provided for the event
	bool actual_provided() const { return actual != amount; }

public:
	virtual std::string to_string() const;

protected:
	// calculates end according to start and type
	void calculate_end();
public:
	// calculates end according to start and type
	static date calculate_end(date start, effective_duration type);

public:
	const std::string* name = nullptr;

	// effective duration is [start, end)
	date start, end;

	// the absolute value of the change to cash the event makes, and is used differently depending on the event type
	double amount;
	// the actual amount (see documentation)
	double actual;

	effective_duration type;

private:
	// the approximate length of the effective duration corresponding to each type
	static constexpr unsigned approx_length_in_days[5] = { 1,7,30,91,365 };
public:
	// @returns the approximate length of the effective duration corresponding to the type
	static constexpr unsigned get_approx_length_in_days(effective_duration type) { return approx_length_in_days[type]; }
};

// an event that happens periodically
class periodic_event : public financial_event
{
public:
	periodic_event(double a, date s, effective_duration t, const std::string& n) : financial_event(a, s, t, n) {}
	periodic_event(double a, date s, date e, effective_duration t, const std::string& n) : financial_event(a, s, e, t, n) {}
	periodic_event(double a, double ac, date s, date e, effective_duration t, const std::string& n) : financial_event(a, ac, s, e, t, n) {}

	periodic_event(const periodic_event& other) : financial_event(other) {}
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

	std::string to_string() const override;
};