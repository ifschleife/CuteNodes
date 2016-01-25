#include "nodescene.h"

#include "cutenodewidget.h"

#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>


NodeScene::NodeScene()
    : _draggedItem{nullptr}
    , _draggingMousePointerOffset{0.0f, 0.0f}
    , _gridSize{25, 25}
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
    if (_draggedItem)
    {
        // Ensure that the item's offset from the mouse cursor stays the same.
        _draggedItem->setPos(event->scenePos() - _draggingMousePointerOffset);
    }
    else
    {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void NodeScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem* lowestItem = GetLowestItemThatWasClicked(event->scenePos());

    // we only want to enter dragging state when the lowest item is of type CuteNodeWidget
    _draggedItem = qgraphicsitem_cast<CuteNodeWidget*>(lowestItem);
    if (_draggedItem)
    {
        _draggingMousePointerOffset = event->scenePos() - _draggedItem->pos();
    }
    else
    {
        QGraphicsScene::mousePressEvent(event);
    }
}

void NodeScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (_draggedItem)
    {
        int x = static_cast<int>(floor(event->scenePos().x() / _gridSize.width()) * _gridSize.width());
        int y = static_cast<int>(floor(event->scenePos().y() / _gridSize.height()) * _gridSize.height());
        _draggedItem->setPos(x, y);
        _draggedItem = nullptr;
    }
    else
    {
        QGraphicsScene::mouseReleaseEvent(event);
    }
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
