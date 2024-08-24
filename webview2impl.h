#ifndef WEBVIEW2IMPL_H
#define WEBVIEW2IMPL_H

#include <QString>

#include <wrl.h>
#include "WebView2.h"

#include "Dispatcher.h"

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
    HWND _hwnd;
    Microsoft::WRL::ComPtr<ICoreWebView2> _webView;
    Microsoft::WRL::ComPtr<ICoreWebView2Controller> _webViewController;

    bool _isLoaded{false};
    Dispatcher _dispatcher;
};

#endif // WEBVIEW2IMPL_H
