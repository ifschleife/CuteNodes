#pragma once

#include <QGraphicsView>


// Adapted from http://stackoverflow.com/a/5156978/578536

class NodeView : public QGraphicsView
{
public:
    explicit NodeView(QWidget* parent = nullptr);
    ~NodeView();

private:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    bool   _isPanning;
    QPoint _panStartPos;
};
