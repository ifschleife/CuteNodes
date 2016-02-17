#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "CuteNode.h"
#include "NodeScene.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _scene{std::make_unique<NodeScene>(QRectF(0.0, 0.0, 3000.0, 3000.0))}
    , _ui{std::make_unique<Ui::MainWindow>()}
{
    _ui->setupUi(this);

    for (int i=3; i<8; ++i)
    {
        _scene->addItem(new CuteNode{{i*300.0, 1000.0}});
    }

    _ui->nodeView->setScene(_scene.get());
    _ui->nodeView->centerOn(1000.0, 1000.0);

    connect(_ui->actionToggleSnap, &QAction::triggered, _scene.get(), &NodeScene::toggleGridSnapping);
    connect(_ui->actionToggleOverlap, &QAction::triggered, _scene.get(), &NodeScene::toggleNodeOverlap);
}

MainWindow::~MainWindow()
{
}
