#pragma once

#include "../money_manager/money_manager.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

#include <functional>

namespace wf = winrt::Windows::Foundation;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;

extern money_manager g_mgr;
extern std::atomic_bool load_system_completed;
extern std::atomic_bool save_system_completed;

constexpr auto integer_wregex = L"[\\d]+";
constexpr auto double_wregex = L"((?:[\\d]*[.])?[\\d]+)";

constexpr auto sf_second_line_wregex = L"((?:[\\d]*[.])?[\\d]+) ((?:[\\d]*[.])?[\\d]+)";
// line of an event. we do not use the precise regex for dates here, as it will be tested in string_to_date()
constexpr auto sf_event_line_wregex = L"([\\w]+) ([\\d\\-]+) ([\\d\\-]+) ((?:[\\d]*[.])?[\\d]+) ((?:[\\d]*[.])?[\\d]+) ([0-4])";

extern const std::wregex integer_wregex_obj;
extern const std::wregex double_wregex_obj;

extern const std::wregex sf_second_line_wregex_obj;
extern const std::wregex sf_event_line_wregex_obj;

constexpr auto save_file_name_w = L"save.sav";
constexpr auto log_file_name_w = L"log.log";

// UWP sucks with std::fstream :(
// We must instead use the system's APIs.

/*
* Loads the system asynchrounously from the file
* 
* @param call_back As the program may load the system from files multiple times, the change may needs to be refelcted somewhere on the UI.
* Thus a parameter of a callback introduced, which is called after all the work is completed and after the coroutine switches the thread back to the UI thread.
*/
winrt::fire_and_forget load_system_from_file_UWP(std::function<void()> call_back = {});
/*
* Saves the system asynchrounously back to file
* 
* @param call_back See above
*/
winrt::fire_and_forget save_system_back_to_file_UWP(std::function<void()> call_back = {});

my_fire_and_forget record_event_UWP(financial_system::event_type type, const void* p_event, double amount);
void register_event_record_handler_UWP();