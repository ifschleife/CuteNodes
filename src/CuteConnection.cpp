#include "CuteConnection.h"

#include "CuteDock.h"

#include <QPainter>


namespace
{
    const QPen defaultPen {Qt::black, 2};
    const QPen hoverPen   {Qt::blue,  2};
    const QPen selectPen  {Qt::red,   2};

    constexpr qreal arrowLength            = 15.0;
    constexpr qreal arrowLengthHeightRatio = 0.3;
    constexpr qreal arrowPositionInPercent = 0.4;
    constexpr qreal minLengthToShowArrow   = 50.0;

    const auto arrowFillColor{Qt::green};

    constexpr qreal degToArc(qreal deg)
    {
        return deg * M_PI / 180.0;
    }
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

    // we draw the arrow head a second time so we can fill it with a different color without
    // "filling" up the bezier curve. this can probably be optimized away somehow.
    const auto polygons = path().toSubpathPolygons();
    if (polygons.size() > 1)
    {
        const auto arrow = polygons.at(1);
        painter->setBrush({arrowFillColor});
        painter->drawPolygon(arrow);
    }
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

    QPainterPath spline(_startPoint);
    spline.cubicTo(c1, c2, _endPoint);

    if (spline.length() > minLengthToShowArrow)
        addArrowToSpline(spline);

    setPath(spline);
}

void CuteConnection::addArrowToSpline(QPainterPath& spline) const
{
    const qreal angleAtArrowEnd{spline.angleAtPercent(arrowPositionInPercent)};
    const qreal dy{sin(degToArc(angleAtArrowEnd)) * arrowLength};
    const qreal dx{cos(degToArc(angleAtArrowEnd)) * arrowLength};

    const QPointF arrowEndPoint = spline.pointAtPercent(arrowPositionInPercent);
    const QPointF tangentEnd{arrowEndPoint.x() - dx, arrowEndPoint.y() + dy};

    const qreal dxScaled{dx * arrowLengthHeightRatio};
    const qreal dyScaled{dy * arrowLengthHeightRatio};
    const QPointF p2{tangentEnd.x() - dyScaled, tangentEnd.y() - dxScaled};
    const QPointF p3{tangentEnd.x() + dyScaled, tangentEnd.y() + dxScaled};

    QPainterPath arrowHead{arrowEndPoint};
    arrowHead.lineTo(p3);
    arrowHead.lineTo(p2);
    arrowHead.lineTo(arrowEndPoint);

    spline.moveTo(arrowEndPoint);
    spline.addPath(arrowHead);
}
