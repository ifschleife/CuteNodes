#include "CuteDock.h"

#include "CuteConnection.h"

#include <QFont>
#include <QJsonObject>
#include <QPainter>


namespace
{
    const QColor defaultColor = Qt::green;
    const QColor hoverColor   = Qt::blue;
    const QFont sansFont{"DejaVu Sans Mono", 6};
}


CuteDock::CuteDock(QGraphicsItem* parent, const DockType dockType, const QPointF& pos)
    : QGraphicsItem(parent)
    , _brush{defaultColor}
    , _type{dockType}
{
    setAcceptHoverEvents(true);
    setPos(pos);

    if (parent)
        _paintRect.setWidth(parent->boundingRect().width() - 2.0);
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
    _name = json["name"].toString();
    _name.prepare(QTransform(), sansFont);
}

void CuteDock::write(QJsonObject& json) const
{
    json["id"]   = _uuid.toString();
    json["name"] = _name.text();
}

QRectF CuteDock::boundingRect() const
{
    return _paintRect;
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

    painter->setPen(Qt::black);
    painter->setFont(sansFont);
//    _name.prepare(QTransform(), sansFont);

    if (isInputDock())
    {
        painter->drawStaticText(QPointF{25.0, 0.0}, _name);
    }
    else if (isOutputDock())
    {
        const qreal xpos = -_name.size().width() - 5.0;
        painter->drawStaticText(QPointF{xpos, 0.0}, _name);
    }
}
