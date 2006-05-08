/*
 * This file is part of QShapes project
 * Copyright (C) 2006 QShapes development team
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <QAction>
#include <QDomDocument>
#include <QHelpEvent>
#include <QFile>
#include <QFileInfo>
#include <QInputDialog>
#include <QMap>
#include <QMapIterator>
#include <QMatrix>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QTextStream>
#include <QToolTip>
#include <QScrollBar>
#include <QSettings>

#include "ActionCombo.h"
#include "ActionConnectionBox.h"
#include "ActionConnectionLine.h"
#include "ActionItem.h"
#include "ActionManager.h"
#include "ActionMoveResize.h"
#include "ActionProperties.h"
#include "ClipboardManager.h"
#include "ConnectionCollector.h"
#include "DlgAssociateFile.h"
#include "DlgProperty.h"
#include "DiagramItemCollector.h"
#include "DiagramLine.h"
#include "DiagramView.h"
#include "ItemConnection.h"
#include "ProductBox.h"
#include "PosAndSize.h"
#include "SelectionBox.h"
#include "SelectionManager.h"
#include "SimplifiedConnection.h"
#include "SystemBox.h"
#include "SuperSystem.h"

DiagramView::DiagramView(QString fileName, ClipboardManager* clipboard, QtCanvas* canvas, QWidget* parent)
    : QtCanvasView(canvas, parent),
      m_pCanvas (canvas),
      m_pClipboardManager (clipboard)
{
    m_strFileName = fileName;
    m_bModified = false;
    m_bPan = false;
    
    m_pItemCollector = new DiagramItemCollector();
    m_pConnCollector = new ConnectionCollector();
    
    m_pActionManager = new ActionManager();
    connect(m_pActionManager, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));
    connect(m_pActionManager, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
    
    m_pSelectionManager = new SelectionManager(canvas);
    connect(m_pSelectionManager, SIGNAL(selectionAvailable(bool)), this, SIGNAL(selectionAvailable(bool)));
    
    m_pPendingItem = NULL;
    m_pItemConnection = NULL;
    m_pSelectionBox = NULL;
    m_iMinZ = 0;
    m_iMaxZ = 0;
    m_iZoomPercentage = 100;
    
    createActions();
    
    setWindowTitle (strippedName(m_strFileName));
}

DiagramView::~DiagramView()
{
    if (m_pPendingItem != NULL)
        delete m_pPendingItem;
    
    if (m_pItemConnection != NULL)
        delete m_pItemConnection;
    
    if (m_pSelectionBox != NULL)
        delete m_pSelectionBox;
    
    if (m_pSelectionManager != NULL)
        delete m_pSelectionManager;
    
    if (m_pActionManager != NULL)
        delete m_pActionManager;
}

void DiagramView::createActions()
{
    cutAct = new QAction(QIcon(":/images/editcut.png"),tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    connect(cutAct, SIGNAL(activated()), this, SLOT(cut()));
    
    copyAct = new QAction(QIcon(":/images/editcopy.png"),tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    connect(copyAct, SIGNAL(activated()), this, SLOT(copy()));
    
    pasteAct = new QAction(QIcon(":/images/editpaste.png"),tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    connect(pasteAct, SIGNAL(activated()), this, SLOT(pasteTo()));
    
    deleteAct = new QAction(QIcon(":/images/delete.png"),tr("&Delete"), this);
    deleteAct->setShortcut(tr("Del"));
    connect(deleteAct, SIGNAL(activated()), this, SLOT(del()));
    
    propertiesAct = new QAction(QIcon(":/images/properties.png"),tr("&Properties..."), this);
    connect(propertiesAct, SIGNAL(activated()), this, SLOT(properties()));
    
    addSystemBoxAct = new QAction(QIcon(":/images/draw_system.xpm"),tr("Add Box"), this);
    connect(addSystemBoxAct, SIGNAL(activated()), this, SLOT(addSystemBox()));

    addProductBoxAct = new QAction(QIcon(":/images/draw_productbox.xpm"),tr("Add Product Box"), this);
    connect(addProductBoxAct, SIGNAL(activated()), this, SLOT(addProductBox()));
    
    addSuperSystemAct = new QAction(QIcon(":/images/draw_supersystem.xpm"),tr("Add Super system box"), this);
    connect(addSuperSystemAct, SIGNAL(activated()), this, SLOT(addSuperSystem()));
    
    addLineAct = new QAction(QIcon(":/images/draw_connector.png"),tr("Add Line"), this);
    connect(addLineAct, SIGNAL(activated()), this, SLOT(addLine()));
    
    bringToFrontAct = new QAction(QIcon(":/images/bringtofront.png"),tr("Bring to &Front"), this);
    connect(bringToFrontAct, SIGNAL(activated()), this, SLOT(bringToFront()));
    
    sendToBackAct = new QAction(QIcon(":/images/sendtoback.png"),tr("Send to &Back"), this);
    connect(sendToBackAct, SIGNAL(activated()), this, SLOT(sendToBack()));
    
    associateFileAct = new QAction(tr("Associate external file"), this);
    connect(associateFileAct, SIGNAL(activated()), this, SLOT(associateFile()));
    
    openAssociateFileAct = new QAction(tr("Open the external file associated "), this);
    connect(openAssociateFileAct, SIGNAL(activated()), this, SLOT(openAssociateFile())); 
}

void DiagramView::closeEvent(QCloseEvent *event)
{
    if (m_bModified)
    {
        QString message = m_strFileName;
        
        message += QString(" has been modified\nAre you sure to close this file?");
        
        if (QMessageBox::warning (this, "Warning",message,QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton) == QMessageBox::Yes)
            event->accept();
        else
            event->ignore();
    }
    else
        event->accept();
}

void DiagramView::contentsContextMenuEvent(QContextMenuEvent *event)
{
    QPoint mousePoint = inverseWorldMatrix().map(event->pos());
    
    if (m_bPan)
        return; //nothing do to while in pan mode
    
    // prevent some strange behaviours when selecting
    if ((m_pSelectionBox != NULL) && (m_pSelectionBox->isVisible()))
    {
        m_pSelectionBox->hide();
        delete m_pSelectionBox;
        m_pSelectionBox = NULL;
        unsetCursor();
        m_pCanvas->update();
    }
    
    QMenu contextMenu(this);
    
    QtCanvasItemList items = m_pCanvas->collisions(mousePoint);
    
    if (!items.empty())
        m_pSelectionManager->selectItem( dynamic_cast<DiagramItem*> (*(items.begin())));
    
    if (!m_pSelectionManager->empty())
    {
        contextMenu.addAction(cutAct);
        contextMenu.addAction(copyAct);
        contextMenu.addAction(deleteAct);
        contextMenu.addSeparator();
        contextMenu.addAction(bringToFrontAct);
        contextMenu.addAction(sendToBackAct);
        contextMenu.addSeparator();
        contextMenu.addAction(propertiesAct);
        
        SuperSystem* box = dynamic_cast<SuperSystem*> (m_pSelectionManager->getFirst());
        
        if ((m_pSelectionManager->size() == 1) && (box != 0))
        {
            contextMenu.addSeparator();
            contextMenu.addAction(associateFileAct);
            
            if (box->hasAssociatedFile())
                contextMenu.addAction(openAssociateFileAct);
        }
    }
    else
    {
        // no selected item, select item under mouse cursor, if avaiable
        QtCanvasItemList items = m_pCanvas->collisions(mousePoint);
        
        if (items.empty())
        {
            contextMenu.addAction(pasteAct);
            contextMenu.addSeparator();
            contextMenu.addAction(addLineAct);
            contextMenu.addAction(addProductBoxAct);
            contextMenu.addAction(addSuperSystemAct);
            contextMenu.addAction(addSystemBoxAct);
        }
        else
        {
            m_pSelectionManager->selectItem( dynamic_cast<DiagramItem*> (*(items.begin())));
            
            contextMenu.addAction(cutAct);
            contextMenu.addAction(copyAct);
            contextMenu.addAction(deleteAct);
            contextMenu.addSeparator();
            contextMenu.addAction(bringToFrontAct);
            contextMenu.addAction(sendToBackAct);
            contextMenu.addSeparator();
            contextMenu.addAction(propertiesAct);
        }
        
    }
    contextMenu.exec(event->globalPos());
}

void DiagramView::setPanMode (bool status)
{
    m_bPan = status;
    
    if (m_bPan)
        setCursor(Qt::PointingHandCursor);
    else
        unsetCursor();
}

void DiagramView::addLine()
{
    addItem(new DiagramLine(m_pCanvas));
}

void DiagramView::addProductBox()
{
    addItem(new ProductBox(m_pCanvas));
}

void DiagramView::addSystemBox()
{
    addItem(new SystemBox(m_pCanvas));
}

void DiagramView::addSuperSystem()
{
    addItem(new SuperSystem(m_pCanvas));
}

void DiagramView::removeSelection()
{
    if (m_pSelectionBox != NULL)
    {
        delete m_pSelectionBox;
        m_pSelectionBox = NULL;
        
        m_pSelectionManager->clearSelection();
    }
}

void DiagramView::addItem(QtCanvasItem *item)
{
    if (m_pPendingItem != NULL)
        delete m_pPendingItem;
    
    m_pPendingItem = item;
    m_pSelectionManager->clearSelection();
    setCursor(Qt::CrossCursor);
}

void DiagramView::zoomPlus()
{
    QMatrix matrix = worldMatrix();
    
    qreal scaling = 1 + 25.0 / m_iZoomPercentage;
    
    m_iZoomPercentage += 25;
    
    if (m_iZoomPercentage <= 300) // limit zoom to 300%
    {
        matrix.scale(scaling, scaling);
        setWorldMatrix (matrix);
        
        emit zoomLevelUpdated (m_iZoomPercentage);
    }
    else
        m_iZoomPercentage = 300;
}

void DiagramView::zoomMinus()
{
    QMatrix matrix = worldMatrix();
    
    qreal scaling = 1 - 25.0 / m_iZoomPercentage;
    
    m_iZoomPercentage -= 25;
    
    if (m_iZoomPercentage >= 25) // limit zoom to 25%
    {
        matrix.scale(scaling, scaling);
        setWorldMatrix (matrix);
        
        emit zoomLevelUpdated (m_iZoomPercentage);
    }
    else
        m_iZoomPercentage = 25;
}

void DiagramView::setZoomLevel (int value)
{
    QMatrix matrix = worldMatrix();
    
    int oldZoom = m_iZoomPercentage;
    
    qreal scaling = 1 + ((double) (value - m_iZoomPercentage)) / m_iZoomPercentage;
    
    m_iZoomPercentage = value;
    
    if ((m_iZoomPercentage >= 25) && (m_iZoomPercentage <= 300))// limit 25% <= zoom <= 300%
    {
        setWorldMatrix (matrix.scale(scaling, scaling));
        
        m_pCanvas->update();
        
        emit zoomLevelUpdated (m_iZoomPercentage);
    }
    else
        m_iZoomPercentage = oldZoom;
}

bool DiagramView::isSelectionAvailable()
{
    if (m_pSelectionManager != NULL)
         return !m_pSelectionManager->empty();

    return false;
}

bool DiagramView::isUndoAvailable()
{
    if (m_pActionManager != NULL)
        return m_pActionManager->isUndoAvailable();

    return false;
}

bool DiagramView::isRedoAvailable()
{
    if (m_pActionManager != NULL)
        return m_pActionManager->isRedoAvailable();

    return false;
}

void DiagramView::undo ()
{
    if (m_pActionManager != NULL)
    {
        m_pSelectionManager->clearSelection();
        m_pActionManager->undo();
        m_pCanvas->update();
    }
}

void DiagramView::redo ()
{
    if (m_pActionManager != NULL)
    {
        m_pSelectionManager->clearSelection();
        m_pActionManager->redo();
        m_pCanvas->update();
    }
}

bool DiagramView::saveEmpty(QString filename)
{
    QDomDocument doc ("QShapes_file");
    
    // create root
    QDomElement root = doc.createElement(strippedName (QString ("root")));
    doc.appendChild(root);
    
    QString xml = doc.toString();
    
    QFile file (filename);

    file.open (QIODevice::WriteOnly | QIODevice::Truncate);

    int result = file.write (xml.toAscii(), xml.length());
    
    file.close();
    
    if (result == -1)
    {
        QMessageBox::critical (this, "Error", "An error occurred, unable to save file");
        return false;
    }
    
    return true;
}

QString DiagramView::generateReport()
{
    return m_pConnCollector->generateReport();
}

bool DiagramView::save()
{
    QDomDocument doc ("QShapes_file");
    
    // create root
    QDomElement root = doc.createElement(strippedName (QString ("root")));
    doc.appendChild(root);
    
    // save all items
    QDomElement items = doc.createElement("Items");
    root.appendChild(items);
    
    m_pItemCollector->save (&doc, &items);
    
    // save all connections
    QDomElement connections = doc.createElement("Connections");
    root.appendChild(connections);
    
    m_pConnCollector->save (&doc, &connections);
    
    QString xml = doc.toString();
    
    QFile file (m_strFileName);

    file.open (QIODevice::WriteOnly | QIODevice::Truncate);

    int result = file.write (xml.toAscii(), xml.length());
    
    file.close();
    
    if (result == -1)
    {
        QMessageBox::critical (this, "Error", "An error occurred, unable to save file");
        return false;
    }
    
    return true;
}

bool DiagramView::load()
{
    QDomDocument doc ("QShapes_file");
    QString errorMsg;
    int errorLine, errorColumn;
    
    QFile file(m_strFileName);
    
    if (!file.open(QIODevice::ReadOnly))
        return false;
    
    if (!doc.setContent(&file, true, &errorMsg, &errorLine, &errorColumn))
    {
        qDebug("Error at line %i, column %i: %s\n", errorLine, errorColumn, errorMsg.toStdString().c_str());
        
        file.close();
        return false;
    }
    file.close();
    
    
    // restore Diagram items
    QDomNode items = doc.elementsByTagName("Items").at(0).firstChild();
    if (!items.isNull())
        m_pItemCollector->load (items, m_pCanvas);
    
    // restore Item Connections
    QDomNode connections = doc.elementsByTagName("Connections").at(0).firstChild();
    if (!connections.isNull())
    {
        // restore connections
        m_pConnCollector->load (connections, m_pItemCollector);
        
        // restore connection references into DiagramBox
        m_pItemCollector->restoreBoxesConnections( items, m_pConnCollector);
    }
   
    return true;
}

QString DiagramView::strippedName(QString fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void DiagramView::contentsMousePressEvent(QMouseEvent *event)
{
    QPoint mousePoint = inverseWorldMatrix().map(event->pos());
    
    if ((event->button() == Qt::LeftButton) && (m_pPendingItem != NULL) && (!m_bPan))
    {
        removeSelection ();
        if (m_pPendingItem->rtti() == DiagramLine::RTTI)
        {   // we're going to add a diagramlines
            
            if (m_pItemConnection != NULL)
                delete m_pItemConnection; //avoid memory leaks
            
            m_pItemConnection = new ItemConnection();
            
            m_pItemConnection->setLine (dynamic_cast<DiagramLine*>(m_pPendingItem));
            
            // we've to check for new connections
            QtCanvasItemList items = m_pCanvas->collisions(mousePoint);
            QtCanvasItem* firstOver = NULL;
            
            if (!items.empty())
                firstOver = *(items.begin());
            
            if ((firstOver != NULL) && (dynamic_cast<DiagramBox*> (firstOver) != 0))
            { 
                // line is over a SystemBox, we create a connection
                m_pItemConnection->setStartBox (dynamic_cast<DiagramBox*>(firstOver));
                
                // create a temporary start point for the connecting line: we take diagrambox middle point
                
                QPoint connection;
                
                ((SystemBox*) firstOver)->getMiddle(connection);
                
                ((DiagramLine*) m_pPendingItem)->setPoints (
                                    connection.x(),
                                    connection.y(),
                                    connection.x(),
                                    connection.y());
            }
            else
            {
                // there's no collision --> no box at connection's beginning
                m_pItemConnection->setStartBox( NULL);
                
                (dynamic_cast<DiagramLine*> (m_pPendingItem))->setPoints (
                        mousePoint.x(),
                        mousePoint.y(),
                        mousePoint.x(),
                        mousePoint.y());
            }
            
            showNewItem (m_pPendingItem);
        }
        else
        {
            // we aren't adding a line, less things to do
            m_pItemCollector->addItem( dynamic_cast<DiagramItem*> (m_pPendingItem));
            m_pPendingItem->move(mousePoint.x(), mousePoint.y());
            
            // create undo action
            ActionItem *actionItem = new ActionItem (m_pItemCollector, dynamic_cast<DiagramItem*> (m_pPendingItem), false);
            
            m_pActionManager->addAction (actionItem, true);
            
            showNewItem (m_pPendingItem);
            m_pPendingItem = NULL;
            unsetCursor();
            
            emit drawFigureAdded();
        }
    }
    else if ((event->button() == Qt::LeftButton)  && (!m_bPan))
    {
         // no pending item, maybe we've selected something
        
        QtCanvasItemList items = m_pCanvas->collisions(mousePoint);
        
        if (items.empty())
        {
            // start a selection
            
            removeSelection();
            
            m_pSelectionBox = new SelectionBox (m_pCanvas);
            m_pSelectionBox->setX(mousePoint.x());
            m_pSelectionBox->setY(mousePoint.y());
            m_pSelectionBox->show();
            
            m_lastPos = mousePoint;
        }
        else
        {
            delete m_pSelectionBox;
            m_pSelectionBox = NULL;
            
            DiagramItem* item;
            
            item = dynamic_cast<DiagramItem*> (*(items.begin()));
            
            if (item != 0)
            {
                if (item->checkExpand(mousePoint.x(), mousePoint.y()))
                {
                    removeSelection();
                    m_pSelectionManager->clearSelection();
                    m_pSelectionManager->selectItem(item);
                    setCursor(Qt::CrossCursor);
                    // TODO change cursor shape and disable horizontal/verical movement with some type of expansions
                }
                else if (m_pSelectionManager->size() <= 1)
                {
                    m_pSelectionManager->selectItem( item);
                }
                else if (m_pSelectionManager->size() > 1)
                {
                    // maybe we're going to move lots of already selected items
                    m_lastPos = mousePoint;
                }
            }
        }
    }
    
    if (event->button() == Qt::LeftButton)
        m_lastPos = mousePoint;
}

void DiagramView::contentsMouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePoint = inverseWorldMatrix().map(event->pos());
    
    if ((event->button() == Qt::NoButton ) && (!m_bPan))
    {
        autoScroll();
        
        if (m_pPendingItem != NULL)
        {
            if (m_pPendingItem->rtti() == DiagramLine::RTTI)
            {
                (dynamic_cast<DiagramLine*> (m_pPendingItem))->expand(
                        mousePoint.x(),
                        mousePoint.y());
                m_pCanvas->update();
            }
        }
        else if ((m_pSelectionBox != NULL) && (m_pSelectionBox->isVisible()))
        {
            m_pSelectionBox->expand(mousePoint.x(), mousePoint.y());
            m_pSelectionBox->show();
            m_pCanvas->update();
        }
        else if (!m_pSelectionManager->empty())
        {
            QList<DiagramItem*> activeItems;
            QList<DiagramItem*>::iterator activeIter;
            QList<DiagramBox*> activeBoxs;
            QList<DiagramBox*>::iterator activeBoxIter;
            
            m_pSelectionManager->getSelectedItems (activeItems);

            for (activeIter = activeItems.begin(); activeIter != activeItems.end(); activeIter++)
            {
                // enable expansion only when there's one selected item
                if  ((*activeIter)->isExpanding() && activeItems.size() == 1)
                {
                    (*activeIter)->expand(mousePoint.x(), mousePoint.y());
                    
                    DiagramBox* box = dynamic_cast<DiagramBox*> ((*activeIter));
                    
                    if (box != 0) // update connections
                        activeBoxs.push_back(box);
                }
                else
                {
                    // item is moving
                    
                    setCursor(Qt::SizeAllCursor);
                    
                    (dynamic_cast<QtCanvasItem*> (*activeIter))->moveBy(
                        mousePoint.x() - m_lastPos.x(),
                        mousePoint.y() - m_lastPos.y());
                    
                    if ((dynamic_cast<DiagramLine*> (*activeIter) != 0) && (!(*activeIter)->getMoving()))
                    {
                        (*activeIter)->setMoving( true);
                        
                        QList<ItemConnection*> connections;
                        QList<ItemConnection*>::iterator iter;
                        m_pConnCollector->findConnectionByItemID((*activeIter)->getID(), connections);
                        
                        // move all connected box with the line
                        // we've to add them to selected items in order to have undo/redo operations
                        
                        for (iter = connections.begin(); iter != connections.end(); iter++)
                        {
                            DiagramBox* box;
                            QtCanvasItem* canvasItem;
                            
                            box = (*iter)->getStartBox();
                            
                            // add item to selected items (useful for undo / redo move operations
                                
                            if ((box != NULL) && (!box->getMoving()))
                            {
                                m_pSelectionManager->addItem( box);
                                box->setMoving( true);
                                activeBoxs.push_back(box);
                            }
                            
                            canvasItem = dynamic_cast<QtCanvasItem*> (box);
                            
                            if (canvasItem !=0)
                                canvasItem->moveBy( mousePoint.x() - m_lastPos.x(),mousePoint.y() - m_lastPos.y());
                            
                            box = (*iter)->getEndBox();
                            
                            if ((box != NULL) && (!box->getMoving()))
                            {
                                m_pSelectionManager->addItem( box);
                                box->setMoving( true);
                                activeBoxs.push_back(box);
                            }
                            
                            canvasItem = dynamic_cast<QtCanvasItem*> (box);
                            if (canvasItem !=0)
                                canvasItem->moveBy(mousePoint.x() - m_lastPos.x(),mousePoint.y() - m_lastPos.y());
                        }
                    }
                    else if (dynamic_cast<DiagramBox*> (*activeIter) != 0)
                    {
                        DiagramBox* box;
                    
                        box = dynamic_cast<DiagramBox*> (*activeIter);
                    
                        activeBoxs.push_back (box);
                    }
                    
                    (*activeIter)->setMoving( true);
                }
            }
            
            m_lastPos = mousePoint;
            
            // update all box connections
            for (activeBoxIter = activeBoxs.begin(); activeBoxIter != activeBoxs.end(); activeBoxIter++)
                (*activeBoxIter)->updateConnections();
            
            m_pCanvas->update();
        }
    }
    else if ((event->button() == Qt::NoButton ) && (m_bPan))
    {
        QScrollBar* hscroll = horizontalScrollBar();
        QScrollBar* vscroll = verticalScrollBar();
        
        vscroll->setValue(vscroll->value() + mousePoint.y() - m_lastPos.y());
        hscroll->setValue(hscroll->value() + mousePoint.x() - m_lastPos.x());
        
        m_lastPos = mousePoint;
    }
}

void DiagramView::contentsMouseReleaseEvent (QMouseEvent * event)
{
    QPoint mousePoint = inverseWorldMatrix().map(event->pos());
    
    if ((event->button() == Qt::LeftButton) && (m_pPendingItem != NULL) && (!m_bPan))
    {
        // going to place a new item
        
        //undo class
        ActionCombo *actionCombo = new ActionCombo ();
        
        if (m_pPendingItem->rtti() == DiagramLine::RTTI)
        {
            // we've to check for connections
            
            QPoint finalPoint = mousePoint;
            
            QtCanvasItemList items = m_pCanvas->collisions(finalPoint);
            
            DiagramBox* endBox = NULL;

            if (!items.empty())
            {
                QtCanvasItemList::iterator iter = items.begin();
                for ( ; iter != items.end(); iter++)
                {
                    endBox = dynamic_cast<DiagramBox*> (*iter);
                    if (endBox != 0)
                        break;
                    else
                        endBox = NULL;
                }
            }
            
            if ((endBox != NULL) && (m_pItemConnection->containsID( endBox->getID())))
            {   // endBox and startBox are the same
                
                // connection isn't valid, we've to delete it (no undo action to perform)
                delete m_pItemConnection;
                m_pItemConnection = NULL;
                
                delete m_pPendingItem;
                
                m_pCanvas->update();
                m_pPendingItem = NULL;
                unsetCursor();
                
                m_pSelectionManager->clearSelection();
                
                return;
            }
            
            m_pItemConnection->setEndBox( endBox);
            
            if (m_pItemConnection->isValid())
            {
                //add the line to the item collector
                m_pItemCollector->addItem( dynamic_cast<DiagramLine*> (m_pPendingItem));
                
                // add the new connection to the collector
                m_pConnCollector->addConnection(m_pItemConnection);
                
                // create undo class
                ActionConnectionLine *actionConnectionLine;
                actionConnectionLine = new ActionConnectionLine (m_pItemCollector, m_pConnCollector, m_pItemConnection, false, true);
                
                actionCombo->addAction (actionConnectionLine);
                
                m_pItemConnection = NULL;
            }
            else
            {
                // connection isn't valid, we've to delete it ( no undo operation to perform)
                delete m_pItemConnection;
                m_pItemConnection = NULL;
                
                // there's no connection
                (dynamic_cast<DiagramLine*> (m_pPendingItem))->expand (finalPoint.x(), finalPoint.y());
                
                m_pItemCollector->addItem( dynamic_cast<DiagramLine*> (m_pPendingItem));
            }
            
            (dynamic_cast<DiagramLine*> (m_pPendingItem))->resetExpand();
            
            // create undo action
            if (actionCombo->empty())
            {
                // adding a simple line, without any connection
                DiagramItem* newItem = m_pItemCollector->getItemByID( m_pItemCollector->getNextID()-1);
                
                if (newItem != NULL)
                {
                    ActionItem *actionItem = new ActionItem (m_pItemCollector, newItem, false);
                
                    actionCombo->addAction( actionItem);
                }
            }
            
            m_pPendingItem = NULL;
            unsetCursor();

            m_pCanvas->update();
            
            emit drawFigureAdded();
        }
        
        if (!actionCombo->empty())
            m_pActionManager->addAction (actionCombo, true);
        else
            delete actionCombo;

    }
    else if ((event->button() == Qt::LeftButton) && (m_pSelectionBox!= NULL) && (m_pSelectionBox->isVisible()) && (!m_bPan))
    {
        // update selected items
        
        if (mousePoint != m_lastPos)
        {
            m_pSelectionBox->expand( mousePoint.x(), mousePoint.y());
            
            QtCanvasItemList selItems = m_pSelectionBox->collisions(true);
            QtCanvasItemList::iterator iter;
            QList<DiagramItem*> selectedItems;
            QRect selection = m_pSelectionBox->rect();
            
            for (iter = selItems.begin(); iter != selItems.end(); iter++)
            {
                DiagramItem* item = dynamic_cast<DiagramItem*>(*iter);
                
                if ((item != 0) && (dynamic_cast<SelectionBox*>(*iter) == 0) && (item->isContained (selection)))
                    selectedItems.push_back (item);
            }
            
            m_pSelectionManager->selectItems( selectedItems);
            m_pSelectionBox->hide();
        }
        else
        {
            m_pSelectionBox->setSize ( 0,0);
            m_pSelectionManager->clearSelection();
        }
        
        m_pCanvas->update();
    }
    else if ((event->button() == Qt::LeftButton) && (!m_pSelectionManager->empty()) && (!m_bPan))
    {
        // finish move/resize event
        
        QList<DiagramItem*> activeItems;
        QList<DiagramItem*>::iterator activeIter;
        m_pSelectionManager->getSelectedItems( activeItems);
        
        QList<PosAndSize*> activePosAndSize;
        QList<PosAndSize*>::iterator activePosAndSizeIter;
        m_pSelectionManager->getSelectedItemsPosAndSize ( activePosAndSize);
        activePosAndSizeIter = activePosAndSize.begin();
        
        ActionCombo* actionCombo = new ActionCombo();
        
        for (activeIter = activeItems.begin(); activeIter != activeItems.end(); activeIter++, activePosAndSizeIter++)
        {
            // enable expansion only when there's one selected item
            if ((*activeIter)->isExpanding() && activeItems.size() == 1)
            {
                (*activeIter)->expand (mousePoint.x(), mousePoint.y());
                
                if ((*activeIter)->getPosAndSize() != (*activePosAndSizeIter))
                {
                    // item has been moved / resized, create undo action
                    ActionMoveResize* moveAction = new ActionMoveResize (m_pItemCollector, *activeIter, *activePosAndSizeIter);
                    
                    // update the surrounding rectangle associated to activeItem
                    m_pSelectionManager->updatePosAndSize (*activeIter);
                    
                    actionCombo->addAction( moveAction);
                }
                
                // item is expanding
                if (dynamic_cast<DiagramLine*> ((*activeIter)) != 0)
                {
                    DiagramLine* line = dynamic_cast<DiagramLine*> ((*activeIter));
                    DiagramBox *newBox = NULL;
                    
                    // check out were the line is going... maybe there's a new connection to create and/or an old one to change
                    QPoint finalPoint = mousePoint;
                    QtCanvasItemList items = m_pCanvas->collisions(finalPoint);
                    QtCanvasItemList::iterator canvasIter = items.begin();
                    
                    for ( ; canvasIter != items.end(); canvasIter++)
                    {
                        newBox = dynamic_cast<DiagramBox*> (*canvasIter);
                        if (newBox != 0)
                            break;
                        else
                            newBox = NULL;
                    }
                    
                    // check if this line is involved in some connection
                    QList<ItemConnection*> connections;
                    QList<ItemConnection*>::iterator iter;
                    m_pConnCollector->findConnectionByItemID(line->getID(), connections);
                    
                    if ((connections.size() == 0) && (newBox != NULL))
                    {   // line isn't part of a connection, create a new one
    
                        if (m_pItemConnection != NULL)
                            delete m_pItemConnection;
                        
                        m_pItemConnection = new ItemConnection ();
                        
                        m_pItemConnection->setLine (line);
                        
                        if (line->isExpandingByBegin())
                            m_pItemConnection->setStartBox (newBox);
                        else
                            m_pItemConnection->setEndBox (newBox);
                        
                        m_pConnCollector->addConnection (m_pItemConnection);
                        
                        // create undo action
                        ActionConnectionLine* actionConnectionLine;
                        actionConnectionLine = new ActionConnectionLine (m_pItemCollector, m_pConnCollector, m_pItemConnection, false, false);
                        
                        actionCombo->addAction (actionConnectionLine);
                        
                        m_pItemConnection = NULL;
                    }
                    else if (connections.size() > 0)
                    {
                        // the line is part of a connection
                        
                        // move all connected box with the line
                        for (iter = connections.begin(); iter != connections.end(); iter++)
                        {
                            DiagramBox *oldBox;
                            
                            if (line->isExpandingByBegin())
                                oldBox = (*iter)->getStartBox();
                            else
                                oldBox = (*iter)->getEndBox();
                            
                            if ((oldBox != NULL) && (newBox != NULL) && (oldBox->getID() == newBox->getID()))
                            {
                                // undo line's expansion (no need of undo action)
                                newBox->updateConnections();
                            }
                            else if ((oldBox != NULL) && (newBox != NULL) && (oldBox->getID() != newBox->getID()))
                            {
                                // update connection
                                
                                //create undo action
                                ActionConnectionBox* actionConnectionBox;
                                actionConnectionBox = new ActionConnectionBox (m_pItemCollector, *iter, m_pConnCollector, line->isExpandingByBegin());
                                
                                actionCombo->addAction (actionConnectionBox);
                                
                                (*iter)->changeBox(line->isExpandingByBegin(), newBox);
                            }
                            else if ((oldBox == NULL) && (newBox != NULL))
                            {
                                // add the new box
                                
                                //create undo action
                                ActionConnectionBox* actionConnectionBox;
                                actionConnectionBox = new ActionConnectionBox (m_pItemCollector, *iter, m_pConnCollector, line->isExpandingByBegin());
                                
                                actionCombo->addAction (actionConnectionBox);
                                
                                (*iter)->addBox (line->isExpandingByBegin(), newBox);
                            }
                            else if ((oldBox != NULL) && (newBox == NULL))
                            {
                                // remove the connection from oldBox and the other box involved in the connection
                                
                                //create undo action
                                ActionConnectionBox* actionConnectionBox;
                                actionConnectionBox = new ActionConnectionBox (m_pItemCollector, *iter, m_pConnCollector, line->isExpandingByBegin());
                                
                                actionCombo->addAction (actionConnectionBox);
                                
                                oldBox->deleteConnection ((*iter)->getID());
                                oldBox->updateConnections();
                                
                                (*iter)->changeBox(line->isExpandingByBegin(), newBox);
                            }
                        }
                    }
                }
                
                (*activeIter)->resetExpand();
                unsetCursor();
            }
            else if ((*activeIter)->getMoving())
            {
                (*activeIter)->setMoving( false);
                if ((*activeIter)->getPosAndSize() != (*activePosAndSizeIter))
                {
                    // item has been moved / resized, create undo action
                    ActionMoveResize* moveAction = new ActionMoveResize (m_pItemCollector, *activeIter, *activePosAndSizeIter);
                    
                    actionCombo->addAction( moveAction);
                    
                    // update the surrounding rectangle associated to activeItem
                    m_pSelectionManager->updatePosAndSize (*activeIter);
                }
                
                DiagramBox* box;
                
                box = dynamic_cast<DiagramBox*> (*activeIter);
                
                if (box != 0)
                {
                    box->updateConnections();
                }
            }
        }
        
        if (!actionCombo->empty())
            m_pActionManager->addAction( actionCombo, true);
        
        unsetCursor();
        m_pCanvas->update();
    }
}

void DiagramView::contentsMouseDoubleClickEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) && (m_pSelectionManager->size() == 1) && (!m_bPan))
    {
        DiagramItem *item = (m_pSelectionManager->getItem(0));
        bool ok;

        QString newText = QInputDialog::getText(this,
            tr("QShapes - Change item's text"), tr("Enter new text:"),
            QLineEdit::Normal, item->getText(), &ok);

        if (ok)
        {
            item->setText(newText);
            m_pCanvas->update();
        }
    }
}

void DiagramView::autoScroll()
{
    /*
        Code taken from "Poa"
        http://developer.berlios.de/projects/poa/
    */
    
    QPoint p = mapFromGlobal(QCursor::pos());

    const int AUTOSCROLL_MARGIN = 16;
    
    QSize viewPortSize = viewport()->size();
    
    int dx = 0, dy = 0;
    
    if (p.y() < AUTOSCROLL_MARGIN)
    {
        // scroll up
        dy = -(AUTOSCROLL_MARGIN - p.y());
    }
    else if (p.y() > viewPortSize.height() - AUTOSCROLL_MARGIN)
    {
        // scroll down
        dy = p.y() - (viewPortSize.height() - AUTOSCROLL_MARGIN);
    }

    if ( p.x() < AUTOSCROLL_MARGIN )
    {
        // scroll left
        dx = -(AUTOSCROLL_MARGIN - p.x());
    }
    else if ( p.x() > viewPortSize.width() - AUTOSCROLL_MARGIN)
    {
        // scroll rigth
        dx = p.x() - (viewPortSize.width() - AUTOSCROLL_MARGIN);
    }

    if (dx != 0 || dy != 0)
    {
        QScrollBar* hscroll = horizontalScrollBar();
        QScrollBar* vscroll = verticalScrollBar();
        
        vscroll->setValue(vscroll->value() + dy);
        hscroll->setValue(hscroll->value() + dx);
    }
}

