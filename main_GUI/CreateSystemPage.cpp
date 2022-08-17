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
    double cash = Cash_Input().Value(), exp = Expectation_Input().Value();

	if (cash >= exp && exp >= 0)
	{
		if (g_mgr.system_loaded) // need user approval if it's already loaded
		{
			approve_creation_if_sys_loaded(cash, exp);
		}
		else
		{
			g_mgr.sys = financial_system(cash, exp, ::cur_date);
			g_mgr.system_loaded.store(true);

			SET_SUCCESS_MESSAGE(Error_Message(), L"System Created!");
		}
	}
	else
	{
		SET_ERROR_MESSAGE(Error_Message(), L"Must have Cash >= Exp >= 0!");
	}
}

winrt::fire_and_forget winrt::main_GUI::implementation::CreateSystemPage::approve_creation_if_sys_loaded(double cash, double exp)
{
    auto dialog = wuxc::ContentDialog();
    dialog.Title(winrt::box_value(L"There is already a system."));
    dialog.Content(winrt::box_value(L"Create a new system when one is already present will override the present one and the operation cannot be undone. Do you wish to continue?"));
    dialog.PrimaryButtonText(L"Continue");
    dialog.CloseButtonText(L"Cancel");

    auto user_approval{ co_await dialog.ShowAsync() };

    if (user_approval == wuxc::ContentDialogResult::Primary) // override the system.
    {
        g_mgr.sys = financial_system(cash, exp, ::cur_date);
        g_mgr.system_loaded.store(true);

        SET_SUCCESS_MESSAGE(Error_Message(), L"System overridden.");
    }
    else
    {
        SET_ERROR_MESSAGE(Error_Message(), L"Operation cancelled.");
    }
}


void winrt::main_GUI::implementation::CreateSystemPage::Error_Message_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    if (g_mgr.system_loaded)
    {
        SET_ERROR_MESSAGE(Error_Message(), L"The system is already loaded. If you create a new one, the previous one will be overridden and cannot be recovered!");
    }
}
