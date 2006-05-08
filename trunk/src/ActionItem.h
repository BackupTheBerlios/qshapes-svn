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

#ifndef ACTIONITEM_H
#define ACTIONITEM_H

/*! 
    \class ActionItem
    \brief Manage undo / redo operations regarding new item creation
    \author Flavio Castelli

    Delete an item just created, or recreates it. Since item has just been created we don't have to care about any connection
 */

#include "Action.h"

class DiagramItem;

class ActionItem : public Action
{
    public:
        //! constructor
        ActionItem(DiagramItemCollector* itemCollector);
        
        //! constructor
        ActionItem(DiagramItemCollector* itemCollector, DiagramItem* item, bool undoFlag);
        
        //! destructor
        ~ActionItem();
        
        Action* apply();
        
    private:
        DiagramItem*                m_pItem; //!< item to delete/recreate
        bool                        m_bUndo; //!< undo flag: if false item has to be destroyed, else it has to be recreated
};

#endif
