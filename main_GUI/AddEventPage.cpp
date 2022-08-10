#include "pch.h"
#include "AddEventPage.h"
#if __has_include("AddEventPage.g.cpp")
#include "AddEventPage.g.cpp"
#endif

#include "date_convertion.h"
#include "../money_manager/event.h"
#include "../money_manager/simulation.h"

namespace winrt::main_GUI::implementation
{
    AddEventPage::AddEventPage()
    {
        InitializeComponent();
    }

    int32_t AddEventPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void AddEventPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

}


void winrt::main_GUI::implementation::AddEventPage::Event_Type_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
{
    // clears message text
    if(Error_Message()) Error_Message().Text(L"");

    Update_State();
}

void winrt::main_GUI::implementation::AddEventPage::Duration_Type_Picker_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
{
    Update_State();
}


void winrt::main_GUI::implementation::AddEventPage::Update_State()
{
    if (Event_Type_Picker())
    {
        if (Event_Type_Picker().SelectedIndex() == 0) // blackout date options as they are not needed by an accidental income
        {
            if (Duration_Type_Picker()) Duration_Type_Picker().IsEnabled(false);
            if (Start_Date_Picker()) Start_Date_Picker().IsEnabled(false);
            if (Dur_Txt_Input()) Dur_Txt_Input().IsEnabled(false);
            if (Dur_Slider_Input()) Dur_Slider_Input().IsEnabled(false);
            if (Continue_Prop_Button()) Continue_Prop_Button().IsEnabled(false);
        }
        else
        {
            if (Duration_Type_Picker()) Duration_Type_Picker().IsEnabled(true);
            if (Start_Date_Picker()) Start_Date_Picker().IsEnabled(true);

            if (End_Date_Picker()) // update end date according to type and start date
            {
                // first enable it to modify its date
                End_Date_Picker().IsEnabled(true);

                date start_date = DateTime_to_date(Start_Date_Picker().Date());
                date end_date = financial_event::calculate_end(start_date, (financial_event::effective_duration)Duration_Type_Picker().SelectedIndex());
                End_Date_Picker().Date(date_to_DateTime(end_date));

                End_Date_Picker().IsEnabled(false);
            }

            if (Event_Type_Picker().SelectedIndex() >= 2) // a proposal
            {
                if (Dur_Txt_Input()) Dur_Txt_Input().IsEnabled(true);
                if (Dur_Slider_Input()) Dur_Slider_Input().IsEnabled(true);
                // the button should only be activited when the simulation result approves it
                //if (Continue_Prop_Button()) Continue_Prop_Button().IsEnabled(true);
            }
            else
            {
                if (Dur_Txt_Input()) Dur_Txt_Input().IsEnabled(false);
                if (Dur_Slider_Input()) Dur_Slider_Input().IsEnabled(false);
                if (Continue_Prop_Button()) Continue_Prop_Button().IsEnabled(false);
            }
        }
    }
}


void winrt::main_GUI::implementation::AddEventPage::Start_Date_Picker_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    // show current date
    if (Start_Date_Picker())
    {
        Start_Date_Picker().Date(cur_DateTime);
    }

    Update_State();
}


void winrt::main_GUI::implementation::AddEventPage::End_Date_Picker_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    Update_State();
}


void winrt::main_GUI::implementation::AddEventPage::Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    Error_Message().Text(L"");

    name = winrt::to_string(Name_Input().Text());
    std::wstring input_amount(Amount_Input().Text());
    if (std::regex_match(input_amount, ::double_wregex_obj))
    {
        amount = std::stof(input_amount);
    }
    else
    {
        SET_ERROR_MESSAGE(Error_Message(), L"Invalid amount input.");
        return;
    }

    if (Event_Type_Picker().SelectedIndex() == 0) // acc income
    {
        g_mgr.sys.add_accidental_income(amount, name);
        SET_SUCCESS_MESSAGE(Error_Message(), L"Event added successfully!");
        return;
    }
    else
    {
        start = DateTime_to_date(Start_Date_Picker().Date());
        if (start < cur_date)
        {
            SET_ERROR_MESSAGE(Error_Message(), L"Start date must not be before today.");
            return;
        }

        type = (financial_event::effective_duration)Duration_Type_Picker().SelectedIndex();

        if (Event_Type_Picker().SelectedIndex() == 1) // fixed income
        {
            if (g_mgr.sys.emplace_fixed_income(name, amount, start, type))
            {
                SET_SUCCESS_MESSAGE(Error_Message(), L"Event added successfully!");
            }
            else
            {
                SET_ERROR_MESSAGE(Error_Message(), L"The name has already been used.");
            }
            return;
        }
        else // proposals
        {
            sim_duration = Dur_Slider_Input().Value();
            simulation sim(g_mgr.sys, cur_date + sim_duration);

            SET_SUCCESS_MESSAGE(Error_Message(), L"You are adding a proposal. Please switch to the simulation area for simulation result and to continue your operation.");

            if (Event_Type_Picker().SelectedIndex() == 2)
            {
                if (sim.emplace_ot_proposal(name, amount, start, type))
                {
                }
                else
                {
                    SET_ERROR_MESSAGE(Error_Message(), L"The name has already been used.");
                    return;
                }
            }
            else if (Event_Type_Picker().SelectedIndex() == 3)
            {
                if (sim.emplace_p_proposal(name, amount, start, type))
                {
                }
                else
                {
                    SET_ERROR_MESSAGE(Error_Message(), L"The name has already been used.");
                    return;
                }
            }

            // run the simulation
            {
                sim_results.clear();

                while (sim.get_date() < sim.end_date)
                {
                    sim.advance_one_day();
                    sim_results.push_back({ sim.in_safe_state(), (int)sim.get_cash() });

                    if (!sim.in_safe_state())
                    {
                        sim.aborted = true;
                        break;
                    }
                }
            }

            // update the calendar
            // we must first set the date to a far away day to let the calendar can update all days near
            Calendar().SetDisplayDate(date_to_DateTime(cur_date + 365));
            // and then set it back
            Calendar().SetDisplayDate(cur_DateTime);
            // only after we do this can the fucking calendarview get the item's date correctly.

            // now the user should focus his eyes to the simulation area
            // and we will activate the button accordingly
            if (!sim.aborted)
            {
                Proposal_CanContinue_Txt().Text(L"Simulation does not find any danger.");
                if (Continue_Prop_Button()) Continue_Prop_Button().IsEnabled(true);
            }
            else
            {
                Proposal_CanContinue_Txt().Text(L"The proposal cannot be added as simulation result does not approve it.");
                if (Continue_Prop_Button()) Continue_Prop_Button().IsEnabled(false);
            }
        }
    }

}

