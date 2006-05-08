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

#include "ui_FormProperty.h"

#include <QBrush>
#include <QIcon>
#include <QPoint>
#include <QPen>
#include <QSize>

class DiagramLine;
class DiagramBox;
class QPaintEvent;
class QString;

/*! \class DlgProperty
    \brief Item's property dialog
    \author Flavio Castelli

    The dialog showing item's property
 */

class DlgProperty : public QDialog, private Ui::DlgProperty
{
    Q_OBJECT

    public:
        //! constructor
        DlgProperty(DiagramLine* line, int connID, QWidget *parent = 0);

        //! constructor
        DlgProperty(DiagramBox* box, QWidget *parent = 0);
        
        //! destructor
        virtual ~DlgProperty();
        
    private slots:
        void itemNameChanged();
        void protectSizePositionChanged (int state);
        void topChanged (int value);
        void topChanged ();
        void leftChanged (int value);
        void leftChanged ();
        void heightChanged (int value);
        void heightChanged ();
        void widthChanged (int value);
        void widthChanged ();
        void lineStartPointXChanged (int value);
        void lineStartPointXChanged ();
        void lineStartPointYChanged (int value);
        void lineStartPointYChanged ();
        void lineEndPointXChanged (int value);
        void lineEndPointXChanged ();
        void lineEndPointYChanged (int value);
        void lineEndPointYChanged ();
        void outlineWidthChanged (int value);
        void outlineWidthChanged ();
        void outlineStyleChanged (int value);
        void outlineColor ();
        void fillStyleChanged(int value);
        void fillColor ();
        void accept();
        
    private:
        
        void createActions();
        void setupBtnOutlineColor();
        void setupComboOutlineStyle();
        void setupComboFillType();
        void updateFrameOutlinePreview();
        QIcon getLineIcon(QSize size, Qt::PenStyle penStyle);
        void updateFrameFillPreview();
        void setupBtnFillColor();
        
        QString m_strText; //!< item's text
        int m_height;
        int m_width;
        QPoint m_boxTopLeft;
        QPoint m_lineStartPoint;
        QPoint m_lineEndPoint;
        QPen m_pen;
        QBrush m_brush;
        DiagramLine* m_pLine;
        DiagramBox* m_pBox;
};
