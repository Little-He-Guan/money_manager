#pragma once

#include "MainPage.g.h"

namespace winrt::main_GUI::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

    public:
        void Main_Menu_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void Main_Menu_SelectionChanged(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);
        void Main_Menu_BackRequested(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const& args);
        void System_BackRequested(Windows::Foundation::IInspectable const& /* sender */, Windows::UI::Core::BackRequestedEventArgs const& args);
        // handle this so that when going back the seleted item will also be correctly updated.
        void On_Navigated(Windows::Foundation::IInspectable const& /* sender */, Windows::UI::Xaml::Navigation::NavigationEventArgs const& args);
        void NavigationViewItem_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::TappedRoutedEventArgs const& e);
        void FirstUseTip1_ActionButtonClick(winrt::Microsoft::UI::Xaml::Controls::TeachingTip const& sender, winrt::Windows::Foundation::IInspectable const& args);

        // handles navigation
        void Main_Menu_Navigate(std::wstring navItemTag, Windows::UI::Xaml::Media::Animation::NavigationTransitionInfo const& transitionInfo);
        /*
        * Tries to go back to the previous page navigated
        * @returns true iff successfully went back
        */
        bool TryGoBack();

    private:
        // The pages corresponding to each navigation item
        std::vector<std::pair<std::wstring, Windows::UI::Xaml::Interop::TypeName>> m_pages;

        wuxc::NavigationViewItem help_item;

        winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
