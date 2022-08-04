#include "pch.h"

#include "event.h"

std::string financial_event::to_string() const
{
	return std::string(
		"Name: " + *name + 
		"\n\t Start: " + start.to_string() + " End: " + end.to_string() +
		"\n\t Amount: " + std::to_string(amount));
}

void financial_event::calculate_end()
{
	end =  financial_event::calculate_end(start, type);
}

date financial_event::calculate_end(date start, effective_duration type)
{
	date end = start;
	switch (type)
	{
	case daily:
		end.advance(1);
		break;

	case weekly:
		end.advance_weeks(1);
		break;

	case monthly:
		end.advance(0, 1);
		break;

	case seasonal:
		switch (end.which_season())
		{
		case date::Spring:
			end.month = summer_start.month;
			end.day = summer_start.day;
			break;
		case date::Summer:
			end.month = autumn_start.month;
			end.day = autumn_start.day;
			break;
		case date::Autumn:
			end.month = winter_start.month;
			end.day = winter_start.day;
			break;
		case date::Winter:
			end.month = spring_start.month;
			end.day = spring_start.day;
			// should go to the next year
			++end.year;
			break;
		}
		break;

	case annual:
		end.advance(0, 0, 1);
		break;
	}

	return end;
}

bool periodic_event::update(date d)
{
	if (d == end)
	{
		start = end;
		calculate_end();

		// an acutal is only applicable to a period, not to all.
		actual = amount;

		return true;
	}
	else
	{
		return false;
	}
}

std::string periodic_event::to_string() const
{
	std::string period_str;
	switch (type)
	{
	case daily:
		period_str = " daily ";
		break;
	case weekly:
		period_str = " weekly ";
		break;
	case monthly:
		period_str = " monthly ";
		break;
	case seasonal:
		period_str = " seasonal ";
		break;
	case annual:
		period_str = " annual ";
		break;
	}

	return financial_event::to_string() + "\n\t Period:" + period_str;
}