void DiagramView::keyPressEvent ( QKeyEvent * event )
{
    if (event->key() == Qt::Key_Control)
        m_pSelectionManager->setMultipleSelection (true);
    else
        event->ignore();
}

void DiagramView::keyReleaseEvent ( QKeyEvent * event )
{
    if (event->key() == Qt::Key_Control)
        m_pSelectionManager->setMultipleSelection (false);
    else
        event->ignore();
}

bool DiagramView::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        
        QPoint mousePoint = inverseWorldMatrix().map(helpEvent->pos());
        
        QtCanvasItemList items = m_pCanvas->collisions(mousePoint);
        if (!items.empty())
        {
            QtCanvasItem* firstOver = *(items.begin());
            DiagramItem* item = dynamic_cast<DiagramItem*>(firstOver);
            
            if ((item != 0) && (item->getID() != 0))
                QToolTip::showText(helpEvent->globalPos(), item->getToolTip());
            else
                event->ignore();
        }
        else
            event->ignore();
    }
    
    return QtCanvasView::event(event);
}

void DiagramView::bringToFront()
{
    QList<DiagramItem*> activeItems;
    QList<DiagramItem*>::iterator activeIter;
    m_pSelectionManager->getSelectedItems( activeItems);
    
    for (activeIter = activeItems.begin(); activeIter != activeItems.end(); activeIter++)
    {
        if ((*activeIter) != NULL)
        {
            ++m_iMaxZ;
            (dynamic_cast<QtCanvasItem*> (*activeIter))->setZ(m_iMaxZ);
        }
    }
    m_pCanvas->update();
}

