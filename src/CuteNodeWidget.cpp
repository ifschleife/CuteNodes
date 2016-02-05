#include "CuteNodeWidget.h"

#include <QBrush>
#include <QGraphicsRectItem>
#include <QPainter>


CuteNode::CuteNode(QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
    setFlags(flags() | ItemContainsChildrenInShape | ItemIsMovable | ItemIsSelectable);

    QBrush brush(Qt::green);
    for (int i=0; i<5; ++i)
    {
        QRectF connectorRectIn{0.0, 50.0+i*30, 20.0, 20.0};
        QGraphicsRectItem* in = new QGraphicsRectItem{connectorRectIn, this};
        in->setBrush(QBrush{Qt::green});

        QRectF connectorRectOut{130.0, 50.0+i*30, 20.0, 20.0};
        QGraphicsRectItem* out = new QGraphicsRectItem{connectorRectOut, this};
        out->setBrush(QBrush{Qt::red});
    }
}

CuteNode::~CuteNode()
{
}


QRectF CuteNode::boundingRect() const
{
    return {0.0, 0.0, 150.0, 300.0};
}

QVariant CuteNode::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemSelectedHasChanged)
    {
        const bool nodeSelected = value.toBool() == true;
        _pen.setColor(nodeSelected ? Qt::red : Qt::black);
    }

    return QGraphicsItem::itemChange(change, value);
}

void CuteNode::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    const QBrush brush(Qt::darkGray);
    painter->setPen(_pen);
    painter->setBrush(brush);
    painter->drawRect(boundingRect());
}
