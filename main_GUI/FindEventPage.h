#pragma once

#include "FindEventPage.g.h"

namespace winrt::main_GUI::implementation
{
    struct FindEventPage : FindEventPageT<FindEventPage>
    {
        FindEventPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct FindEventPage : FindEventPageT<FindEventPage, implementation::FindEventPage>
    {
    };
}