void DiagramView::sendToBack()
{
    QList<DiagramItem*> activeItems;
    QList<DiagramItem*>::iterator activeIter;
    m_pSelectionManager->getSelectedItems( activeItems);
    
    for (activeIter = activeItems.begin(); activeIter != activeItems.end(); activeIter++)
    {
        if ((*activeIter) != NULL)
        {
            --m_iMinZ;
            (dynamic_cast<QtCanvasItem*> (*activeIter))->setZ(m_iMinZ);
            m_pCanvas->update();
        }
    }
}

void DiagramView::associateFile()
{
    if (m_pSelectionManager->size() != 1)
        return;
    
    SuperSystem* system = dynamic_cast<SuperSystem*>(m_pSelectionManager->getFirst());
    
    if (system == 0)
        return;
    
    QString filename;
    bool newFile;
    
    DlgAssociateFile dlg (&filename, &newFile, this);
    dlg.exec();
    
    if (newFile)
    {
        // create the new file
        if (!saveEmpty(filename))
        {
            emit associateFile(false);
            return;
        }
    }
    
    system->associateFile (filename);
    emit associateFile (true);
}

void DiagramView::openAssociateFile()
{
    SuperSystem* item;
    item = dynamic_cast<SuperSystem*>(m_pSelectionManager->getFirst());
    
    if ((m_pSelectionManager->size() == 1) && (item != 0))
        emit openAssociateFile (item->getAssociatedFile());
}


