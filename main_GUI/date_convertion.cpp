#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include "date_convertion.h"

#include <chrono>
#include <ctime>

date DateTime_to_date(const wwf::DateTime& t)
{
    std::time_t temp = winrt::clock::to_time_t(t);
    std::tm* temp2 = std::gmtime(&temp);

    return date(temp2->tm_year + 1900, temp2->tm_mon + 1, temp2->tm_mday);
}

wwf::DateTime date_to_DateTime(date t)
{
    std::tm temp;
    temp.tm_year = t.year - 1900;
    temp.tm_mon = t.month - 1;
    temp.tm_mday = t.day;
    // the rest we don't care
    temp.tm_hour = 0;
    temp.tm_min = 0;
    temp.tm_sec = 1;
    temp.tm_wday = 0;
  
    return winrt::clock::from_time_t(std::mktime(&temp));
}

wwf::DateTime current_DateTime()
{
    time_t cur_time = time(NULL);
    return winrt::clock::from_time_t(cur_time);
}

date cur_date = zero_date;
wwf::DateTime cur_DateTime;