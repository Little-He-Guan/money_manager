#pragma once

#include "SettingsPage.g.h"

namespace winrt::main_GUI::implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage>
    {
        SettingsPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void Dark_Mode_Toggle_Toggled(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void Version_Text_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        winrt::fire_and_forget request_app_restart_to_apply_theme();
    public:
        void Dark_Mode_Toggle_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage, implementation::SettingsPage>
    {
    };
}
