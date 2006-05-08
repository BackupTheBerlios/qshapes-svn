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

#include "ActionConnectionBox.h"

#include "ConnectionCollector.h"
#include "DiagramBox.h"
#include "DiagramItemCollector.h"

ActionConnectionBox::ActionConnectionBox(DiagramItemCollector* itemCollector)
    : Action( itemCollector),
      m_startBoxID (-1),
      m_endBoxID (-1),
      m_connectionID (-1)
{
    m_pConnCollector = NULL;
    m_bChangeStartBox = true;
}

ActionConnectionBox::ActionConnectionBox(DiagramItemCollector* itemCollector, DiagramBox* startBox, DiagramBox* endBox, ItemConnection* connection, ConnectionCollector* connCollector, bool changeStartBox)
    : Action (itemCollector),
      m_startBoxID (-1),
      m_endBoxID (-1)
{
    m_pConnCollector = connCollector;
    
    m_connectionID = connection->getID();
    
    if (startBox != NULL)
        m_startBoxID = startBox->getID();
    
    if (endBox != NULL)
        m_endBoxID = endBox->getID();
    
    m_bChangeStartBox = changeStartBox;
}

ActionConnectionBox::ActionConnectionBox(DiagramItemCollector* itemCollector, ItemConnection* connection, ConnectionCollector* connCollector, bool changeStartBox)
    : Action (itemCollector)
{
    m_pConnCollector = connCollector;
    
    DiagramBox* box = NULL;
    
    box = connection->getStartBox();
    
    if (box != NULL)
        m_startBoxID = box->getID();
    
    box = connection->getEndBox();
    
    if (box != NULL)
        m_endBoxID = box->getID();
    
    m_connectionID = connection->getID();
    
    m_bChangeStartBox = changeStartBox;
}

ActionConnectionBox::~ActionConnectionBox()
{
}

Action* ActionConnectionBox::apply()
{
    // create redo class
    ItemConnection* connection = m_pConnCollector->getByID( m_connectionID);
    
    if (connection == NULL)
        return NULL;
    
    ActionConnectionBox* redo = new ActionConnectionBox (m_pItemCollector, connection->getStartBox(), connection->getEndBox(), connection, m_pConnCollector, m_bChangeStartBox);
    
    DiagramBox* box = NULL;
    
    // restore original state
    if ((m_bChangeStartBox) && (m_startBoxID != -1))
        box = dynamic_cast<DiagramBox*>(m_pItemCollector->getItemByID( m_startBoxID));
    else if ((!m_bChangeStartBox) && (m_endBoxID != -1))
        box = dynamic_cast<DiagramBox*>(m_pItemCollector->getItemByID( m_endBoxID));
    
    connection->changeBox( m_bChangeStartBox, box);
    
    return redo;
}
