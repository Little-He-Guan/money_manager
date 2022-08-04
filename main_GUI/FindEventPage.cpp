#include "pch.h"
#include "FindEventPage.h"
#if __has_include("FindEventPage.g.cpp")
#include "FindEventPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::main_GUI::implementation
{
    FindEventPage::FindEventPage()
    {
        InitializeComponent();
    }

    int32_t FindEventPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void FindEventPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void FindEventPage::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
