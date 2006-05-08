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

#include "SimplifiedConnection.h"

#include "DiagramBox.h"
#include "ItemConnection.h"


SimplifiedConnection::SimplifiedConnection ()
    : m_startBoxID(-1),
      m_endBoxID(-1),
      m_lineID(-1),
      m_ID (-1)
{
    
}

SimplifiedConnection::SimplifiedConnection (ItemConnection* connection)
    : m_startBoxID(-1),
      m_endBoxID(-1),
      m_lineID(-1)
{
    DiagramBox *box = NULL;
    
    box = connection->getStartBox();
    
    if (box != NULL)
        m_startBoxID = box->getID();
    
    box = connection->getEndBox();
    
    if (box != NULL)
        m_endBoxID = box->getID();
    
    m_lineID = connection->getLineID();
}

/*!
    Also translate the original connection IDs according to the translation map defined into translatedID
 */
SimplifiedConnection::SimplifiedConnection (SimplifiedConnection* connection, QMap<int, int>& translatedID)
    : m_startBoxID(-1),
      m_endBoxID(-1),
      m_lineID(-1)

{
    const int defaultValue = -1;
    
    if (connection->getStartBoxID() != -1)
        m_startBoxID = translatedID.value(connection->getStartBoxID(), defaultValue);
    
    if (connection->getEndBoxID() != -1)
        m_endBoxID = translatedID.value(connection->getEndBoxID(), defaultValue);
    
    if (connection->getLineID() != -1)
        m_lineID = translatedID.value(connection->getLineID(), defaultValue);
}

void SimplifiedConnection::translate (QMap<int, int>& translatedID)
{
    const int defaultValue = -1;
    
    if (m_startBoxID != -1)
        m_startBoxID = translatedID.value(m_startBoxID, defaultValue);
    
    if (m_endBoxID != -1)
        m_endBoxID= translatedID.value(m_endBoxID, defaultValue);
    
    if (m_lineID != -1)
        m_lineID= translatedID.value(m_lineID, defaultValue);
}
