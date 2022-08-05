#pragma once

#include "LogView.g.h"

namespace winrt::main_GUI::implementation
{
    struct LogView : LogViewT<LogView>
    {
        LogView() = default;
        LogView(winrt::hstring log_line);

        winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& value);
        void PropertyChanged(winrt::event_token const& token);

        winrt::hstring Name();
        void Name(winrt::hstring const& value);
        winrt::hstring Date();
        void Date(winrt::hstring const& value);
        winrt::hstring Amount();
        void Amount(winrt::hstring const& value);
    private:
        winrt::hstring name;
        winrt::hstring date;
        winrt::hstring amount;

        winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct LogView : LogViewT<LogView, implementation::LogView>
    {
    };
}
