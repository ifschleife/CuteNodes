#pragma once

#include <QGraphicsItem>
#include <QPen>
#include <QRectF>


class CuteNode : public QGraphicsItem
{
public:
    explicit CuteNode(QGraphicsItem* parent = nullptr);
    ~CuteNode() override;

    QRectF boundingRect() const override;

private:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QRectF _paintRect{0.0, 0.0, 140.0, 300.0};
    QPen   _pen{Qt::black};
};
