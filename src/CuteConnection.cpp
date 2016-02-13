#include "CuteConnection.h"

#include "CuteDock.h"

#include <QPainter>


CuteConnection::CuteConnection(QGraphicsItem* startItem)
    : QGraphicsPathItem{QPainterPath{startItem->pos()}}
    , _connectedItems{startItem, nullptr}
{
    setPen(QPen(Qt::black, 2));
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

    // we need to close the path to trigger a paint event, even calling update won't work
    QPainterPath temp(_startPoint);
    temp.lineTo(_endPoint); // we just set it to a line to not waste too many cycles
    setPath(temp);
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
    const QPointF center = path().pointAtPercent(0.5);
    const QPointF c1 = {center.x(), _startPoint.y()};
    const QPointF c2 = {center.x(), _endPoint.y()};

    QPainterPath newSpline(_startPoint);
    newSpline.cubicTo(c1, c2, _endPoint);

    setPath(newSpline);
}
