#pragma once

#include "../money_manager/money_manager.h"

#include "pch.h"

namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;

extern money_manager g_mgr;
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

constexpr auto theme_settings_entry = L"themeSettings";

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

// Loads sys from strings
// @returns true if it's loaded, or false if the strings do not describe a valid system.
bool load_system_from_strings_UWP(const wfc::IVector<winrt::hstring>& strings, financial_system& sys);

my_fire_and_forget record_event_UWP(financial_system::event_type type, const void* p_event, double amount);
void register_event_record_handler_UWP();

// switches the app's content to loading page
// @returns the previous content
void switch_to_loading_page();
// switches it back to main page
void switch_back_to_main_page();
// goes back to the previous page the app holds
void switch_back_to_previous_page();