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

#include "DiagramItemCollector.h"

#include "ConnectionCollector.h"
#include "DiagramLine.h"
#include "ProductBox.h"
#include "SystemBox.h"
#include "SuperSystem.h"
#include "XmlSupport.h"

#include <QtAlgorithms>
#include <QDomDocument>
#include <qtcanvas.h>

DiagramItemCollector::DiagramItemCollector()
{
    m_firstFreeID = 1;
    m_freeID.clear();
}

/*! Takes care of cleaning operations.
    \sa clear()
 */
DiagramItemCollector::~DiagramItemCollector()
{
    clear();
}

/*!
    De-allocate all DiagramItem instances, empty m_data and reset m_IDCounter
 */
void DiagramItemCollector::clear()
{
    for (int i = 0; i < m_data.size(); i++)
        if (m_data[i] != NULL)
            delete m_data[i];
            
    m_data.clear();
    m_firstFreeID = 1;
    m_freeID.clear();
}

/*!
    Assing a new unambiguous ID to the ItemConnection and shows the connection (\sa makeConnection())<br>
    If forced is true we reuse item's ID.
 */
void DiagramItemCollector::addItem (DiagramItem* item, bool forced)
{
    if (!forced)
    {
        if (!m_freeID.empty())
        {
            item->setID(m_freeID[0]);
            m_freeID.erase (m_freeID.begin());
        }
        else
            item->setID(m_firstFreeID++);
    }
    
    m_data.push_back(item);
    
    QtCanvasItem* canvasItem = dynamic_cast<QtCanvasItem*>(item);
    
    if ((canvasItem != 0) && (!canvasItem->isVisible()))
        canvasItem->show();
}

void DiagramItemCollector::delItem (int ID)
{
    QList<DiagramItem*>::iterator iter;

    for (iter = m_data.begin(); iter != m_data.end(); iter++)
    {
        if ((*iter)->getID() == ID)
        {
            delete *iter;
            m_data.erase(iter);
            return;
        }
    }
}

DiagramItem* DiagramItemCollector::getItemByID(int ID)
{
    QList<DiagramItem*>::iterator iter;

    for (iter = m_data.begin(); iter != m_data.end(); iter++)
    {
        if ((*iter)->getID() == ID)
        {
            return *iter;
        }
    }
    
    return NULL;
}

void DiagramItemCollector::save (QDomDocument* document, QDomElement* parent)
{
    QList<DiagramItem*>::iterator iter;

    for (iter = m_data.begin(); iter != m_data.end(); iter++)
    {
        QDomElement item = document->createElement("Item");
        
        (*iter)->save (document, &item);
        
        parent->appendChild(item);
    }
}

void DiagramItemCollector::load (QDomNode items, QtCanvas* canvas)
{
    QList<int> usedID;
    
    while(!items.isNull()) 
    {
        QDomElement e = items.toElement(); // try to convert the node to an element.
        if(!e.isNull())
        {
            QString tagName = e.tagName();
            loadItem (items, canvas, usedID);
        }
        items = items.nextSibling();
    }
    
    updateIDCounter (usedID);
}

void DiagramItemCollector::loadItem (QDomNode item, QtCanvas* canvas, QList<int>& usedID)
{
    switch (nodeItemType (item))
    {
        case DiagramLine::RTTI:
        {
            DiagramLine* line = new DiagramLine (canvas);
            if (!line->load (&item))
            {
                qDebug ("An error occurred while loading a DiagramLine\n");
                delete line;
            }
            else
            {
                addItem (line, true);
                usedID.push_back (line->getID());
            }
            break;
        }
        case SystemBox::RTTI:
        {
            SystemBox* box = new SystemBox (canvas);
            if (!box->load (&item))
            {
                qDebug ("An error occurred while loading a SystemBox\n");
                delete box;
            }
            else
            {
                addItem (box, true);
                usedID.push_back (box->getID());
            }
            break;
        }
        case SuperSystem::RTTI:
        {
            SuperSystem* box = new SuperSystem (canvas);
            if (!box->load (&item))
            {
                qDebug ("An error occurred while loading a SystemBox\n");
                delete box;
            }
            else
            {
                addItem (box, true);
                usedID.push_back (box->getID());
            }
            break;
        }
        case ProductBox::RTTI:
        {
            SystemBox* box = new ProductBox (canvas);
            if (!box->load (&item))
            {
                qDebug ("An error occurred while loading a SystemBox\n");
                delete box;
            }
            else
            {
                addItem (box, true);
                usedID.push_back (box->getID());
            }
            break;
        }
        default:
            qDebug ("Item with unknown type, discarding\n");
            return;
            break;
    }
}

void DiagramItemCollector::restoreBoxesConnections (QDomNode items, ConnectionCollector* connCollector)
{
    while(!items.isNull()) 
    {
        QDomElement e = items.toElement(); // try to convert the node to an element.
        if(!e.isNull())
        {
            QString tagName = e.tagName();
            restoreBoxConnections (items, connCollector);
        }
        items = items.nextSibling();
    }
}

void DiagramItemCollector::restoreBoxConnections (QDomNode item, ConnectionCollector* connCollector)
{
    QDomElement element;
    
    element = item.firstChildElement(QString ("ConnectionsID"));
    
    if (!element.isNull())
    {
        // item has a <Connections> child, going to restore them
        bool check;
        int boxID;
        DiagramBox* box;
        DiagramItem* diagramItem;
        QList<int> connectionsID;
        
        boxID = getNodeNumValue ( item, QString("ID"), &check);

        if (!check)
            return;
        
        diagramItem = getItemByID (boxID);
        
        if (diagramItem == NULL)
            return;
        
        box = dynamic_cast<DiagramBox*>(diagramItem);
        
        if (box == 0)
            return;
        
        // load top connections
        readConnectionID(element, QString("Top"), connectionsID);
        
        for (int i = 0; i < connectionsID.size(); i++)
        {
            ItemConnection* connection = connCollector->getByID( connectionsID[i]);
            
            if (connection != NULL)
                box->addConnection( connection, DiagramBox::TOP);
        }
        
        // load bottom connections
        readConnectionID(element, QString("Bottom"), connectionsID);
        
        for (int i = 0; i < connectionsID.size(); i++)
        {
            ItemConnection* connection = connCollector->getByID( connectionsID[i]);
            
            if (connection != NULL)
                box->addConnection( connection, DiagramBox::BOTTOM);
        }
        
        // load left connections
        readConnectionID(element, QString("Left"), connectionsID);
        
        for (int i = 0; i < connectionsID.size(); i++)
        {
            ItemConnection* connection = connCollector->getByID( connectionsID[i]);
            
            if (connection != NULL)
                box->addConnection( connection, DiagramBox::LEFT);
        }
        
        // load bottom connections
        readConnectionID(element, QString("Right"), connectionsID);
        
        for (int i = 0; i < connectionsID.size(); i++)
        {
            ItemConnection* connection = connCollector->getByID( connectionsID[i]);
            
            if (connection != NULL)
                box->addConnection( connection, DiagramBox::RIGHT);
        }
    }
}

void DiagramItemCollector::updateIDCounter (QList<int>&usedID)
{
    // reverse sort of usedID
    
    qSort(usedID.begin(), usedID.end(), std::greater<int>());
    
    while (!usedID.empty())
    {
        // add available ID to m_freeID
        for (int i = m_firstFreeID ; i < usedID[usedID.size() -1 ]; i++)
            m_freeID.push_back(i);

        m_firstFreeID = usedID[usedID.size() -1 ] + 1;
        
        usedID.pop_back();
    }
}
