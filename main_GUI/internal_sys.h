#pragma once

#include "../money_manager/money_manager.h"

#include <Windows.h>

extern money_manager g_mgr;

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

bool load_system_from_file_UWP();
void save_system_back_to_file_UWP();

void record_event_UWP(financial_system::event_type type, const void* p_event, double amount);
void register_event_record_handler_UWP();