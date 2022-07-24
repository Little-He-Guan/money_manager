// this file defines test cases for class date

#include "test_unit.h"
#include "../money_manager/date.h"

#include <iostream>

DEFINE_TEST_CASE(test_date_which_season)

	// spring
	{
		// boundary case
		ASSERT_EQUALS(date::Spring, date(111, 2, 1).which_season(), "expected to get the season right")
		ASSERT_EQUALS(date::Spring, date(111, 4, 30).which_season(), "expected to get the season right")

		// in the middle
		ASSERT_EQUALS(date::Spring, date(111, 3, 21).which_season(), "expected to get the season right")
	}

	// summer
	{
		// boundary case
		ASSERT_EQUALS(date::Summer, date(111, 5, 1).which_season(), "expected to get the season right")
		ASSERT_EQUALS(date::Summer, date(111, 7, 31).which_season(), "expected to get the season right")

		// in the middle
		ASSERT_EQUALS(date::Summer, date(111, 5, 3).which_season(), "expected to get the season right")
		ASSERT_EQUALS(date::Summer, date(111, 7, 6).which_season(), "expected to get the season right")
	}

	// autumn
	{
		// boundary case
		ASSERT_EQUALS(date::Autumn, date(111, 8, 1).which_season(), "expected to get the season right")
		ASSERT_EQUALS(date::Autumn, date(111, 10, 31).which_season(), "expected to get the season right")

		// in the middle
		ASSERT_EQUALS(date::Autumn, date(111, 9, 30).which_season(), "expected to get the season right")
		ASSERT_EQUALS(date::Autumn, date(111, 10, 15).which_season(), "expected to get the season right")
	}

	// winter
	{
		// boundary case
		ASSERT_EQUALS(date::Winter, date(111, 11, 1).which_season(), "expected to get the season right")
		ASSERT_EQUALS(date::Winter, date(111, 1, 31).which_season(), "expected to get the season right")

		// in the middle
		ASSERT_EQUALS(date::Winter, date(111, 1, 1).which_season(), "expected to get the season right")
		ASSERT_EQUALS(date::Winter, date(111, 12, 25).which_season(), "expected to get the season right")
	}

ENDDEF_TEST_CASE

