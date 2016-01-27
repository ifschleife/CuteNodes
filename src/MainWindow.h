#pragma once

#include <memory>

#include <QMainWindow>


namespace Ui
{
    class MainWindow;
}

class NodeScene;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent);
    ~MainWindow();

private:
    std::unique_ptr<NodeScene>      _scene;
    std::unique_ptr<Ui::MainWindow> _ui;
};
