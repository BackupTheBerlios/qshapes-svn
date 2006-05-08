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

#include "ActionProperties.h"

#include "DiagramItem.h"
#include "DiagramItemCollector.h"
#include "DiagramBox.h"
#include "PosAndSize.h"

ActionProperties::ActionProperties(DiagramItemCollector* itemCollector)
    : Action (itemCollector),
      m_revertItemID (-1)
{
    m_pPosAndSize = NULL;
    m_text = "item";
}

ActionProperties::ActionProperties(DiagramItemCollector* itemCollector, DiagramItem* revertItem)
    : Action (itemCollector),
      m_revertItemID (-1)
{
    if (revertItem != NULL)
        m_revertItemID = revertItem->getID();
    
    m_pPosAndSize = revertItem->getPosAndSize();
    m_brush = revertItem->getBrush();
    m_pen = revertItem->getPen();
    m_text = revertItem->getText();
}

ActionProperties::~ActionProperties()
{
}

Action* ActionProperties::apply()
{
    ActionProperties* undo = NULL;
    
    DiagramItem* item = NULL;
    
    if (m_revertItemID != -1)
        item = m_pItemCollector->getItemByID( m_revertItemID);
    
    if (item == NULL)
        return NULL;
    
    if (m_pPosAndSize != NULL)
    {
        undo = new ActionProperties (m_pItemCollector, item);
        
        item->setPosAndSize(m_pPosAndSize);
        item->changeBrush (m_brush);
        item->changePen (m_pen);
        item->setText (m_text);
        
        // update box connections
        DiagramBox *box = dynamic_cast<DiagramBox*> (item);
        
        if (box != 0)
        {
            box->updateConnections();
            
            // item is a DiagramBox, action with DiagramBox have high priority
            undo->setHightPriority( false);
        }
    }
    
    return undo;
}
