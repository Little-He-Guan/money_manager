#pragma once

/*
* Rep Invariant:
* year > 0 && 0 < month <= 12 && day is present for the year and month
* 
* Note: as date is only 64-bit on most implementations, passing by value is preferred.
*/
struct date
{
public:
	// don't know which valid date it should be; let it be the first day of the century
	constexpr date() : year(2000), month(1), day(1) {}
	constexpr date(unsigned in_year, unsigned short in_month, unsigned short in_day) : year(in_year), month(in_month), day(in_day) {}

	constexpr date(const date& other) : year(other.year), month(other.month), day(other.day) {}

	~date() = default;

	date& operator=(const date& right)
	{
		year = right.year;
		month = right.month;
		day = right.day;

		return *this;
	}
	
public:
	// @returns true iff the rep invariant holds
	constexpr bool valid() const
	{
		if (year == 0)
		{
			return false;
		}
		if (month == 0 || month > 12)
		{
			return false;
		}
		if (day == 0 || day > 31)
		{
			return false;
		}
		else if (day > 28) // 28 <= day <= 31
		{
			switch (day)
			{
			case 29:
				if (month == 2) // check if it is a leap year
				{
					if (!is_leap())
					{
						return false;
					}
				}
				break;
			case 30:
				if (month == 2)
				{
					return false;
				}
				break;
			case 31:
				if (month_days[month] != 31)
				{
					return false;
				}
				break;
			}
		}

		return true;
	}

public:
	/*
	* Advance the date by the time specified by the arguments.
	* If more than one argument is not 0, the function works as if advance(day); advance(0,month); advance(0,0,year); are being called in the order.
	* 
	* Note: if the date is invalid, then the behaviour is undefined.
	* 
	* @param year: increase the year member by the number, and may only modify the day member to 28 Feb. if the date is 29 Feb., which the target year does not have.
	* @param month: increase the month and year member in the normal way, and may only modify the day member if the current day is not present in the target month, 
	* which would result in day becoming 1 and month be added an additional 1 (i.e. going to the first day of the next month of the target month).
	* @param day: increases the day, month, and year member in the normal way.
	*/
	constexpr void advance(unsigned short in_day, unsigned short in_month = 0, unsigned short in_year = 0)
	{
		// first advance day
		{
			// remaining days
			int rem_d = (int)in_day - (int)((month != 2 ? month_days[month] : (is_leap() ? 29 : 28)) - day);

			if (rem_d > 0) // need to cross months, and even possibly years
			{
				// first we have already crossed one month
				if (month + 1 < 13)
				{
					month += 1;
				}
				else
				{
					month = 1;
					year += 1;
				}

				// see how many more months and years have to be crossed
				while (true)
				{
					if (month != 2) // we can use month_days now
					{
						if (rem_d > month_days[month])
						{
							rem_d -= month_days[month];

							if (month + 1 < 13)
							{
								month += 1;
							}
							else
							{
								month = 1;
								year += 1;
							}
						}
						else
						{
							break;
						}
					}
					else // we need to check on leap year
					{
						unsigned short feb_day = is_leap() ? 29 : 28;
						
						if (rem_d > feb_day)
						{
							rem_d -= feb_day;
							++month; // month = 3
						}
						else
						{
							break;
						}
					}
				}

				// if (rem_d == 0) // this is impossible as the condition rem_d > month_days[month], not >=
				day = rem_d;
			}
			else
			{
				day += in_day;
			}
		}

		// then advance month
		{
			month += in_month;

			while (month > 12) // cross years if necessary
			{
				month -= 12;
				++year;
			}

			// check if day is present in the target month
			if (month != 2) // we can use month_days now
			{
				if (day > month_days[month]) // it is not present
				{
					month += 1;

					if (month == 13)
					{
						month = 1;
						++year;
					}

					day = 1;
				}
			}
			else // we cannot use month_days
			{
				if (day > (is_leap() ? 29 : 28)) // it is not present
				{
					month += 1; // month = 3
					day = 1;
				}
			}
		}

		// finally advance year
		{
			year += in_year;

			// check if month:day is present in the target year
			if (month == 2 && day == 29) // this is the only possibility that it may not
			{
				if (!is_leap())
				{
					month = 3;
					day = 1;
				}
			}
		}
	}

	/*
	* Equals to calling advance(7 * week);
	*/
	constexpr void advance_weeks(unsigned short week) { advance(7 * week); }

	// @returns how many days d needs to advance to be equal to *this, or -(d - *this) if *this < d
	constexpr int operator-(date d) const;

public:
	enum seasons
	{
		Spring,
		Summer,
		Autumn,
		Winter
	};

