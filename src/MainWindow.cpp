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
    , _sceneFileName{}
    , _ui{std::make_unique<Ui::MainWindow>()}
{
    _ui->setupUi(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_actionLoadScene_triggered()
{
    _sceneFileName = QFileDialog::getOpenFileName(this, tr("Open Scene"), "..", tr("JSON (*.json)"));
    if (_sceneFileName.isNull())
        return;

    QFile sceneFile{_sceneFileName};
    if (!sceneFile.open(QIODevice::ReadOnly))
    {
        _sceneFileName = "";
        return;
    }

    setWindowTitle(_sceneFileName);

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
        disconnect(_ui->actionToggleSnap,    &QAction::triggered, _scene.get(), &NodeScene::toggleGridSnapping);
        disconnect(_ui->actionToggleOverlap, &QAction::triggered, _scene.get(), &NodeScene::toggleNodeOverlap);
    }
    _scene = std::make_unique<NodeScene>();

    const auto json = document.object();
    _scene->read(json);

    _ui->nodeView->setScene(_scene.get());
    const auto centerx = json["scene"].toObject()["centerx"].toDouble();
    const auto centery = json["scene"].toObject()["centery"].toDouble();
    _ui->nodeView->centerOn(centerx, centery);

    connect(_ui->actionToggleSnap,    &QAction::triggered, _scene.get(), &NodeScene::toggleGridSnapping);
    connect(_ui->actionToggleOverlap, &QAction::triggered, _scene.get(), &NodeScene::toggleNodeOverlap);
}

void MainWindow::on_actionSaveScene_triggered()
{
    QFile sceneFile{_sceneFileName};
    if (!sceneFile.open(QIODevice::WriteOnly))
    {
        _sceneFileName = QFileDialog::getSaveFileName(this, tr("Save Scene"), "..", tr("JSON (*.json)"));
        sceneFile.setFileName(_sceneFileName);
        if (!sceneFile.open(QIODevice::ReadWrite))
            return;
    }

    SaveSceneToFile(sceneFile);
}

void MainWindow::on_actionSaveSceneAs_triggered()
{
    _sceneFileName = QFileDialog::getSaveFileName(this, tr("Save Scene"), "..", tr("JSON (*.json)"));
    QFile sceneFile(_sceneFileName);
    if (!sceneFile.open(QIODevice::WriteOnly))
        return;

    SaveSceneToFile(sceneFile);
}

void MainWindow::SaveSceneToFile(QFile& sceneFile)
{
    auto json = QJsonObject();
    QJsonObject sceneObject
    {
        // this does not give the center of the viewport :(
        {"centerx", _ui->nodeView->sceneRect().center().x()},
        {"centery", _ui->nodeView->sceneRect().center().y()}
    };
    json.insert("scene", sceneObject);
    _scene->write(json);

    const auto document = QJsonDocument(json);
    sceneFile.write(document.toJson());
}
