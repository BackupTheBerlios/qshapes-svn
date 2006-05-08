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

#include "ActionManager.h"
#include "Action.h"

ActionManager::ActionManager(QObject * parent)
    : QObject (parent),
      m_bRedoUsed (false)
{
}


ActionManager::~ActionManager()
{
    clear();
}

void ActionManager::clear()
{
    for (int i = 0; i < m_dataUndo.size(); i++)
    {
        if (m_dataUndo[i] != NULL)
            delete m_dataUndo[i];
    }
    
    m_dataUndo.clear();
    emit undoAvailable (false);
    
    clearRedo();
}

void ActionManager::clearRedo()
{
    for (int i = 0; i < m_dataRedo.size(); i++)
    {
        if (m_dataRedo[i] != NULL)
            delete m_dataRedo[i];
    }
    
    m_dataRedo.clear();
    
    emit redoAvailable( false);
}

/*!
if undoAction is true action is an undo action, otherwise it's a redo action
*/
void ActionManager::addAction( Action* action, bool undoAction)
{
    if (action == NULL)
        return;
    
    if (undoAction)
    {
        m_dataUndo.push_back(action);
        emit undoAvailable ( true);
        
        // each time we add an undo item we've to clear the redo actions for coherence
        if (m_bRedoUsed)
        {
            clearRedo();
            m_bRedoUsed = false;
        }
    }
    else
    {
        m_dataRedo.push_back(action);
        emit redoAvailable ( true);
    }
}

void ActionManager::undo ()
{
    if (m_dataUndo.empty())
        return;
    
    Action* redo = NULL;
    
    if (m_dataUndo[m_dataUndo.size() -1] != NULL)
    {
        redo = (m_dataUndo[m_dataUndo.size() -1])->apply();
        delete m_dataUndo[m_dataUndo.size() -1];
    }
    
    m_dataUndo.pop_back();
    
    if (m_dataUndo.empty())
        emit undoAvailable (false);
    
    addAction (redo, false);
}

void ActionManager::redo ()
{
    if (m_dataRedo.empty())
        return;
    
    Action* undo = NULL;
    
    if (m_dataRedo[m_dataRedo.size() -1] != NULL)
    {
        undo = (m_dataRedo[m_dataRedo.size() -1])->apply();
        delete m_dataRedo[m_dataRedo.size() -1];
    }
    
    m_dataRedo.pop_back();
    
    if (m_dataRedo.empty())
        emit redoAvailable (false);
    
    m_bRedoUsed = true;
    
    if (undo != NULL)
    {
        m_dataUndo.push_back (undo);
        emit undoAvailable( true);
    }
}
