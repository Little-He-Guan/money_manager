#pragma once

#include "date.h"
#include "event.h"

#include <string>
#include <map>

/*
* The ADT that represents the system (see the documentation)
*/
class financial_system
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
	financial_system() : 
		current_cash(0.0),
		expectation(0.0),
		current_date() {}
	financial_system(double c, double e, date d) :
		current_cash(c),
		expectation(e),
		current_date(d) {}

	financial_system(const financial_system& other) :
		current_date(other.current_date),
		current_cash(other.current_cash), expectation(other.expectation),
		ot_proposals(other.ot_proposals), p_proposals(other.p_proposals), incomes(other.incomes) {}

	virtual ~financial_system() = default;

public:
	// constructs an one-time proposal in-place with the name and the args
	// @returns true iff the proposal is successfully added (i.e. if the name is not occupied)
	template <typename... Args>
	bool emplace_ot_proposal(const std::string& name, Args&&... args)
	{
		auto p = ot_proposals.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(args..., name));
		if (p.second) // reassign name
		{
			p.first->second.name = &p.first->first;
		}
		return p.second;
	}
	// constructs a periodic proposal in-place with the name and the args
	// @returns true iff the proposal is successfully added (i.e. if the name is not occupied)
	template <typename... Args>
	bool emplace_p_proposal(const std::string& name, Args&&... args)
	{
		auto p = p_proposals.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(args..., name));
		if (p.second) // reassign name
		{
			p.first->second.name = &p.first->first;
		}
		return p.second;
	}
	// constructs a fixed income in-place with the name and the args
	// @returns true iff the proposal is successfully added (i.e. if the name is not occupied)
	template <typename... Args>
	bool emplace_fixed_income(const std::string& name, Args&&... args)
	{
		auto p = incomes.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(args..., name));
		if (p.second) // reassign name
		{
			p.first->second.name = &p.first->first;
		}
		return p.second;
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

	// @returns a pointer to the proposal if found, or nullptr otherwise
	const class financial_event* find_ot_proposal(const std::string& name);
	// @returns a pointer to the proposal if found, or nullptr otherwise
	const class periodic_event* find_p_proposal(const std::string& name);
	// @returns a pointer to the income if found, or nullptr otherwise
	const class periodic_event* find_fixed_income(const std::string& name);

public:
	double get_cash() const { return current_cash; }
	double get_expectation() const { return expectation; }
	date get_date() const { return current_date; }

	void set_expectation(double new_exp) { expectation = new_exp; }

public:
	// advances the date by 1 day and updates the system
	void advance_one_day();
	void advance_until(date d) { while (current_date < d) advance_one_day(); }

	bool in_safe_state() const { return current_cash >= expectation; }

protected:
	/*
	* records an event that has just took effect
	* (we do not record any that has been added but not have completed yet, as the money only changes when they becomes completed)
	* 
	* @param type: the event type
	* @param p_event: pointer to the event, or ignored if the event is an accidental income
	* @param ai_amount: amount of event if it is an accidental income, or ignored otherwise
	*/
	virtual void record_event(event_type type, const class financial_event* p_event, double ai_amount = 0.0);

protected:
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