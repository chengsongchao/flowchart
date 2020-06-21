﻿#ifndef SCENE_H
#define SCENE_H
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "node.h"
#include "link.h"
class Scene : public QGraphicsScene
{
public:
    Scene(QObject* parent = nullptr)
        : QGraphicsScene(parent)
    {

    }
    Scene(qreal x, qreal y, qreal width, qreal height, QObject *parent = nullptr)
        : QGraphicsScene(x, y, width, height)
    {

    }

    void setStart(Node* node)
    {
        m_start = node;
    }

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override
    {
        if(m_start)
        {
            auto childs = items(event->scenePos());
            if(!childs.empty())
            {
                for(auto child : childs)
                {
                    m_end = dynamic_cast<Node*>(child);
                    if(m_end && m_start != m_end)
                    {
                        Link *link = new Link(m_start, m_end);
                        addItem(link);
                        break;
                    }
                }
            }
            if(m_tempLink)
                removeItem(m_tempLink);

            m_start = nullptr;
            m_end = nullptr;
            m_tempLink = nullptr;
        }
        QGraphicsScene::mouseReleaseEvent(event);
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override
    {
        if(m_start)
        {
            if(!m_tempLink)
            {
                m_tempLink = new QGraphicsLineItem;
                m_tempLink->setPen(QPen(QColor(Qt::darkRed), 1.0));
                addItem(m_tempLink);
            }
            m_tempLink->setLine(QLineF(m_start->pos(), event->scenePos()));
            return;
        }
        QGraphicsScene::mouseMoveEvent(event);
    }
    Node* m_start{nullptr};
    Node* m_end{nullptr};
    QGraphicsLineItem* m_tempLink{nullptr};
};

#endif // SCENE_H
