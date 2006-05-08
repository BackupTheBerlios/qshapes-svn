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

#ifndef DIAGRAMITEM_H_
#define DIAGRAMITEM_H_

/*! 
    \class DiagramItem
    \brief The most generic item avaiable
    \author Flavio Castelli

    DiagramItem is the parent of all the other diagram items.
*/

class PosAndSize;
class QDomDocument;
class QDomElement;
class QDomNode;
class QPoint;
class QRect;

#include <QBrush>
#include <QPen>
#include <QString>

class DiagramItem
{
    public:
        //! constructor
        DiagramItem();
        
        //! constructor
        DiagramItem (DiagramItem *diagramItem);
        
        //! destructor
        virtual ~DiagramItem();
        
        //! set the item ID
        void setID (int ID) {m_ID = ID;}
        
        //! return the item ID
        int getID () {return m_ID;}
        
        //! change item's pen
        virtual void changePen (QPen pen) = 0;
        
        //! get item's pen
        QPen getPen();
        
        //! change item's brush
        virtual void changeBrush (QBrush brush) = 0;
        
        //! return item's brush
        QBrush getBrush () { return m_brush; }

        //! return a string containing class' name
        virtual QString getType () = 0;
        
        //! expand the item to new position
        virtual void expand (int mouseX, int mouseY) = 0;
        /*!
        This is a pure virtual function, implemented by the inherited classes
        */
        
        //! check if point(mouseX,mouseY) is one of the item's expansion points
        virtual bool checkExpand (int mouseX, int mouseY) = 0;
        /*!
            This is a pure virtual function, implemented by the inherited classes
         */
        
        //! check if the item is expanding 
        virtual bool isExpanding() = 0;
        /*!
            Return true if the item is expanding, false otherwise.
        
            This is a pure virtual function, implemented by the inherited classes
         */
        
        //! reset item's expanding state
        virtual void resetExpand() = 0;
        
        //! return true if item is moving, otherwise false
        bool getMoving() {return m_bMoving;}
        
        //! set item's moving state
        virtual void setMoving(bool state);
        
        //! return true if item is inside the rectangle defined by selection, false otherwise
        virtual bool isContained (QRect& selection) = 0;
    
        //! return item's position and size according to PosAndSize. \sa PosAndSize
        virtual PosAndSize* getPosAndSize() = 0;
        
        //! set item's position and size. \sa PosAndSize
        virtual void setPosAndSize(PosAndSize* posAndSize) = 0;
        
        //! return item's tooltip message
        virtual QString getToolTip();
        
        //! set the text associated to item
        virtual void setText (QString newText) = 0;
        
        //! return the text associated to item
        QString getText() {return m_strText;};
        
        //! set item's tooltip message
        void setToolTip (QString message);
        
        //! return the top left point of the item
        virtual void getTopLeft (QPoint& point) = 0;
        
        //! return a new instance of the itself
        virtual DiagramItem* clone () = 0;
        
        //! add item details to an xml document
        virtual void save (QDomDocument *document, QDomElement *parent);
        
        //! restore item details from a QDomNode
        virtual bool load (QDomNode* item);
        
        bool operator== (DiagramItem* item2);
        
    protected:
        int m_ID; //!< item's ID
        bool         m_bMoving; //!< moving state
        QString      m_strToolTip; //!< item's tooltip message
        QString      m_strText;   //!< the string containing SystemBox's text
        QPen         m_pen;   //!< the pen used for drawing item
        QBrush       m_brush; //!< the brush used for drawing item
};

#endif /*DIAGRAMITEM_H_*/
