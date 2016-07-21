#include "CuteDock.h"

#include "CuteConnection.h"

#include <QFont>
#include <QJsonObject>
#include <QPainter>


namespace
{
    const QColor    defaultColor = Qt::green;
    const QColor    hoverColor   = Qt::blue;
    const QFont     sansFont{"DejaVu Sans Mono", 8};

    constexpr qreal connectorToNameMargin{5.0};
    constexpr qreal connectorDefaultHeight{10.0};
    constexpr qreal connectorWidth{10.0};
}


CuteDock::CuteDock(QGraphicsItem* parent, const DockType dockType)
    : QGraphicsItem(parent)
    , _brush{defaultColor}
    , _type{dockType}
{
    setAcceptHoverEvents(true);

    calculateConnectorPath(connectorDefaultHeight);
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

    if (isInputDock())
    {
        _namePos.setX(_connectorPath.boundingRect().right() + connectorToNameMargin);
    }
    else
    {
        _namePos.setX(_connectorPath.boundingRect().left() - _name.size().width() - connectorToNameMargin);
    }

    calculateConnectorPath(_name.size().height());
}

void CuteDock::write(QJsonObject& json) const
{
    json["id"]   = _uuid.toString();
    json["name"] = _name.text();
}

QRectF CuteDock::boundingRect() const
{
    const auto width  = parentItem() ? parentItem()->boundingRect().width() - 2.0 : 100.0;
    const auto height = _name.size().height();
    return {0.0, 0.0, width, height};
}

bool CuteDock::canAcceptConnectionAtPos(const QPointF& scenePos) const
{
    return isInputDock() && _connectorPath.boundingRect().contains(mapFromScene(scenePos));
}

QPointF CuteDock::getConnectionMagnet() const
{
    return mapToScene(_connectorPath.boundingRect().center());
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

void CuteDock::calculateConnectorPath(const qreal connectorHeight)
{
    const auto connectorRect = QRectF{0.0, 0.0, connectorWidth, connectorHeight};
    auto path = QPainterPath{{0.0, 0.0}};
    path.moveTo(connectorRect.right(), connectorRect.center().y());
    path.lineTo(connectorRect.bottomLeft());
    path.lineTo(connectorRect.topLeft());
    path.closeSubpath();

    if (isOutputDock())
    {
        path.translate(boundingRect().width() - connectorRect.width(), 0.0);
    }

    _connectorPath.swap(path);
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
    painter->drawPath(_connectorPath);

    painter->setPen(Qt::black);
    painter->setFont(sansFont);
    painter->drawStaticText(_namePos, _name);
}
