#include "CuteNode.h"

#include "Connector.h"

#include <QBrush>
#include <QGraphicsRectItem>
#include <QPainter>


CuteNode::CuteNode(QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
    setFlags(flags() | ItemContainsChildrenInShape | ItemIsMovable | ItemIsSelectable);

    for (int i=0; i<5; ++i)
    {
        _inputConnectors.emplace_back(new Connector{this, {0.0, 50.0 + i*30.0}});
        _outputConnectors.emplace_back(new Connector{this, {120.0, 50.0 + i*30.0}});
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
