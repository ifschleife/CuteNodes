#pragma once

#include <QBrush>
#include <QGraphicsItem>
#include <QStaticText>
#include <QUuid>


class CuteConnection;


class CuteDock : public QGraphicsItem
{
public:
    enum class DockType : uint32_t
    {
        Input,
        Output
    };

public:
    explicit CuteDock(QGraphicsItem* parent, const DockType dockType);
    ~CuteDock() override;

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

    enum { Type = UserType + 2 };
    int type() const override { return Type; }

    QRectF boundingRect() const override;

    bool canAcceptConnectionAtPos(const QPointF& scenePos) const;

    QUuid getUuid() const { return _uuid; }

    QPointF getConnectionMagnet() const;

    CuteConnection* getConnection() const          { return _connection; }
    void setConnection(CuteConnection* connection) { _connection = connection; }

    bool isInputDock() const  { return _type == DockType::Input; }
    bool isOutputDock() const { return _type == DockType::Output; }

    void hideConnectionPreview();
    void showConnectionPreview();

private:
    void calculateConnectorPath(const qreal connectorHeight);

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QBrush          _brush;
    CuteConnection* _connection{nullptr};
    QPainterPath    _connectorPath{{0.0, 0.0}};
    QStaticText     _name;
    QPointF         _namePos{0.0, 0.0};
    DockType        _type;
    QUuid           _uuid;
};
