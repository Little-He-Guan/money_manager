#include "pch.h"
#include "SavePage.h"
#if __has_include("SavePage.g.cpp")
#include "SavePage.g.cpp"
#endif

#include "internal_sys.h"

#include <winrt/Windows.Storage.Pickers.h>

#include "App.h" // for switching to load page

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::main_GUI::implementation
{
    SavePage::SavePage()
    {
        InitializeComponent();
    }

    int32_t SavePage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void SavePage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void SavePage::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        if (g_mgr.system_loaded)
        {
            auto update_save_state = [weak_this{ get_weak() }]() -> void
            {
                if (auto strong_this{ weak_this.get() })
                {
                    SET_SUCCESS_MESSAGE(strong_this->Msg_Text(), L"Successfully saved the system");
                    strong_this->Saving_Progress().IsActive(false);
                }
            };

            SET_NORMAL_MESSAGE(Msg_Text(), L"Please wait...");
            Saving_Progress().IsActive(true);
            ::save_system_back_to_file_UWP(update_save_state);
        }
        else
        {
            SET_ERROR_MESSAGE(Msg_Text(), L"There is no system currently.");
        }
    }
}

void winrt::main_GUI::implementation::SavePage::MenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    auto callback = [weak_this{ get_weak() }](int res) ->void
    {
        if (auto strong_this{ weak_this.get() })
        {
            switch (res)
            {
            case 0:
                SET_SUCCESS_MESSAGE(strong_this->Msg_Text(), L"Successfully exported save file to the destination folder.");
                break;
            case 1:
                SET_ERROR_MESSAGE(strong_this->Msg_Text(), L"The save file does not exist.");
                break;
            case 2:
                SET_ERROR_MESSAGE(strong_this->Msg_Text(), L"Operation cancelled.");
                break;
            }
        }

    };

    export_file(save_file_name_w, callback);
}


void winrt::main_GUI::implementation::SavePage::MenuFlyoutItem_Click_1(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    auto callback = [weak_this{ get_weak() }](int res) ->void
    {
        if (auto strong_this{ weak_this.get() })
        {
            switch (res)
            {
            case 0:
                SET_SUCCESS_MESSAGE(strong_this->Msg_Text(), L"Successfully exported the log file to the destination folder.");
                break;
            case 1:
                SET_ERROR_MESSAGE(strong_this->Msg_Text(), L"The log file does not exist.");
                break;
            case 2:
                SET_ERROR_MESSAGE(strong_this->Msg_Text(), L"Operation cancelled.");
                break;
            }
        }
    };

    export_file(log_file_name_w, callback);
}


void winrt::main_GUI::implementation::SavePage::MenuFlyoutItem_Click_2(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    auto callback = [weak_this{ get_weak() }](bool res) ->void
    {
        if (auto strong_this{ weak_this.get() })
        {
            if (res)
            {
                SET_SUCCESS_MESSAGE(strong_this->Msg_Text(), L"Successfully imported the save file and reloaded the system.");
            }
            else
            {
                SET_ERROR_MESSAGE(strong_this->Msg_Text(), L"The operation was cancelled or the file you selected is an invalid save file.");
            }
        }
    };

    import_file(save_file_name_w, callback);
}


void winrt::main_GUI::implementation::SavePage::MenuFlyoutItem_Click_3(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    auto callback = [weak_this{ get_weak() }](bool res) ->void
    {
        if (auto strong_this{ weak_this.get() })
        {
            if (res)
            {
                SET_SUCCESS_MESSAGE(strong_this->Msg_Text(), L"Successfully imported the log file.");
            }
            else
            {
                SET_ERROR_MESSAGE(strong_this->Msg_Text(), L"Operation cancelled.");
            }
        }
    };

    import_file(log_file_name_w, callback);
}

