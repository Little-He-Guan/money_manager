#include "pch.h"
#include "AddEventPage.h"
#if __has_include("AddEventPage.g.cpp")
#include "AddEventPage.g.cpp"
#endif

#include "date_convertion.h"
#include "../money_manager/event.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::main_GUI::implementation
{
    AddEventPage::AddEventPage()
    {
        InitializeComponent();
    }

    int32_t AddEventPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void AddEventPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

}


void winrt::main_GUI::implementation::AddEventPage::Event_Type_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
{
    Update_State();
}

void winrt::main_GUI::implementation::AddEventPage::Duration_Type_Picker_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
{
    Update_State();
}


void winrt::main_GUI::implementation::AddEventPage::Update_State()
{
    if (Event_Type_Picker())
    {
        if (Event_Type_Picker().SelectedIndex() == 0) // blackout date options as they are not needed by an accidental income
        {
            if (Duration_Type_Picker()) Duration_Type_Picker().IsEnabled(false);
            if (Start_Date_Picker()) Start_Date_Picker().IsEnabled(false);
            if (Sim_Duration_Input()) Sim_Duration_Input().IsEnabled(false);
        }
        else
        {
            if (Duration_Type_Picker()) Duration_Type_Picker().IsEnabled(true);
            if (Start_Date_Picker()) Start_Date_Picker().IsEnabled(true);
            if (Sim_Duration_Input()) Sim_Duration_Input().IsEnabled(true);

            if (End_Date_Picker()) // update end date according to type and start date
            {
                // first enable it to modify its date
                End_Date_Picker().IsEnabled(true);

                date start_date = DateTime_to_date(Start_Date_Picker().Date());
                date end_date = financial_event::calculate_end(start_date, (financial_event::effective_duration)Duration_Type_Picker().SelectedIndex());
                End_Date_Picker().Date(date_to_DateTime(end_date));

                End_Date_Picker().IsEnabled(false);
            }
        }
    }
}


void winrt::main_GUI::implementation::AddEventPage::Start_Date_Picker_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    // show current date
    if (Start_Date_Picker())
    {
        Start_Date_Picker().Date(cur_DateTime);
    }

    Update_State();
}


void winrt::main_GUI::implementation::AddEventPage::End_Date_Picker_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    Update_State();
}


void winrt::main_GUI::implementation::AddEventPage::Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{

}
