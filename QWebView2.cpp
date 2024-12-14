#include "QWebView2.h"

#include <QResizeEvent>

QWebView2::QWebView2(QWidget* parent)
{
    _view = new WebView2Impl(parent->winId());
    _view->addNavigationCompleted([this]() {
        emit navigationCompleted();
    });
}

QWebView2::~QWebView2()
{
    if (_view)
    {
        delete _view;
        _view = nullptr;
    }
}

void QWebView2::postWebMessageAsJson(const QString& str)
{
    if (_view)
        _view->postWebMessageAsJson(str);
}

void QWebView2::setVirtualHostNameToFolderMapping(const QString& domain, const QString& folderPath)
{
    if (_view)
        _view->setVirtualHostNameToFolderMapping(domain, folderPath);
}

void QWebView2::setUrl(const QString& url)
{
    if (_view)
        _view->navigate(url);
}

void QWebView2::goForward()
{
    if (_view)
        _view->goForward();
}

void QWebView2::goBack()
{
    if (_view)
        _view->goBack();
}

void QWebView2::printToPdf(const QString& outputPath, std::function<void(bool)> pdfCallback)
{
    if (_view)
        _view->printToPdf(outputPath, pdfCallback);
}

WebView2Impl* QWebView2::getWebViewImpl()
{
    return _view;
}

void QWebView2::resize()
{
    if (_view)
    {
        auto geo = this->geometry();
        _view->resize(
            geo.top() * this->devicePixelRatioF(),
            geo.left() * this->devicePixelRatioF(),
            geo.right() * this->devicePixelRatioF(),
            geo.bottom() * this->devicePixelRatioF());
    }
}

void QWebView2::resizeEvent(QResizeEvent *event)
{
    resize();
}

void QWebView2::showEvent(QShowEvent *event)
{
    resize();
    if (_view)
        _view->show();
}

void QWebView2::hideEvent(QHideEvent *event)
{
    resize();
    if (_view)
        _view->hide();
}
