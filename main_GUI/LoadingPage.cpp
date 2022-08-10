#include "pch.h"
#include "LoadingPage.h"
#if __has_include("LoadingPage.g.cpp")
#include "LoadingPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::main_GUI::implementation
{
    LoadingPage::LoadingPage()
    {
        InitializeComponent();
    }

    int32_t LoadingPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void LoadingPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}
