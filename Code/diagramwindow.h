﻿#ifndef DIAGRAMWINDOW_H
#define DIAGRAMWINDOW_H

#include <QMainWindow>
#include <QPair>

class QAction;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsView;
class Link;
class Node;

class DiagramWindow : public QMainWindow
{
    Q_OBJECT

public:
    DiagramWindow();

private slots:
    void addNode();
    void addLink();
    void del();
    void cut();
    void copy();
    void paste();
    void bringToFront();
    void sendToBack();
    void properties();
    void updateActions();

private:
    typedef QPair<Node*, Node*> NodePair;

    void createActions();
    void createMenus();
    void createToolBars();
    void setZValue(int z);
    void setupNode(Node* node);
    Node* selectedNode() const;
    Link* selectedLink() const;
    NodePair selectedNodePair() const;

    QMenu* fileMenu;
    QMenu* editMenu;
    QToolBar* editToolBar;
    QAction* exitAction;
    QAction* addNodeAction;
    QAction* addLinkAction;
    QAction* deleteAction;
    QAction* cutAction;
    QAction* copyAction;
    QAction* pasteAction;
    QAction* bringToFrontAction;
    QAction* sendToBackAction;
    QAction* propertiesAction;
    QAction* m_shapeAction;

    QGraphicsScene* scene;
    QGraphicsView* view;

    int minZ;
    int maxZ;
    int seqNumber;
};

#endif
