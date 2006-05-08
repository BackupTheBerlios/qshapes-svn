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

#ifndef DLGABOUT_H
#define DLGABOUT_H

/*! \class DlgAbout
    \brief QShapes's about dialog
    \author Flavio Castelli

    The dialog used to show QShapes informations.
 */

#include <QDialog>
#include "ui_FormAbout.h"

class DlgAbout : public QDialog, private Ui::DlgAbout
{
    Q_OBJECT
            
    public:
        DlgAbout(int versMaj, int versMin, QWidget* parent = 0);

        virtual ~DlgAbout();
};

#endif
