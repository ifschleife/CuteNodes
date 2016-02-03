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
        for (auto node: _draggedNodes)
        {
            QPointF newNodePos = event->scenePos() - node->getMousePosOffset();
            if (_gridSnapping)
            {
                newNodePos.setX((round(newNodePos.x() / _gridSize.width())) * _gridSize.width());
                newNodePos.setY((round(newNodePos.y() / _gridSize.height())) * _gridSize.height());
            }

            if (draggedNodePositionIsValid(node, newNodePos))
            {
                node->setPos(newNodePos);
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
        return item->topLevelItem() != node;
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
        QGraphicsWidget* clickedWidget = clickedItem ? clickedItem->topLevelWidget() : nullptr;
        bool nodeWasClicked = qgraphicsitem_cast<CuteNodeWidget*>(clickedWidget) != nullptr;

        if (nodeWasClicked)
        {
            _draggedNodes.reserve(selectedItems().size());
            for (auto item: selectedItems())
            {
                auto node = qgraphicsitem_cast<CuteNodeWidget*>(item);
                if (node)
                {
                    _draggedNodes.emplace_back(node);
                    node->storeMousePosOffset(event->scenePos());
                    // this removes glitches when moving an item after panning/scrolling
                    invalidate(node->boundingRect());
                }
            }
        }
    }
}

void NodeScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // for some reason buttons() will show Qt::NoButton, so we call button() instead
    if (event->button() == Qt::LeftButton)
    {
        _draggedNodes.clear();
    }

    QGraphicsScene::mouseReleaseEvent(event);
}
