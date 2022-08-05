#include "pch.h"
#include "SavePage.h"
#if __has_include("SavePage.g.cpp")
#include "SavePage.g.cpp"
#endif

#include "internal_sys.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::main_GUI::implementation
{
    SavePage::SavePage()
    {
        InitializeComponent();
    }

    int32_t SavePage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void SavePage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void SavePage::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        ::save_system_back_to_file_UWP();

        Msg_Text().Text(L"Save Successful!");
    }
}
