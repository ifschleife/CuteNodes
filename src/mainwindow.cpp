#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cutenodewidget.h"
#include "nodescene.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _ui{std::make_unique<Ui::MainWindow>()}
{
    _ui->setupUi(this);

    NodeScene* scene = new NodeScene;

    scene->addItem(new CuteNodeWidget);

    _ui->graphicsView->rect();
    _ui->graphicsView->setScene(scene);
    _ui->graphicsView->resize(this->size());
    _ui->graphicsView->setSceneRect(_ui->graphicsView->rect());
    _ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    _ui->graphicsView->show();
}

MainWindow::~MainWindow()
{
}
