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

    winrt::fire_and_forget LogPage::UpdateLines()
    {
        // capture calling thread context.
        winrt::apartment_context ui_thread;
        // get strong this so it will not expire
        auto weak_this = get_weak();
        // and switch to a background thread
        co_await winrt::resume_background();

        auto log_file{ co_await Windows::Storage::ApplicationData::Current().LocalFolder().CreateFileAsync(log_file_name_w, ws::CreationCollisionOption::OpenIfExists) };

        auto log_lines{ co_await ws::FileIO::ReadLinesAsync(log_file) };

        // switch back to UI thread
        co_await ui_thread;

        if (auto strong_this{ weak_this.get() })
        {
            auto size = strong_this->Log_View().Items().Size();

            if (size == 0)
            {
                for (const auto& line : log_lines)
                {
                    strong_this->Log_View().Items().Append(main_GUI::LogView(line));
                }
            }
        }
    }

}

void winrt::main_GUI::implementation::LogPage::Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    UpdateLines();
}