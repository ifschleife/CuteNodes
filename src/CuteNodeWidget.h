#pragma once

#include <QGraphicsWidget>


class CuteNodeWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    explicit CuteNodeWidget(QGraphicsItem* parent = nullptr, Qt::WindowFlags flags = 0);
    ~CuteNodeWidget();

private:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};
