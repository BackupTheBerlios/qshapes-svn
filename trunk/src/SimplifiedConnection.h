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

#ifndef SIMPLIFIEDCONNECTION_
#define SIMPLIFIEDCONNECTION_

#include <QMap>

class ItemConnection;

/*! 
    \class SimplifiedConnection
    \brief A simplified version of ItemConnection

    This class contains only the ID of the connection items and a translation method for paste operations.
 */
 
class SimplifiedConnection
{
    public:
        //! contructor
        SimplifiedConnection ();
        
        //! contructor from a standard ItemConnection
        SimplifiedConnection (ItemConnection* connection);
        
        //! constructor from a SimplifiedConnection
        SimplifiedConnection (SimplifiedConnection* connection, QMap<int, int>& translatedID);
        
        //! destructor
        virtual ~SimplifiedConnection() {};
        
        //! return start box ID
        int getStartBoxID() { return m_startBoxID; }
        
        //! return end box ID
        int getEndBoxID() { return m_endBoxID; }
        
        //! return line ID
        int getLineID() { return m_lineID; }
        
        //! translate the connection IDs according to the translation map
        void translate (QMap<int, int>& translatedID);
        
    private:
        int m_startBoxID; //!< start box ID
        int m_endBoxID; //!< end box ID
        int m_lineID; //!< line ID
        int m_ID; //!< connection ID
};

#endif //SIMPLIFIEDCONNECTION_
