#include "webview2impl.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl.h>
#include <wil/com.h>
#include <QDebug>

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

                        _dispatcher.WorkLoop();
                        _state.store(WebView2ImplState::Loaded);
                        _dispatcher.Clear(); // Make sure it is empty, not needed anymore
                        return S_OK;
                    }).Get());
                return S_OK;
            }).Get());
}

void WebView2Impl::destroy()
{
    _state.store(WebView2ImplState::Failed);
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
        _dispatcher.Invoke([&, bounds]() { _webViewController->put_Bounds(bounds); });
        break;
    default:
        break;
    }
}

void WebView2Impl::postWebMessageAsJson(const QString& str)
{
    switch (_state.load())
    {
    case WebView2ImplState::Loaded:
        _webView->PostWebMessageAsJson(str.toStdWString().c_str());
        break;
    case WebView2ImplState::Empty:
        _dispatcher.Invoke([&, str]() { _webView->PostWebMessageAsJson(str.toStdWString().c_str()); });
        break;
    default:
        break;
    }
}

void WebView2Impl::setVirtualHostNameToFolderMappingInternal(const QString& domain, const QString& folderPath)
{
    wil::com_ptr<ICoreWebView2_3> webview2_3;
    _webView->QueryInterface(IID_PPV_ARGS(&webview2_3));
    if (webview2_3)
    {
        webview2_3->SetVirtualHostNameToFolderMapping(domain.toStdWString().c_str(),
                                                    folderPath.toStdWString().c_str(),
                                                    COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW);
    }
}

void WebView2Impl::setVirtualHostNameToFolderMapping(const QString& domain, const QString& folderPath)
{
    switch (_state.load())
    {
    case WebView2ImplState::Loaded:
        setVirtualHostNameToFolderMappingInternal(domain, folderPath);
        break;
    case WebView2ImplState::Empty:
        _dispatcher.Invoke([&, domain, folderPath]() { setVirtualHostNameToFolderMappingInternal(domain, folderPath); });
        break;
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
        _dispatcher.Invoke([&, url]() { _webView->Navigate(url.toStdWString().c_str()); });
        break;
    default:
        break;
    }
}

void WebView2Impl::addNavigationCompletedInternal(std::function<void()> callback)
{
    _webView->add_NavigationCompleted(
        Callback<ICoreWebView2NavigationCompletedEventHandler>(
            [this, callback](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
                callback();
                return S_OK;
            }).Get(),
        nullptr
    );
}

void WebView2Impl::addNavigationCompleted(std::function<void()> callback)
{
    switch (_state.load())
    {
    case WebView2ImplState::Loaded:
        addNavigationCompletedInternal(callback);
        break;
    case WebView2ImplState::Empty:
        _dispatcher.Invoke([&, callback]() { addNavigationCompletedInternal(callback); });
        break;
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

void WebView2Impl::printToPdf(const QString& outputPath, std::function<void(bool)> pdfCallback)
{
    if (_webView.Get())
    {
        wil::com_ptr<ICoreWebView2_7> webview2_7;
        _webView->QueryInterface(IID_PPV_ARGS(&webview2_7));
        if (webview2_7)
        {
            webview2_7->PrintToPdf(outputPath.toStdWString().c_str(), nullptr,
                Callback<ICoreWebView2PrintToPdfCompletedHandler>(
                [this, pdfCallback](HRESULT errorCode, BOOL isSuccessful) -> HRESULT {
                    pdfCallback(isSuccessful);
                    return S_OK;
                }).Get()
            );
        }
    }
}

void WebView2Impl::show()
{
    switch (_state.load())
    {
    case WebView2ImplState::Loaded:
        _webViewController->put_IsVisible(true);
        break;
    case WebView2ImplState::Empty:
        _dispatcher.Invoke([&]() { _webViewController->put_IsVisible(true); });
        break;
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
        _dispatcher.Invoke([&]() { _webViewController->put_IsVisible(false); });
        break;
    default:
        break;
    }
}

Microsoft::WRL::ComPtr<ICoreWebView2> WebView2Impl::getWebView()
{
    return _webView;
}

Microsoft::WRL::ComPtr<ICoreWebView2Controller> WebView2Impl::getWebViewController()
{
    return _webViewController;
}
