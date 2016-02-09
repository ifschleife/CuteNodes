#include "NodeScene.h"

#include "CuteConnection.h"
#include "CuteDock.h"
#include "CuteNode.h"

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>


namespace
{
    constexpr qreal ZValueDefault   = -1.0;
    constexpr qreal ZValueShowOnTop = 1.0;
}


NodeScene::NodeScene(const QRectF& sceneRect)
    : QGraphicsScene(sceneRect, nullptr)
{

}

NodeScene::~NodeScene()
{

}


void NodeScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    qreal left = int(rect.left()) - (int(rect.left()) % _gridSize.width());
    qreal top = int(rect.top()) - (int(rect.top()) % _gridSize.height());

    QVarLengthArray<QLineF, 100> lines;

    for (qreal x = left; x < rect.right(); x += _gridSize.width())
    {
        lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    }

    for (qreal y = top; y < rect.bottom(); y += _gridSize.height())
    {
        lines.append(QLineF(rect.left(), y, rect.right(), y));
    }

    QPen pen(Qt::lightGray);
    painter->setPen(pen);
    painter->drawLines(lines.data(), lines.size());
}

void NodeScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        for (const auto& draggedNode: _draggedNodes)
        {
            QPointF newNodePos = event->scenePos() - draggedNode.second;
            if (_gridSnapping)
            {
                newNodePos.setX((round(newNodePos.x() / _gridSize.width())) * _gridSize.width());
                newNodePos.setY((round(newNodePos.y() / _gridSize.height())) * _gridSize.height());
            }

            if (_nodeOverlap || draggedNodePositionIsValid(draggedNode.first, newNodePos))
            {
                draggedNode.first->setPos(newNodePos);
            }
        }

        if (_connectionStartItem)
        {
            QLineF line = _connection->line();
            line.setP2(event->scenePos());
            _connection->setLine(line);

            QGraphicsItem* prevEndItem = _connectionEndItem;
            bool showingPreview = false;

            QGraphicsItem* dock = getTopLevelDockAtPos(event->scenePos());
            if (dock && dock != _connectionStartItem)
            {
                // only show preview once
                if (prevEndItem != dock)
                {
                    qgraphicsitem_cast<CuteDock*>(dock)->showConnectionPreview();
                    _connectionEndItem = dock;
                }
                showingPreview = true;
            }

            // only hide previously shown preview when there was one
            if (prevEndItem && !showingPreview)
            {
                qgraphicsitem_cast<CuteDock*>(prevEndItem)->hideConnectionPreview();
                _connectionEndItem = nullptr;
            }
        }

        event->accept();
        return;
    }

    QGraphicsScene::mouseMoveEvent(event);
}

bool NodeScene::draggedNodePositionIsValid(const QGraphicsItem* node, const QPointF& nodePos) const
{
    // this is the bounding rect the item will have when it has been moved
    QRectF newBoundingRect = node->sceneBoundingRect();
    newBoundingRect.moveTo(nodePos);

    // check if there are other items in this new area
    QList<QGraphicsItem*> potentialColliders = items(newBoundingRect, Qt::IntersectsItemBoundingRect);

    bool collision = std::any_of(potentialColliders.begin(), potentialColliders.end(), [&node](const auto& item)
    {
        return item->type() == CuteNode::Type && item != node;
    });

    return !collision;
}

void NodeScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // needs to be called first so selection is up to date
    QGraphicsScene::mousePressEvent(event);

    if (event->buttons() == Qt::LeftButton)
    {
        QGraphicsItem* clickedItem = itemAt(event->scenePos(), QTransform());
        const int itemType = clickedItem ? clickedItem->type() : 0;

        if (itemType == CuteNode::Type)
        {
            startDraggingSelectedNodes(event->scenePos());
        }
        else if (itemType == CuteDock::Type)
        {
            CuteDock* startDock = qgraphicsitem_cast<CuteDock*>(clickedItem);
            _connection = new CuteConnection{{event->scenePos(), event->scenePos()}, startDock};
            addItem(_connection);

            _connectionStartItem = clickedItem;
        }
    }
}

void NodeScene::startDraggingSelectedNodes(const QPointF& dragStartPos)
{
    const auto selectedNodes = getSelectedNodes();
    _draggedNodes.reserve(selectedNodes.size());
    for (const auto& node: selectedNodes)
    {
        // make sure dragged node is drawn on top of other nodes
        node->setZValue(ZValueShowOnTop);
        // store mouse pointer offset next to dragged node
        _draggedNodes.emplace_back(std::make_pair(node, dragStartPos - node->pos()));
        // this removes glitches when moving an item after panning/scrolling
        invalidate(node->boundingRect());
    }
}

std::vector<QGraphicsItem*> NodeScene::getSelectedNodes() const
{
    QList<QGraphicsItem*> items = selectedItems();

    auto lastNode = std::partition(items.begin(), items.end(), [](const auto& item)
    {
        return qgraphicsitem_cast<CuteNode*>(item) != nullptr;
    });
    return {items.begin(), lastNode};
}

void NodeScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // for some reason buttons() will show Qt::NoButton, so we call button() instead
    if (event->button() == Qt::LeftButton)
    {
        if (_connectionStartItem)
        {
            QGraphicsItem* dock = getTopLevelDockAtPos(event->scenePos());
            if (dock && dock != _connectionStartItem)
            {
                // connection is valid now
                _connection->setEndDock(qgraphicsitem_cast<CuteDock*>(dock));
            }
            else
            {
                // remove connection when it does not end in another dock
                delete _connection;
            }

            _connectionStartItem = nullptr;
            _connection = nullptr;
        }

        // reset z value to default
        for (const auto& node: _draggedNodes)
        {
            node.first->setZValue(ZValueDefault);
        }
        _draggedNodes.clear();
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

QGraphicsItem* NodeScene::getTopLevelDockAtPos(const QPointF& scenePos) const
{
    QList<QGraphicsItem*> itemsAtCursor = items(scenePos, Qt::IntersectsItemBoundingRect, Qt::DescendingOrder);
    const auto iter = std::find_if(itemsAtCursor.begin(), itemsAtCursor.end(), [](const auto& item)
    {
        return item->type() == CuteDock::Type;
    });

    return iter != itemsAtCursor.end() ? *iter : nullptr;
}
