#pragma once

#include <QGraphicsWidget>
#include <QPen>


class CuteNodeWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    explicit CuteNodeWidget(QGraphicsItem* parent = nullptr, Qt::WindowFlags winFlags = 0);
    ~CuteNodeWidget() override;

    const QPointF& getMousePosOffset() const;
    void storeMousePosOffset(const QPointF& mousePos);

private:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QPointF _mousePosOffset{0.0, 0.0};
    QPen    _pen{Qt::black};
};
