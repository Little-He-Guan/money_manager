#include "pch.h"
#include "SettingsPage.h"
#if __has_include("SettingsPage.g.cpp")
#include "SettingsPage.g.cpp"
#endif

#include "internal_sys.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

#include "../money_manager/money_manager.h"

namespace winrt::main_GUI::implementation
{
    SettingsPage::SettingsPage()
    {
        InitializeComponent();
    }

    int32_t SettingsPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void SettingsPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}


void winrt::main_GUI::implementation::SettingsPage::Dark_Mode_Toggle_Toggled(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    auto settings = ws::ApplicationData::Current().LocalSettings().Values();

    if (Dark_Mode_Toggle().IsOn() != (bool)winrt::unbox_value<int>(settings.Lookup(theme_settings_entry))) // the theme has changed
    {
        request_app_restart_to_apply_theme();
    }
}


void winrt::main_GUI::implementation::SettingsPage::Version_Text_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    Version_Text().Text(winrt::hstring(L"Money Manager Version ") + winrt::to_hstring(money_manager::version_str));
}

winrt::fire_and_forget winrt::main_GUI::implementation::SettingsPage::request_app_restart_to_apply_theme()
{
    auto dialog = wuxc::ContentDialog();
    dialog.Title(winrt::box_value(L"The app will be restarted."));
    dialog.Content(winrt::box_value(L"To apply a new theme, the application must be restarted. Please save your work before proceeding. Do you wish to continue?"));
    dialog.PrimaryButtonText(L"Continue");
    dialog.CloseButtonText(L"Cancel");

    auto user_approval{ co_await dialog.ShowAsync() };

    auto settings = ws::ApplicationData::Current().LocalSettings().Values();

    if (user_approval == wuxc::ContentDialogResult::Primary) // restart the app
    {
        if (Dark_Mode_Toggle().IsOn()) // dark mode
        {
            settings.Insert(theme_settings_entry, winrt::box_value(1));
        }
        else // light mode
        {
            settings.Insert(theme_settings_entry, winrt::box_value(0));
        }

        auto restart_result{ co_await wac::CoreApplication::RequestRestartAsync(L"") };
        if (restart_result == wac::AppRestartFailureReason::NotInForeground || restart_result == wac::AppRestartFailureReason::Other) // failed to restart
        {
            // send a notification to let the user restart the app themselves.
        }
    }
    else // cancelled
    {
        Dark_Mode_Toggle().IsOn((bool)winrt::unbox_value<int>(settings.Lookup(theme_settings_entry)));
    }
}


void winrt::main_GUI::implementation::SettingsPage::Dark_Mode_Toggle_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    auto settings = ws::ApplicationData::Current().LocalSettings().Values();
    Dark_Mode_Toggle().IsOn((bool)winrt::unbox_value<int>(settings.Lookup(theme_settings_entry)));
}
