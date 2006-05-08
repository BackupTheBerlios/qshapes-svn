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

#include "DiagramItem.h"
#include "SelectionManager.h"

#include <QtAlgorithms>
#include <qtcanvas.h>
#include <QRect>

SelectionManager::SelectionManager(QtCanvas* canvas, QObject * parent)
    : QObject (parent)
{
    m_pCanvas = canvas;
    m_bMultipleSelect = false;
    
    m_data.clear();
    m_dataPosAndSize.clear();
    
    emit selectionAvailable(false);
}

SelectionManager::~SelectionManager()
{
}

void SelectionManager::clearSelection()
{
    QtCanvasItem *canvasItem;
    for (int i = 0; i < m_data.size(); i++)
    {
        if (m_data[i] != NULL)
        {
            canvasItem = dynamic_cast<QtCanvasItem*> (m_data[i]);

            if (canvasItem != 0)
                canvasItem->setActive(false);
        }
    }
    
    m_data.clear();
    m_dataPosAndSize.clear();
    
    m_pCanvas->update();
    
    emit selectionAvailable( false);
}

/*!
    if multiple selection is enabled check if item is already selected, if it happens it deselect it
*/
void SelectionManager::selectItem(DiagramItem* item)
{
    QtCanvasItem *canvasItem;
    
    if (!m_bMultipleSelect)
        clearSelection();

    if (item == NULL)
        return;
    
    if (m_bMultipleSelect)
    {
        // check if item is already selected
        QList<DiagramItem*>::iterator iter;
        
        iter = qFind (m_data.begin(), m_data.end(), item);
        
        if (iter != m_data.end())
        {
            // item is already selected, we've to deselect it
            canvasItem = dynamic_cast<QtCanvasItem*> (*iter);
            
            if (canvasItem != 0)
                canvasItem->setActive(false);
            
            m_data.erase(iter);
        }
        else
        {
            canvasItem = dynamic_cast<QtCanvasItem*> (item);
            
            if (canvasItem != 0)
                canvasItem->setActive (true);
            
            m_data.push_back (item);
            m_dataPosAndSize.push_back(item->getPosAndSize());
        }
    }
    else
    {
        QtCanvasItem *canvasItem = dynamic_cast<QtCanvasItem*> (item);
            
        if (canvasItem != 0)
            canvasItem->setActive (true);
        
        m_data.push_back (item);
        m_dataPosAndSize.push_back(item->getPosAndSize());
    }
    
    m_pCanvas->update();
    
    if (!m_data.empty())
        emit selectionAvailable (true);
    else
        emit selectionAvailable (false);
}

/*!
    This method doesn't take care of m_bMultipleSelect, it adds an item to the selected ones. If item is already contained in m_data it does nothing.
*/
void SelectionManager::addItem ( DiagramItem* item)
{
    if (item == NULL)
        return;
    
    // check if item is already selected
    QList<DiagramItem*>::iterator iter;
    
    iter = qFind (m_data.begin(), m_data.end(), item);
    
    if (iter == m_data.end())
    {
        m_data.push_back (item);
        m_dataPosAndSize.push_back(item->getPosAndSize());
    }
}


/*!
    clear old selection and creates a new one with passed items
*/
void SelectionManager::selectItems(QList<DiagramItem*>& items)
{
    QtCanvasItem *canvasItem;
    clearSelection();
    
    QList<DiagramItem*>::iterator iter;
    
    for (iter = items.begin(); iter != items.end(); iter++)
    {
        if ((*iter) != NULL)
        {
            canvasItem = dynamic_cast<QtCanvasItem*> (*iter);
            
            if (canvasItem != 0)
                canvasItem->setActive (true);
            
            m_data.push_back (*iter);
            m_dataPosAndSize.push_back((*iter)->getPosAndSize());
        }
    }
    
    m_pCanvas->update();
    
    emit selectionAvailable (true);
}

DiagramItem* SelectionManager::getFirst()
{
    if (m_data.size() > 0)
        return m_data[0];
    else
        return NULL;
}

void SelectionManager::getSelectedItems (QList<DiagramItem*>& items)
{
    QList<DiagramItem*>::iterator iter;
    
    for (iter = m_data.begin(); iter != m_data.end(); iter++)
    {
        if ((*iter) != NULL)
            items.push_back (*iter);
    }
}

void SelectionManager::getSelectedItemsPosAndSize (QList<PosAndSize*>& items)
{
    QList<PosAndSize*>::iterator iter;
    
    for (iter = m_dataPosAndSize.begin(); iter != m_dataPosAndSize.end(); iter++)
        items.push_back (*iter);
}

DiagramItem* SelectionManager::getItem (int pos)
{
    if (pos < m_data.size())
        return m_data[pos];
    else
        return NULL;
}

PosAndSize* SelectionManager::getItemPosAndSize (int pos)
{
    if (pos < m_dataPosAndSize.size())
        return m_dataPosAndSize[pos];
    else
        return NULL;
}

void SelectionManager::getUpperLeftPoint (QPoint& upperLeftPoint)
{
    QPoint itemPoint;
    
    if (m_data.empty())
    {
        upperLeftPoint.setX (0);
        upperLeftPoint.setY (0);
        return;
    }
    
    QList<DiagramItem*>::iterator iter = m_data.begin();
    
    (*iter)->getTopLeft (upperLeftPoint);
    iter ++;
    
    for ( ; iter != m_data.end(); iter++)
    {
        (*iter)->getTopLeft (itemPoint);
        
        if (itemPoint.y() < upperLeftPoint.y())
            upperLeftPoint.setY (itemPoint.y());
        
        if (itemPoint.x() < upperLeftPoint.x())
            upperLeftPoint.setX (itemPoint.x());
    }
}

void SelectionManager::updatePosAndSize (DiagramItem *item)
{
    int pos;
    
    for (pos = 0; pos < m_data.size(); pos++)
    {
        if (m_data[pos]->getID() == item->getID())
            break;
    }
    
    if (pos < m_dataPosAndSize.size())
        m_dataPosAndSize[pos] = m_data[pos]->getPosAndSize();
}
