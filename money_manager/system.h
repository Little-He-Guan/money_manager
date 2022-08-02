#pragma once

#include "date.h"
#include "event.h"

#include <string>
#include <map>
#include <filesystem>

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

	void add_accidental_income(double value, const std::string & name);

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
	const class financial_event* find_ot_proposal(const std::string& name) const;
	// @returns a pointer to the proposal if found, or nullptr otherwise
	const class periodic_event* find_p_proposal(const std::string& name) const;
	// @returns a pointer to the income if found, or nullptr otherwise
	const class periodic_event* find_fixed_income(const std::string& name) const;

	// @returns a pointer to the proposal if found, or nullptr otherwise
	class financial_event* edit_ot_proposal(const std::string& name);
	// @returns a pointer to the proposal if found, or nullptr otherwise
	class periodic_event* edit_p_proposal(const std::string& name);
	// @returns a pointer to the income if found, or nullptr otherwise
	class periodic_event* edit_fixed_income(const std::string& name);

public:
	double get_cash() const { return current_cash; }
	double get_expectation() const { return expectation; }
	date get_date() const { return current_date; }

	const auto& get_p_proposals() const { return p_proposals; }
	const auto& get_ot_proposals() const { return ot_proposals; }
	const auto& get_fixed_incomes() const { return incomes; }

	void set_expectation(double new_exp) { expectation = new_exp; }
	void set_cash(double new_cash) { current_cash = new_cash; }

	/*
	* Resets the system.
	* Clears all events and sets current_cash, expectation, and current_date to new_cash, new_exp, and new_date respectivelly.
	*/
	void reset(double new_cash, double new_exp, date new_date);

public:
	// advances the date by 1 day and updates the system
	void advance_one_day();
	// repeats advance_one_day until current date becomes d
	void advance_until(date d) { while (current_date < d) advance_one_day(); }

	bool in_safe_state() const { return current_cash >= expectation; }

public:
	// predicts the average amount of money one can spend from now to end.
	// Note: end must > current_date. otherwise the behaviour is undefined.
	// 
	// @returns the amount, or a minus number if the cash would not meet the expectation
	double predict(date end);

public:
	// @returns a string the describes the current state of the system.
	std::string to_string() const;
	// @returns a short version of the decription of the current state.
	std::string to_string_short() const;

protected:
	/*
	* records an event that has just took effect
	* (we do not record any that has been added but not have completed yet, as the money only changes when they becomes completed)
	* 
	* @param type: the event type
	* @param p_event: pointer to the event, or pointer to std::string (the event's name) if the event is an accidental income
	* @param amount: amount of event if it is an accidental income, or the actual of the event otherwise
	*/
	virtual void record_event(event_type type, const void* p_event, double amount);

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

constexpr auto log_file_name = "log.log";
extern const std::filesystem::path log_file_path;