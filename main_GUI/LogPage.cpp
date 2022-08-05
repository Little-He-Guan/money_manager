#include "pch.h"
#include "LogPage.h"
#if __has_include("LogPage.g.cpp")
#include "LogPage.g.cpp"
#endif

#include "internal_sys.h"

#include <winrt/Windows.Storage.h>

namespace ws = Windows::Storage;
namespace wfc = Windows::Foundation::Collections;

namespace winrt::main_GUI::implementation
{
    Windows::Foundation::Collections::IVector<winrt::hstring> LogPage::log_lines;

    LogPage::LogPage()
    {
        InitializeComponent();
    }

    int32_t LogPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void LogPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void LogPage::UpdateLines()
    {
        update_log_lines();

        auto size = Log_View().Items().Size();

        if (size == 0)
        {
            for (const auto& line : log_lines)
            {
                Log_View().Items().Append(main_GUI::LogView(line));
            }
        }
    }

}

void winrt::main_GUI::implementation::LogPage::update_log_lines()
{
    Windows::Storage::StorageFile log_file = Windows::Storage::ApplicationData::Current().LocalFolder().CreateFileAsync(log_file_name_w, ws::CreationCollisionOption::OpenIfExists).get();

    log_lines = ws::FileIO::ReadLinesAsync(log_file).get();
}

void winrt::main_GUI::implementation::LogPage::Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    UpdateLines();
}