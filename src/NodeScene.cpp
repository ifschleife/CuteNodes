#include "NodeScene.h"

#include "CuteConnection.h"
#include "CuteDock.h"
#include "CuteNode.h"

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
    const QRectF backgroundRect = sceneRect() & rect;
    const qreal left = int(backgroundRect.left()) - (int(backgroundRect.left()) % _gridSize.width());
    const qreal top = int(backgroundRect.top()) - (int(backgroundRect.top()) % _gridSize.height());

    QVarLengthArray<QLineF, 100> lines;

    for (qreal x = left; x < backgroundRect.right(); x += _gridSize.width())
    {
        lines.append(QLineF(x, backgroundRect.top(), x, backgroundRect.bottom()));
    }

    for (qreal y = top; y < backgroundRect.bottom(); y += _gridSize.height())
    {
        lines.append(QLineF(backgroundRect.left(), y, backgroundRect.right(), y));
    }

    painter->setPen(Qt::lightGray);
    painter->drawLines(lines.data(), lines.size());

    painter->setPen({Qt::darkGray, 3});
    painter->drawRect(sceneRect());
}

void NodeScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        if (_draggedNodes.size() > 0)
        {
            handleNodeDragging(event->scenePos());
        }
        else if (_drawnConnection)
        {
            handleConnectionDrawing(event->scenePos());
        }

        event->accept();
        return;
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void NodeScene::handleNodeDragging(const QPointF& mousePos)
{
    for (const auto& draggedNode: _draggedNodes)
    {
        QPointF newNodePos = mousePos - draggedNode.second;
        if (_gridSnapping)
        {
            newNodePos.setX((round(newNodePos.x() / _gridSize.width())) * _gridSize.width());
            newNodePos.setY((round(newNodePos.y() / _gridSize.height())) * _gridSize.height());
        }

        if (draggedNodePositionInSceneRect(draggedNode.first, newNodePos))
        {
            if (_nodeOverlap || draggedNodePositionIsValid(draggedNode.first, newNodePos))
            {
                draggedNode.first->setPos(newNodePos);
            }
        }
    }
}

bool NodeScene::draggedNodePositionInSceneRect(const QGraphicsItem* node, const QPointF& nodePos) const
{
    // this is the bounding rect the item will have when it has been moved
    QRectF newBoundingRect = node->sceneBoundingRect();
    newBoundingRect.moveTo(nodePos);

    return sceneRect().contains(newBoundingRect);
}

bool NodeScene::draggedNodePositionIsValid(const QGraphicsItem* node, const QPointF& nodePos) const
{
    // this is the bounding rect the item will have when it has been moved
    QRectF newBoundingRect = node->sceneBoundingRect();
    newBoundingRect.moveTo(nodePos);

    // check if there are other items in this new area
    const QList<QGraphicsItem*> potentialColliders = items(newBoundingRect, Qt::IntersectsItemBoundingRect);

    bool collision = std::any_of(potentialColliders.begin(), potentialColliders.end(), [&node](const auto& item)
    {
        return item->type() == CuteNode::Type && item != node;
    });

    return !collision;
}

void NodeScene::handleConnectionDrawing(const QPointF& mousePos)
{
    _drawnConnection->updateEndPoint(mousePos);

    auto prevEndItem = _drawnConnection->getEndItem();
    bool showingPreview = false;

    auto item = getTopLevelItemAtPos(mousePos, CuteInputDock::Type);
    auto dock = qgraphicsitem_cast<CuteInputDock*>(item);

    if (dock && !dock->getConnection())
    {
        // only show preview once
        if (prevEndItem != dock)
        {
            qgraphicsitem_cast<CuteInputDock*>(dock)->showConnectionPreview();
            _drawnConnection->setEndItem(dock);
        }
        showingPreview = true;
    }

    // only hide previously shown preview when there was one
    if (prevEndItem && !showingPreview)
    {
        qgraphicsitem_cast<CuteInputDock*>(prevEndItem)->hideConnectionPreview();
        _drawnConnection->setEndItem(nullptr);
    }
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
        else if (itemType == CuteOutputDock::Type)
        {
            _drawnConnection = new CuteConnection{clickedItem};
            // this ensures that the connection's spline is valid
            _drawnConnection->updateEndPoint(event->scenePos());
            addItem(_drawnConnection);
        }
        else if (itemType == CuteInputDock::Type)
        {
            auto dock = qgraphicsitem_cast<CuteInputDock*>(clickedItem);
            auto connection = dock->getConnection();
            if (connection)
            {
                dock->setConnection(nullptr); // remove connection from dock

                // set connection to invalid (drawing) state
                _drawnConnection = connection;
                _drawnConnection->setEndItem(nullptr);
            }
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
        if (_drawnConnection)
        {
            auto item = getTopLevelItemAtPos(event->scenePos(), CuteInputDock::Type);
            auto dock = qgraphicsitem_cast<CuteInputDock*>(item);

            if (dock && !dock->getConnection())
            {
                _drawnConnection->setEndItem(dock);
                _drawnConnection->setZValue(-2.0); // hide behind nodes
                _drawnConnection->setAsValid();
            }
            else
            {
                // remove connection when it does not end in another dock
                delete _drawnConnection;
            }

            _drawnConnection = nullptr;
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

QGraphicsItem* NodeScene::getTopLevelItemAtPos(const QPointF& scenePos, int itemType) const
{
    QList<QGraphicsItem*> itemsAtPos = items(scenePos, Qt::IntersectsItemBoundingRect, Qt::DescendingOrder);
    const auto iter = std::find_if(itemsAtPos.begin(), itemsAtPos.end(), [&itemType](const auto& item)
    {
        return item->type() == itemType;
    });

    return iter != itemsAtPos.end() ? *iter : nullptr;
}
