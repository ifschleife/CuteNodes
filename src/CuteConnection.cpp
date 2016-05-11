#include "CuteConnection.h"

#include "CuteDock.h"

#include <math.h>
#include <QPainter>


namespace
{
    const QPen defaultPen {Qt::black, 2};
    const QPen deletePen  {Qt::black, 2, Qt::DashLine};
    const QPen hoverPen   {Qt::blue,  2};
    const QPen selectPen  {Qt::red,   2};

    constexpr qreal arrowEndInPercent      = 0.5;
    constexpr qreal arrowLength            = 15.0;
    constexpr qreal arrowLengthHeightRatio = 0.3;
    constexpr qreal minLengthToShowArrow   = 50.0;

    const auto arrowFillColor{Qt::green};

    constexpr qreal degToArc(qreal deg)
    {
        return deg * M_PI / 180.0;
    }
}


CuteConnection::CuteConnection(CuteDock* source)
    : QGraphicsPathItem{QPainterPath{source->scenePos()}}
    , _connectedDocks{source, nullptr}
    , _destItemPos{source->scenePos()}
    , _sourceItemPos{source->scenePos()}
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
    if (_connectedDocks.first)
        _connectedDocks.first->setConnection(nullptr);
    if (_connectedDocks.second)
        _connectedDocks.second->setConnection(nullptr);
}


CuteDock* CuteConnection::getDestinationItem() const
{
    return _connectedDocks.second;
}

void CuteConnection::setDestinationItem(CuteDock* item)
{
    _connectedDocks.second = item;
}

CuteDock* CuteConnection::getSourceItem() const
{
    return _connectedDocks.first;
}

void CuteConnection::hideDeletionPreview()
{
    setPen(defaultPen);
}

void CuteConnection::showDeletionPreview()
{
    setPen(deletePen);
}

void CuteConnection::setAsValid()
{
    if (!_connectedDocks.first || !_connectedDocks.second)
        return;

    _connectedDocks.first->setConnection(this);
    _connectedDocks.second->setConnection(this);
}

void CuteConnection::updateDestinationItemPosition(const QPointF& destItemPos)
{
    _destItemPos = destItemPos;

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
    if (_connectedDocks.first)
        _sourceItemPos = _connectedDocks.first->getConnectionMagnet();

    if (_connectedDocks.second)
        _destItemPos = _connectedDocks.second->getConnectionMagnet();
}

void CuteConnection::calculateSpline()
{
    // initially the path will be empty, to not get a glitch on the first paint we use the startPoint as center
    const QPointF center = path().isEmpty() ? _sourceItemPos : path().pointAtPercent(0.5);
    const QPointF c1 = {center.x(), _sourceItemPos.y()};
    const QPointF c2 = {center.x(), _destItemPos.y()};

    QPainterPath spline(_sourceItemPos);
    spline.cubicTo(c1, c2, _destItemPos);

    if (spline.length() > minLengthToShowArrow)
        addArrowToSpline(spline);

    setPath(spline);
}

void CuteConnection::addArrowToSpline(QPainterPath& spline) const
{
    const qreal arrowStartInPercent{spline.percentAtLength((spline.length() * arrowEndInPercent) - arrowLength)};
    const qreal angleAtArrowStart{spline.angleAtPercent(arrowStartInPercent)};
    const qreal dy{sin(degToArc(angleAtArrowStart)) * arrowLength};
    const qreal dx{cos(degToArc(angleAtArrowStart)) * arrowLength};

    const QPointF arrowEndPoint = spline.pointAtPercent(arrowStartInPercent); // p1 of arrow triangle
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
