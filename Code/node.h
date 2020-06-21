#ifndef NODE_H
#define NODE_H

#include <QApplication>
#include <QColor>
#include <QGraphicsItem>
#include <QSet>

class NodeResizeEvent;
class CustomNodeEvent
{
public:
    enum NodeMouseEvent
    {
        CustonStart = 0,
        Move,
        Resize,
        AddLine,
    };

    CustomNodeEvent(){}
    virtual NodeMouseEvent type() const
    {
        return CustonStart;
    }

    virtual NodeResizeEvent* nodeResizeEvent()
    {
        return nullptr;
    }

};

class NodeResizeEvent : public CustomNodeEvent
{
public:
    enum DirectionType
    {
        LeftTop,
        RightTop,
        RightBottom,
        LeftBottom
    };
    NodeResizeEvent(DirectionType dir, const QRect& originalRect, const QPointF& startPos)
        : m_dir(dir)
        , m_originalRect(originalRect)
        , m_startPos(startPos)
    {}

    NodeMouseEvent type() const override
    {
        return Resize;
    }

    NodeResizeEvent* nodeResizeEvent() override
    {
        return this;
    }

    QRect curRect(const QPointF& curPos)
    {
        QRect curRect = m_originalRect;
        int xAdd = curPos.x() - m_startPos.x();
        int yAdd = curPos.y() - m_startPos.y();
        if(m_dir == LeftTop)
        {
            curRect.adjust(xAdd, yAdd, -xAdd, -yAdd);
        }
        else if(m_dir == RightTop)
        {
            curRect.adjust(-xAdd, yAdd, xAdd, -yAdd);
        }
        else if(m_dir == RightBottom)
        {
            curRect.adjust(-xAdd, -yAdd, xAdd, yAdd);
        }
        else if(m_dir == LeftBottom)
        {
            curRect.adjust(xAdd, -yAdd, -xAdd, yAdd);
        }
        if(curRect.width() < 24)
        {
            int subWid = 24 - curRect.width();
            curRect.adjust(-subWid/2, 0, subWid, 0);
        }
        if(curRect.height() < 24)
        {
            int subHeight = 24 - curRect.height();
            curRect.adjust(0, -subHeight/2, 0, subHeight);
        }
        return curRect;
    }

private:
    QPointF m_startPos;
    QRect m_originalRect;
    DirectionType m_dir;
};

class NodeMoveEvent : public CustomNodeEvent
{
public:
    NodeMoveEvent(){}

    NodeMouseEvent type() const override
    {
        return Move;
    }
};

class NodeAddLineEvent : public CustomNodeEvent
{
public:
    NodeAddLineEvent(){}

    NodeMouseEvent type() const override
    {
        return AddLine;
    }
};

class Link;
class Node : public QGraphicsObject
{
    Q_DECLARE_TR_FUNCTIONS(Node)

    enum NodeStats
    {
        Normal,
        Hover,
    };
public:
    Node();
    ~Node();

    void setText(const QString& text);
    QString text() const;
    void setTextColor(const QColor& color);
    QColor textColor() const;
    void setOutlineColor(const QColor& color);
    QColor outlineColor() const;
    void setBackgroundColor(const QColor& color);
    QColor backgroundColor() const;

    void addLink(Link* link);
    void removeLink(Link* link);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value);

private:
    QRect outlineRect() const;
    int roundness(double size) const;
    bool testResizeEvent(QGraphicsSceneMouseEvent* event);
    bool testAddLineEvent(QGraphicsSceneMouseEvent* event);
    void updateCursor(QGraphicsSceneMouseEvent* event);

    QVector<QRectF> scaleHandlerVector();
    QVector<QRectF> addLineHandlerVector();

    void drawNormalOutline(QPainter *painter);
    void drawScaleHandle(QPainter *painter);
    void drawAddLineHandle(QPainter *painter);
    void drawText(QPainter *painter);

    QSet<Link*> m_links;
    QString m_text;
    QColor m_textColor;
    QColor m_backgroundColor;
    QColor m_outlineColor;

    QRect m_rect{0, 0, 100, 70};

    NodeStats m_nodeStats{Normal};
    std::shared_ptr<CustomNodeEvent> m_nodeCustomEvent{nullptr};
};

#endif
