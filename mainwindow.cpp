#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "QWebView2.h"
#include <qlayout.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto webView = new QWebView2(this);
    webView->setUrl("https://www.bing.com/");

    connect(webView, &QWebView2::navigationCompleted, this, [this]() {
        // Do something here like
        //_viewPDFGenerator->GetWebView()->postWebMessageAsJson("your-message");
        // or  _viewPDFGenerator->GetWebView()->printToPdf(...)
    });

    setCentralWidget(webView);
}

MainWindow::~MainWindow()
{
    delete ui;
}