void winrt::main_GUI::implementation::AddEventPage::CalendarView_CalendarViewDayItemChanging(winrt::Windows::UI::Xaml::Controls::CalendarView const& sender, winrt::Windows::UI::Xaml::Controls::CalendarViewDayItemChangingEventArgs const& args)
{
    // use this shit to enable adding some text to some items
    static auto find_the_text_block = [](const decltype(args)& arg) -> auto
    {
        auto child_count = wuxm::VisualTreeHelper::GetChildrenCount(arg.Item());
        wuxc::TextBlock t;

        for (int i = 0; i < child_count; ++i)
        {
            auto child = wuxm::VisualTreeHelper::GetChild(arg.Item(), i);
            if (child.try_as<wuxc::TextBlock>(t)) // the control we want
            {
                return t;
            }
        }

        return t;
    };

    if (args.Phase() == 0) // don't render anything
    {
        // Register callback for next phase.
        args.RegisterUpdateCallback({ this,&AddEventPage::CalendarView_CalendarViewDayItemChanging });
    }
    else if (args.Phase() == 1) // set black-out dates
    {
        // Register callback for next phase.
        args.RegisterUpdateCallback({ this,&AddEventPage::CalendarView_CalendarViewDayItemChanging });
    }
    else
    {
        auto diff = DateTime_to_date(args.Item().Date()) - cur_date;
        auto t = find_the_text_block(args);
        if (diff >= 0 && diff < sim_results.size()) // the day is in the simulation
        {
            args.Item().IsBlackout(false);
            t.Text(t.Text() + L"\nmoney\n" + std::to_wstring(sim_results[diff].second));
            args.Item().SetDensityColors({ sim_results[diff].first ? wu::Colors::Green() : wu::Colors::Red() });
        }
        else
        {
            args.Item().IsBlackout(true);
        }
    }
}


void winrt::main_GUI::implementation::AddEventPage::Button_Click_1(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    if (Event_Type_Picker().SelectedIndex() == 2)
    {
        if (g_mgr.sys.emplace_ot_proposal(name, amount, start, type))
        {
            SET_SUCCESS_MESSAGE(Error_Message(), L"Event added successfully!");
        }
        else
        {
            SET_ERROR_MESSAGE(Error_Message(), L"The name has already been used.");
        }
    }
    else if (Event_Type_Picker().SelectedIndex() == 3)
    {
        if (g_mgr.sys.emplace_p_proposal(name, amount, start, type))
        {
            SET_SUCCESS_MESSAGE(Error_Message(), L"Event added successfully!");
        }
        else
        {
            SET_ERROR_MESSAGE(Error_Message(), L"The name has already been used.");
        }
    }
    else
    {
        SET_ERROR_MESSAGE(Error_Message(), L"Please do not change the selection");
    }

    Continue_Prop_Button().IsEnabled(false);
}


void winrt::main_GUI::implementation::AddEventPage::Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    Dur_Slider_Input().Value(g_mgr.default_simulation_duration);
}


void winrt::main_GUI::implementation::AddEventPage::Dur_Txt_Input_KeyDown(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::KeyRoutedEventArgs const& e)
{
    if (e.Key() == Windows::System::VirtualKey::Enter) // only when enter is pressed do we try to sync the value
    {
        std::wstring val_str(Dur_Txt_Input().Text());
        if (std::regex_match(val_str, integer_wregex_obj))
        {
            Dur_Slider_Input().Value(std::stoi(val_str));
        }
    }
}


void winrt::main_GUI::implementation::AddEventPage::Name_Input_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::TextChangedEventArgs const& e)
{
    // clears message text
    if (Error_Message()) Error_Message().Text(L"");
}
