#pragma once

#include "LoadingPage.g.h"

namespace winrt::main_GUI::implementation
{
    struct LoadingPage : LoadingPageT<LoadingPage>
    {
        LoadingPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct LoadingPage : LoadingPageT<LoadingPage, implementation::LoadingPage>
    {
    };
}
