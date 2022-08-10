#include "internal_sys.h"

#include "pch.h"

#include <Windows.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Security.Cryptography.h>

#include "date_convertion.h"

namespace ws = Windows::Storage;
namespace wfc = Windows::Foundation::Collections;
namespace wsc = Windows::Security::Cryptography;

money_manager g_mgr;

const std::wregex double_wregex_obj(::double_wregex);
const std::wregex integer_wregex_obj(::integer_wregex);

const std::wregex sf_second_line_wregex_obj(sf_second_line_wregex);
const std::wregex sf_event_line_wregex_obj(sf_event_line_wregex);

std::atomic_bool load_system_completed(false);

winrt::fire_and_forget load_system_from_file_UWP(std::function<void()> call_back)
{
    if (load_system_completed) // no need to reload
    {
        co_return; // do nothing
    }
    else
    {
        // capture calling thread context.
        winrt::apartment_context ui_thread;
        // and switch to a background thread
        co_await winrt::resume_background();

        // create the save file if it does not exist
        auto sav_file{ co_await ws::ApplicationData::Current().LocalFolder().CreateFileAsync(save_file_name_w, ws::CreationCollisionOption::OpenIfExists) };
        // read all lines from it
        auto lines{ co_await ws::FileIO::ReadLinesAsync(sav_file) };

        if (lines.Size() < 5) // no system saved in that file
        {
            load_system_completed.store(true);

            // reflect the system state by calling the callback (after switching the context)
            co_await ui_thread;
            if(call_back) call_back();

            co_return;
        }

        date saved_date;
        double cash, expectation;

        int num_ot_p, num_p_p, num_i;

        std::string name;
        date start, end;
        double amount, actual;
        financial_event::effective_duration type;

        std::wstring line;

        // first line: date
        {
            line = lines.GetAt(0);

            saved_date = string_to_date(winrt::to_string(line));

            if (zero_date == saved_date)
            {
                throw std::runtime_error("Unexpected error: save file corrupted");
            }
        }


        // second line: cash and expectation
        {
            line = lines.GetAt(1);

            std::wsmatch match;
            if (!std::regex_match(line, match, sf_second_line_wregex_obj))
            {
                throw std::runtime_error("Unexpected error: save file corrupted");
            }
            else
            {
                cash = std::stof(match[1].str());
                expectation = std::stof(match[2].str());
            }
        }

        // set what's already read
        g_mgr.sys.reset(cash, expectation, saved_date);

        // third line: number of ot-proposals
        {
            line = lines.GetAt(2);

            if (!std::regex_match(line, integer_wregex_obj))
            {
                throw std::runtime_error("Unexpected error: save file corrupted");
            }
            else
            {
                num_ot_p = std::stoi(line);
            }
        }

        // lines of ot-proposals
        {
            for (int i = 0; i != num_ot_p; ++i)
            {
                line = lines.GetAt(3 + i);

                std::wsmatch match;
                if (!std::regex_match(line, match, sf_event_line_wregex_obj))
                {
                    throw std::runtime_error("Unexpected error: save file corrupted");
                }
                else
                {
                    name = winrt::to_string(match[1].str());
                    start = string_to_date(winrt::to_string(match[2].str())); end = string_to_date(winrt::to_string(match[3].str()));
                    if (zero_date == start || zero_date == end)
                    {
                        throw std::runtime_error("Unexpected error: save file corrupted");
                    }
                    amount = std::stof(match[4].str());
                    actual = std::stof(match[5].str());
                    type = (financial_event::effective_duration)std::stoi(match[6].str());

                    g_mgr.sys.emplace_ot_proposal(name, amount, actual, start, end, type);
                }
            }
        }

        // number of p_proposals
        {
            line = lines.GetAt(3 + num_ot_p);

            if (!std::regex_match(line, integer_wregex_obj))
            {
                throw std::runtime_error("Unexpected error: save file corrupted");
            }
            else
            {
                num_p_p = std::stoi(line);
            }
        }

        // lines of p-proposals
        for (int i = 0; i != num_p_p; ++i)
        {
            line = lines.GetAt(3 + num_ot_p + 1 + i);

            std::wsmatch match;
            if (!std::regex_match(line, match, sf_event_line_wregex_obj))
            {
                throw std::runtime_error("Unexpected error: save file corrupted");
            }
            else
            {
                name = winrt::to_string(match[1].str());
                start = string_to_date(winrt::to_string(match[2].str())); end = string_to_date(winrt::to_string(match[3].str()));
                if (zero_date == start || zero_date == end)
                {
                    throw std::runtime_error("Unexpected error: save file corrupted");
                }
                amount = std::stof(match[4].str());
                actual = std::stof(match[5].str());
                type = (financial_event::effective_duration)std::stoi(match[6].str());

                g_mgr.sys.emplace_p_proposal(name, amount, actual, start, end, type);
            }
        }

        // number of p_proposals
        {
            line = lines.GetAt(3 + num_ot_p + 1 + num_p_p);

            if (!std::regex_match(line, integer_wregex_obj))
            {
                throw std::runtime_error("Unexpected error: save file corrupted");
            }
            else
            {
                num_i = std::stoi(line);
            }
        }

        // lines of p-proposals
        for (int i = 0; i != num_i; ++i)
        {
            line = lines.GetAt(3 + num_ot_p + 1 + num_p_p + 1 + i);

            std::wsmatch match;
            if (!std::regex_match(line, match, sf_event_line_wregex_obj))
            {
                throw std::runtime_error("Unexpected error: save file corrupted");
            }
            else
            {
                name = winrt::to_string(match[1].str());
                start = string_to_date(winrt::to_string(match[2].str())); end = string_to_date(winrt::to_string(match[3].str()));
                if (zero_date == start || zero_date == end)
                {
                    throw std::runtime_error("Unexpected error: save file corrupted");
                }
                amount = std::stof(match[4].str());
                actual = std::stof(match[5].str());
                type = (financial_event::effective_duration)std::stoi(match[6].str());

                g_mgr.sys.emplace_fixed_income(name, amount, actual, start, end, type);
            }
        }

        // test only. See what happens if the system takes very long to load.
        //Sleep(2000);

        // update the system state.
        {
            load_system_completed.store(true);
            g_mgr.system_loaded.store(true);
            if (zero_date != ::cur_date)
            {
                g_mgr.sys.advance_until(::cur_date);
            }
            else
            {
                set_cur_date(money_manager::get_current_date());
                g_mgr.sys.advance_until(::cur_date);
            }
        }

        // reflect the system state by calling the callback (after switching the context)
        co_await ui_thread;
        if (call_back) call_back();
    }

}

