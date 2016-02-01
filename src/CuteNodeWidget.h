#pragma once

#include <QGraphicsWidget>
#include <QPen>


class CuteNodeWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    explicit CuteNodeWidget(QGraphicsItem* parent = nullptr, Qt::WindowFlags winFlags = 0);
    ~CuteNodeWidget() override;

private:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QPen _pen{Qt::black};
};
