#ifndef WEBVIEW2IMPL_H
#define WEBVIEW2IMPL_H

#include <QString>

#include <wrl.h>
#include "WebView2.h"

#include "Dispatcher.h"

enum WebView2ImplState
{
    Empty,
    Loaded,
    Failed,
};

class WebView2Impl
{
public:
    WebView2Impl(unsigned long long windowId);
    virtual ~WebView2Impl();

    void resize(int x, int y, int w, int h);
    void navigate(const QString& url);

    void goForward();
    void goBack();

    void show();
    void hide();

private:
    void destroy();

private:
    HWND _hwnd;
    Microsoft::WRL::ComPtr<ICoreWebView2> _webView;
    Microsoft::WRL::ComPtr<ICoreWebView2Controller> _webViewController;

    std::atomic<WebView2ImplState> _state{WebView2ImplState::Empty};

    Dispatcher _dispatcher;
    std::mutex _dispatcherMutex;
};

#endif // WEBVIEW2IMPL_H