void DiagramView::cut()
{
    copy();
    del();
}

void DiagramView::copy()
{
    QList<ItemConnection*> connections;
    
    if ((m_pClipboardManager == NULL) || (m_pSelectionManager->empty()))
        return;
    
    QList<DiagramItem*> activeItems;
    QList<DiagramItem*>::iterator activeIter;
    m_pSelectionManager->getSelectedItems( activeItems);
    
    for (activeIter = activeItems.begin(); activeIter != activeItems.end(); activeIter++)
    {
        if ((*activeIter) == NULL)
            continue;
        
        m_pConnCollector->findConnectionByItemID( (*activeIter)->getID(), connections);
    }
    
    QPoint selUpperLeft;
    
    m_pSelectionManager->getUpperLeftPoint(selUpperLeft);
    
    m_pClipboardManager->addItems( activeItems, connections, selUpperLeft);
}

void DiagramView::pasteTo()
{
    // get current mouse position
    
    QPoint pastePoint = (cursor()).pos();
    pastePoint = mapFromGlobal (pastePoint);
    paste (&pastePoint);
}

void DiagramView::paste(QPoint* pastePoint)
{
    QList<DiagramItem*> itemsToPaste;
    QList<DiagramItem*>::iterator iter;
    QList<SimplifiedConnection*> connToRestore;
    QMap<int, int> translatedID; // map with old ID as key, new ID as value
    QMap<int, int>::iterator mapIter;
    QtCanvasItem* canvasItem;
    
    if (pastePoint == NULL)
    {
        QPoint origin (0,0);
        pastePoint = &origin;
    }
    
    // retrieve useful data
    m_pClipboardManager->getItems( m_pCanvas, itemsToPaste, *pastePoint);
    
    // clear old active items, pasted items will be the only one selected
    if (!itemsToPaste.empty())
        m_pSelectionManager->clearSelection();
    
    // add pasted items to collectors
    for (iter = itemsToPaste.begin(); iter != itemsToPaste.end(); iter++)
    {
        // save old ID, it'll replace during paste action
        int oldID = (*iter)->getID();
        
        // save new item ID
        int newID = m_pItemCollector->getNextID();
            
        // update ID translation map
        translatedID.insert (oldID, newID);
            
        // add item to the collector
        m_pItemCollector->addItem( *iter);

        canvasItem = dynamic_cast<QtCanvasItem*> (*iter);
        canvasItem->show();
    }
    
    // select all pasted items
    m_pSelectionManager->selectItems(itemsToPaste);
    
    // restore connections
    m_pClipboardManager->getTranslatedConnections (connToRestore, translatedID);
    
    for (QList<SimplifiedConnection*>::iterator connIter = connToRestore.begin(); connIter != connToRestore.end(); connIter++)
    {
        DiagramBox* startBox = NULL;
        DiagramBox* endBox = NULL;
        DiagramLine* line = NULL;
        DiagramItem* item = NULL;
        int ID;
        
        
        // find startBox
        ID = (*connIter)->getStartBoxID();
        if (ID != -1)
            item = m_pItemCollector->getItemByID ((int)ID);
         
        if ((item != NULL) && (dynamic_cast<DiagramBox*> (item) != 0))
            startBox = dynamic_cast<DiagramBox*> (item);
        
        // find endBox
        ID = (*connIter)->getEndBoxID();
        
        if (ID != -1)
            item = m_pItemCollector->getItemByID ((int)ID);
        else
            item = NULL;
        
        if ((item != NULL) && (dynamic_cast<DiagramBox*> (item) != 0))
            endBox = dynamic_cast<DiagramBox*> (item);
        
        
        // find line
        ID =(*connIter)->getLineID();
        
        if (ID != -1)
            item = m_pItemCollector->getItemByID ((int) ID);
        else
            item = NULL;
        
        if ((item != NULL) && (dynamic_cast<DiagramLine*> (item) != 0))
            line = dynamic_cast<DiagramLine*> (item);
        
        if (line != NULL)
        {
            ItemConnection* connection = new ItemConnection (startBox, endBox, line);
            m_pConnCollector->addConnection( connection);
        }
    }
    
    // deallocate connToRestore
    for (int i = 0; i < connToRestore.size(); i++)
    {
        if (connToRestore[i] != NULL)
            delete connToRestore[i];
    }
}

