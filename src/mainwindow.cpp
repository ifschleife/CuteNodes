#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui{std::make_unique<Ui::MainWindow>()}
{
    m_ui->setupUi(this);
}

MainWindow::~MainWindow()
{
}
