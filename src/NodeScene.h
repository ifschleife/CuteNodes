#pragma once

#include <QGraphicsScene>
#include <vector>


// Adapted from http://stackoverflow.com/a/15054118/578536

class NodeScene : public QGraphicsScene
{
public:
    explicit NodeScene(const QRectF& sceneRect);
    ~NodeScene() override;

public slots:
    void toggleGridSnapping()  { _gridSnapping = !_gridSnapping; }
    void toggleNodeCollision() { _nodeCollision = !_nodeCollision; }

private:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    bool draggedNodePositionIsValid(const QGraphicsItem* node, const QPointF& nodePos) const;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void startDraggingSelectedNodes(const QPointF& dragStartPos);
    std::vector<QGraphicsItem*> getSelectedNodes() const;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    typedef std::pair<QGraphicsItem*, QPointF> DraggedNode;

    std::vector<DraggedNode> _draggedNodes;
    QSize                    _gridSize{20, 20};
    bool                     _gridSnapping{true};
    bool                     _nodeCollision{false};
};
