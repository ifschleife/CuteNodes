#pragma once

#include <QGraphicsScene>


// Adapted from http://stackoverflow.com/a/15054118/578536

class NodeScene : public QGraphicsScene
{
public:
    explicit NodeScene(const QRectF& sceneRect);
    ~NodeScene() override;

public slots:
    void ToggleGridSnapping();

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

private:
    QGraphicsItem* GetLowestItemThatWasClicked(const QPointF& clickPos);

private:
    QGraphicsItem* _draggedItem;
    QPointF        _draggingMousePointerOffset;
    QSize          _gridSize;
    bool           _gridSnapping;
};
