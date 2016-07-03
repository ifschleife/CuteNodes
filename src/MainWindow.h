#pragma once

#include <memory>

#include <QMainWindow>


namespace Ui
{
    class MainWindow;
}

class NodeScene;
class QFile;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString& sceneToOpen, QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_actionExit_triggered();
    void on_actionLoadScene_triggered();
    void on_actionNewScene_triggered();
    void on_actionSaveScene_triggered();
    void on_actionSaveSceneAs_triggered();

private:
    void CreateScene(const QJsonObject& json);
    void LoadSceneFromFile();
    void SaveSceneToFile(QFile& sceneFile);

private:
    std::unique_ptr<NodeScene>      _scene;
    QString                         _sceneFileName;
    std::unique_ptr<Ui::MainWindow> _ui;
};
