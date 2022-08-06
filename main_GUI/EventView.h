#pragma once

#include "EventView.g.h"

#include "../money_manager/event.h"

namespace winrt::main_GUI::implementation
{
    struct EventView : EventViewT<EventView>
    {
        EventView() = delete;
        EventView(winrt::hstring n, winrt::hstring tt, winrt::hstring a, int32_t t);
        EventView(financial_event& e, int32_t t);

        winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& value);
        void PropertyChanged(winrt::event_token const& token);

        winrt::hstring Name() const;
        void Name(winrt::hstring const& value);
        winrt::hstring TimeText() const;
        void TimeText(winrt::hstring const& value);
        winrt::hstring Amount() const;
        void Amount(winrt::hstring const& value);

        int32_t Type() const;
        void Type(int32_t value);

    public:
        void set_p_event(financial_event& e) { p_event = &e; }

        void apply_actual(double a) { p_event->actual = a; }
        void apply_amount(double a) { p_event->amount = a; }

    private:
        winrt::hstring name;
        winrt::hstring time_text;
        winrt::hstring amount;

        winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;

        int32_t type;

    public:
        financial_event* p_event = nullptr;
    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct EventView : EventViewT<EventView, implementation::EventView>
    {
    };
}
