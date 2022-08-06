#include "pch.h"
#include "EventView.h"
#if __has_include("EventView.g.cpp")
#include "EventView.g.cpp"
#endif

namespace winrt::main_GUI::implementation
{
	EventView::EventView(winrt::hstring n, winrt::hstring tt, winrt::hstring a, int32_t t)
		:name(n), time_text(tt), amount(a), type(t)
	{
	}

	EventView::EventView(financial_event& e, int32_t t) : p_event(&e)
	{
		switch (t)
		{
		case 0:
			name = L"Fixed Income ";
			break;
		case 1:
			name = L"One-time Proposal ";
			break;
		case 2:
			name = L"Periodic Proposal ";
			break;
		default:
			name = L"Event ";
			break;
		}
		name = name + winrt::to_hstring(*e.name);
		amount = winrt::to_hstring(t == 0 ? L"+ " : L"- ") + winrt::to_hstring(e.amount) + L" (" + winrt::to_hstring(e.actual) + L")";
		type = t;

		time_text = winrt::hstring(L"Start: ") + winrt::to_hstring(e.start.to_string()) + L", end: " + winrt::to_hstring(e.end.to_string());
	}

	winrt::event_token EventView::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_propertyChanged.add(handler);
	}

	void EventView::PropertyChanged(winrt::event_token const& token)
	{
		m_propertyChanged.remove(token);
	}

	winrt::hstring EventView::Name() const
	{
		return name;
	}
	void EventView::Name(winrt::hstring const& value)
	{
		if (value != name)
		{
			name = value;
			m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Name" });
		}
	}

	winrt::hstring EventView::TimeText() const
	{
		return time_text;
	}
	void EventView::TimeText(winrt::hstring const& value)
	{
		if (value != time_text)
		{
			time_text = value;
			m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"TimeText" });
		}
	}

	winrt::hstring EventView::Amount() const
	{
		return amount;
	}
	void EventView::Amount(winrt::hstring const& value)
	{
		if (value != amount)
		{
			amount = value;
			m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Amount" });
		}
	}

	int32_t EventView::Type() const
	{
		return type;
	}
	void EventView::Type(int32_t value)
	{
		if (value != type)
		{
			type = value;
			m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Type" });
		}
	}
}
