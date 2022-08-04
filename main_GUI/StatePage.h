#pragma once

#include "StatePage.g.h"

namespace winrt::main_GUI::implementation
{
    struct StatePage : StatePageT<StatePage>
    {
        StatePage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void Grid_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        void Update_State();
    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct StatePage : StatePageT<StatePage, implementation::StatePage>
    {
    };
}
