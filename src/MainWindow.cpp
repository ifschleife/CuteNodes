#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "CuteNodeWidget.h"
#include "NodeScene.h"
#include "NodeView.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _ui{std::make_unique<Ui::MainWindow>()}
{
    _ui->setupUi(this);

    NodeScene* scene = new NodeScene(QRectF(0.0, 0.0, 3000.0, 3000.0));
    scene->addItem(new CuteNodeWidget);

    _ui->nodeView->rect();
    _ui->nodeView->setScene(scene);
    _ui->nodeView->centerOn(0.0, 0.0);
    _ui->nodeView->setDragMode(QGraphicsView::RubberBandDrag);
    _ui->nodeView->show();

    connect(_ui->actionToggleSnap, &QAction::triggered, scene, &NodeScene::ToggleGridSnapping);
}

MainWindow::~MainWindow()
{
}
