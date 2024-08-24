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

    setCentralWidget(webView);
}

MainWindow::~MainWindow()
{
    delete ui;
}
