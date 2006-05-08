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

#include "ClipboardManager.h"
#include "DiagramItem.h"
#include "DiagramLine.h"
#include "ItemConnection.h"
#include "SimplifiedConnection.h"
#include "SystemBox.h"

#include <QtAlgorithms>

ClipboardManager::ClipboardManager(QObject* parent)
    : QObject (parent)
{
}

/*! Takes care of cleaning operations.
    \sa clear()
 */
ClipboardManager::~ClipboardManager()
{
    clear();
}

/*!
    De-allocate all ItemConnection instances, empty m_data and reset m_IDCounter
 */
void ClipboardManager::clear()
{
    for (int i = 0; i < m_data.size(); i++)
    {
        if (m_data[i] != NULL)
            delete m_data[i];
    }
    
    m_data.clear();
    
    for (int i = 0; i < m_validConnections.size(); i++)
    {
        if (m_validConnections[i] != NULL)
            delete m_validConnections[i];
    }
    
    m_validConnections.clear();
    
    emit pasteAvailable( false);
}

bool ClipboardManager::empty()
{
    return m_data.empty();
}

void ClipboardManager::addItems (QList<DiagramItem*>& items, QList<ItemConnection*>& connections, QPoint& selUpperLeft)
{
    // clear old buffered items
    clear();
    
    //add new items
    for (QList<DiagramItem*>::iterator iter = items.begin(); iter != items.end(); iter++)
    {
        if (dynamic_cast<DiagramLine*> (*iter) != 0)
        {
            // allocate new DiagramLine
            DiagramLine *originalLine = dynamic_cast<DiagramLine*> (*iter);
            DiagramLine *line = new DiagramLine (originalLine);
            
            // update position
            QPoint start = originalLine->startPoint();
            QPoint end = originalLine->endPoint();
            
            // traslate points
            start.setX(start.x() - selUpperLeft.x());
            start.setY(start.y() - selUpperLeft.y());
            
            end.setX(end.x() - selUpperLeft.x());
            end.setY(end.y() - selUpperLeft.y());
            
            line->setPoints (start.x(), start.y(), end.x(), end.y());

            // add the new item to the clipboard
            m_data.push_back (line);
        }
        else if (dynamic_cast<DiagramBox*> (*iter) != 0)
        {
            DiagramBox *originalBox = dynamic_cast<DiagramBox*> (*iter);
            DiagramBox *box = dynamic_cast<DiagramBox*>(originalBox->clone());
            
            if (box == 0)
                continue;
            
            // update position
            QPoint topLeft;
            originalBox->getTopLeft (topLeft);
            
            // traslate box
            topLeft.setX(topLeft.x() - selUpperLeft.x());
            topLeft.setY(topLeft.y() - selUpperLeft.y());
            
            box->setTopLeft( topLeft);
            
            // add the new item to the clipboard
            m_data.push_back (box);
        }
    }
    
    // save all connections
    saveConnections (items, connections);
    
    if (!m_data.empty())
        emit pasteAvailable( true);
}

/*!
    an old connection is still valid if both involved DiagramBox are contained into availableItems.
*/
void ClipboardManager::saveConnections(QList<DiagramItem*>& availableItems, QList<ItemConnection*>& connections)
{
    for (QList<ItemConnection*>::iterator connIter = connections.begin(); connIter != connections.end(); connIter++)
    {
        DiagramBox* box = NULL;
        DiagramLine* line = NULL;
        QList<DiagramItem*>::iterator itemIter;
        
        bool startBoxAvailable = false;
        bool endBoxAvailable = false;
        bool lineAvailable = false;
        
        // search startBox into availableItems
        box = (*connIter)->getStartBox();
        
        if (box != NULL)
        {
            itemIter = qFind (availableItems.begin(), availableItems.end(), box);
             
            if (itemIter != availableItems.end())
                 startBoxAvailable = true;
        }
        else
            startBoxAvailable = true;
        
        // search endBox into availableItems
        box = (*connIter)->getEndBox();
        
        if (box != NULL)
        {
            itemIter = qFind (availableItems.begin(), availableItems.end(), box);
             
            if (itemIter != availableItems.end())
                endBoxAvailable = true;
        }
        else
            endBoxAvailable = true;
        
        // search line into availableItems
        line = (*connIter)->getLine();
        
        itemIter = qFind (availableItems.begin(), availableItems.end(), line);
        
        if (itemIter != availableItems.end())
            lineAvailable = true;
        
        if ((lineAvailable) && ((startBoxAvailable) || (endBoxAvailable)))
        {
            SimplifiedConnection *simpleConn = new SimplifiedConnection (*connIter);
            m_validConnections.push_back (simpleConn);
        }
    }
}

/*!
    pastePoint is the upper left point of the rectangle containing all buffered items
*/
void ClipboardManager::getItems (QtCanvas* canvas, QList<DiagramItem*>& items, QPoint& pastePoint)
{
    QList<DiagramItem*>::iterator iter;
    
    for (iter = m_data.begin(); iter != m_data.end(); iter++)
    {
        if (dynamic_cast<DiagramLine*> (*iter) != 0)
        {
            // allocate new DiagramLine
            DiagramLine *clipboardLine = dynamic_cast<DiagramLine*> (*iter);
            DiagramLine *line = new DiagramLine (clipboardLine, canvas);
            
            // translate the box according to pastePoint
            QPoint start, end;
            
            start = line->startPoint();
            end = line->endPoint();
            
            start.setX(start.x() + pastePoint.x());
            start.setY(start.y() + pastePoint.y());
            
            end.setX(end.x() + pastePoint.x());
            end.setY(end.y() + pastePoint.y());
            
            line->setPoints (start.x(), start.y(), end.x(), end.y());
            
            // add item to collector
            items.push_back (line);
        }
        else if (dynamic_cast<DiagramBox*> (*iter) != 0)
        {
            DiagramBox *clipboardBox = dynamic_cast<DiagramBox*> (*iter);
            DiagramBox *box = dynamic_cast<DiagramBox*>(clipboardBox->clone());
            
            if (box == 0)
                continue;
            
            QtCanvasItem* canvasItem = dynamic_cast<QtCanvasItem*>(box);
            canvasItem->setCanvas( canvas);
            
            // translate the box according to pastePoint
            QPoint topLeft;
            
            clipboardBox->getTopLeft(topLeft);

            topLeft.setX (topLeft.x() + pastePoint.x());
            topLeft.setY (topLeft.y() + pastePoint.y());
            
            box->setTopLeft( topLeft);
            
            // add item to collector
            items.push_back (box);
        }
    }
}

/*!
    connections are translated according to the translation map provided by translatedID
*/
void ClipboardManager::getTranslatedConnections (QList<SimplifiedConnection*>& connToRestore, QMap<int, int>& translatedID)
{
    QList<SimplifiedConnection*>::iterator iter = m_validConnections.begin();
    
    for ( ; iter != m_validConnections.end(); iter++)
    {
        if (*iter != NULL)
        {
            SimplifiedConnection *translatedConn = new SimplifiedConnection (*iter, translatedID);
        
            connToRestore.push_back (translatedConn);
        }
    }
}
