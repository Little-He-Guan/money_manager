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
        auto update_save_state = [this, strong_this {get_strong()} /*Capture a strong this to keep this valid*/]() -> void
        {
            Msg_Text().Text(L"Save Successful!");
        };

        ::save_system_back_to_file_UWP(update_save_state);
    }
}
