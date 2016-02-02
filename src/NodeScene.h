#pragma once

#include <QGraphicsScene>
#include <vector>


class CuteNodeWidget;


// Adapted from http://stackoverflow.com/a/15054118/578536

class NodeScene : public QGraphicsScene
{
public:
    explicit NodeScene(const QRectF& sceneRect);
    ~NodeScene() override;

public slots:
    void toggleGridSnapping();

private:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    bool draggedNodePositionIsValid(const QGraphicsItem* node, const QPointF& nodePos) const;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    std::vector<CuteNodeWidget*> _draggedNodes;
    QSize                        _gridSize{20, 20};
    bool                         _gridSnapping{true};
};
