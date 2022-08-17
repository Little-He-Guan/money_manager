#include "pch.h"
#include "HelpPage.h"
#if __has_include("HelpPage.g.cpp")
#include "HelpPage.g.cpp"
#endif

#include <winrt/Windows.System.h>

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::main_GUI::implementation
{
    HelpPage::HelpPage()
    {
        InitializeComponent();
    }

    int32_t HelpPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void HelpPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

}


void winrt::main_GUI::implementation::HelpPage::View_Manual_button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    display_user_manual();
}

winrt::fire_and_forget winrt::main_GUI::implementation::HelpPage::display_user_manual()
{
    auto manual{ co_await ws::StorageFile::GetFileFromApplicationUriAsync(wf::Uri(user_manual_uri_w)) };
    co_await Windows::System::Launcher::LaunchFileAsync(manual);
}
