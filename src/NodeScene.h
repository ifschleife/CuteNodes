#pragma once

#include <QGraphicsScene>
#include <vector>


class CuteConnection;
class NodeFactory;
class QMenu;


// Adapted from http://stackoverflow.com/a/15054118/578536

class NodeScene : public QGraphicsScene
{
public:
    NodeScene();
    ~NodeScene() override = default;

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

public slots:
    void toggleGridVisibility();
    void toggleGridSnapping() { _gridSnapping = !_gridSnapping; }
    void toggleNodeOverlap()  { _nodeOverlap  = !_nodeOverlap;  }

private:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    void readConnections(const QJsonArray& connections);
    void writeConnections(QJsonObject& json) const;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void addMenuEntriesForSingleSelection(const QPointF& scenePos, QMenu& menu);
    void addMenuEntriesForMultiSelection(QMenu& menu);
    void addMenuEntriesForEmptySelection(const QPointF& scenePos, QMenu& menu);

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void handleNodeDragging(const QPointF& mousePos);
    bool draggedNodePositionInSceneRect(const QGraphicsItem* node, const QPointF& nodePos) const;
    bool draggedNodePositionIsValid(const QGraphicsItem* node, const QPointF& nodePos) const;
    void handleConnectionDrawing(const QPointF& mousePos) const;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void startDraggingSelectedNodes(const QPointF& dragStartPos);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    QGraphicsItem* getTopLevelItemAtPos(const QPointF& scenePos, int itemType) const;

    template<typename ItemType>
    std::vector<ItemType*> getItemsOfType() const;
    template<typename ItemType>
    std::vector<QGraphicsItem*> getSelectedItems() const;

private:
    QSize                    _gridSize{20, 20};
    bool                     _gridSnapping{true};
    bool                     _gridVisible{true};
    NodeFactory*             _nodeFactory{nullptr};
    bool                     _nodeOverlap{false};

    // temps
    CuteConnection*          _drawnConnection{nullptr};

    typedef std::pair<QGraphicsItem*, QPointF> DraggedNode;
    std::vector<DraggedNode> _draggedNodes;
};
