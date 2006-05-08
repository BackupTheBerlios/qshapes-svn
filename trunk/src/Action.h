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

#ifndef ACTION_H
#define ACTION_H

/*! 
    \class Action
    \brief Generic user action
    \author Flavio Castelli

    Virtual pure class for user actions
 */

class DiagramItemCollector;

class Action
{
    public:
        //! constructor
        Action ();
        
        //! constructor
        Action(DiagramItemCollector* itemCollector);

        //! destructor
        virtual ~Action();

        //! apply action
        virtual Action* apply() = 0;
        
        //! return item's priority
        bool getHightPriority() { return m_bHighPriority; }

        //! set item's priority
        void setHightPriority(int priority) { m_bHighPriority = priority; }
        
    protected:
        DiagramItemCollector* m_pItemCollector;  //!< the diagram item collector of the active view
        
        bool m_bHighPriority; //!< action's priority. This attribute is used only in ActionCombo. \sa ActionCombo
};

#endif
