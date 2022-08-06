#include "pch.h"
#include "CreateSystemPage.h"
#if __has_include("CreateSystemPage.g.cpp")
#include "CreateSystemPage.g.cpp"
#endif

#include "internal_sys.h"
#include "date_convertion.h"

using namespace winrt;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;

namespace winrt::main_GUI::implementation
{
    CreateSystemPage::CreateSystemPage()
    {
        InitializeComponent();
    }

    int32_t CreateSystemPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void CreateSystemPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}


void winrt::main_GUI::implementation::CreateSystemPage::Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    auto cash_str = Cash_Input().Text(), exp_str = Expectation_Input().Text();

    if (std::regex_match(std::wstring(cash_str), ::double_wregex_obj) && std::regex_match(std::wstring(exp_str), ::double_wregex_obj)) // correct input
    {
        double cash = std::stof(std::wstring(cash_str)), exp = std::stof(std::wstring(exp_str));
        if (cash >= exp && exp >= 0)
        {
            g_mgr.sys = financial_system(cash, exp, ::cur_date);
            g_mgr.system_loaded = true;

            Error_Message().Foreground(Media::SolidColorBrush(Colors::Green()));
            Error_Message().Text(L"System Created!");
        }
        else
        {
            Error_Message().Text(L"Must have Cash >= Exp >= 0!");
        }
    }
    else
    {
        Error_Message().Text(L"Invalid input!");
    }
}


void winrt::main_GUI::implementation::CreateSystemPage::Error_Message_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    if (g_mgr.system_loaded)
    {
        SET_ERROR_MESSAGE(Error_Message(), L"The system is already loaded. If you create a new one, the previous one will be overridden and cannot be recovered!");
    }
}
