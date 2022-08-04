/*
* This file defines conversions between the money manager date class and the WinRT DateTime class
*/
#pragma once

#include "../money_manager/date.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Foundation.Collections.h>

namespace wwf = winrt::Windows::Foundation;

date DateTime_to_date(const wwf::DateTime& t);
wwf::DateTime date_to_DateTime(date t);
wwf::DateTime current_DateTime();

extern date cur_date;
extern wwf::DateTime cur_DateTime;

inline void set_cur_date(date d) { cur_date = d; }
inline void set_cur_DateTime() { cur_DateTime = current_DateTime(); }