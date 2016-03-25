#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "CuteNode.h"
#include "NodeScene.h"

#include <QDebug>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _scene{nullptr}
    , _ui{std::make_unique<Ui::MainWindow>()}
{
    _ui->setupUi(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_actionLoadScene_triggered()
{
    const auto sceneFileName = QFileDialog::getOpenFileName(this);
    if (sceneFileName.isNull())
        return;

    QFile sceneFile{sceneFileName};
    if (!sceneFile.open(QIODevice::ReadOnly))
        return;

    const auto content = sceneFile.readAll();

    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(content, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        qDebug() << parseError.errorString() << " at " << parseError.offset;
        return;
    }

    if (_scene)
    {
        auto scene = _scene.release();
        disconnect(_ui->actionToggleSnap, &QAction::triggered, scene, &NodeScene::toggleGridSnapping);
        disconnect(_ui->actionToggleOverlap, &QAction::triggered, scene, &NodeScene::toggleNodeOverlap);
        delete scene;
    }
    _scene = std::make_unique<NodeScene>();

    const auto json = document.object();
    _scene->read(json);

    _ui->nodeView->setScene(_scene.get());
    const auto centerx = json["scene"].toObject()["centerx"].toDouble();
    const auto centery = json["scene"].toObject()["centery"].toDouble();
    _ui->nodeView->centerOn(centerx, centery);

    connect(_ui->actionToggleSnap, &QAction::triggered, _scene.get(), &NodeScene::toggleGridSnapping);
    connect(_ui->actionToggleOverlap, &QAction::triggered, _scene.get(), &NodeScene::toggleNodeOverlap);
}
