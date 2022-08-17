#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

#include "date_convertion.h"
#include "internal_sys.h"

namespace winrt::main_GUI::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();

        // load the pages
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"State_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::StatePage>()));
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"Create_System_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::CreateSystemPage>()));
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"Save_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::SavePage>()));
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"Add_Event_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::AddEventPage>()));
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"Find_Event_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::FindEventPage>()));
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"Run_Sim_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::RunSimulationPage>()));
        m_pages.emplace_back(std::piecewise_construct, std::forward_as_tuple(L"Log_Page"), std::forward_as_tuple(winrt::xaml_typename<main_GUI::LogPage>()));
    }

}

void winrt::main_GUI::implementation::MainPage::Main_Menu_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    // Add handler for ContentFrame navigation.
    ContentFrame().Navigated({ this, &MainPage::On_Navigated });

    // go to state page
    ContentFrame().Navigate(Windows::UI::Xaml::Interop::TypeName(winrt::xaml_typename<main_GUI::StatePage>()));

    // if it's first launch, open the teaching tip.
    auto settings = ws::ApplicationData::Current().LocalSettings().Values();
    if (winrt::unbox_value<int>(settings.Lookup(first_launch_entry)) == 1)
    {
        FirstUseTip1().IsOpen(true);
        settings.Insert(first_launch_entry, winrt::box_value(0));
    }
    else
    {
        FirstUseTip1().IsOpen(false);
    }
}


void winrt::main_GUI::implementation::MainPage::Main_Menu_SelectionChanged(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, muxc::NavigationViewSelectionChangedEventArgs const& args)
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


void winrt::main_GUI::implementation::MainPage::Main_Menu_BackRequested(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, muxc::NavigationViewBackRequestedEventArgs const& args)
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
        Main_Menu().SelectedItem(Main_Menu().SettingsItem().as<muxc::NavigationViewItem>());
        Main_Menu().Header(winrt::box_value(L"Settings"));
    }
    else if (ContentFrame().SourcePageType().Name == winrt::xaml_typename<main_GUI::HelpPage>().Name)
    {
		Main_Menu().SelectedItem(HelpItem());
		Main_Menu().Header(HelpItem().Content());
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
                        eachMenuItem.try_as<muxc::NavigationViewItem>();
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
    else if (Main_Menu().IsPaneOpen() && (Main_Menu().DisplayMode() == muxc::NavigationViewDisplayMode::Compact || Main_Menu().DisplayMode() == muxc::NavigationViewDisplayMode::Minimal)) // Don't go back if the nav pane is overlayed.
    {
        return false;
    }

    ContentFrame().GoBack();
    return true;
}


void winrt::main_GUI::implementation::MainPage::NavigationViewItem_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::TappedRoutedEventArgs const& e)
{
    ContentFrame().Navigate(winrt::xaml_typename<main_GUI::HelpPage>());
}


void winrt::main_GUI::implementation::MainPage::FirstUseTip1_ActionButtonClick(winrt::Microsoft::UI::Xaml::Controls::TeachingTip const& sender, winrt::Windows::Foundation::IInspectable const& args)
{
    // go to help page
    ContentFrame().Navigate(winrt::xaml_typename<main_GUI::HelpPage>());
    // close the teaching tip
    FirstUseTip1().IsOpen(false);
}
