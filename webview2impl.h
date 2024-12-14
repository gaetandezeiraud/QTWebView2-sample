#ifndef WEBVIEW2IMPL_H
#define WEBVIEW2IMPL_H

#include <QString>
#include <wrl.h>
#include <WebView2.h>

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
    virtual ~WebView2Impl() = default;

    void resize(int x, int y, int w, int h);
    void postWebMessageAsJson(const QString& str);
    void setVirtualHostNameToFolderMapping(const QString& domain, const QString& folderPath);
    void navigate(const QString& url);
    void addNavigationCompleted(std::function<void()> callback);

    void goForward();
    void goBack();
    void printToPdf(const QString& outputPath, std::function<void(bool)> pdfCallback);

    void show();
    void hide();

    Microsoft::WRL::ComPtr<ICoreWebView2> getWebView();
    Microsoft::WRL::ComPtr<ICoreWebView2Controller> getWebViewController();

private:
    void setVirtualHostNameToFolderMappingInternal(const QString& domain, const QString& folderPath);
    void addNavigationCompletedInternal(std::function<void()> callback);
    void destroy();

private:
    HWND _hwnd;
    Microsoft::WRL::ComPtr<ICoreWebView2> _webView;
    Microsoft::WRL::ComPtr<ICoreWebView2Controller> _webViewController;

    std::atomic<WebView2ImplState> _state{WebView2ImplState::Empty};
    Dispatcher _dispatcher;
};

#endif // WEBVIEW2IMPL_H
