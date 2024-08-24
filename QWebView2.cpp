#include "QWebView2.h"

#include <QResizeEvent>

QWebView2::QWebView2(QWidget* parent)
{
    _view = new WebView2Impl(parent->winId());
}

QWebView2::~QWebView2()
{
    if (_view)
    {
        delete _view;
        _view = nullptr;
    }
}

void QWebView2::openDevToolsWindow()
{

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
