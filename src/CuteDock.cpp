#include "CuteDock.h"

#include <QPainter>


namespace
{
    const QColor defaultColor = Qt::green;
    const QColor hoverColor   = Qt::blue;
    const QColor selectColor  = Qt::red;
}


CuteDock::CuteDock(QGraphicsItem* parent, const QPointF& pos)
    : QGraphicsItem(parent)
    , _brush{defaultColor}
{
    setAcceptHoverEvents(true);
    setFlags(flags() | ItemIsSelectable);
    setPos(pos);
}

CuteDock::~CuteDock()
{
}


QRectF CuteDock::boundingRect() const
{
    return _paintRect.marginsAdded({1.0, 1.0, 1.0, 1.0});
}

QPointF CuteDock::getConnectionMagnet() const
{
    return mapToScene(_paintRect.center());
}

void CuteDock::hideConnectionPreview()
{
    _brush.setColor(defaultColor);
    update();
}

void CuteDock::showConnectionPreview()
{
    _brush.setColor(selectColor);
    update();
}

void CuteDock::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    _brush.setColor(hoverColor);
    QGraphicsItem::hoverEnterEvent(event);
}

void CuteDock::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    _brush.setColor(defaultColor);
    QGraphicsItem::hoverLeaveEvent(event);
}

QVariant CuteDock::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemSelectedHasChanged)
    {
        const bool selected = value.toBool() == true;
        _brush.setColor(selected ? selectColor : defaultColor);
    }

    return QGraphicsItem::itemChange(change, value);
}

void CuteDock::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen({_brush.color()});
    painter->setBrush(_brush);
    painter->drawRect(_paintRect);
}
