#include "pch.h"
#include "StatePage.h"
#if __has_include("StatePage.g.cpp")
#include "StatePage.g.cpp"
#endif

#include "internal_sys.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::main_GUI::implementation
{
    StatePage::StatePage()
    {
        InitializeComponent();
    }

    int32_t StatePage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void StatePage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}


void winrt::main_GUI::implementation::StatePage::Grid_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{   
    Update_State();
}

void winrt::main_GUI::implementation::StatePage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e)
{
    Update_State();
}

void winrt::main_GUI::implementation::StatePage::Update_State()
{
	if (g_mgr.system_loaded)
	{
        SET_SUCCESS_MESSAGE(Sys_Loaded_Text(), L"System Loaded!");

		std::string state_str = g_mgr.sys.to_string();
		Sys_State_Text().Text(std::wstring(state_str.begin(), state_str.end()));
	}
	else
	{
        SET_ERROR_MESSAGE(Sys_Loaded_Text(), L"You have no system now. Create a new one to use the program.");
		Sys_State_Text().Text(L"System not Loaded.");
	}
}
