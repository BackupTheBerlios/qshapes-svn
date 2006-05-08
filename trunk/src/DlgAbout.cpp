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

#include "DlgAbout.h"

#include <QString>

DlgAbout::DlgAbout(int versMaj, int versMin, QWidget* parent)
    : QDialog(parent, Qt::Dialog)
{
    setupUi(this);
    
    QString about;
    
    about.sprintf("QShapes version %i.%i<br> (c) 2006 , <i>QShapes's developers</i>", versMaj, versMin);
    labelAbout->setText (about);
    
    QString authors ("Flavio Castelli <A HREF=\"mailto:flavio.castelli@gmail.com\">flavio.castelli@gmail.com</A><br>");
    authors += QString("Davide Di Siena <A HREF=\"mailto:davide.disiena@gmail.com\">davide.disiena@gmail.com</A><br>");
    authors += QString ("Marco Salvi<br>");
    authors += QString ("Lorenzo Zambelli <A HREF=\"mailto:zambellilorenzo@gmail.com\">zambellilorenzo@gmail.com</A>");
    
    labelAuthors->setText (authors);
    
    labelIcon->setPixmap(QPixmap(":/images/qshapes.png"));
}


DlgAbout::~DlgAbout()
{
}


