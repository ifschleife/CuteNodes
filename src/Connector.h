#pragma once

#include <QBrush>
#include <QGraphicsItem>
#include <QRectF>


class Connector : public QGraphicsItem
{
public:
    explicit Connector(QGraphicsItem* parent, const QPointF& pos);
    ~Connector() override;

    enum { Type = UserType + 2 };

    QRectF boundingRect() const override;
    int type() const override { return Type; }

private:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QRectF _paintRect{0.0, 0.0, 20.0, 10.0};
    QBrush _brush;
};
