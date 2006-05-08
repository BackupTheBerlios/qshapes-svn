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

#ifndef ACTIONMOVERESIZE_H
#define ACTIONMOVERESIZE_H

#include "Action.h"

class DiagramItem;
class PosAndSize;

/*! 
    \class ActionMoveResize
    \brief Class for undo/redo move actions
    \author Flavio Castelli

    This class is used to perform undo/redo move actions
 */

class ActionMoveResize : public Action
{
    public:
        //! constructor
        ActionMoveResize(DiagramItemCollector* itemCollector);
        
        //! constructor
        ActionMoveResize(DiagramItemCollector* itemCollector, DiagramItem* revertItem, PosAndSize* posAndSize);
        
        //! destructor
        ~ActionMoveResize();
        
        //! set the original item state
        void setRevertItem (DiagramItem* revertItem);
        
        //! set the initial surrounding rectangle
        void setPosAndSize (PosAndSize* posAndSize) { m_pPosAndSize = posAndSize; }
        
        //! apply action
        Action* apply();
        
    private:
        PosAndSize* m_pPosAndSize; //!< item's position and size. \sa PosAndSize
        int m_revertItemID; //!< item to revert ID
};

#endif
