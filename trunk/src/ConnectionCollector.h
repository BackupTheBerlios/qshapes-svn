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

#ifndef CONNECTIONCOLLECTOR_H
#define CONNECTIONCOLLECTOR_H

/*! \class ConnectionCollector
    \author Flavio Castelli
    \brief This is the collector of all ItemConnection instances

    ConnectionCollector takes care of this features:
    - assign ID to ItemConnection, avoid duplicates
    - de-allocate all ItemConnection \sa ConnectionCollector::clear()
 */

class DiagramItemCollector;
class ItemConnection;
class QDomDocument;
class QDomElement;
class QDomNode;

#include <QList>
#include <QString>

class ConnectionCollector
{
    public:
        
        //! constructor
        ConnectionCollector();
        
        //! destructor
        virtual ~ConnectionCollector();
        
        //! add a new ItemConnection, if forceID is false a new ID is assigned to the connection
        void addConnection (ItemConnection* connection, bool forceID = false);
        
        //! find all connection where there's the item defined by itemID
        void findConnectionByItemID (int itemID, QList<int>& connectionsID);
        
        //! find all connection where there's the item defined by itemID
        void findConnectionByItemID (int itemID, QList<ItemConnection*>& connections);
        
        //! find the connection with the line defined by lineID
        int findConnectionByLineID (int lineID);
        
        //! return connection defined by connectionID
        ItemConnection* getByID (int connectionID);
        
        //! delete connection defined by ID
        void deleteConnection (int ID);
        
        //! delete connection where there's the line defined by lineID
        bool deleteConnectionByLineID (int lineID);
        
        //! save all connections into an xml document
        void save (QDomDocument *document, QDomElement *parent);
        
        //! load all connections from an xml document
        void load (QDomNode connections, DiagramItemCollector* itemCollector);
        
        //! generate a text report
        QString generateReport();
        
    private:
        
        //! load a single connection starting form an xml QDomNode
        void loadConnection (QDomNode connection, DiagramItemCollector* itemCollector, QList<int>& usedID);
        
        //! update m_freeID and m_firstFreeID
        void updateIDCounter (QList<int>&usedID);
        
        int m_firstFreeID; //!< The next free ID avaiable
        
        QList<int> m_freeID; //!< Free fragmented ID
        
        QList<ItemConnection*> m_data; //!< vector containing all ItemConnection instances
        
        //! de-allocate all ItemConnection
        void clear();
};

#endif
