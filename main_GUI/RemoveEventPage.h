#pragma once

#include "RemoveEventPage.g.h"

namespace winrt::main_GUI::implementation
{
    struct RemoveEventPage : RemoveEventPageT<RemoveEventPage>
    {
        RemoveEventPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct RemoveEventPage : RemoveEventPageT<RemoveEventPage, implementation::RemoveEventPage>
    {
    };
}
