#include "pch.h"
#include "RemoveEventPage.h"
#if __has_include("RemoveEventPage.g.cpp")
#include "RemoveEventPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::main_GUI::implementation
{
    RemoveEventPage::RemoveEventPage()
    {
        InitializeComponent();
    }

    int32_t RemoveEventPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void RemoveEventPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void RemoveEventPage::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
