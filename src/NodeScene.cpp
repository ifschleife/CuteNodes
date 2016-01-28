#include "NodeScene.h"

#include "CuteNodeWidget.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>


NodeScene::NodeScene(const QRectF& sceneRect)
    : QGraphicsScene(sceneRect, nullptr)
    , _draggedItem{nullptr}
    , _draggingMousePointerOffset{0.0f, 0.0f}
    , _gridSize{20, 20}
    , _gridSnapping{true}
{

}

NodeScene::~NodeScene()
{

}

void NodeScene::ToggleGridSnapping()
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
            QPointF scenePos = event->scenePos() - _draggingMousePointerOffset;

            if (_gridSnapping)
            {
                scenePos.setX((round(scenePos.x() / _gridSize.width())) * _gridSize.width());
                scenePos.setY((round(scenePos.y() / _gridSize.height())) * _gridSize.height());
            }

            _draggedItem->setPos(scenePos);

            event->accept();
            return;
        }
    }

    event->ignore();
}

void NodeScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        QGraphicsItem* lowestItem = GetLowestItemThatWasClicked(event->scenePos());
        // we only want to enter dragging state when the lowest item is of type CuteNodeWidget
        _draggedItem = qgraphicsitem_cast<CuteNodeWidget*>(lowestItem);

        if (_draggedItem)
        {
            _draggingMousePointerOffset = event->scenePos() - _draggedItem->pos();

            // this removes glitches when moving an item after scrolling
            invalidate(_draggedItem->sceneBoundingRect());

            event->accept();
            return;
        }
    }

    event->ignore();
}

void NodeScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
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

QGraphicsItem* NodeScene::GetLowestItemThatWasClicked(const QPointF& clickPos)
{
    QList<QGraphicsItem*> clickedItems = items(clickPos, Qt::IntersectsItemShape, Qt::AscendingOrder);
    if (clickedItems.size() == 0)
    {
        return nullptr;
    }

    return clickedItems[0];
}
