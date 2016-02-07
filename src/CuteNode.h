#pragma once

#include <QGraphicsItem>
#include <QPen>
#include <QRectF>


class Connector;


class CuteNode : public QGraphicsItem
{
public:
    explicit CuteNode(QGraphicsItem* parent = nullptr);
    ~CuteNode() override;

    enum { Type = UserType + 1 };

    QRectF boundingRect() const override;
    int type() const override { return Type; }

private:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    std::vector<Connector*> _inputConnectors;
    std::vector<Connector*> _outputConnectors;

    QRectF _paintRect{0.0, 0.0, 140.0, 300.0};
    QPen   _pen{Qt::black};
};
