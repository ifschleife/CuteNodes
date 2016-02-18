#pragma once

#include <QGraphicsScene>
#include <vector>


class CuteConnection;


// Adapted from http://stackoverflow.com/a/15054118/578536

class NodeScene : public QGraphicsScene
{
public:
    explicit NodeScene(const QRectF& sceneRect);
    ~NodeScene() override;

public slots:
    void toggleGridSnapping()  { _gridSnapping  = !_gridSnapping; }
    void toggleNodeOverlap()   { _nodeOverlap   = !_nodeOverlap;  }

private:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void handleNodeDragging(const QPointF& mousePos);
    bool draggedNodePositionInSceneRect(const QGraphicsItem* node, const QPointF& nodePos) const;
    bool draggedNodePositionIsValid(const QGraphicsItem* node, const QPointF& nodePos) const;
    void handleConnectionDrawing(const QPointF& mousePos);

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void startDraggingSelectedNodes(const QPointF& dragStartPos);
    std::vector<QGraphicsItem*> getSelectedNodes() const;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    QGraphicsItem* getTopLevelItemAtPos(const QPointF& scenePos, int itemType) const;

private:
    QSize                    _gridSize{20, 20};
    bool                     _gridSnapping{true};
    bool                     _nodeOverlap{false};

    // temps
    CuteConnection*          _drawnConnection{nullptr};

    typedef std::pair<QGraphicsItem*, QPointF> DraggedNode;
    std::vector<DraggedNode> _draggedNodes;
};
