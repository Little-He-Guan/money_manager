#pragma once

#include "date.h"

#include <string>
#include <map>

/*
* The ADT that represents the system (see the documentation)
*/
class system
{
public:
	enum class event_type
	{
		one_time_proposal,
		periodic_proposal,
		fixed_income,
		accidental_income
	};

public:
	system() : current_cash(0.0), current_date() {}
	virtual ~system() {}

public:
	// constructs an one-time proposal in-place with the name and the args
	// @returns true iff the proposal is successfully added (i.e. if the name is not occupied)
	template <typename... Args>
	bool emplace_ot_proposal(const std::string& name, Args&&... args)
	{
		return ot_proposals.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(std::forward(args)..., name)).second;
	}
	// constructs a periodic proposal in-place with the name and the args
	// @returns true iff the proposal is successfully added (i.e. if the name is not occupied)
	template <typename... Args>
	bool emplace_p_proposal(const std::string& name, Args&&... args)
	{
		return p_proposals.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(std::forward(args)..., name)).second;
	}
	// constructs a fixed income in-place with the name and the args
	// @returns true iff the proposal is successfully added (i.e. if the name is not occupied)
	template <typename... Args>
	bool emplace_fixed_income(const std::string& name, Args&&... args)
	{
		return incomes.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(std::forward(args)..., name)).second;
	}

	void add_accidental_income(double value);

	// removes an one-time proposal from the system
	// @returns true iff the proposal is found and removed
	bool remove_ot_proposal(const std::string& name) { return (bool)ot_proposals.erase(name); }
	// removes a periodic proposal from the system
	// @returns true iff the proposal is found and removed
	bool remove_p_proposal(const std::string& name) { return (bool)p_proposals.erase(name); }
	// removes a fixed income from the system
	// @returns true iff the income is found and removed
	bool remove_fixed_income(const std::string& name) { return (bool)incomes.erase(name); }

public:
	// advances the date by 1 day and updates the system
	void advance_one_day();
	void advance_until(date d) { while (current_date < d) advance_one_day(); }

public:
	/*
	* records an event that has just took effect
	* (we do not record any that has been added but not have completed yet, as the money only changes when they becomes completed)
	* 
	* @param type: the event type
	* @param p_event: pointer to the event, or ignored if the event is an accidental income
	* @param ai_amount: amount of event if it is an accidental income, or ignored otherwise
	*/
	virtual void record_event(event_type type, const class financial_event* p_event, double ai_amount = 0.0);

private:
	// one-time proposals
	std::map<std::string, class financial_event> ot_proposals;
	// periodic proposals
	std::map<std::string, class periodic_event> p_proposals;
	// fixed incomes
	std::map<std::string, class periodic_event> incomes;

	date current_date;
	double current_cash;
	double expectation;
};