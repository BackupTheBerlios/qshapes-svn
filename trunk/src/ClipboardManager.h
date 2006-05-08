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

#ifndef CLIPBOARDMANAGER_
#define CLIPBOARDMANAGER_

#include <QList>
#include <QMap>
#include <QObject>

class DiagramItem;
class ItemConnection;
class QPoint;
class QtCanvas;
class SimplifiedConnection;

/*! 
    \class ClipboardManager
    \brief Class that handles cut & copied DiagramItems
    \author Flavio Castelli

    This class manage all cut & copied DiagramItems, between different DiagramViews
 */
class ClipboardManager : public QObject
{
    Q_OBJECT
    
    public:
        //! constructor
        ClipboardManager (QObject* parent = 0);
        
        //! destructor
        virtual ~ClipboardManager();
    
        //! add DiagramItems to the buffer
        void addItems (QList<DiagramItem*>& items, QList<ItemConnection*>& connections, QPoint& selUpperLeft);
        
        //! return all informations needed by the paste operation
        void getItems (QtCanvas* canvas, QList<DiagramItem*>& items, QPoint& pastePoint);

        //! return the connections to restore
        void getTranslatedConnections (QList<SimplifiedConnection*>& connToRestore, QMap<int, int>& translatedID);
        
        //! return true il the clipboard is empty, false otherwise
        bool empty();
        
    signals:
        //! qt signal, emitted when clipboard is empty or it's filled with something
        void pasteAvailable (bool status);

    private:
        //! de-allocate all buffered items
        void clear();
        
        //! save old connections that are still valid
        void saveConnections(QList<DiagramItem*>& availableItems, QList<ItemConnection*>& connections);
        
        QList<DiagramItem*> m_data; //!< vector containing all buffered items
        QList<SimplifiedConnection*> m_validConnections; //!< vector containing all valid connections
};

#endif /*CLIPBOARDMANAGER_*/
