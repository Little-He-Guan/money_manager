#include "date.h"

const std::regex date::date_str_regex_obj(date::date_str_regex);

date string_to_date(const std::string& str)
{
	std::smatch m;
	if (std::regex_match(str, m, date::date_str_regex_obj))
	{
		return date(atoi(m[3].str().c_str()), atoi(m[1].str().c_str()), atoi(m[2].str().c_str()));
	}
	else
	{
		return zero_date;
	}
}
