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

#include "ActionCombo.h"
#include "Action.h"

#include <QtAlgorithms>

ActionCombo::ActionCombo()
    : Action (NULL)
{
    m_data.clear();
}

ActionCombo::ActionCombo(QList<Action*>& actions)
{
    m_data.clear();
    
    for (int i = 0; i < actions.size(); i++)
        m_data.push_back (actions[i]);
}

ActionCombo::~ActionCombo()
{
    for (int i = 0; i < m_data.size(); i++)
    {
        if (m_data [i] != NULL)
            delete m_data[i];
    }
    
    m_data.clear();
}

void ActionCombo::addAction (Action* action)
{
    if (action != NULL)
        m_data.push_back (action);
}

Action* ActionCombo::apply()
{
    QList<Action*> redoActions;
    QList<Action*>::iterator iter;
    Action * redo;
    
    //we've to sort action, we've to execute high priority actions before other ones
    
    qSort (m_data.begin(), m_data.end(), sortActionByPriority);
    
    for (iter = m_data.begin(); iter != m_data.end(); iter++)
    {
        if ((*iter) == NULL)
            continue;
        
        redo = (*iter)->apply();
        redoActions.push_back(redo);
    }
    
    ActionCombo *comboAction = new ActionCombo (redoActions);
    
    return comboAction;
}


///////////////////////
// Support functions //
///////////////////////


bool sortActionByPriority( Action* action1, Action* action2)
{
    if (action1->getHightPriority() && action2->getHightPriority())
        return false;
    else if (action1->getHightPriority() && !action2->getHightPriority())
        return true;
    else
        return true;
}

