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

#ifndef ACTIONCONNECTIONLINE_H
#define ACTIONCONNECTIONLINE_H

/*! 
    \class ActionConnectionLine
    \brief Manage undo / redo operations regarding connections
    \author Flavio Castelli

    Delete or recreate a connection between two items. This action is activated when user deletes a line connecting two boxes.
 */

#include "Action.h"

class ConnectionCollector;
class DiagramBox;
class DiagramLine;
class DiagramItemCollector;
class ItemConnection;

class ActionConnectionLine : public Action
{
    public:
        
        //! constructor
        ActionConnectionLine(DiagramItemCollector* itemCollector);
        
        //! constructor
        ActionConnectionLine(DiagramItemCollector* itemCollector, ConnectionCollector *connCollector, ItemConnection* connection, bool undoFlag, bool deleteLine);
        
        //! constructor
        ActionConnectionLine(DiagramItemCollector* itemCollector, ConnectionCollector *connCollector, int startBoxID, int endBoxID, DiagramLine* line, int connectionID, bool undoFlag, bool deleteLine);
        
        //! destructor
        ~ActionConnectionLine();
        
        Action* apply();
        
    private:
        int             m_startBoxID; //!< connection start box ID
        int             m_endBoxID; //!< connection end box ID
        DiagramLine*    m_pLine; //!< line connecting startBox with endBox
        int             m_lineID; //!< connection's line ID
        int    m_connectionID; //!< the connection ID
        bool            m_bUndo; //!< undo flag: if false the connection has to be destroyed, else it has to be recreated
        bool            m_bDeleteLine; //!< if false when the connection is destroyed line's connection is destroyed too, otherwise it's preserved
        ConnectionCollector* m_pConnCollector; //!< the connection collector of the active view
};

#endif
