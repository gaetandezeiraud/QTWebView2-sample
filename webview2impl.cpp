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
                // Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
                env->CreateCoreWebView2Controller(_hwnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                    [&](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
                        if (controller != nullptr) {
                            _webViewController = controller;
                            _webViewController->get_CoreWebView2(&_webView);
                            _webViewController->put_IsVisible(false); // Hide by default
                        }
                        _dispatcher.WorkLoop();
                        _isLoaded = true;
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

void WebView2Impl::resize(int top, int left, int right, int bottom)
{
    RECT bounds;
    bounds.top = top;
    bounds.left = left;
    bounds.right = right;
    bounds.bottom = bottom;

    if (_isLoaded)
        _webViewController->put_Bounds(bounds);
    else
        _dispatcher.Invoke([&, bounds]() { _webViewController->put_Bounds(bounds); });
}

void WebView2Impl::navigate(const QString& url)
{
    if (_isLoaded)
        _webView->Navigate(url.toStdWString().c_str());
    else
        _dispatcher.Invoke([&, url]() { _webView->Navigate(url.toStdWString().c_str()); });
}

void WebView2Impl::goForward()
{
    _webView->GoForward();
}

void WebView2Impl::goBack()
{
    _webView->GoBack();
}

void WebView2Impl::show()
{
    if (_isLoaded)
        _webViewController->put_IsVisible(true);
    else
        _dispatcher.Invoke([&]() { _webViewController->put_IsVisible(true); });
}

void WebView2Impl::hide()
{
    if (_isLoaded)
        _webViewController->put_IsVisible(false);
    else
        _dispatcher.Invoke([&]() { _webViewController->put_IsVisible(false); });
}
