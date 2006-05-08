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

#ifndef DIAGRAMITEMCOLLECTOR_H_
#define DIAGRAMITEMCOLLECTOR_H_

#include "DiagramItem.h"

#include <QList>

class ConnectionCollector;
class QDomDocument;
class QDomElement;
class QDomNode;
class QtCanvas;

/*! \class DiagramItemCollector
    \brief The collector of DiagramItem instances
    \author Flavio Castelli

    DiagramItemCollector  takes care of this features:
    - assign ID to DiagramItem, avoid duplicates
    - de-allocate all DiagramItem \sa DiagramItemCollector::clear()
 */
class DiagramItemCollector
{
    public:
        //! class constructor
        DiagramItemCollector();
        
        //! class destructor
        virtual ~DiagramItemCollector();
        
        //! add a new DiagramItem
        void addItem (DiagramItem* item, bool forced = false);
        
        //! delete a DiagramItem defined by ID
        void delItem (int ID);
        
        //! return item defined by ID
        DiagramItem* getItemByID(int ID);
        
        //! return first avaiable ID without incrementing local counter
        int getNextID() { return m_firstFreeID; }
        
        //! save all diagram items into an xml document
        void save (QDomDocument* document, QDomElement* parent);
        
        //! load all diagram items from an xml document
        void load (QDomNode items, QtCanvas* canvas);
        
        //! restore diagram box connections
        void restoreBoxConnections (QDomNode items, ConnectionCollector* connCollector);
        
        //! restore all diagram boxes connections
        void restoreBoxesConnections (QDomNode items, ConnectionCollector* connCollector);
    private:
        
        //! de-allocate all ItemConnection
        void clear();
    
        //! load a single item starting form an xml QDomNode
        void loadItem (QDomNode item, QtCanvas* canvas, QList<int>& usedID);
        
        //! update m_freeID and m_firstFreeID
        void updateIDCounter (QList<int>&usedID);
        
        int m_firstFreeID; //!< The next free ID avaiable
        QList<int> m_freeID; //!< Free fragmented ID
        QList<DiagramItem*> m_data; //!< vector containing all DiagramItem instances
};

#endif /*DIAGRAMITEMCOLLECTOR_H_*/
