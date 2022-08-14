#pragma once

#include "SavePage.g.h"

#include <functional>

namespace winrt::main_GUI::implementation
{
    struct SavePage : SavePageT<SavePage>
    {
        SavePage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        // save system to file
        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
        // export save
        void MenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        // export log
        void MenuFlyoutItem_Click_1(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        // import save
        void MenuFlyoutItem_Click_2(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        // import log
        void MenuFlyoutItem_Click_3(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        // File I/O coroutines

        /*
        * Imports a user's custom file to the location where the program's files are stored
        * @param callback called within the ui thread after the picking.
        * Its parameter indicates whether the operation succeeded (true) or not (false)
        */
        winrt::fire_and_forget import_file(winrt::hstring filename, std::function<void(bool)> callback = {});
        /*
        * Exports the save/log file to a user specified location
        * @param callback called within the ui thread after the picking.
        * Its parameter indicates whether the result of the operation:
        * 0: succeeded
        * 1: file does not exist
        * 2: operation cancelled
        */
        winrt::fire_and_forget export_file(winrt::hstring filename, std::function<void(int)> callback = {});

    public:
        void ConfirmCheckBox_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void ConfirmCheckBox_Unchecked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void Import_Confirmation_Dialog_Opened(winrt::Windows::UI::Xaml::Controls::ContentDialog const& sender, winrt::Windows::UI::Xaml::Controls::ContentDialogOpenedEventArgs const& args);
    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct SavePage : SavePageT<SavePage, implementation::SavePage>
    {
    };
}
