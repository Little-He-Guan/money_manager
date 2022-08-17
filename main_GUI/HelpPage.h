#pragma once

#include "HelpPage.g.h"

namespace winrt::main_GUI::implementation
{
    struct HelpPage : HelpPageT<HelpPage>
    {
        HelpPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void View_Manual_button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        // Displays the manual in the system pdf reader
        winrt::fire_and_forget display_user_manual();
    };
}

constexpr auto user_manual_uri_w = L"ms-appx:///User Manual.pdf";

namespace winrt::main_GUI::factory_implementation
{
    struct HelpPage : HelpPageT<HelpPage, implementation::HelpPage>
    {
    };
}
