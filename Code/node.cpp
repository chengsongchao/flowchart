#include <QtGui>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>
#include "link.h"
#include "node.h"
#include "scene.h"

Node::Node()
{
    m_textColor = Qt::darkGreen;
    m_outlineColor = Qt::darkBlue;
    m_backgroundColor = Qt::white;

    setAcceptHoverEvents(true);
    setFlags(ItemIsMovable | ItemIsSelectable);
}

Node::~Node()
{
    foreach (Link *link, m_links)
        delete link;
}

void Node::setText(const QString& text)
{
    prepareGeometryChange();
    m_text = text;
    update();
}

QString Node::text() const
{
    return m_text;
}

void Node::setTextColor(const QColor &color)
{
    m_textColor = color;
    update();
}

QColor Node::textColor() const
{
    return m_textColor;
}

void Node::setOutlineColor(const QColor &color)
{
    m_outlineColor = color;
    update();
}

QColor Node::outlineColor() const
{
    return m_outlineColor;
}

void Node::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    update();
}

QColor Node::backgroundColor() const
{
    return m_backgroundColor;
}

void Node::addLink(Link *link)
{
    m_links.insert(link);
}

void Node::removeLink(Link *link)
{
    m_links.remove(link);
}

QRectF Node::boundingRect() const
{
    const int Margin = 10; //缩放, addLine控制柄, 已经缩小时重绘容错
    return outlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}

QPainterPath Node::shape() const
{
    QRectF rect = boundingRect();

    QPainterPath path;
    path.addRoundRect(rect, roundness(rect.width()),
                      roundness(rect.height()));
    return path;
}

void Node::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget * /* widget */)
{
    drawNormalOutline(painter);

    if (option->state & QStyle::State_Selected)
        drawScaleHandle(painter);
    if(option->state & QStyle::State_Selected ||
            m_nodeStats == Hover)
        drawAddLineHandle(painter);

    drawText(painter);
}

void Node::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_nodeStats = Hover;
    update();
    QGraphicsItem::hoverEnterEvent(event);
}
void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    unsetCursor();
    m_nodeStats = Normal;
    update();
    QGraphicsItem::hoverLeaveEvent(event);
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QString text = QInputDialog::getText(event->widget(),
                           tr("Edit Text"), tr("Enter new text:"),
                           QLineEdit::Normal, m_text);
    if (!text.isEmpty())
        setText(text);

    //QGraphicsItem::mouseDoubleClickEvent(event);
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_nodeCustomEvent)
    {
        CustomNodeEvent::NodeMouseEvent mouseEvent = m_nodeCustomEvent->type();
        if(mouseEvent == CustomNodeEvent::Resize)
        {
            NodeResizeEvent*  nodeEvent = m_nodeCustomEvent->nodeResizeEvent();
            if(nodeEvent)
            {
                m_rect = nodeEvent->curRect(event->pos());
                update();
            }
            return;
        }
        else if(mouseEvent == CustomNodeEvent::AddLine)
        {
            return;
        }
        else if(mouseEvent == CustomNodeEvent::Move)
        {
            QGraphicsItem::mouseMoveEvent(event);
            return;
        }
        for(auto link : m_links)
        {
            link->trackNodes();
        }
    }

    updateCursor(event);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(testResizeEvent(event))
        return;
    if(testAddLineEvent(event))
        return;

    m_nodeCustomEvent = std::make_shared<NodeMoveEvent>();
    QGraphicsItem::mousePressEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_nodeCustomEvent = nullptr;
    QGraphicsItem::mouseReleaseEvent(event);
}

void Node::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    QGraphicsItem::dragMoveEvent(event);
}

QVariant Node::itemChange(GraphicsItemChange change,
                          const QVariant &value)
{
    if (change == ItemPositionHasChanged)
    {
        foreach (Link *link, m_links)
            link->trackNodes();
    }
    return QGraphicsItem::itemChange(change, value);
}

QRect Node::outlineRect() const
{
    /*const int Padding = 8;
    QFontMetrics metrics = QApplication::fontMetrics();
    QRect rect = metrics.boundingRect(m_text);
    rect.adjust(-Padding, -Padding, +Padding, +Padding);*/

    QRect rect = m_rect;
    rect.translate(-rect.center());
    return rect;
}

int Node::roundness(double size) const
{
    const int Diameter = 12;
    return 100 * Diameter / int(size);
}

QVector<QRectF> Node::scaleHandlerVector()
{
    const QRect nodeRect = outlineRect();
    QRectF handleRect{-4, -4, 8, 8};
    QVector<QRectF> handleRects;
    handleRects.push_back((handleRect.translated(nodeRect.topLeft())));
    handleRects.push_back(handleRect.translated(nodeRect.topRight()));
    handleRects.push_back(handleRect.translated(nodeRect.bottomRight()));
    handleRects.push_back(handleRect.translated(nodeRect.bottomLeft()));
    return handleRects;
}

