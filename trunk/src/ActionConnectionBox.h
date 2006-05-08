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

#ifndef ACTIONCONNECTIONBOX_H
#define ACTIONCONNECTIONBOX_H

/*! 
    \class ActionConnectionBox
    \brief Manage undo / redo operations regarding connections
    \author Flavio Castelli

    Modify the connection extremes . This action is activated when user moves the line connecting two boxes to a new point.
 */

#include "Action.h"

class ConnectionCollector;
class DiagramBox;
class ItemConnection;

class ActionConnectionBox : public Action
{
    public:
        //! constructor
        ActionConnectionBox(DiagramItemCollector* itemCollector);
        
        //! constructor
        ActionConnectionBox(DiagramItemCollector* itemCollector, ItemConnection* connection, ConnectionCollector* connCollector, bool changeStartBox);
        
        //! constructor
        ActionConnectionBox(DiagramItemCollector* itemCollector, DiagramBox* startBox, DiagramBox* endBox, ItemConnection* connection, ConnectionCollector* connCollector, bool changeStartBox);
        
        //! destructor
        ~ActionConnectionBox();
        
        Action* apply();
        
    private:
        int     m_startBoxID; //!< connection start box ID
        int     m_endBoxID; //!< connection end box ID
        bool    m_bChangeStartBox; //!< true if changing startBox, otherwise false --> changing endBox
        int m_connectionID; //!< the connection ID
        ConnectionCollector* m_pConnCollector; //!< the connection collector of the active view
};

#endif
