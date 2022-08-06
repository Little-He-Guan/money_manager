#include "pch.h"
#include "SettingsPage.h"
#if __has_include("SettingsPage.g.cpp")
#include "SettingsPage.g.cpp"
#endif

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
    auto app = wux::Application::Current();

    if (app)
    {
        if (Dark_Mode_Toggle().IsOn())
        {
            app.RequestedTheme(wux::ApplicationTheme::Dark);
        }
        else
        {
            app.RequestedTheme(wux::ApplicationTheme::Light);
        }
    }

}


void winrt::main_GUI::implementation::SettingsPage::Version_Text_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    Version_Text().Text(winrt::hstring(L"Money Manager Version ") + winrt::to_hstring(money_manager::version_str));
}
