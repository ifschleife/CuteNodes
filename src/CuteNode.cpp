#include "CuteNode.h"

#include "CuteDock.h"

#include <QPainter>


CuteNode::CuteNode(const QPointF& scenePos, QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
    setFlags(flags() | ItemContainsChildrenInShape | ItemIsMovable | ItemIsSelectable);
    setPos(scenePos);

    for (int i=0; i<5; ++i)
    {
        _inputConnectors.emplace_back(new CuteInputDock{this, {0.0, 50.0 + i*30.0}});
        _outputConnectors.emplace_back(new CuteOutputDock{this, {120.0, 50.0 + i*30.0}});
    }
}

CuteNode::~CuteNode()
{
}


QRectF CuteNode::boundingRect() const
{
    return _paintRect.marginsAdded({1.0, 1.0, 1.0, 1.0});
}

QVariant CuteNode::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemSelectedHasChanged)
    {
        const bool nodeSelected = value.toBool();
        _pen.setColor(nodeSelected ? Qt::red : Qt::black);
    }

    return QGraphicsItem::itemChange(change, value);
}

void CuteNode::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    const QBrush brush(Qt::darkGray);
    painter->setPen(_pen);
    painter->setBrush(brush);
    painter->drawRect(_paintRect);
}
