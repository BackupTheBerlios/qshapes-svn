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

#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QList>

/*! 
    \class ActionManager
    \brief Manage the undo / redo actions
    \author Flavio Castelli

    Takes care of undo/redo actions
 */

class Action;

class ActionManager : public QObject
{
    Q_OBJECT
            
    public:
        
        //! constructor
        ActionManager(QObject * parent = 0);

        //! destructor
        ~ActionManager();
        
        //! undo last action
        void undo ();
        
        //! undo last action
        void redo ();
        
        //! add an action to the manager
        void addAction (Action* action, bool undoAction);
        
        //! return true if there're undo actions availables
        bool isUndoAvailable() { return !m_dataUndo.empty(); }
        
        //! return true if there're redo actions availables
        bool isRedoAvailable() { return !m_dataRedo.empty(); }
        
    signals:
        //! qt signal, notify the undo availability
        void undoAvailable (bool status);
        
        //! qt signal, notify the redo availability
        void redoAvailable (bool status);
        
    private:
        
        //! clear all redo actions
        void clearRedo();
        
        //! de-allocate all ItemConnection
        void clear();
        
        QList<Action*> m_dataUndo; //!< vector containing undo actions
        
        QList<Action*> m_dataRedo; //!< vector containing redo actions
        
        bool m_bRedoUsed; //!< bool value used for undo coherence (if you apply a redo, and then make a new change we've to clear all redo actions)
};

#endif
