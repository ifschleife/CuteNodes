#include "CuteNode.h"

#include "CuteDock.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QPainter>


namespace
{
    constexpr qreal verticalDockOffset = 50.0;
    constexpr qreal verticalDockDist   = 30.0;
}


CuteNode::CuteNode(QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
    setFlags(flags() | ItemContainsChildrenInShape | ItemIsMovable | ItemIsSelectable);
}

CuteNode::~CuteNode()
{
}


QRectF CuteNode::boundingRect() const
{
    return _paintRect.marginsAdded({1.0, 1.0, 1.0, 1.0});
}

void CuteNode::read(const QJsonObject& json)
{
    const auto xpos = json["xpos"].toDouble();
    const auto ypos = json["ypos"].toDouble();
    setPos(xpos, ypos);

    _inputConnectors.clear();
    const auto inputs  = json["inputs"].toArray();
    for (const auto& inputCfg: inputs)
    {
        const auto dockPos = QPointF{0.0, verticalDockOffset + _inputConnectors.size()*verticalDockDist};
        auto dock = new CuteInputDock{this, dockPos};
        dock->read(inputCfg.toObject());

        _inputConnectors.push_back(dock);
    }

    _outputConnectors.clear();
    const auto outputs = json["outputs"].toArray();
    for (const auto& cfg: outputs)
    {
        const auto dockPos = QPointF{120.0, verticalDockOffset + _outputConnectors.size()*verticalDockDist};
        auto dock = new CuteOutputDock{this, dockPos};
        dock->read(cfg.toObject());

        _outputConnectors.push_back(dock);
    }

    _name = json["name"].toString();
}

void CuteNode::write(QJsonObject& json) const
{
    json["name"] = _name;
    json["xpos"] = scenePos().x();
    json["ypos"] = scenePos().y();

    QJsonArray destArray;
    for (const auto& dock: _inputConnectors)
    {
        QJsonObject dockJson;
        dock->write(dockJson);
        destArray.append(dockJson);
    }
    json["inputs"] = destArray;

    QJsonArray sourceArray;
    for (const auto& dock: _outputConnectors)
    {
        QJsonObject dockJson;
        dock->write(dockJson);
        sourceArray.append(dockJson);
    }
    json["outputs"] = sourceArray;
}

QVariant CuteNode::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemSelectedHasChanged)
    {
        const bool nodeSelected = value.toBool();
        _pen.setColor(nodeSelected ? Qt::red : Qt::black);
    }

    return QGraphicsItem::itemChange(change, value);
}

void CuteNode::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    const QBrush brush(Qt::darkGray);
    painter->setPen(_pen);
    painter->setBrush(brush);
    painter->drawRect(_paintRect);
}
