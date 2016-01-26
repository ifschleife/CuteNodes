#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "CuteNodeWidget.h"
#include "NodeScene.h"


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

    connect(_ui->actionToggleSnap, &QAction::triggered, scene, &NodeScene::ToggleGridSnapping);
}

MainWindow::~MainWindow()
{
}
