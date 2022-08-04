#pragma once

#include "CreateSystemPage.g.h"

#include <regex>

namespace winrt::main_GUI::implementation
{
    struct CreateSystemPage : CreateSystemPageT<CreateSystemPage>
    {
        CreateSystemPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        static constexpr auto double_regex = L"((?:[\\d]*[.])?[\\d]+)";
        static const std::wregex double_regex_obj;
    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct CreateSystemPage : CreateSystemPageT<CreateSystemPage, implementation::CreateSystemPage>
    {
    };
}
