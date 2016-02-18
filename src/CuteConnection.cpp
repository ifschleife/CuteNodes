#include "CuteConnection.h"

#include "CuteDock.h"

#include <QPainter>


namespace
{
    const QPen defaultPen {Qt::black, 2};
    const QPen hoverPen   {Qt::blue,  2};
    const QPen selectPen  {Qt::red,   2};
}


CuteConnection::CuteConnection(QGraphicsItem* startItem)
    : QGraphicsPathItem{QPainterPath{startItem->scenePos()}}
    , _connectedItems{startItem, nullptr}
    , _endPoint{startItem->scenePos()}
    , _startPoint{startItem->scenePos()}
{
    setAcceptHoverEvents(true);
    setFlags(flags() | ItemIsSelectable);
    setPen(defaultPen);

    // this is needed so that path is valid, otherwise paint will never be called
    calculateSpline();
}

CuteConnection::~CuteConnection()
{
    // invalidate both ends of the connection before deleting the connection
    auto startItem = qgraphicsitem_cast<CuteOutputDock*>(_connectedItems.first);
    if (startItem)
        startItem->setConnection(nullptr);
    auto endItem = qgraphicsitem_cast<CuteInputDock*>(_connectedItems.second);
    if (endItem)
        endItem->setConnection(nullptr);
}


QGraphicsItem* CuteConnection::getEndItem() const
{
    return _connectedItems.second;
}

void CuteConnection::setEndItem(QGraphicsItem* item)
{
    _connectedItems.second = item;
}

QGraphicsItem* CuteConnection::getStartItem() const
{
    return _connectedItems.first;
}

void CuteConnection::setAsValid()
{
    auto inputDock  = qgraphicsitem_cast<CuteOutputDock*>(_connectedItems.first);
    auto outputDock = qgraphicsitem_cast<CuteInputDock*>(_connectedItems.second);
    if (!inputDock || !outputDock)
        return;

    inputDock->setConnection(this);
    outputDock->setConnection(this);
}

void CuteConnection::updateEndPoint(const QPointF& endPoint)
{
    _endPoint = endPoint;

    update();
}


void CuteConnection::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    if (!isSelected())
        setPen(hoverPen);
    setZValue(1.0);

    QGraphicsPathItem::hoverEnterEvent(event);
}

void CuteConnection::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    if (!isSelected())
    {
        setZValue(-2.0);
        setPen(defaultPen);
    }

    QGraphicsPathItem::hoverLeaveEvent(event);
}

QVariant CuteConnection::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemSelectedHasChanged)
    {
        const bool selected = value.toBool() == true;
        setPen(selected ? selectPen : defaultPen);
        setZValue(selected ? 1.0 : -2.0);
    }

    return QGraphicsPathItem::itemChange(change, value);
}


void CuteConnection::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(pen());

    snapEndPointsToItems();
    calculateSpline();

    painter->drawPath(path());
}

void CuteConnection::snapEndPointsToItems()
{
    const auto startDock = qgraphicsitem_cast<CuteOutputDock*>(_connectedItems.first);
    if (startDock)
        _startPoint = startDock->getConnectionMagnet();

    const auto endItem = qgraphicsitem_cast<CuteInputDock*>(_connectedItems.second);
    if (endItem)
        _endPoint = endItem->getConnectionMagnet();
}

void CuteConnection::calculateSpline()
{
    // initially the path will be empty, to not get a glitch on the first paint we use the startPoint as center
    const QPointF center = path().isEmpty() ? _startPoint : path().pointAtPercent(0.5);
    const QPointF c1 = {center.x(), _startPoint.y()};
    const QPointF c2 = {center.x(), _endPoint.y()};

    QPainterPath newSpline(_startPoint);
    newSpline.cubicTo(c1, c2, _endPoint);

    setPath(newSpline);
}
