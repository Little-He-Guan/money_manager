#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

#include "date_convertion.h"
#include "internal_sys.h"

#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Storage.h>

namespace winrt::main_GUI::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();

        // initialize the dates
        set_cur_date(money_manager::get_current_date());
        set_cur_DateTime();

        // load the pages
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"State_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::StatePage>()));
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"Create_System_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::CreateSystemPage>()));
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"Save_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::SavePage>()));
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"Add_Event_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::AddEventPage>()));
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"Find_Event_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::FindEventPage>()));
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"Remove_Event_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::RemoveEventPage>()));
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"Run_Sim_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::RunSimulationPage>()));
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"Log_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::LogPage>()));
    }


    int32_t MainPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}


namespace wuxc = winrt::Windows::UI::Xaml::Controls;

void winrt::main_GUI::implementation::MainPage::Main_Menu_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    // initialize the system
    bool load_successful = ::load_system_from_file_UWP();
    // update the system state.
    if (load_successful)
    {
        g_mgr.system_loaded = true;
        if (zero_date != ::cur_date)
        {
            g_mgr.sys.advance_until(::cur_date);
        }
        else
        {
            throw std::runtime_error("Unexpected error: cannot get current date");
        }
    }

    // Add handler for ContentFrame navigation.
    ContentFrame().Navigated({ this, &MainPage::On_Navigated });

    // go to state page
    ContentFrame().Navigate(Windows::UI::Xaml::Interop::TypeName(winrt::xaml_typename<main_GUI::StatePage>()));
}


void winrt::main_GUI::implementation::MainPage::Main_Menu_SelectionChanged(winrt::Windows::UI::Xaml::Controls::NavigationView const& sender, winrt::Windows::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args)
{
    if (args.IsSettingsSelected())
    {
        Main_Menu_Navigate(L"settings", args.RecommendedNavigationTransitionInfo());
    }
    else if (args.SelectedItemContainer())
    {
        Main_Menu_Navigate(
            winrt::unbox_value_or<winrt::hstring>(args.SelectedItemContainer().Tag(), L"").c_str(),
            args.RecommendedNavigationTransitionInfo());
    }
}


void winrt::main_GUI::implementation::MainPage::Main_Menu_BackRequested(winrt::Windows::UI::Xaml::Controls::NavigationView const& sender, winrt::Windows::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const& args)
{
    TryGoBack();
}

void winrt::main_GUI::implementation::MainPage::System_BackRequested(
    Windows::Foundation::IInspectable const& /* sender */,
    Windows::UI::Core::BackRequestedEventArgs const& args)
{
    if (!args.Handled())
    {
        args.Handled(TryGoBack());
    }
}

void winrt::main_GUI::implementation::MainPage::On_Navigated(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Navigation::NavigationEventArgs const& args)
{
    if (ContentFrame().SourcePageType().Name ==
        winrt::xaml_typename<main_GUI::SettingsPage>().Name)
    {
        // SettingsItem is not part of NavView.MenuItems, and doesn't have a Tag.
        Main_Menu().SelectedItem(Main_Menu().SettingsItem().as<wuxc::NavigationViewItem>());
        Main_Menu().Header(winrt::box_value(L"Settings"));
    }
    else if (ContentFrame().SourcePageType().Name != L"")
    {
        for (const auto& [tag, name] : m_pages)
        {
            if (name.Name == args.SourcePageType().Name)
            {
                for (auto&& eachMenuItem : Main_Menu().MenuItems())
                {
                    auto navigationViewItem =
                        eachMenuItem.try_as<wuxc::NavigationViewItem>();
                    {
                        if (navigationViewItem)
                        {
                            winrt::hstring hstringValue = winrt::unbox_value_or<winrt::hstring>(navigationViewItem.Tag(), L"");
                            if (hstringValue == tag)
                            {
                                Main_Menu().SelectedItem(navigationViewItem);
                                Main_Menu().Header(navigationViewItem.Content());
                            }
                        }
                    }
                }
            }
        }
    }
}

void winrt::main_GUI::implementation::MainPage::Main_Menu_Navigate(std::wstring navItemTag, Windows::UI::Xaml::Media::Animation::NavigationTransitionInfo const& transitionInfo)
{
    Windows::UI::Xaml::Interop::TypeName pageTypeName;
    if (navItemTag == L"settings")
    {
        pageTypeName = winrt::xaml_typename<main_GUI::SettingsPage>();
    }
    else
    {
        // find the page by tag
        for (const auto& [tag, name] : m_pages)
        {
            if (tag == navItemTag)
            {
                pageTypeName = name;
                break;
            }
        }
    }

    // Get the page type before navigation so you can prevent duplicate
    // entries in the backstack.
    Windows::UI::Xaml::Interop::TypeName preNavPageType = ContentFrame().CurrentSourcePageType();

    // Navigate only if the selected page isn't currently loaded.
    if (pageTypeName.Name != L"" && preNavPageType.Name != pageTypeName.Name)
    {
        ContentFrame().Navigate(pageTypeName, nullptr, transitionInfo);
    }
}

bool winrt::main_GUI::implementation::MainPage::TryGoBack()
{
    if (!ContentFrame().CanGoBack())
    {
        return false;
    }
    else if (Main_Menu().IsPaneOpen() && (Main_Menu().DisplayMode() == wuxc::NavigationViewDisplayMode::Compact || Main_Menu().DisplayMode() == wuxc::NavigationViewDisplayMode::Minimal)) // Don't go back if the nav pane is overlayed.
    {
        return false;
    }

    ContentFrame().GoBack();
    return true;
}