winrt::fire_and_forget save_system_back_to_file_UWP(std::function<void()> call_back)
{
    // capture calling thread context.
    winrt::apartment_context calling_thread;
    // and switch to a background thread
    co_await winrt::resume_background();

    // replace the file with a new one
    auto sav_file{ co_await Windows::Storage::ApplicationData::Current().LocalFolder().CreateFileAsync(save_file_name_w, ws::CreationCollisionOption::ReplaceExisting) };

    // do not try to write the file line by line using FileIO::writetext
    // or it would be a mess
    Windows::Foundation::Collections::IVector<winrt::hstring> lines{ winrt::single_threaded_vector< winrt::hstring>() };

    lines.Append(winrt::to_hstring(g_mgr.sys.get_date().to_string()));
    lines.Append(winrt::to_hstring(g_mgr.sys.get_cash()) + L" " + winrt::to_hstring(g_mgr.sys.get_expectation()));

    lines.Append(winrt::to_hstring(g_mgr.sys.get_ot_proposals().size()));
    for (const auto& [n, e] : g_mgr.sys.get_ot_proposals())
    {
        lines.Append(winrt::to_hstring(*e.name) + L" " + winrt::to_hstring(e.start.to_string()) + L" " + winrt::to_hstring(e.end.to_string()) + L" " + winrt::to_hstring(e.amount) + L" " + winrt::to_hstring(e.actual) + L" " + winrt::to_hstring((int)e.type));
    }

    lines.Append(winrt::to_hstring(g_mgr.sys.get_p_proposals().size()));
    for (const auto& [n, e] : g_mgr.sys.get_p_proposals())
    {
        lines.Append(winrt::to_hstring(*e.name) + L" " + winrt::to_hstring(e.start.to_string()) + L" " + winrt::to_hstring(e.end.to_string()) + L" " + winrt::to_hstring(e.amount) + L" " + winrt::to_hstring(e.actual) + L" " + winrt::to_hstring((int)e.type));
    }

    lines.Append(winrt::to_hstring(g_mgr.sys.get_fixed_incomes().size()));
    for (const auto& [n, e] : g_mgr.sys.get_fixed_incomes())
    {
        lines.Append(winrt::to_hstring(*e.name) + L" " + winrt::to_hstring(e.start.to_string()) + L" " + winrt::to_hstring(e.end.to_string()) + L" " + winrt::to_hstring(e.amount) + L" " + winrt::to_hstring(e.actual) + L" " + winrt::to_hstring((int)e.type));
    }

    co_await ws::FileIO::WriteLinesAsync(sav_file, lines);

    co_await calling_thread;
    if (call_back) call_back();
}

my_fire_and_forget record_event_UWP(financial_system::event_type type, const void* p_event, double amount)
{
    // switch to background non-UI thread
    co_await winrt::resume_background();

    auto log_file{ co_await Windows::Storage::ApplicationData::Current().LocalFolder().CreateFileAsync(log_file_name_w, ws::CreationCollisionOption::OpenIfExists) };

    Windows::Foundation::Collections::IVector<winrt::hstring> lines{ winrt::single_threaded_vector< winrt::hstring>() };
    
	winrt::hstring type_str;
	switch (type)
	{
	case financial_system::event_type::accidental_income:
		type_str = L"Accidental income ";
		break;
	case financial_system::event_type::fixed_income:
		type_str = L"Fixed income ";
		break;
	case financial_system::event_type::periodic_proposal:
		type_str = L"Periodic proposal ";
		break;
	case financial_system::event_type::one_time_proposal:
		type_str = L"One-time proposal ";
		break;
	}

	if (type == financial_system::event_type::accidental_income)
	{
       lines.Append(type_str + winrt::to_hstring(*reinterpret_cast<const std::string*>(p_event)) + L" at " + winrt::to_hstring(cur_date.to_string()) + L" +" + winrt::to_hstring(amount));
	}
	else
	{
		const auto& e = *reinterpret_cast<const financial_event*>(p_event);
		if (type == financial_system::event_type::fixed_income) // for incomes it's +
		{
            lines.Append(type_str + winrt::to_hstring(*e.name) + L" at " + winrt::to_hstring(cur_date.to_string()) + L" +" + winrt::to_hstring(amount));
		}
		else // for proposals it's -
		{
            lines.Append(type_str + winrt::to_hstring(*e.name) + L" at " + winrt::to_hstring(cur_date.to_string()) + L" -" + winrt::to_hstring(amount));
		}
	}

    co_await ws::FileIO::AppendLinesAsync(log_file, lines);
}

void register_event_record_handler_UWP()
{
    financial_system::record_handler_UWP = &record_event_UWP;
}