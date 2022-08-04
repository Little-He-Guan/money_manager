#pragma once

#include "AddEventPage.g.h"

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
    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct AddEventPage : AddEventPageT<AddEventPage, implementation::AddEventPage>
    {
    };
}
