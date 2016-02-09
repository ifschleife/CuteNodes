#include "CuteConnection.h"

#include "CuteDock.h"

#include <QPainter>


CuteConnection::CuteConnection(const QLineF& line, CuteDock* startItem)
    : QGraphicsLineItem(line)
    , _connectedDocks{startItem, nullptr}
{
    setPen(QPen(Qt::black, 2));
}

CuteConnection::~CuteConnection()
{
}


void CuteConnection::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(pen());

    if (connectionIsValid())
    {
        const QPointF p1 = _connectedDocks.first->getConnectionMagnet();
        const QPointF p2 = _connectedDocks.second->getConnectionMagnet();

        setLine({p1, p2});
    }

    painter->drawLine(line());
}
