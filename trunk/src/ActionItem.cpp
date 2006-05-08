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

#include "ActionItem.h"
#include "DiagramItemCollector.h"

#include <qtcanvas.h>

ActionItem::ActionItem(DiagramItemCollector* itemCollector)
    : Action (itemCollector)
{
    m_pItem = NULL;
    m_bUndo = true; // by default it's an undo operation
}

ActionItem::ActionItem(DiagramItemCollector* itemCollector, DiagramItem* item, bool undoFlag)
    : Action (itemCollector),
      m_pItem (NULL)
{
    m_pItemCollector = itemCollector;
    
    if (item != NULL)
        m_pItem = item->clone();
    
    m_bUndo = undoFlag;
}

ActionItem::~ActionItem()
{
    if (m_pItem != NULL)
        delete m_pItem;
}

Action* ActionItem::apply()
{
    if (m_pItem == NULL)
        return NULL;
    
    ActionItem* redo = new ActionItem (m_pItemCollector, m_pItem, !m_bUndo);
    
    if (m_bUndo)
    {
        // we've to recreate item
        DiagramItem* newItem = m_pItem->clone();
        
        m_pItemCollector->addItem (newItem, true);
        
        QtCanvasItem *canvasItem = dynamic_cast<QtCanvasItem*> (newItem);
        
        if (canvasItem != 0)
        {
            canvasItem->show();
            (canvasItem->canvas())->update();
        }
    }
    else
    {
        // we've to delete item
        m_pItemCollector->delItem (m_pItem->getID());
    }
    
    return redo;
}
