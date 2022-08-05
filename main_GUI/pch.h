#pragma once
#include <windows.h>
#include <unknwn.h>
#include <restrictederrorinfo.h>
#include <hstring.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Navigation.h>

#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>

using namespace winrt;

namespace wu = Windows::UI;
namespace wux = Windows::UI::Xaml;
namespace wuxc = Windows::UI::Xaml::Controls;
namespace wuxm = Windows::UI::Xaml::Media;

// remember to add a ; after each of these macros
#define SET_ERROR_MESSAGE(txtblock, msg) txtblock.Foreground(wuxm::SolidColorBrush(wu::Colors::Red())); txtblock.Text(msg)
#define SET_SUCCESS_MESSAGE(txtblock, msg) txtblock.Foreground(wuxm::SolidColorBrush(wu::Colors::Green())); txtblock.Text(msg)