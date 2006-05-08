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

#include "ConnectionCollector.h"
#include "DiagramItemCollector.h"
#include "ItemConnection.h"

#include <QtAlgorithms>
#include <QDomDocument>

ConnectionCollector::ConnectionCollector()
{
    m_firstFreeID = 0;
}

/*! Takes care of cleaning operations.
    \sa clear()
*/
ConnectionCollector::~ConnectionCollector()
{
    clear();
}

/*!
    De-allocate all ItemConnection instances, empty m_data and reset m_firstFreeID
*/
void ConnectionCollector::clear()
{
    for (int i = 0; i < m_data.size(); i++)
    {
        if (m_data[i] != NULL)
            delete m_data[i];
    }
    
    m_data.clear();
    m_firstFreeID = 0;
}

/*!
    Assing a new unambiguous ID to the ItemConnection and shows the connection (\sa makeConnection())
*/
void ConnectionCollector::addConnection (ItemConnection* connection, bool forceID)
{
    if (!forceID)
    {
        if (!m_freeID.empty())
        {
            connection->setID(m_freeID[0]);
            m_freeID.erase (m_freeID.begin());
        }
        else
            connection->setID(m_firstFreeID++);
    }
    
    connection->makeConnection();
    
    m_data.push_back(connection);
}

/*!
    populate vector connectionsID with the ID of the ItemConnection matching the search
*/
void ConnectionCollector::findConnectionByItemID (int itemID, QList<int>& connectionsID)
{
    QList<int>::iterator iter;
    
    for (int i = 0; i < m_data.size(); i++)
    {
        if (m_data[i]->containsID(itemID))
        {
            // avoid duplicates
            iter = qFind (connectionsID.begin(), connectionsID.end(), m_data[i]->getID());
            
            if (iter == connectionsID.end())
                connectionsID.push_back(m_data[i]->getID());
        }
    }
}

/*!
    populate vector connections with the ItemConnection instances matching the search
 */
void ConnectionCollector::findConnectionByItemID (int itemID, QList<ItemConnection*>& connections)
{
    QList<ItemConnection*>::iterator iter;
    
    for (int i = 0; i < m_data.size(); i++)
    {
        if (m_data[i] == NULL)
            continue;
        
        if (m_data[i]->containsID(itemID))
        {
            // avoid duplicates
            iter = qFind(connections.begin(), connections.end(), m_data[i]);
            
            if (iter == connections.end())
                connections.push_back(m_data[i]);
        }
    }
}

/*!
    return -1 if no connection is found
*/
int ConnectionCollector::findConnectionByLineID (int lineID)
{
    QList<ItemConnection*>::iterator iter;
    
    for (iter = m_data.begin(); iter != m_data.end(); iter++)
    {
        if ((*iter)->getLineID() == lineID)
            return (*iter)->getID();
    }
    
    return -1;
}

void ConnectionCollector::deleteConnection (int ID)
{
    QList<ItemConnection*>::iterator iter;
    for (iter = m_data.begin(); iter != m_data.end(); iter++)
    {
        if ((*iter)->getID() == ID)
        {
            //(*iter)->clean();
            delete *iter;
            m_data.erase (iter);
            break;
        }
    }
}

/*!
    return true if delete operation finish without errors, otherwise return false
*/
bool ConnectionCollector::deleteConnectionByLineID (int lineID)
{
    QList<ItemConnection*>::iterator iter;
    
    for (iter = m_data.begin(); iter != m_data.end(); iter++)
    {
        if ((*iter)->getLineID() == lineID)
        {
            (*iter)->clean();
            delete *iter;
            m_data.erase (iter);
            return true;
        }
    }
    
    return false;
}

ItemConnection* ConnectionCollector::getByID (int connectionID)
{
    QList<ItemConnection*>::iterator iter;
    
    for (iter = m_data.begin(); iter != m_data.end(); iter++)
    {
        if ((*iter)->getID() == connectionID)
            return (*iter);
    }
    
    return NULL;
}

void ConnectionCollector::save (QDomDocument* document, QDomElement* parent)
{
    QList<ItemConnection*>::iterator iter;

    for (iter = m_data.begin(); iter != m_data.end(); iter++)
    {
        QDomElement item = document->createElement("Connection");
        
        (*iter)->save (document, &item);
        
        parent->appendChild(item);
    }
}

void ConnectionCollector::load (QDomNode connections, DiagramItemCollector* itemCollector)
{
    QList<int> usedID;
    
    while(!connections.isNull()) 
    {
        QDomElement e = connections.toElement(); // try to convert the node to an element.
        if(!e.isNull())
            loadConnection (connections, itemCollector, usedID);

        connections = connections.nextSibling();
    }
}

void ConnectionCollector::loadConnection (QDomNode connection, DiagramItemCollector* itemCollector, QList<int>& usedID)
{
    ItemConnection* itemConn = new ItemConnection ();
    
    if (itemConn->load (connection, itemCollector))
    {
        // add the new connection
        m_data.push_back(itemConn);
        
        usedID.push_back(itemConn->getID());
    }
    else
        delete itemConn;
}

void ConnectionCollector::updateIDCounter (QList<int>&usedID)
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

/*!
    Each report line is composed in this way: Start box's name - connection line's  name -end box's name
*/
QString ConnectionCollector::generateReport()
{
    QString report;
    ItemConnection* conn;

    for (int i = 0; i < m_data.size(); i++)
    {
        conn = m_data.at(i);
        if (conn != NULL)
            report += conn->generateReport();
    }
    
    return report;
}
