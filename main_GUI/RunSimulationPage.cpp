#include "pch.h"
#include "RunSimulationPage.h"
#if __has_include("RunSimulationPage.g.cpp")
#include "RunSimulationPage.g.cpp"
#endif

#include "date_convertion.h"
#include "internal_sys.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::main_GUI::implementation
{
    RunSimulationPage::RunSimulationPage() : sim(g_mgr.sys, cur_date)
    {
        InitializeComponent();
    }

    int32_t RunSimulationPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void RunSimulationPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}


void winrt::main_GUI::implementation::RunSimulationPage::Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    if (sim.get_date() != cur_date) // simulation has already been run. create a new sim object
    {
        sim = simulation(g_mgr.sys, cur_date);
    }

    sim_results.clear();

    int duration = static_cast<int>(Dur_Slider_Input().Value());
    sim.end_date = sim.get_date() + duration;

    double total = 0.0;
    while (sim.get_date() < sim.end_date)
    {
        bool bSafeState = sim.in_safe_state();

        sim_results.push_back({ bSafeState, (int)sim.get_cash() });
        total += sim.get_cash() - sim.get_expectation();

        sim.advance_one_day();

        if (!bSafeState)
        {
            sim.aborted = true;
            break;
        }

    }

    if (sim.aborted)
    {
        SET_ERROR_MESSAGE(Sim_Res_Txt(), winrt::hstring(L"The system would not be in a safe state at ") + winrt::to_hstring(sim.get_date().to_string()));
    }
    else
    {
        total /= (double)(sim.get_date() - cur_date);
        SET_SUCCESS_MESSAGE(Sim_Res_Txt(), 
            winrt::hstring(L"The simulation does not find any danger. And the money you can use averagely (each day) during the simulation duration is ") + winrt::to_hstring(total));
    }

    // update the calendar
    // we must first set the date to a far away day to let the calendar can update all days near
    Calendar().SetDisplayDate(date_to_DateTime(cur_date + 365));
    // and then set it back
    Calendar().SetDisplayDate(cur_DateTime);
    // only after we do this can the fucking calendarview get the item's date correctly.
}


void winrt::main_GUI::implementation::RunSimulationPage::CalendarView_CalendarViewDayItemChanging(winrt::Windows::UI::Xaml::Controls::CalendarView const& sender, winrt::Windows::UI::Xaml::Controls::CalendarViewDayItemChangingEventArgs const& args)
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
        args.RegisterUpdateCallback({ this,&RunSimulationPage::CalendarView_CalendarViewDayItemChanging });
    }
    else if (args.Phase() == 1) // set black-out dates
    {
        // Register callback for next phase.
        args.RegisterUpdateCallback({ this,&RunSimulationPage::CalendarView_CalendarViewDayItemChanging });
    }
    else
    {
        auto diff = DateTime_to_date(args.Item().Date()) - cur_date;
        auto t = find_the_text_block(args);
        if (diff >= 0 && diff < sim_results.size()) // the day is in the simulation
        {
            args.Item().IsBlackout(false);
            t.Text(t.Text() + L"\nmoney\n" + std::to_wstring(sim_results[diff].second));
            args.Item().SetDensityColors({sim_results[diff].first ? wu::Colors::Green() : wu::Colors::Red()});
        }
        else
        {
            args.Item().IsBlackout(true);
        }
    }
}


void winrt::main_GUI::implementation::RunSimulationPage::Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    Dur_Slider_Input().Value(g_mgr.default_simulation_duration);
}

void winrt::main_GUI::implementation::RunSimulationPage::Dur_Txt_Input_KeyDown(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::KeyRoutedEventArgs const& e)
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
