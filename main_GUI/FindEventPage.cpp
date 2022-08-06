#include "pch.h"
#include "FindEventPage.h"
#if __has_include("FindEventPage.g.cpp")
#include "FindEventPage.g.cpp"
#endif

#include "date_convertion.h"
#include "internal_sys.h"

#include "EventView.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::main_GUI::implementation
{
    FindEventPage::FindEventPage()
    {
        InitializeComponent();
    }

    int32_t FindEventPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void FindEventPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

}

void winrt::main_GUI::implementation::FindEventPage::update_event_list()
{
    if (Event_List())
    {
        int type = 3;
        std::string name(Search_Bar() ? winrt::to_string(Search_Bar().Text()) : "");

        if (Event_Type_Picker())
        {
            type = Event_Type_Picker().SelectedIndex();
        }
        else
        {
            //type = 3;
        }

        Event_List().Items().Clear();

        if (type == 0)
        {
            for (auto& [n, e] : g_mgr.sys.get_fixed_incomes())
            {
                winrt::com_ptr<main_GUI::implementation::EventView> imp = winrt::make_self< main_GUI::implementation::EventView>(e, 0);

                if (std::strstr(n.c_str(), name.c_str()) != nullptr) // also works if name is empty
                {
                    Event_List().Items().Append(*imp);
                }
            }
        }
        else if (type == 1)
        {
            for (auto& [n, e] : g_mgr.sys.get_ot_proposals())
            {
                winrt::com_ptr<main_GUI::implementation::EventView> imp = winrt::make_self< main_GUI::implementation::EventView>(e, 1);

                if (std::strstr(n.c_str(), name.c_str()) != nullptr) // also works if name is empty
                {
                    Event_List().Items().Append(*imp);
                }
            }
        }
        else if (type == 2)
        {
            for (auto& [n, e] : g_mgr.sys.get_p_proposals())
            {
                winrt::com_ptr<main_GUI::implementation::EventView> imp = winrt::make_self< main_GUI::implementation::EventView>(e, 2);

                if (std::strstr(n.c_str(), name.c_str()) != nullptr) // also works if name is empty
                {
                    Event_List().Items().Append(*imp);
                }
            }
        }
        else if (type == 3)
        {
            for (auto& [n, e] : g_mgr.sys.get_fixed_incomes())
            {
                winrt::com_ptr<main_GUI::implementation::EventView> imp = winrt::make_self< main_GUI::implementation::EventView>(e, 0);

                if (std::strstr(n.c_str(), name.c_str()) != nullptr) // also works if name is empty
                {
                    Event_List().Items().Append(*imp);
                }
            }
            for (auto& [n, e] : g_mgr.sys.get_ot_proposals())
            {
                winrt::com_ptr<main_GUI::implementation::EventView> imp = winrt::make_self< main_GUI::implementation::EventView>(e, 1);

                if (std::strstr(n.c_str(), name.c_str()) != nullptr) // also works if name is empty
                {
                    Event_List().Items().Append(*imp);
                }
            }
            for (auto& [n, e] : g_mgr.sys.get_p_proposals())
            {
                winrt::com_ptr<main_GUI::implementation::EventView> imp = winrt::make_self< main_GUI::implementation::EventView>(e, 2);

                if (std::strstr(n.c_str(), name.c_str()) != nullptr) // also works if name is empty
                {
                    Event_List().Items().Append(*imp);
                }
            }
        }
    }
}

void winrt::main_GUI::implementation::FindEventPage::update_event_data()
{
    if (Event_List())
    {
        main_GUI::implementation::EventView* pe = winrt::get_self<main_GUI::implementation::EventView>(Event_List().SelectedItem().as<main_GUI::EventView>());

        if (pe)
        {
            Amount_Input().Text(winrt::to_hstring(pe->p_event->amount));
            Actual_Input().Text(winrt::to_hstring(pe->p_event->actual));

            if (pe->p_event->end == cur_date + 1)
            {
                Actual_Button().IsEnabled(true);
            }
            else
            {
                Actual_Button().IsEnabled(false);
            }
            Amount_Button().IsEnabled(true);
            Remove_Button().IsEnabled(true);
        }
        else 
        {
            Amount_Input().Text(L"Select an event to view");
            Actual_Input().Text(L"Select an event to view");
            Amount_Button().IsEnabled(false);
            Actual_Button().IsEnabled(false);
            Remove_Button().IsEnabled(false);
        }
    }
}


void winrt::main_GUI::implementation::FindEventPage::Event_Type_Picker_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
{
    update_event_list();
}


void winrt::main_GUI::implementation::FindEventPage::Event_List_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
{
    update_event_data();
}


void winrt::main_GUI::implementation::FindEventPage::Amount_Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    if (std::regex_match(std::wstring(Amount_Input().Text()), double_wregex_obj))
    {
        double a = std::stof(std::wstring(Amount_Input().Text()));

        if (a > 0.0)
        {
            main_GUI::implementation::EventView* pe = winrt::get_self<main_GUI::implementation::EventView>(Event_List().SelectedItem().as<main_GUI::EventView>());
            if (pe) pe->p_event->amount = a;
        }

        update_event_list();
        update_event_data();
    }
}


void winrt::main_GUI::implementation::FindEventPage::Search_Bar_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::TextChangedEventArgs const& e)
{
    update_event_list();
}


void winrt::main_GUI::implementation::FindEventPage::Actual_Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    if (std::regex_match(std::wstring(Actual_Input().Text()), double_wregex_obj))
    {
        double a = std::stof(std::wstring(Actual_Input().Text()));

        if (a > 0.0)
        {
            main_GUI::implementation::EventView* pe = winrt::get_self<main_GUI::implementation::EventView>(Event_List().SelectedItem().as<main_GUI::EventView>());
            if(pe) pe->p_event->actual = a;
        }

        update_event_list();
        update_event_data();
    }
}


void winrt::main_GUI::implementation::FindEventPage::Event_List_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    update_event_list();
    update_event_data();
}


void winrt::main_GUI::implementation::FindEventPage::Remove_Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    main_GUI::implementation::EventView* pe = winrt::get_self<main_GUI::implementation::EventView>(Event_List().SelectedItem().as<main_GUI::EventView>());

    if (pe != nullptr)
    {
        switch (pe->Type())
        {
        case 0:
            g_mgr.sys.remove_fixed_income(*(pe->p_event->name));
            break;
        case 1:
            g_mgr.sys.remove_ot_proposal(*(pe->p_event->name));
            break;
        case 2:
            g_mgr.sys.remove_p_proposal(*(pe->p_event->name));
            break;
        }

        update_event_list();
    }

    Remove_Flyout().Hide();
}


void winrt::main_GUI::implementation::FindEventPage::Close_Flyout_Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    Remove_Flyout().Hide();
}
