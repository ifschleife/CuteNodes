#include "CuteNode.h"

#include "CuteDock.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QPainter>


namespace
{
    constexpr QRectF headerRect         = {5.0, 5.0, 130.0, 50.0};
    constexpr qreal  verticalDockOffset = 70.0;
    constexpr qreal  verticalDockDist   = 20.0;
}


CuteNode::CuteNode(QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , _icon{}
{
    setFlags(flags() | ItemContainsChildrenInShape | ItemIsMovable | ItemIsSelectable);
    _icon.fill(Qt::black);
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
    qreal bottomPos = 70.0;

    const auto xpos = json["xpos"].toDouble();
    const auto ypos = json["ypos"].toDouble();
    setPos(xpos, ypos);

    _inputConnectors.clear();
    const auto inputs  = json["inputs"].toArray();
    for (const auto& inputCfg: inputs)
    {
        auto dock = new CuteDock{this, CuteDock::DockType::Input};
        dock->read(inputCfg.toObject());

        const auto dockPosVertical = verticalDockOffset + _inputConnectors.size()*verticalDockDist;
        dock->setPos({0.0, dockPosVertical});

        bottomPos = dockPosVertical;

        _inputConnectors.push_back(dock);
    }

    _outputConnectors.clear();
    const auto outputs = json["outputs"].toArray();
    for (const auto& cfg: outputs)
    {
        auto dock = new CuteDock{this, CuteDock::DockType::Output};
        dock->read(cfg.toObject());

        const auto dockPosVertical = verticalDockOffset + (_inputConnectors.size() + _outputConnectors.size()) * verticalDockDist;
        dock->setPos({0.0, dockPosVertical});

        bottomPos = dockPosVertical;

        _outputConnectors.push_back(dock);
    }

    _name = json["name"].toString();

    QFile pathToIcon(json["icon"].toString());
    if (pathToIcon.exists())
    {
        _icon.load(pathToIcon.fileName());
        _iconPath = pathToIcon.fileName();
    }

    bottomPos += (1.5*verticalDockDist); // padding

    _paintRect.setBottom(bottomPos);
}

void CuteNode::write(QJsonObject& json) const
{
    json["icon"] = _iconPath;
    json["name"] = _name.text();
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

    paintHeader(painter);
}

void CuteNode::paintHeader(QPainter* painter)
{
    painter->setPen(Qt::black);
    painter->drawRect(headerRect);
    if (!_icon.isNull())
        painter->drawPixmap({5, 5, 50, 50}, _icon);
    painter->drawStaticText(QPointF{80.0, 25.0}, _name);
}
