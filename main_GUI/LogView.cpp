#include "pch.h"
#include "LogView.h"
#if __has_include("LogView.g.cpp")
#include "LogView.g.cpp"
#endif

namespace winrt::main_GUI::implementation
{
    LogView::LogView(winrt::hstring log_line)
    {
        auto ws = std::wstring(log_line);

        auto at_point = ws.find_last_of(L"at");
        auto last_space_point = ws.find_last_of(L" ");

        try
        {
            name = ws.substr(0, at_point - 2);
            date = ws.substr(at_point - 1, last_space_point - at_point + 1);
            amount = ws.substr(last_space_point + 1);
        }
        catch (std::out_of_range e)
        {
            throw std::runtime_error("log file damaged!");
        }
    }

    winrt::event_token LogView::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        return m_propertyChanged.add(handler);
    }

    void LogView::PropertyChanged(winrt::event_token const& token)
    {
        m_propertyChanged.remove(token);
    }

    winrt::hstring LogView::Name()
    {
        return name;
    }

    void LogView::Name(winrt::hstring const& value)
    {
        if (value != name)
        {
            name = value;
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Name" });
        }
    }

    winrt::hstring LogView::Date()
    {
        return date;
    }

    void LogView::Date(winrt::hstring const& value)
    {
        if (value != date)
        {
            date = value;
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Date" });
        }
    }

    winrt::hstring LogView::Amount()
    {
        return amount;
    }

    void LogView::Amount(winrt::hstring const& value)
    {
        if (value != amount)
        {
            amount = value;
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Amount" });
        }
    }

}