void DiagramView::del()
{
    QList<DiagramItem*> activeItems;
    QList<DiagramItem*>::iterator activeIter;
    
    m_pSelectionManager->getSelectedItems( activeItems);
    
    ActionCombo *actionCombo = new ActionCombo();
    
    m_pSelectionManager->clearSelection();
    
    for (activeIter = activeItems.begin(); activeIter != activeItems.end(); activeIter++)
    {
        if ((*activeIter) != NULL)
        {
            DiagramItem* toRemove;
            
            QtCanvasItem* canvasItem = dynamic_cast<QtCanvasItem*> (*activeIter);
            
            if (canvasItem == 0)
                return;  // something went wrong
            
            canvasItem->hide();
            
            toRemove = (*activeIter);
            
            if (toRemove == 0)
                continue;
            
            if (dynamic_cast<DiagramLine*> (toRemove) != 0)
            {
                // maybe we've to delete a connection
                QList<ItemConnection*> connections;
                QList<ItemConnection*>::iterator connIter;
                
                m_pConnCollector->findConnectionByItemID(toRemove->getID(),connections);
                
                for (connIter = connections.begin(); connIter != connections.end(); connIter++)
                {
                    // create undo action
                    ActionConnectionLine* actionConnectionLine = new ActionConnectionLine (m_pItemCollector, m_pConnCollector, *connIter, true, true);
                    
                    actionCombo->addAction( actionConnectionLine);
                    
                    // delete connection
                    m_pConnCollector->deleteConnection ((*connIter)->getID());
                }
            }
            else if (dynamic_cast<DiagramBox*> (toRemove) != 0)
            {   // maybe we've to update a connection
                QList<ItemConnection*> connections;
                
                m_pConnCollector->findConnectionByItemID(toRemove->getID(),connections);
                
                for (int i = 0; i < connections.size(); i++)
                {
                    connections[i]->deleteID (toRemove->getID());
                    
                    // check if connection is still correct, otherwise we've to delete it
                    if (!connections[i]->isValid())
                        m_pConnCollector->deleteConnection( connections[i]->getID());
                }
            }
            
            m_pItemCollector->delItem (toRemove->getID());
        }
    }
    
    m_pActionManager->addAction( actionCombo, true);
    
    m_pCanvas->update();
}

