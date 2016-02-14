#include "CuteConnection.h"

#include "CuteDock.h"

#include <QPainter>


CuteConnection::CuteConnection(QGraphicsItem* startItem)
    : QGraphicsPathItem{QPainterPath{startItem->scenePos()}}
    , _connectedItems{startItem, nullptr}
    , _endPoint{startItem->scenePos()}
    , _startPoint{startItem->scenePos()}
{
    setPen(QPen(Qt::black, 2));

    // this is needed so that path is valid, otherwise paint will never be called
    calculateSpline();
}

CuteConnection::~CuteConnection()
{
}


QGraphicsItem* CuteConnection::getStartItem() const
{
    return _connectedItems.first;
}

QGraphicsItem* CuteConnection::getEndItem() const
{
    return _connectedItems.second;
}

void CuteConnection::setEndItem(QGraphicsItem* item)
{
    _connectedItems.second = item;
}

void CuteConnection::updateEndPoint(const QPointF& endPoint)
{
    _endPoint = endPoint;

    update();
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
