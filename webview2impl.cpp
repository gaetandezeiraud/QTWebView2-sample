#include "webview2impl.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl.h>

using namespace Microsoft::WRL;

WebView2Impl::WebView2Impl(unsigned long long windowId)
{
    _hwnd = reinterpret_cast<HWND>(windowId);

    CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [&](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                if (FAILED(result))
                {
                    destroy();
                    return result;
                }

                // Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
                env->CreateCoreWebView2Controller(_hwnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                    [&](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
                        if (FAILED(result))
                        {
                            destroy();
                            return result;
                        }

                        _webViewController = controller;
                        _webViewController->get_CoreWebView2(&_webView);
                        _webViewController->put_IsVisible(false); // Hide by default

                        std::lock_guard<std::mutex> lock(_dispatcherMutex);
                        _dispatcher.WorkLoop();
                        _state.store(WebView2ImplState::Loaded);
                        _dispatcher.Clear(); // Make sure it is empty, not needed anymore
                        return S_OK;
                    }).Get());
                return S_OK;
            }).Get());
}

WebView2Impl::~WebView2Impl()
{
    _webView->Release();
    _webViewController->Release();
}

void WebView2Impl::destroy()
{
    _state.store(WebView2ImplState::Failed);
    std::lock_guard<std::mutex> lock(_dispatcherMutex);
    _dispatcher.Clear();
    _webView->Release();
    _webViewController->Release();
}

void WebView2Impl::resize(int top, int left, int right, int bottom)
{
    RECT bounds;
    bounds.top = top;
    bounds.left = left;
    bounds.right = right;
    bounds.bottom = bottom;

    switch (_state.load())
    {
    case WebView2ImplState::Loaded:
        _webViewController->put_Bounds(bounds);
        break;
    case WebView2ImplState::Empty:
    {
        std::lock_guard<std::mutex> lock(_dispatcherMutex);
        _dispatcher.Invoke([&, bounds]() { _webViewController->put_Bounds(bounds); });
        break;
    }
    default:
        break;
    }
}

void WebView2Impl::navigate(const QString& url)
{
    switch (_state.load())
    {
    case WebView2ImplState::Loaded:
        _webView->Navigate(url.toStdWString().c_str());
        break;
    case WebView2ImplState::Empty:
    {
        std::lock_guard<std::mutex> lock(_dispatcherMutex);
        _dispatcher.Invoke([&, url]() { _webView->Navigate(url.toStdWString().c_str()); });
        break;
    }
    default:
        break;
    }
}

void WebView2Impl::goForward()
{
    if (_webView.Get())
        _webView->GoForward();
}

void WebView2Impl::goBack()
{
    if (_webView.Get())
        _webView->GoBack();
}

void WebView2Impl::show()
{
    switch (_state.load())
    {
    case WebView2ImplState::Loaded:
        _webViewController->put_IsVisible(true);
        break;
    case WebView2ImplState::Empty:
    {
        std::lock_guard<std::mutex> lock(_dispatcherMutex);
        _dispatcher.Invoke([&]() { _webViewController->put_IsVisible(true); });
        break;
    }
    default:
        break;
    }
}

void WebView2Impl::hide()
{
    switch (_state.load())
    {
    case WebView2ImplState::Loaded:
        _webViewController->put_IsVisible(false);
        break;
    case WebView2ImplState::Empty:
    {
        std::lock_guard<std::mutex> lock(_dispatcherMutex);
        _dispatcher.Invoke([&]() { _webViewController->put_IsVisible(false); });
        break;
    }
    default:
        break;
    }
}
