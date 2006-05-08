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

#ifndef SELECTEDITEMMANAGER_H
#define SELECTEDITEMMANAGER_H

#include <QList>
#include <QObject>

class DiagramItem;
class PosAndSize;
class QtCanvas;
class QPoint;

/*! 
    \class SelectionManager
    \brief Manage the DiagramView selected items
    \author Flavio Castelli

    Takes care of selecting-deselecting DiagramItems
 */

class SelectionManager : public QObject
{
    Q_OBJECT

    public:
        
        //! constructor
        SelectionManager(QtCanvas *canvas, QObject * parent = 0);

        //! destructor
        ~SelectionManager();
    
        //! deselect all items
        void clearSelection();
        
        //! select item
        void selectItem(DiagramItem* item);
        
        //! select all items
        void selectItems(QList<DiagramItem*>& items);
        
        //! add a new item to the selecion
        void addItem ( DiagramItem* item);
        
        //! return all selected items
        void getSelectedItems (QList<DiagramItem*>& items);
        
        //! return first selected item
        DiagramItem* getFirst();
        
        //! return all items position and size informations. \sa PosAndSize
        void getSelectedItemsPosAndSize (QList<PosAndSize*>& items);
        
        //! return multiple selection value
        bool getMultipleSelection () { return m_bMultipleSelect;}
        
        //! enable/disable multiple selection
        void setMultipleSelection (bool enable) { m_bMultipleSelect = enable;}
        
        //! return true if there are no selected items, false otherwise
        bool empty() { return m_data.empty();}

        //! return the number of active items
        int size() { return m_data.size();}
        
        //! return the active item at pos
        DiagramItem* getItem (int pos);
        
        //! return item's position and size. \sa PosAndSize
        PosAndSize* getItemPosAndSize (int pos);
        
        //! updates item's position and size. \sa PosAndSize
        void updatePosAndSize (DiagramItem* item);
        
        //! return the top left point of the rectangle containing all selected item
        void getUpperLeftPoint (QPoint& upperLeftPoint);
        
    signals:
        //! qt signal, notify the selection availability
        void selectionAvailable(bool status);
        
    private:
        QList<DiagramItem*> m_data; //!< all selected items
        QList<PosAndSize*> m_dataPosAndSize; //!< all the items position and size. \sa PosAndSize
        bool m_bMultipleSelect; //!< multiple selection flag
        QtCanvas* m_pCanvas; //!< diagraview's canvas
};

#endif
