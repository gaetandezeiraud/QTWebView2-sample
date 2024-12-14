#ifndef QWEBVIEW2_H
#define QWEBVIEW2_H

#include <QWidget>

#include "webview2impl.h"

class QWebView2 : public QWidget {
    Q_OBJECT
public:
    QWebView2(QWidget* parent = nullptr);
    virtual ~QWebView2();

public Q_SLOTS:
    void postWebMessageAsJson(const QString& str);
    void setVirtualHostNameToFolderMapping(const QString& domain, const QString& folderPath);
    void setUrl(const QString& url);
    void goForward();
    void goBack();
    void printToPdf(const QString& outputPath, std::function<void(bool)> pdfCallback);

    WebView2Impl* getWebViewImpl();

signals:
    void navigationCompleted();

protected:
    void resize();
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

protected:
    WebView2Impl* _view = nullptr;
};

#endif // QWEBVIEW2_H
