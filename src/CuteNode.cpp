#include "CuteNode.h"

#include "CuteDock.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QPainter>


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
    static int inctr = 0;
    for (const auto& input: inputs)
    {
        const auto dockPos = QPointF{0.0, 50.0 + inctr*30.0};
        _inputConnectors.emplace_back(new CuteInputDock{this, dockPos});
        ++inctr;
    }

    _outputConnectors.clear();
    const auto outputs = json["outputs"].toArray();
    static int outctr = 0;
    for (const auto& output: outputs)
    {
        const auto dockPos = QPointF{120.0, 50.0 + outctr*30.0};
        _inputConnectors.emplace_back(new CuteInputDock{this, dockPos});
        ++outctr;
    }

    _name = json["name"].toString();
}

void CuteNode::write(QJsonObject& json) const
{
    Q_UNUSED(json);
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
