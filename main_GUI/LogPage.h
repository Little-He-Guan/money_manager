#pragma once

#include "LogPage.g.h"
#include "internal_sys.h"

#include <fstream>

namespace winrt::main_GUI::implementation
{
    struct LogPage : LogPageT<LogPage>
    {
        LogPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void UpdateLines();

    private:
        static Windows::Foundation::Collections::IVector<winrt::hstring> log_lines;

        static void update_log_lines();

    public:
        void Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct LogPage : LogPageT<LogPage, implementation::LogPage>
    {
    };
}
