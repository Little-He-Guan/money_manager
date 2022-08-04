#include "pch.h"
#include "RunSimulationPage.h"
#if __has_include("RunSimulationPage.g.cpp")
#include "RunSimulationPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::main_GUI::implementation
{
    RunSimulationPage::RunSimulationPage()
    {
        InitializeComponent();
    }

    int32_t RunSimulationPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void RunSimulationPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void RunSimulationPage::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
