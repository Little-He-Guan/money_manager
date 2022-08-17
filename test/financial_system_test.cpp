#include "test_unit.h"

#include "../money_manager/system.h"

#include <iostream>

/*
* Assume that find, remove, get, set, and in_safe_state work correctly
*/

// tests for adding events
DEFINE_TEST_CASE(test_system_add)

	financial_system sys {};

	// adding an one-time proposal
	{
		ASSERT_TRUE(sys.emplace_ot_proposal(std::string("111"), 100.0, date(2000, 1, 11), financial_event::weekly),
			"expected to add successfully")

		ASSERT_TRUE(nullptr != sys.find_ot_proposal("111"), "expected to have that added")

		// adding another 
		ASSERT_TRUE(sys.emplace_ot_proposal(std::string("222"), 100.0, date(1919, 1, 11), financial_event::monthly),
			"expected to add successfully")
		auto p = sys.find_ot_proposal("222");
		ASSERT_TRUE(nullptr != p, "expected to have that added")
		ASSERT_EQUALS("222", p->name, "expected to add the one we wanted to add")

		// adding with a name already used
		ASSERT_FALSE(sys.emplace_ot_proposal(std::string("111"), 100.0, date(2022, 2, 2), financial_event::daily),
			"expected to not add it")
	}

	// adding a periodic proposal
	{
		ASSERT_TRUE(sys.emplace_p_proposal(std::string("111"), 100.0, date(2000, 1, 11), financial_event::weekly),
			"expected to add successfully")

		ASSERT_TRUE(nullptr != sys.find_p_proposal("111"), "expected to have that added")

		// adding another 
		ASSERT_TRUE(sys.emplace_p_proposal(std::string("222"), 100.0, date(1919, 1, 11), financial_event::monthly),
			"expected to add successfully")
		auto p = sys.find_p_proposal("222");
		ASSERT_TRUE(nullptr != p, "expected to have that added")
		ASSERT_EQUALS("222", p->name, "expected to add the one we wanted to add")

		// adding with a name already used
		ASSERT_FALSE(sys.emplace_p_proposal(std::string("111"), 100.0, date(2022, 2, 2), financial_event::daily),
			"expected to not add it")
	}

	// adding an income
	{
		ASSERT_TRUE(sys.emplace_fixed_income(std::string("111"), 100.0, date(2000, 1, 11), financial_event::weekly),
			"expected to add successfully")

		ASSERT_TRUE(nullptr != sys.find_fixed_income("111"), "expected to have that added")

		// adding another 
		ASSERT_TRUE(sys.emplace_fixed_income(std::string("222"), 100.0, date(1919, 1, 11), financial_event::monthly),
			"expected to add successfully")
		auto p = sys.find_fixed_income("222");
		ASSERT_TRUE(nullptr != p, "expected to have that added")
		ASSERT_EQUALS("222", p->name, "expected to add the one we wanted to add")

		// adding with a name already used
		ASSERT_FALSE(sys.emplace_fixed_income(std::string("111"), 100.0, date(2022, 2, 2), financial_event::daily),
			"expected to not add it")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_system_advance_day)

	financial_system sys(2000.0,500.0,date(2022,6,30));

	sys.emplace_fixed_income("123", 1000.0, date(2022, 6, 1), financial_event::monthly);
	sys.emplace_ot_proposal("111", 521.0, date(2022, 6, 30), financial_event::daily);
	sys.emplace_p_proposal("333", 999.0, date(2022, 6, 24), financial_event::weekly);

	// will not be completed
	sys.emplace_ot_proposal("222", 666.0, date(2022, 6, 25), financial_event::seasonal);

	sys.advance_one_day();

	ASSERT_EQUALS(date(2022, 7, 1), sys.get_date(), "expected to change to date")
	ASSERT_EQUALS(2000.0 + 1000.0 - 521.0 - 999.0 , sys.get_cash(), "expected to change to cash")

	ASSERT_EQUALS(nullptr, sys.find_ot_proposal("111"), "expected to remove all completed fixed proposals")

	ASSERT_EQUALS(date(2022, 7, 1), sys.find_fixed_income("123")->start, "expected to update the periodic events")
	ASSERT_EQUALS(date(2022, 7, 1), sys.find_p_proposal("333")->start, "expected to update the periodic events")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_system_predict)

	financial_system sys(3000.0, 1000.0, date(2022, 7, 23));

	sys.emplace_fixed_income("123", 2000.0, date(2022, 6, 1), financial_event::monthly);
	sys.emplace_fixed_income("456", 10000.0, date(2022, 7, 31), financial_event::annual);
	sys.emplace_p_proposal("111", 999.0, date(2022, 7, 11), financial_event::monthly);
	sys.emplace_p_proposal("333", 100.0, date(2022, 6, 29), financial_event::weekly);

	sys.emplace_ot_proposal("444", 1000.0, date(2022, 7, 22), financial_event::weekly);
	// will not be completed
	sys.emplace_ot_proposal("222", 666.0, date(2022, 6, 25), financial_event::annual);

	ASSERT_EQUALS((2000.0 * 31.0/30.0 + 10000.0 * 31.0/365.0 - 999.0 * 31.0/30.0 - 100.0 * 31.0/7.0 - 1000.0 * 6.0/7.0 + 3000.0 - 1000.0) / 31.0, sys.predict(date(2022, 8, 23)), "expected to have the amount right")

ENDDEF_TEST_CASE

void test_financial_system()
{
	ghl::test_unit unit
	{
		{
			&test_system_add,
			&test_system_advance_day,
			&test_system_predict
		},
		"tests for class financial_system"
	};

	unit.execute();

	std::cout << unit.get_msg() << "\n";
}