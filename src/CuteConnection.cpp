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

void CuteConnection::setEndItem(QGraphicsItem* item)
{
    _connectedItems.second = item;
}

void CuteConnection::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(pen());

    if (connectionIsValid())
    {
        auto start = qgraphicsitem_cast<CuteDock*>(_connectedItems.first);
        auto end   = qgraphicsitem_cast<CuteDock*>(_connectedItems.second);
        const QPointF p1 = start ? start->getConnectionMagnet() : line().p1();
        const QPointF p2 = end ? end->getConnectionMagnet() : line().p2();

        setLine({p1, p2});
    }

    painter->drawLine(line());
}