// test if the total ordering works as expected
DEFINE_TEST_CASE(test_date_order)

	// equality and inequality
	{
		date d1(123, 4, 12), d2(123, 4, 12);

		ASSERT_TRUE(d1 == d1 && !(d1 != d1), "self-equality")

		ASSERT_TRUE(d1 == d2 && d2 == d1, "expected to have them equal")
		ASSERT_TRUE(!(d1 != d2) && !(d2 != d1), "expected to get false from !=")

		d1.day = 13;
		ASSERT_TRUE(d1 != d2 && d2 != d1, "expected to have them unequal")
		ASSERT_TRUE(!(d1 == d2) && !(d2 == d1), "expected to get false from ==")

		d2.day = 13;
		d2.month = 7;
		ASSERT_TRUE(d1 != d2 && d2 != d1, "expected to have them unequal")
		ASSERT_TRUE(!(d1 == d2) && !(d2 == d1), "expected to get false from ==")

		d1.month = 7;
		d1.year = 2001;
		ASSERT_TRUE(d1 != d2 && d2 != d1, "expected to have them unequal")
		ASSERT_TRUE(!(d1 == d2) && !(d2 == d1), "expected to get false from ==")
	}

	// ordering
	{
		date d1(2000, 4, 12), d2(2000, 4, 12);

		// equals
		ASSERT_TRUE(d1 <= d2, "expected to have the ordering right when they are equal")
		ASSERT_TRUE(d1 >= d2, "expected to have the ordering right when they are equal")

		// difference in year
		d1.year = 1000;
		ASSERT_TRUE(d1 < d2 && d1 <= d2 && d2 > d1 && d2 >= d1, "expected to have the ordering right")
		ASSERT_TRUE(!(d1 > d2) && !(d1 >= d2) &&!(d2 < d1) && !(d2 <= d1), "expected to have the ordering right")
		d1.month = 3; d1.day = 22;
		ASSERT_TRUE(d1 < d2 && d1 <= d2, "expected to have the ordering right")
		ASSERT_TRUE(!(d1 > d2) && !(d1 >= d2) && !(d2 < d1) && !(d2 <= d1), "expected to have the ordering right")
		d1.month = 5; d1.day = 20;
		ASSERT_TRUE(d1 < d2 && d1 <= d2, "expected to have the ordering right")
		ASSERT_TRUE(!(d1 > d2) && !(d1 >= d2) && !(d2 < d1) && !(d2 <= d1), "expected to have the ordering right")

		// difference in month
		d1.year = 2000;
		d2.month = 7;
		ASSERT_TRUE(d1 < d2 && d1 <= d2 && d2 > d1 && d2 >= d1, "expected to have the ordering right")
		ASSERT_TRUE(!(d1 > d2) && !(d1 >= d2) &&!(d2 < d1) && !(d2 <= d1), "expected to have the ordering right")
		d1.month = 3; d1.day = 5;
		ASSERT_TRUE(d1 < d2 && d1 <= d2, "expected to have the ordering right")
		ASSERT_TRUE(!(d1 > d2) && !(d1 >= d2) && !(d2 < d1) && !(d2 <= d1), "expected to have the ordering right")
		d2.month = 10; d2.day = 20;
		ASSERT_TRUE(d1 < d2 && d1 <= d2, "expected to have the ordering right")
		ASSERT_TRUE(!(d1 > d2) && !(d1 >= d2) && !(d2 < d1) && !(d2 <= d1), "expected to have the ordering right")

		// difference in day
		d1.month = d2.month = 6;
		ASSERT_TRUE(d1 < d2 && d1 <= d2 && d2 > d1 && d2 >= d1, "expected to have the ordering right")
		ASSERT_TRUE(!(d1 > d2) && !(d1 >= d2) &&!(d2 < d1) && !(d2 <= d1), "expected to have the ordering right")
		d1.month = d2.month = 1;
		d1.year = d2.year = 2020;
		d1.day = 1; d2.day = 31;
		ASSERT_TRUE(d1 < d2 && d1 <= d2, "expected to have the ordering right")
		ASSERT_TRUE(!(d1 > d2) && !(d1 >= d2) && !(d2 < d1) && !(d2 <= d1), "expected to have the ordering right")
		d1.day = 3; d2.day = 2;
		ASSERT_TRUE(d1 > d2 && d1 >= d2, "expected to have the ordering right")
		ASSERT_TRUE(!(d1 < d2) && !(d1 <= d2) && !(d2 > d1) && !(d2 >= d1), "expected to have the ordering right")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_date_advance_day)

	// advance some days that do not change month and year.
	{
		date d(3, 2, 1);

		d.advance(1);
		ASSERT_EQUALS(date(3, 2, 2), d, "expected to have only the day increased by 1")

		d = date(4, 2, 20); // leap year
		d.advance(9);
		ASSERT_EQUALS(date(4, 2, 29), d, "expected to have only the day increased by 9")

		d = date(341, 3, 20); // 31 days
		d.advance(11);
		ASSERT_EQUALS(date(341, 3, 31), d, "expected to have only the day increased by 11")
	}

	// advance some days that will change the month, not the year
	{
		date d(100, 2, 3); // special non-leap year

		// feb 28 or 29
		{
			d.advance(30);
			ASSERT_EQUALS(date(100, 3, 5), d, "expected to have the result date correct")

			d = date(400, 2, 10); // special leap year
			d.advance(20);
			ASSERT_EQUALS(date(400, 3, 1), d, "expected to have the result date correct")
		}

		// 30 or 31
		{
			d = date(2022, 7, 10); // 31 days
			d.advance(30);
			ASSERT_EQUALS(date(2022, 8, 9), d, "expected to have the result date correct")

			d = date(2022, 9, 20); // 30 days
			d.advance(20);
			ASSERT_EQUALS(date(2022, 10, 10), d, "expected to have the result date correct")
		}

		// across multiple months
		{
			d = date(16, 1, 11);
			d.advance(100);
			ASSERT_EQUALS(date(16, 4, 20), d, "expected to have the result date correct")

			d.month = 7;
			d.advance(61);
			ASSERT_EQUALS(date(16, 9, 19), d, "expected to have the result date correct")
		}
	}

	// advance some days that will possibly change all of the three
	{
		date d(2000, 2, 29);
		d.advance(365);
		ASSERT_EQUALS(date(2001, 2, 28), d, "expected to have the result date correct")

		d = date(2099, 8, 5);
		d.advance(210);
		ASSERT_EQUALS(date(2100, 3, 3), d, "expected to have the result date correct")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_date_advance_month)

	// advance some months that will not affect the day and the year
	{
		date d(123, 4, 5);

		d.advance(0, 1);
		ASSERT_EQUALS(date(123, 5, 5), d, "expected to have the result date correct")
	}

	// advance some months that will only affect the day (i.e. the day is not present in the target month)
	{
		date d(400, 1, 31); // special leap year

		// 31 to 30 or 28/29
		{
			d.advance(0, 1);
			ASSERT_EQUALS(date(400, 3, 1), d, "expected to have the result date correct")

			d.day = 31;
			d.advance(0, 3);
			ASSERT_EQUALS(date(400, 7, 1), d, "expected to have the result date correct")
		}

		// 30 to 28/29
		{
			d = date(2022, 1, 30);
			d.advance(0, 1);
			ASSERT_EQUALS(date(2022, 3, 1), d, "expected to have the result date correct")
		}

		// 29 to 28
		{
			d = date(2022, 1, 29);
			d.advance(0, 1);
			ASSERT_EQUALS(date(2022, 3, 1), d, "expected to have the result date correct")
		}


	}

	// across years
	{
		date d(16, 1, 11);
		d.advance(0, 12);
		ASSERT_EQUALS(date(17, 1, 11), d, "expected to have the result date correct")

		d.day = 31;
		d.advance(0, 32);
		ASSERT_EQUALS(date(19, 10, 1), d, "expected to have the result date correct")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_date_advance_year)

	// when day and month are not influenced (i.e. when it is not Feb. 29 to a non-leap year)
	{
		date d(999, 9, 9);

		d.advance(0, 0, 666);
		ASSERT_EQUALS(date(1665,9,9), d, "expected to have the result date correct")

		// leap year to leap year
		d = date(4, 2, 29);
		d.advance(0, 0, 396);
		ASSERT_EQUALS(date(400, 2, 29), d, "expected to have the result date correct")
	}

	// when day and month are not influenced (i.e. when it is Feb. 29 to a non-leap year)
	{
		date d(800, 2, 29); // special leap year
		
		d.advance(0, 0, 200);
		ASSERT_EQUALS(date(1000, 3, 1), d, "expected to have the result date correct")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_date_advance_all)

	/*
	* The advance method only specifies that it will have the same effect as calling advance on day, month, year sequentially, 
	* and we can only test it this way
	*/

	date d(1238,9,23);
	// expected
	date e = d;

	d.advance(34, 35, 123);
	e.advance(34); e.advance(0, 35); e.advance(0, 0, 123);

	ASSERT_EQUALS(e, d, "expected to have the same result")

	d.advance(724, 0, 17);
	e.advance(724); e.advance(0, 0); e.advance(0, 0, 17);

	ASSERT_EQUALS(e, d, "expected to have the same result")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_date_minus)

	// when *this < d
	{
		date d2(2024, 12, 3), d1(2022, 8, 9);

		ASSERT_EQUALS(-(d2 - d1), d1 - d2, "expected to get a negated value")
	}

	// when *this = d
	{
		date d(123, 1, 30);

		ASSERT_EQUALS(0, d - d, "expected to get 0")
	}

	// when *this > d
	{
		// when only day is different
		{
			ASSERT_EQUALS(15, date(2000,2,20) - date(2000,2,5), "expected to have the difference correct")
		}

		// when month is additionally different
		{
			ASSERT_EQUALS(89, date(2000, 9, 30) - date(2000, 7, 3), "expected to have the difference correct")
			// day is smaller
			ASSERT_EQUALS(39, date(1204, 3, 2) - date(1204, 1, 23), "expected to have the difference correct")
		}

		// when all are different
		{
			ASSERT_EQUALS(886, date(2022,12,4) - date(2020, 7,1), "expected to have the difference correct")
			// month is smaller
			ASSERT_EQUALS(40177, date(2111, 1, 1) - date(2000, 12, 31), "expected to have the difference correct")
		}
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_date_and_string)

	// date to string
	{
		date d(2001, 3, 12);
		ASSERT_EQUALS("3-12-2001", d.to_string(), "expected to have the string right")
	}

	// string to date
	{
		// invalid str
		ASSERT_EQUALS(zero_date, string_to_date("2-100-1020"), "expected to return the invalid date if the string is not in correct format")

		// normal MM-DD-YYYY
		ASSERT_EQUALS(date(1923, 11, 22), string_to_date("11-22-1923"), "expected to return have the date correct")
		ASSERT_EQUALS(date(1923, 1, 2), string_to_date("01-02-1923"), "expected to return have the date correct")

		// with other numbers of digits

		ASSERT_EQUALS(date(66666, 11, 22), string_to_date("11-22-66666"), "expected to return have the date correct")
		ASSERT_EQUALS(date(333, 9, 9), string_to_date("9-9-333"), "expected to return have the date correct")
	}

ENDDEF_TEST_CASE

void test_date()
{
	ghl::test_unit unit
	{
		{
			&test_date_which_season,
			&test_date_order,
			&test_date_advance_day,
			&test_date_advance_month,
			&test_date_advance_year,
			&test_date_advance_all,
			&test_date_minus,
			&test_date_and_string
		},
		"tests for class date"
	};

	unit.execute();

	std::cout << unit.get_msg() << "\n";
}