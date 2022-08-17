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

#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Security.Cryptography.h>

#include "winrt/Microsoft.UI.Xaml.Automation.Peers.h"
#include "winrt/Microsoft.UI.Xaml.Controls.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt/Microsoft.UI.Xaml.Media.h"
#include "winrt/Microsoft.UI.Xaml.XamlTypeInfo.h"

#include <functional>

using namespace winrt;

namespace wa = Windows::ApplicationModel;
namespace wac = Windows::ApplicationModel::Core;

namespace wu = Windows::UI;
namespace wuc = Windows::UI::Core;
namespace wux = Windows::UI::Xaml;
namespace wuxc = Windows::UI::Xaml::Controls;
namespace wuxm = Windows::UI::Xaml::Media;

namespace mu = Microsoft::UI;
namespace mux = Microsoft::UI::Xaml;
namespace muxc = Microsoft::UI::Xaml::Controls;
namespace muxm = Microsoft::UI::Xaml::Media;

namespace ws = Windows::Storage;
namespace wf = Windows::Foundation;
namespace wfc = Windows::Foundation::Collections;
namespace wsc = Windows::Security::Cryptography;

// remember to add a ; after each of these macros
#define SET_ERROR_MESSAGE(txtblock, msg) txtblock.Foreground(wuxm::SolidColorBrush(wu::Colors::Red())); txtblock.Text(msg)
#define SET_SUCCESS_MESSAGE(txtblock, msg) txtblock.Foreground(wuxm::SolidColorBrush(wu::Colors::Green())); txtblock.Text(msg)
#define SET_NORMAL_MESSAGE(txtblock, msg) txtblock.Foreground(wuxm::SolidColorBrush(wu::Colors::Orange())); txtblock.Text(msg)