QVector<QRectF> Node::addLineHandlerVector()
{
    const QRect nodeRect = outlineRect();
    QRectF handleRect{-4, -4, 8, 8};
    QVector<QRectF> handleRects;
    handleRects.push_back(handleRect.translated(QPointF(nodeRect.left(), 0)));  //左
    handleRects.push_back(handleRect.translated(QPointF(0, nodeRect.top())));  //上
    handleRects.push_back(handleRect.translated(QPointF(nodeRect.right(), 0)));  //右
    handleRects.push_back(handleRect.translated(QPointF(0, nodeRect.bottom())));  //下
    return handleRects;
}

bool Node::testResizeEvent(QGraphicsSceneMouseEvent* event)
{
    auto testContain = [&](const QVector<QRectF>& rects, const QPointF& p)->bool{
        int idx = NodeResizeEvent::LeftTop;
        for(auto& rect :  rects)
        {
            if(rect.contains(p))
            {
                NodeResizeEvent::DirectionType ty = static_cast<NodeResizeEvent::DirectionType>(idx);
                m_nodeCustomEvent = std::make_shared<NodeResizeEvent>(ty, m_rect, p);
                return true;
            }
            idx++;
        }
        return false;
    };

    QPointF curPos = event->pos();
    QVector<QRectF> scaleRects = scaleHandlerVector();
    return testContain(scaleRects, curPos);
}

bool Node::testAddLineEvent(QGraphicsSceneMouseEvent* event)
{
    auto testContain = [&](const QVector<QRectF>& rects, const QPointF& p)->bool{
        int idx = NodeResizeEvent::LeftTop;
        for(auto& rect :  rects)
        {
            if(rect.contains(p))
            {
                m_nodeCustomEvent = std::make_shared<NodeAddLineEvent>();
                return true;
            }
            idx++;
        }
        return false;
    };

    QPointF curPos = event->pos();
    QVector<QRectF> addRects = addLineHandlerVector();
    if(testContain(addRects, curPos))
    {
        auto sc = dynamic_cast<Scene*>(scene());
        if(sc)
        {
            sc->setStart(this);
            return true;
        }
    }
    return false;
}

void Node::updateCursor(QGraphicsSceneMouseEvent* event)
{
    auto testContain = [&](const QVector<QRectF>& rects, const QPointF& p)->bool{
        int idx = NodeResizeEvent::LeftTop;
        for(auto& rect :  rects)
        {
            if(rect.contains(p))
            {
                if(idx == 0 || idx == 2)
                {
                    setCursor(Qt::SizeFDiagCursor);
                }
                else if(idx == 1 || idx == 3)
                {
                    setCursor(Qt::SizeBDiagCursor);
                }
                return true;
            }
            idx++;
        }
        return false;
    };

    QPointF curPos = mapFromScene(event->scenePos());
    QVector<QRectF> scaleRects = scaleHandlerVector();
    if(testContain(scaleRects, curPos))
       return;

    QVector<QRectF> addLineRects = addLineHandlerVector();
    for(auto& rect :  addLineRects)
    {
        if(rect.contains(curPos))
        {
            setCursor(Qt::CrossCursor);
            return;
        }
    }
    unsetCursor();
}

void Node::drawNormalOutline(QPainter *painter)
{
    const QRect nodeRect = outlineRect();
    painter->save();
    QPen pen(m_outlineColor);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->setBrush(m_backgroundColor);
    painter->drawRoundRect(nodeRect, 1, 1);
    painter->restore();
}

void Node::drawScaleHandle(QPainter *painter)
{
    painter->save();
    //如果这里不设置取消反锯齿, pen的最小宽度为2
    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setPen(QPen(QColor("#883333"), 1));
    painter->setBrush(Qt::white);
    QRectF handleRect{-4, -4, 8, 8};
    QVector<QRectF> handleRects = scaleHandlerVector();
    painter->drawRects(handleRects);

    painter->restore();
}

void Node::drawAddLineHandle(QPainter *painter)
{
    painter->save();
    //如果这里不设置取消反锯齿, pen的最小宽度为2
    //painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setPen(QPen(QColor("#883333"), 1));
    painter->setBrush(Qt::white);

    QVector<QRectF> handleRects = addLineHandlerVector();
    for(auto handlerRect : handleRects)
    {
        painter->drawEllipse(handlerRect);
    }

    painter->restore();
}

void Node::drawText(QPainter *painter)
{

    painter->setPen(m_textColor);
    const QRect nodeRect = outlineRect();
    painter->drawText(nodeRect, Qt::AlignCenter, m_text);
}
