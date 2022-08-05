#pragma once

#include "RunSimulationPage.g.h"

#include "../money_manager/simulation.h"

namespace winrt::main_GUI::implementation
{
    struct RunSimulationPage : RunSimulationPageT<RunSimulationPage>
    {
        RunSimulationPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void CalendarView_CalendarViewDayItemChanging(winrt::Windows::UI::Xaml::Controls::CalendarView const& sender, winrt::Windows::UI::Xaml::Controls::CalendarViewDayItemChangingEventArgs const& args);

    private:
        simulation sim;

        std::vector<std::pair<bool, int>> sim_results;

    public:
        void Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void Dur_Txt_Input_KeyDown(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::KeyRoutedEventArgs const& e);
    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct RunSimulationPage : RunSimulationPageT<RunSimulationPage, implementation::RunSimulationPage>
    {
    };
}