	/*
	* Spring: {2,3,4}
	* Summer: {5,6,7}
	* Autumn: {8,9,10}
	* Winter: {11,12,1}
	* 
	* @returns which season the date belongs to, according to its month
	*/
	constexpr seasons which_season() const
	{
		if (2 <= month && month < 5)
		{
			return seasons::Spring;
		}
		// now either month < 2 or month >= 5
		else if (month < 2)
		{
			return seasons::Winter;
		}
		// now month >= 5
		else if (month < 8)
		{
			return seasons::Summer;
		}
		else if (month < 11)
		{
			return seasons::Autumn;
		}
		else
		{
			return seasons::Winter;
		}
	}

public:
	unsigned year;
	unsigned short month;
	unsigned short day;

private:
	// a constexpr array used to quickly know how many days a month has (do not use the data for Feb.)
	// We add 0 as its first element so that we can simply use month, not month-1, as the index
	static constexpr unsigned short month_days[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };

	// @returns true iff year is a leap year
	constexpr bool is_leap() const
	{
		if (year % 4 == 0)
		{
			if (year % 100 != 0)
			{
				return true;
			}
			else if (year % 400 == 0)
			{
				return true;
			}
		}

		return false;
	}
};

// recall that pass by value is preferred.

/*
* These together define a total ordering <=, and its assymmetric ordering >=, on date:
* 1. if years are different for two d1,d2, then d1 > or < d2 iff d1.year > or < d2.year
* 2. otherwise, the years are the same, then d1 > or < d2 iff d1.month > or < d2.month
* 3. otherwise, the years and the months are the same, then d1 > or < d2 iff d1.day > or < d2.day
* 4. d1 = d2 iff all components are equal correspondingly; d1 <= d2 iff d1<d2 || d1=d2
* 
* As the ordering on each component is a total ordering, it immediately follows that the ordering on the whole date is also a one.
* (One can think the date as a Cartesian product Y x M x D)
*/ 

constexpr bool operator==(date d1, date d2)
{
	return d1.year == d2.year && d1.month == d2.month && d1.day == d2.day;
}
constexpr bool operator!=(date d1, date d2)
{
	return d1.year != d2.year || d1.month != d2.month || d1.day != d2.day;
}
constexpr bool operator>=(date d1, date d2)
{
	if (d1.year > d2.year)
	{
		return true;
	}
	else if (d1.year < d2.year)
	{
		return false;
	}
	else // d1.year == d2.year
	{
		if (d1.month > d2.month)
		{
			return true;
		}
		else if (d1.month < d2.month)
		{
			return false;
		}
		else
		{
			return d1.day >= d2.day;
		}
	}
}
constexpr bool operator<=(date d1, date d2)
{
	if (d1.year < d2.year)
	{
		return true;
	}
	else if (d1.year > d2.year)
	{
		return false;
	}
	else // d1.year == d2.year
	{
		if (d1.month < d2.month)
		{
			return true;
		}
		else if (d1.month > d2.month)
		{
			return false;
		}
		else
		{
			return d1.day <= d2.day;
		}
	}
}
constexpr bool operator>(date d1, date d2)
{
	if (d1.year > d2.year)
	{
		return true;
	}
	else if (d1.year < d2.year)
	{
		return false;
	}
	else // d1.year == d2.year
	{
		if (d1.month > d2.month)
		{
			return true;
		}
		else if (d1.month < d2.month)
		{
			return false;
		}
		else
		{
			return d1.day > d2.day;
		}
	}
}
constexpr bool operator<(date d1, date d2)
{
	if (d1.year < d2.year)
	{
		return true;
	}
	else if (d1.year > d2.year)
	{
		return false;
	}
	else // d1.year == d2.year
	{
		if (d1.month < d2.month)
		{
			return true;
		}
		else if (d1.month > d2.month)
		{
			return false;
		}
		else
		{
			return d1.day < d2.day;
		}
	}
}

// @returns a date equals to d1.advance(d2.day, d2.month, d2.year)
constexpr date operator+(date d1, date d2)
{
	date res = d1;
	res.advance(d2.day, d2.month, d2.year);

	return res;
}
// d1 will be advanced as if calling d1.advance(d2.day, d2.month, d2.year)
// @returns a reference to d1
constexpr date& operator+=(date& d1, date d2)
{
	d1.advance(d2.day, d2.month, d2.year);
	return d1;
}

constexpr int date::operator-(date d) const
{
	if (*this < d)
	{
		return -(d - *this);
	}
	else
	{
		int res = 0;

		while (true)
		{
			if (d.year != year || d.month != month) // d is not in the same month as *this. try to advance a month and stop at the first day of the next month
			{
				if (d.month != 2) // can use month_days
				{
					res += (month_days[d.month] - d.day + 1); // + 1 as we will stop at the first day of the next month
				}
				else
				{
					res += ((d.is_leap() ? 29 : 28) - d.day + 1); // + 1 as we will stop at the first day of the next month
				}

				d.day = 1;
				// advance month by 1
				d.advance(0, 1);
				continue;
			}
			else // d is not in the same month as *this. only need to care about day
			{
				res += (day - d.day);
				break;
			}
		}

		return res;
	}
}

/*
* These mark important time points in a year.
* One can copy any of these and adjust the year as he wants.
*/
constexpr date
new_year_start(1, 1, 1),
spring_start(1, 2, 1),
summer_start(1, 5, 1),
autumn_start(1, 8, 1),
winter_start(1, 11, 1);