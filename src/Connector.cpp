#include "Connector.h"

#include <QPainter>


namespace
{
    const QColor defaultColor = Qt::green;
    const QColor hoverColor   = Qt::blue;
    const QColor selectColor  = Qt::red;
}


Connector::Connector(QGraphicsItem* parent, const QPointF& pos)
    : QGraphicsItem(parent)
    , _brush{defaultColor}
{
    setAcceptHoverEvents(true);
    setFlags(flags() | ItemIsSelectable);
    setPos(pos);
}

Connector::~Connector()
{
}


QRectF Connector::boundingRect() const
{
    return _paintRect.marginsAdded({1.0, 1.0, 1.0, 1.0});
}

void Connector::hideConnectionPreview()
{
    _brush.setColor(defaultColor);
    update();
}

void Connector::showConnectionPreview()
{
    _brush.setColor(selectColor);
    update();
}

void Connector::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    _brush.setColor(hoverColor);
    QGraphicsItem::hoverEnterEvent(event);
}

void Connector::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    _brush.setColor(defaultColor);
    QGraphicsItem::hoverLeaveEvent(event);
}

QVariant Connector::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemSelectedHasChanged)
    {
        const bool selected = value.toBool() == true;
        _brush.setColor(selected ? selectColor : defaultColor);
    }

    return QGraphicsItem::itemChange(change, value);
}

void Connector::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen({_brush.color()});
    painter->setBrush(_brush);
    painter->drawRect(_paintRect);
}
