#include "event.h"

void financial_event::calculate_end()
{
	end = start;
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
}

bool periodic_event::update(date d)
{
	if (d == end)
	{
		start = end;
		calculate_end();

		return true;
	}
	else
	{
		return false;
	}
}