#pragma once

#include <QBrush>
#include <QGraphicsItem>


class CuteConnection;


class CuteDock : public QGraphicsItem
{
public:
    explicit CuteDock(QGraphicsItem* parent, const QPointF& pos);
    ~CuteDock() override;

    enum { Type = UserType + 2 };

    QRectF boundingRect() const override;
    int type() const override { return Type; }

    QPointF getConnectionMagnet() const;

    CuteConnection* getConnection() const          { return _connection; }
    void setConnection(CuteConnection* connection) { _connection = connection; }

    void hideConnectionPreview();
    void showConnectionPreview();

private:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QBrush          _brush;
    CuteConnection* _connection{nullptr};
    QRectF          _paintRect{0.0, 0.0, 20.0, 10.0};
};


class CuteInputDock : public CuteDock
{
public:
    explicit CuteInputDock(QGraphicsItem* parent, const QPointF& pos) : CuteDock(parent, pos) {}
    ~CuteInputDock() override {}

    enum { Type = UserType + 3 };
    int type() const override { return Type; }
};


class CuteOutputDock : public CuteDock
{
public:
    explicit CuteOutputDock(QGraphicsItem* parent, const QPointF& pos) : CuteDock(parent, pos) {}
    ~CuteOutputDock() override {}

    enum { Type = UserType + 4 };
    int type() const override { return Type; }
};
