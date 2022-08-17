#pragma once

#include "AddEventPage.g.h"

#include "../money_manager/simulation.h"
#include "internal_sys.h"

namespace winrt::main_GUI::implementation
{
    struct AddEventPage : AddEventPageT<AddEventPage>
    {
        AddEventPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void Event_Type_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e);

        /*
        * Updates the visual effect according to current user selection
        */
        void Update_State();
        void Duration_Type_Picker_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void Start_Date_Picker_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void End_Date_Picker_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        // called when add button is clicked
        void Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void CalendarView_CalendarViewDayItemChanging(winrt::Windows::UI::Xaml::Controls::CalendarView const& sender, winrt::Windows::UI::Xaml::Controls::CalendarViewDayItemChangingEventArgs const& args);
        // continue button
        void Button_Click_1(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void Name_Input_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::TextChangedEventArgs const& e);
        void Dur_Txt_Input_Value_Changed(winrt::Windows::Foundation::IInspectable const& sender, muxc::NumberBoxValueChangedEventArgs const& e);

    private:
        ::simulation sim;

        std::string name;
        double amount;
        date start;
        financial_event::effective_duration type;
        int sim_duration;
    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct AddEventPage : AddEventPageT<AddEventPage, implementation::AddEventPage>
    {
    };
}