void DiagramView::properties()
{
    if (m_pSelectionManager->size() == 1)
    {
        DiagramItem* item = m_pSelectionManager->getFirst();
        
        DiagramBox* box = NULL;
        DlgProperty* dlg;
        
        if (dynamic_cast<DiagramLine*>(item) != 0)
        {
            DiagramLine* line = dynamic_cast<DiagramLine*>(item);
            
            dlg = new DlgProperty (line, m_pConnCollector->findConnectionByLineID( line->getID()), this);
        }
        else if (dynamic_cast<DiagramBox*>(item) != 0)
        {
            box = dynamic_cast<DiagramBox*>(item);
            dlg = new DlgProperty (box, this);
        }
        else
            return;
        
        ActionProperties* actionProperties = new ActionProperties (m_pItemCollector, item);
        
        if (dlg->exec() == QDialog::Accepted)
        {
            m_pActionManager->addAction (actionProperties, true);
            
            if (box != NULL)
                box->updateConnections();
            
            m_pCanvas->update();
        }
        else
            // nothing changed
            delete actionProperties;
        
        delete dlg;
    }
}

void DiagramView::showNewItem(QtCanvasItem *item)
{
    //setActiveItem (item);
    DiagramItem* diagramItem = dynamic_cast<DiagramItem*> (item);
    if (diagramItem != 0)
    {
        m_pSelectionManager->selectItem (diagramItem);
        
        bringToFront();
        item->show();
        m_pCanvas->update();
    }
}

QString DiagramView::userFriendlyCurrentFile()
{
    if (m_strFileName.contains ("Untitled"))
        return m_strFileName;
    else
        return strippedName(m_strFileName);
}
