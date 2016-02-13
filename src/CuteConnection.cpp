#include "CuteConnection.h"

#include "CuteDock.h"

#include <QPainter>


CuteConnection::CuteConnection(const QLineF& line, QGraphicsItem* startItem)
    : QGraphicsLineItem(line)
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

void CuteConnection::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(pen());

    snapLineToItems();
    painter->drawLine(line());
}

void CuteConnection::snapLineToItems()
{
    QPointF p1 = line().p1();
    QPointF p2 = line().p2();

    const auto startItem = qgraphicsitem_cast<CuteOutputDock*>(_connectedItems.first);
    if (startItem)
        p1 = startItem->getConnectionMagnet();

    const auto endItem = qgraphicsitem_cast<CuteInputDock*>(_connectedItems.second);
    if (endItem)
        p2 = endItem->getConnectionMagnet();

    setLine({p1, p2});
}
