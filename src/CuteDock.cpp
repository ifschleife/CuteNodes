#include "CuteDock.h"

#include "CuteConnection.h"

#include <QJsonObject>
#include <QPainter>


namespace
{
    const QColor defaultColor = Qt::green;
    const QColor hoverColor   = Qt::blue;
}


CuteDock::CuteDock(QGraphicsItem* parent, const QPointF& pos)
    : QGraphicsItem(parent)
    , _brush{defaultColor}
{
    setAcceptHoverEvents(true);
    setPos(pos);
}

CuteDock::~CuteDock()
{
    if (_connection)
    {
        // the connection is not parented through Qt so we need to delete it manually
        delete _connection;
    }
}


void CuteDock::read(const QJsonObject& json)
{
    const auto idString = json["id"].toString();
    _uuid = QUuid(idString);
}

void CuteDock::write(QJsonObject& json) const
{
    json["id"] = _uuid.toString();
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
    _brush.setColor(hoverColor);
    update();
}

void CuteDock::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    if (!isSelected())
        _brush.setColor(hoverColor);

    QGraphicsItem::hoverEnterEvent(event);
}

void CuteDock::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    if (!isSelected())
        _brush.setColor(defaultColor);

    QGraphicsItem::hoverLeaveEvent(event);
}

void CuteDock::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen({_brush.color()});
    painter->setBrush(_brush);
    painter->drawRect(_paintRect);
}
