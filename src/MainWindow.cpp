#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "CuteNodeWidget.h"
#include "NodeScene.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _scene{std::make_unique<NodeScene>(QRectF(0.0, 0.0, 3000.0, 3000.0))}
    , _ui{std::make_unique<Ui::MainWindow>()}
{
    _ui->setupUi(this);

    for (int i=0; i<5; ++i)
    {
        CuteNodeWidget* node = new CuteNodeWidget;
        node->setPos(i*300.0, 300.0);
        _scene->addItem(node);
    }

    _ui->nodeView->setScene(_scene.get());
    _ui->nodeView->centerOn(0.0, 0.0);
    _ui->nodeView->setDragMode(QGraphicsView::RubberBandDrag);
    _ui->nodeView->setRenderHints(QPainter::TextAntialiasing | QPainter::HighQualityAntialiasing);
    _ui->nodeView->show();

    connect(_ui->actionToggleSnap, &QAction::triggered, _scene.get(), &NodeScene::toggleGridSnapping);
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_actionAddNode_triggered()
{
    CuteNodeWidget* node = new CuteNodeWidget;
    node->setPos(500, 300);
    _scene->addItem(node);
}
