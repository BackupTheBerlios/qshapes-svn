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

#include "ActionConnectionLine.h"

#include "ConnectionCollector.h"
#include "DiagramBox.h"
#include "DiagramItemCollector.h"
#include "DiagramLine.h"

ActionConnectionLine::ActionConnectionLine(DiagramItemCollector* itemCollector)
    : Action (itemCollector),
      m_startBoxID (-1),
      m_endBoxID (-1),
      m_lineID (-1),
      m_connectionID (-1)
{
    m_pConnCollector = NULL;
    m_pLine = NULL;
    m_bUndo = true; // by default it's an undo operation
    m_bDeleteLine = true;
}

ActionConnectionLine::ActionConnectionLine(DiagramItemCollector* itemCollector, ConnectionCollector *connCollector, int startBoxID, int endBoxID, DiagramLine* line, int connectionID, bool undoFlag, bool deleteLine)
    : Action (itemCollector),
      m_startBoxID (startBoxID),
      m_endBoxID (endBoxID),
      m_lineID (-1),
      m_connectionID (connectionID)
{
    m_pConnCollector = connCollector;
    m_bDeleteLine = deleteLine;
    
    // allocate backup of line
    
    if (deleteLine)
    {
        m_pLine = new DiagramLine (line);
        m_lineID = m_pLine->getID();
    }
    else if (line != NULL)
        m_lineID = line->getID();
    
    m_bUndo = undoFlag;
}

ActionConnectionLine::ActionConnectionLine(DiagramItemCollector* itemCollector, ConnectionCollector *connCollector, ItemConnection* connection, bool undoFlag, bool deleteLine)
    : Action (itemCollector),
      m_startBoxID ( -1),
      m_endBoxID (-1)
{
    DiagramBox* box = NULL;
    
    box = connection->getStartBox();
    
    if (box != NULL)
        m_startBoxID = box->getID();
    
    box = connection->getEndBox();
    
    if (box != NULL)
        m_endBoxID = box->getID();
    
    m_pConnCollector = connCollector;
    m_bDeleteLine = deleteLine;
    
    // allocate backup of line
    if (deleteLine)
    {
        m_pLine = new DiagramLine (connection->getLine());
        m_lineID = m_pLine->getID();
    }
    else if (connection->getLine() != NULL)
        m_lineID = (connection->getLine())->getID();
    
    m_connectionID = connection->getID();
    
    m_bUndo = undoFlag;
}

ActionConnectionLine::~ActionConnectionLine()
{
    if ((m_bDeleteLine) && (m_pLine != NULL))
        delete m_pLine;
}

Action* ActionConnectionLine::apply()
{
    if ((m_pConnCollector == NULL) || (m_pItemCollector == NULL))
        return NULL;
    
    ActionConnectionLine* redo;
    
    if (m_bUndo)
    {
        // we've to recreate the connection
        
        DiagramLine* line;
        DiagramBox *startBox = NULL;
        DiagramBox *endBox = NULL;
        
        if (m_bDeleteLine)
            line = new DiagramLine (m_pLine, m_pLine->canvas());
        else
            line = dynamic_cast<DiagramLine*>(m_pItemCollector->getItemByID( m_lineID));
        
        m_pItemCollector->addItem( line, true);
        line->show();
        
        if (m_startBoxID != -1)
            startBox = dynamic_cast<DiagramBox*>(m_pItemCollector->getItemByID (m_startBoxID));
        
        if (m_endBoxID != -1)
            endBox = dynamic_cast<DiagramBox*>(m_pItemCollector->getItemByID (m_endBoxID));
        
        ItemConnection* connection = new ItemConnection (startBox, endBox, line);
        
        if (m_connectionID != -1)
        {
            connection->setID( m_connectionID);
            m_pConnCollector->addConnection( connection, true);
        }
        else
        {
            printf ("ActionConnectionLine: unable to restore a connection\n");
            m_pConnCollector->addConnection( connection);
        }
        
        // create redo class
        redo = new ActionConnectionLine(m_pItemCollector, m_pConnCollector, m_startBoxID, m_endBoxID, line, m_connectionID, !m_bUndo, m_bDeleteLine);
    }
    else
    {
        // we've to delete the connection
        
        DiagramLine* line;
        
        if (m_bDeleteLine)
            line = m_pLine;
        else
            line = dynamic_cast<DiagramLine*>(m_pItemCollector->getItemByID( m_lineID));
        
        // we've to destroy the connection
        if (!m_pConnCollector->deleteConnectionByLineID( m_lineID))
            return NULL; // unable to delete connection
        
        if (m_bDeleteLine)
            m_pItemCollector->delItem( m_lineID);
        
        // connection successfully deleted, create redo action
        redo = new ActionConnectionLine(m_pItemCollector, m_pConnCollector, m_startBoxID, m_endBoxID, line, m_connectionID, !m_bUndo, m_bDeleteLine);
    }
    
    return redo;
}
