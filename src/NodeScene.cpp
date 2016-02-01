#include "NodeScene.h"

#include "CuteNodeWidget.h"

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>


NodeScene::NodeScene(const QRectF& sceneRect)
    : QGraphicsScene(sceneRect, nullptr)
{

}

NodeScene::~NodeScene()
{

}


void NodeScene::toggleGridSnapping()
{
    _gridSnapping = !_gridSnapping;
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
        if (_draggedItem)
        {
            // Ensure that the item's offset from the mouse cursor stays the same.
            QPointF newScenePos = event->scenePos() - _draggingMousePointerOffset;

            if (_gridSnapping)
            {
                newScenePos.setX((round(newScenePos.x() / _gridSize.width())) * _gridSize.width());
                newScenePos.setY((round(newScenePos.y() / _gridSize.height())) * _gridSize.height());
            }

            if (draggedNodePositionIsValid(newScenePos))
            {
                _draggedItem->setPos(newScenePos);
            }

            event->accept();
            return;
        }
    }

    event->ignore();
}

bool NodeScene::draggedNodePositionIsValid(const QPointF& nodePos) const
{
    // this is the bounding rect the item will have when it has been moved
    QRectF newBoundingRect = _draggedItem->sceneBoundingRect();
    newBoundingRect.moveTo(nodePos);

    // check if there are other items in this new area
    QList<QGraphicsItem*> itemsInNewBoundingRect = items(newBoundingRect, Qt::IntersectsItemBoundingRect);
    for (const auto collidingItem: itemsInNewBoundingRect)
    {
        // ignore child items
        QGraphicsItem* topItem = collidingItem->topLevelItem();
        if (topItem != _draggedItem)
        {
            return false;
        }
    }

    return true;
}

void NodeScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        QGraphicsItem* clickedItem = itemAt(event->scenePos(), QTransform());
        QGraphicsWidget* clickedNode = clickedItem ? clickedItem->topLevelWidget() : nullptr;

        QList<QGraphicsItem*> selectedNodes = selectedItems();
        for (auto selectedNode: selectedNodes)
        {
            selectedNode->setSelected(false);
        }

        if (clickedNode)
        {
            clickedNode->setSelected(true);

            _draggedItem = clickedNode;
            _draggingMousePointerOffset = event->scenePos() - _draggedItem->pos();

            // this removes glitches when moving an item after scrolling
            invalidate(_draggedItem->sceneBoundingRect());
        }

        event->accept();
        return;
    }

    event->ignore();
}

void NodeScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // for some reason buttons() will show Qt::NoButton, so we call button() instead
    if (event->button() == Qt::LeftButton)
    {
        if (_draggedItem)
        {
            _draggedItem = nullptr;

            event->accept();
            return;
        }
    }

    event->ignore();
}

void NodeScene::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    if (_draggedItem)
    {
        // this removes glitches when moving an item while scrolling
        invalidate(_draggedItem->sceneBoundingRect());
    }
    QGraphicsScene::wheelEvent(event);
}