winrt::fire_and_forget winrt::main_GUI::implementation::SavePage::import_file(winrt::hstring filename, std::function<void(bool)> callback)
{
    // capture calling thread context.
    winrt::apartment_context ui_thread;

    auto dialog_res { co_await Import_Confirmation_Dialog().ShowAsync() };
    if (dialog_res == wuxc::ContentDialogResult::Primary) // continue
    {
        SET_NORMAL_MESSAGE(Msg_Text(), L"Please wait...");
        Saving_Progress().IsActive(true);

        auto picker = ws::Pickers::FileOpenPicker();
        picker.SuggestedStartLocation(ws::Pickers::PickerLocationId::Desktop);
        picker.ViewMode(ws::Pickers::PickerViewMode::List);

        std::wstring temp(filename);
        std::wstring extension(temp.substr(temp.find_last_of(L'.')));

        picker.FileTypeFilter().Append(extension);
        // no matter what the original name of the picked file is, its content will be copied to become the imported file
        picker.FileTypeFilter().Append(L"*");

        auto file{ co_await picker.PickSingleFileAsync() };

        // and switch to a background thread
        co_await winrt::resume_background();

        if (file != nullptr)
        {
            if (extension == L".sav") // the save file has been replace. A system reload is required
            {
                // first we see if it's a valid save file by loading a temp system from it.
                financial_system sys;

                auto lines{ co_await ws::FileIO::ReadLinesAsync(file) };
                bool loaded = false;
                if (lines.Size() >= 5)
                {
                    loaded = load_system_from_strings_UWP(lines, sys);
                }

                if (loaded) // a valid save file
                {
                    // replace the current system with the loaded one
                    g_mgr.sys = sys;
                    g_mgr.system_loaded = true;
                    // replace the save file with the file
                    co_await file.CopyAsync(ws::ApplicationData::Current().LocalFolder(), save_file_name_w, ws::NameCollisionOption::ReplaceExisting);
                }
                else // invalid save file
                {
                    co_await ui_thread;
                    Saving_Progress().IsActive(false);
                    if (callback) callback(false);

                    co_return;
                }
            }
            else if (extension == L".log") // replace the log file
            {
                co_await file.CopyAsync(ws::ApplicationData::Current().LocalFolder(), log_file_name_w, ws::NameCollisionOption::ReplaceExisting);
            }
            else // copy as-is
            {
                co_await file.CopyAsync(ws::ApplicationData::Current().LocalFolder(), file.Name(), ws::NameCollisionOption::ReplaceExisting);
            }

            co_await ui_thread;
            Saving_Progress().IsActive(false);
            if (callback) callback(true);
        }
        else // operation cancelled
        {
            co_await ui_thread;
            Saving_Progress().IsActive(false);
            if (callback) callback(false);
        }
    }
    else
    {
        SET_ERROR_MESSAGE(Msg_Text(), L"Operation cancelled.");
    }

    co_return;
}

winrt::fire_and_forget winrt::main_GUI::implementation::SavePage::export_file(winrt::hstring filename, std::function<void(int)> callback)
{
    // capture calling thread context.
    winrt::apartment_context ui_thread;

    SET_NORMAL_MESSAGE(Msg_Text(), L"Please wait...");
    Saving_Progress().IsActive(true);

    auto picker = ws::Pickers::FolderPicker();
    picker.SuggestedStartLocation(ws::Pickers::PickerLocationId::Desktop);

    picker.FileTypeFilter().Append(L"*");

    auto folder{ co_await picker.PickSingleFolderAsync() };

    // and switch to a background thread
    co_await winrt::resume_background();

    if (folder != nullptr)
    {
        if (filename == save_file_name_w) // save file
        {
			auto sav_file{ co_await Windows::Storage::ApplicationData::Current().LocalFolder().CreateFileAsync(save_file_name_w, ws::CreationCollisionOption::OpenIfExists) };

			auto dest_file{ co_await folder.CreateFileAsync(save_file_name_w, ws::CreationCollisionOption::ReplaceExisting) };
			co_await sav_file.CopyAndReplaceAsync(dest_file);

        }
        else if (filename == log_file_name_w) // log file
        {
			auto log_file{ co_await Windows::Storage::ApplicationData::Current().LocalFolder().CreateFileAsync(log_file_name_w, ws::CreationCollisionOption::OpenIfExists) };

			auto dest_file{ co_await folder.CreateFileAsync(log_file_name_w, ws::CreationCollisionOption::ReplaceExisting) };
			co_await log_file.CopyAndReplaceAsync(dest_file);
        }
        else /*do nothing*/ {}

        co_await ui_thread;
        Saving_Progress().IsActive(false);
        if (callback) callback(0);
    }
    else // operation cancelled
    {
        co_await ui_thread;
        Saving_Progress().IsActive(false);
        if (callback) callback(2);
    }

    co_return;
}


void winrt::main_GUI::implementation::SavePage::ConfirmCheckBox_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    Import_Confirmation_Dialog().IsPrimaryButtonEnabled(true);
}


void winrt::main_GUI::implementation::SavePage::ConfirmCheckBox_Unchecked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    Import_Confirmation_Dialog().IsPrimaryButtonEnabled(false);
}


void winrt::main_GUI::implementation::SavePage::Import_Confirmation_Dialog_Opened(winrt::Windows::UI::Xaml::Controls::ContentDialog const& sender, winrt::Windows::UI::Xaml::Controls::ContentDialogOpenedEventArgs const& args)
{
    ConfirmCheckBox().IsChecked(false);
}
