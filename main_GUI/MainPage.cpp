#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

#include <Windows.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Storage.h>

// the following header cannot be used by UWP apps (but ... why do I have to use it then?)
enum _SHGDNF { SHGDN_NORMAL = 0, SHGDN_INFOLDER = 0x1, SHGDN_FOREDITING = 0x1000, SHGDN_FORADDRESSBAR = 0x4000, SHGDN_FORPARSING = 0x8000 }; typedef DWORD SHGDNF;
#include <WindowsStorageCom.h> // to convert the storagefile to a handle

#include "date_convertion.h"
#include "internal_sys.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

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
    }

    MainPage::~MainPage()
    {
        Save_System();

        CloseHandle(hSave);
        CloseHandle(hLog);
    }

    int32_t MainPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }


    HANDLE MainPage::hSave, MainPage::hLog;
}


namespace wuxc = winrt::Windows::UI::Xaml::Controls;

void winrt::main_GUI::implementation::MainPage::Main_Menu_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    get_file_handles();

    // initialize the system with custom file path (required by UWP)
    bool load_successful = g_mgr.load_from_file("", hSave);
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
                break;
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

void winrt::main_GUI::implementation::MainPage::Save_System()
{
    CloseHandle(hSave);

    // override the file
    Windows::Storage::StorageFile sav_file = Windows::Storage::ApplicationData::Current().LocalFolder().CreateFileAsync(L"save.sav", Windows::Storage::CreationCollisionOption::ReplaceExisting).get();

    // try to get an IStorageItemHandleAccess interface from the StorageFile 
    auto sav_fileAccessor = sav_file.as<IStorageItemHandleAccess>();

    HRESULT hr;
    if (FAILED(hr = sav_fileAccessor->Create(HANDLE_ACCESS_OPTIONS::HAO_READ | HAO_WRITE,
        HANDLE_SHARING_OPTIONS::HSO_SHARE_NONE,
        HANDLE_OPTIONS::HO_NONE,
        nullptr,
        &hSave)))
    {
        throw_hresult(hr);
    }

    g_mgr.save_back_to_file("", hSave);
}

void winrt::main_GUI::implementation::MainPage::get_file_handles()
{
    // create the files if the do not exist (we cannot do this in UWP with fstream)
    Windows::Storage::StorageFile sav_file = Windows::Storage::ApplicationData::Current().LocalFolder().TryGetItemAsync(L"save.sav").get().try_as<Windows::Storage::StorageFile>();
    if (sav_file != nullptr) // exists. do nothing
    {
    }
    else // create the file
    {
        sav_file = Windows::Storage::ApplicationData::Current().LocalFolder().CreateFileAsync(L"save.sav").get();
    }

    Windows::Storage::StorageFile log_file = Windows::Storage::ApplicationData::Current().LocalFolder().TryGetItemAsync(L"log.log").get().try_as<Windows::Storage::StorageFile>();
    if (log_file != nullptr) // exists. do nothing
    {
    }
    else // create the file
    {
        log_file = Windows::Storage::ApplicationData::Current().LocalFolder().CreateFileAsync(L"log.log").get();
    }

    // try to get an IStorageItemHandleAccess interface from the StorageFile 
    auto sav_fileAccessor = sav_file.as<IStorageItemHandleAccess>();

    HRESULT hr;
    if (FAILED(hr = sav_fileAccessor->Create(HANDLE_ACCESS_OPTIONS::HAO_READ | HAO_WRITE, // write access is required
        HANDLE_SHARING_OPTIONS::HSO_SHARE_NONE,
        HANDLE_OPTIONS::HO_NONE,
        nullptr,
        &hSave)))
    {
        throw_hresult(hr);
    }

    // try to get an IStorageItemHandleAccess interface from the StorageFile 
    auto log_fileAccessor = log_file.as<IStorageItemHandleAccess>();

    if (FAILED(hr = log_fileAccessor->Create(HANDLE_ACCESS_OPTIONS::HAO_READ | HAO_WRITE, // write access is required
        HANDLE_SHARING_OPTIONS::HSO_SHARE_NONE,
        HANDLE_OPTIONS::HO_NONE,
        nullptr,
        &hLog)))
    {
        throw_hresult(hr);
    }

    financial_system::set_log_handle(hLog);
}

