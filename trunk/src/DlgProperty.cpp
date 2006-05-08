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

#include "DlgProperty.h"
#include "DiagramLine.h"
#include "DiagramBox.h"

#include <QColorDialog>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QString>

DlgProperty::DlgProperty(DiagramLine* line,int connID, QWidget *parent)
    : QDialog(parent, Qt::Dialog)
{
    setupUi(this);

    m_pLine = line;
    m_pBox = NULL;
    
    m_lineStartPoint = line->startPoint();
    m_lineEndPoint = line->endPoint();
    
    spinLineStartX->setValue(m_lineStartPoint.x());
    spinLineStartY->setValue(m_lineStartPoint.y());
    spinLineEndX->setValue(m_lineEndPoint.x());
    spinLineEndY->setValue(m_lineEndPoint.y());
    
    m_strText = line->getText();
    
    m_pen = line->getPen();
    
    editName->setText(m_strText);
    
    createActions();
    
    setupBtnOutlineColor();
    setupComboOutlineStyle();
    updateFrameOutlinePreview();
    
    setupBtnFillColor();
    updateFrameFillPreview();
    labelFillPreview->hide();
    
    tabWidget->removeTab(2);
    stackedWidget->setCurrentWidget (pageLine);
    
    if (connID != -1)
    {
        stackedWidget->setEnabled (false);
        checkProtect->setEnabled (false);
    }
}

DlgProperty::DlgProperty(DiagramBox* box, QWidget *parent)
    : QDialog(parent, Qt::Dialog)
{
    setupUi(this);

    m_pBox = box;
    m_pLine = NULL;
    
    m_strText = box->getText();
    
    box->getTopLeft(m_boxTopLeft);
    
    m_height = box->getHeight();
    m_width = box->getWidth();
    m_pen = box->getPen();
    m_brush = box->getBrush();
    
    editName->setText(m_strText);
    spinTop->setValue(m_boxTopLeft.y());
    spinLeft->setValue(m_boxTopLeft.x());
    spinHeight->setValue(m_height);
    spinWidth->setValue(m_width);
    
    createActions();
    
    setupBtnOutlineColor();
    setupComboOutlineStyle();
    updateFrameOutlinePreview();
    
    setupComboFillType();
    
    stackedWidget->setCurrentWidget (pageBox);
}

DlgProperty::~DlgProperty()
{
}

void DlgProperty::createActions()
{
    // general tab
    connect (editName, SIGNAL (editingFinished()), this, SLOT (itemNameChanged()));
    
    connect (checkProtect, SIGNAL(stateChanged ( int )), this, SLOT (protectSizePositionChanged(int)));
    
    connect (spinTop,SIGNAL (valueChanged ( int )), this, SLOT (topChanged (int)));
    connect (spinTop,SIGNAL (editingFinished ()), this, SLOT (topChanged ()));
    
    connect (spinLeft,SIGNAL (valueChanged ( int )), this, SLOT (leftChanged (int)));
    connect (spinLeft,SIGNAL (editingFinished ()), this, SLOT (leftChanged ()));
    
    connect (spinHeight,SIGNAL (valueChanged ( int )), this, SLOT (heightChanged (int)));
    connect (spinHeight,SIGNAL (editingFinished ()), this, SLOT (heightChanged ()));
    
    connect (spinWidth, SIGNAL (valueChanged ( int )), this, SLOT (widthChanged (int)));
    connect (spinWidth,SIGNAL (editingFinished ()), this, SLOT (widthChanged ()));
    
    connect (spinLineStartX, SIGNAL (valueChanged ( int )), this, SLOT (lineStartPointXChanged (int)));
    connect (spinLineStartX,SIGNAL (editingFinished ()), this, SLOT (lineStartPointXChanged ()));
    
    connect (spinLineStartY, SIGNAL (valueChanged ( int )), this, SLOT (lineStartPointYChanged (int)));
    connect (spinLineStartY,SIGNAL (editingFinished ()), this, SLOT (lineStartPointYChanged ()));
    
    connect (spinLineEndX, SIGNAL (valueChanged ( int )), this, SLOT (lineEndPointXChanged (int)));
    connect (spinLineEndX,SIGNAL (editingFinished ()), this, SLOT (lineEndPointXChanged ()));
    
    connect (spinLineEndY, SIGNAL (valueChanged ( int )), this, SLOT (lineEndPointYChanged (int)));
    connect (spinLineEndY,SIGNAL (editingFinished ()), this, SLOT (lineEndPointYChanged ()));
    
    // outline tab
    connect (spinOutlineWidth, SIGNAL (valueChanged ( int )), this, SLOT (outlineWidthChanged (int)));
    connect (spinOutlineWidth,SIGNAL (editingFinished ()), this, SLOT (outlineWidthChanged ()));
    
    connect (comboOutlineStyle, SIGNAL (currentIndexChanged ( int )), this, SLOT (outlineStyleChanged (int)));
    connect (btnOutlineColor, SIGNAL (clicked () ), this, SLOT (outlineColor ()));
    
    // fill tab
    connect (comboFillType, SIGNAL (currentIndexChanged ( int )), this, SLOT (fillStyleChanged (int)));
    connect (btnFillColor, SIGNAL (clicked () ), this, SLOT (fillColor ()));
}

void DlgProperty::setupBtnOutlineColor()
{
    // setup btnOutlineColor
    QPixmap pixmap (btnOutlineColor->iconSize());
    
    pixmap.fill (m_pen.color());
    
    QIcon icon (pixmap);
    btnOutlineColor->setIcon (icon);
}

void DlgProperty::itemNameChanged()
{
    m_strText = editName->text();
}

QIcon DlgProperty::getLineIcon(QSize size, Qt::PenStyle penStyle)
{
    QPixmap pixmap (size);
    pixmap.fill(Qt::white);
    
    QPen pen;
    pen.setColor(Qt::black);
    pen.setStyle (penStyle);
    pen.setWidth (2);
    
    QPainter painter (&pixmap);
    painter.setPen (pen);
    
    painter.drawLine(5, pixmap.size().height() / 2, pixmap.size().width() - 5, pixmap.size().height() / 2);
    
    return QIcon (pixmap);
}

void DlgProperty::setupComboOutlineStyle()
{
    //setup comboOutlineStyle
    QString string ("");
    QSize size = comboOutlineStyle->iconSize();

    comboOutlineStyle->disconnect (this);
    
    comboOutlineStyle->addItem (getLineIcon (size, Qt::SolidLine), string);
    comboOutlineStyle->addItem (getLineIcon (size, Qt::DashLine), string);
    comboOutlineStyle->addItem (getLineIcon (size, Qt::DotLine), string);
    comboOutlineStyle->addItem (getLineIcon (size, Qt::DashDotLine), string);
    comboOutlineStyle->addItem (getLineIcon (size, Qt::DashDotDotLine), string);
    
    switch (m_pen.style())
    {
        case Qt::SolidLine:
            comboOutlineStyle->setCurrentIndex(0);
            break;
        case Qt::DashLine:
            comboOutlineStyle->setCurrentIndex(1);
            break;
        case Qt::DotLine:
            comboOutlineStyle->setCurrentIndex(2);
            break;
        case Qt::DashDotLine:
            comboOutlineStyle->setCurrentIndex(3);
            break;
        case Qt::DashDotDotLine:
            comboOutlineStyle->setCurrentIndex(4);
            break;
        default:
            break;
    }
    
    connect (comboOutlineStyle, SIGNAL (currentIndexChanged ( int )), this, SLOT (outlineStyleChanged (int)));
    
    spinOutlineWidth->setValue(m_pen.width());
    
}

void DlgProperty::protectSizePositionChanged (int state)
{
    if (state == Qt::Unchecked)
    {
        stackedWidget->setEnabled(true);
        groupPositionSize->setEnabled(true);
    }
    else if (state == Qt::Checked)
    {
        stackedWidget->setEnabled(false);
        groupPositionSize->setEnabled(false);
    }
}

void DlgProperty::heightChanged ()
{
    heightChanged (spinHeight->value());
}

void DlgProperty::heightChanged (int value)
{
    if (checkAspect->checkState() == Qt::Checked)
    {
        double aspectRatio = (double) m_width / (double) m_height;
        
        m_height = value;
        
        m_width = (int) (aspectRatio * m_height);
        
        spinWidth->disconnect(this);
        
        spinWidth->setValue (m_width);
        
        connect (spinWidth, SIGNAL (valueChanged ( int )), this, SLOT (widthChanged (int)));
    }
    else
        m_height = value;
}

void DlgProperty::widthChanged ()
{
    widthChanged (spinWidth->value());
}

void DlgProperty::widthChanged (int value)
{
    if (checkAspect->checkState() == Qt::Checked)
    {
        double aspectRatio = (double) m_width / (double) m_height;
        
        m_width = value;
        
        m_height = (int) (aspectRatio * m_width);
        
        spinHeight->disconnect(this);
        
        spinHeight->setValue (m_height);
        
        connect (spinHeight,SIGNAL (valueChanged ( int )), this, SLOT (heightChanged (int)));
    }
    else
        m_width = value;
}

void DlgProperty::lineStartPointXChanged ()
{
    m_lineStartPoint.setX(spinLineStartX->value());
}

void DlgProperty::lineStartPointXChanged (int value)
{
    m_lineStartPoint.setX(value);
}

void DlgProperty::lineStartPointYChanged ()
{
    m_lineStartPoint.setY(spinLineStartY->value());
}

void DlgProperty::lineStartPointYChanged (int value)
{
    m_lineStartPoint.setY(value);
}

void DlgProperty::lineEndPointXChanged ()
{
    m_lineEndPoint.setX(spinLineEndX->value());
}

void DlgProperty::lineEndPointXChanged (int value)
{
    m_lineEndPoint.setX(value);
}

void DlgProperty::lineEndPointYChanged ()
{
    m_lineEndPoint.setY(spinLineEndY->value());
}

void DlgProperty::lineEndPointYChanged (int value)
{
    m_lineEndPoint.setY(value);
}

void DlgProperty::topChanged ()
{
    m_boxTopLeft.setY(spinTop->value());
}

void DlgProperty::topChanged (int value)
{
    m_boxTopLeft.setY(value);
}

void DlgProperty::leftChanged ()
{
    m_boxTopLeft.setX(spinTop->value());
}

void DlgProperty::leftChanged (int value)
{
    m_boxTopLeft.setX(value);
}

void DlgProperty::outlineWidthChanged ()
{
    m_pen.setWidth (spinOutlineWidth->value());
    updateFrameOutlinePreview();
}

void DlgProperty::outlineWidthChanged (int value)
{
    m_pen.setWidth (value);
    updateFrameOutlinePreview();
}

void DlgProperty::outlineStyleChanged (int value)
{
    switch (value)
    {
        case 0:
            m_pen.setStyle (Qt::SolidLine);
            updateFrameOutlinePreview();
            break;
        case 1:
            m_pen.setStyle (Qt::DashLine);
            updateFrameOutlinePreview();
            break;
        case 2:
            m_pen.setStyle (Qt::DotLine);
            updateFrameOutlinePreview();
            break;
        case 3:
            m_pen.setStyle (Qt::DashDotLine);
            updateFrameOutlinePreview();
            break;
        case 4:
            m_pen.setStyle (Qt::DashDotDotLine);
            updateFrameOutlinePreview();
            break;
    }
}

void DlgProperty::updateFrameOutlinePreview()
{
    QPixmap pixmap (frameOutlinePreview->width(), frameOutlinePreview->height());
    pixmap.fill(Qt::white);
    
    QPainter painter (&pixmap);
    painter.setPen (m_pen);
    painter.drawLine(5, pixmap.size().height() / 2, pixmap.size().width() - 5, pixmap.size().height() / 2);
    
    
    labelOutlinePreview->setPixmap (pixmap);
}

void DlgProperty::outlineColor()
{
    QColor color = QColorDialog::getColor();
    m_pen.setColor (color);
    
    setupBtnOutlineColor();
    updateFrameOutlinePreview();
}

void DlgProperty::fillStyleChanged(int value)
{
    switch (value)
    {
        case 0:
            // transparent
            m_brush.setStyle (Qt::NoBrush);
            updateFrameFillPreview();
            btnFillColor->setEnabled (false);
            labelFillColor->setEnabled(false);
            labelFillPreview->hide();
            break;
        case 1:
            m_brush.setStyle (Qt::SolidPattern);
            btnFillColor->setEnabled (true);
            labelFillColor->setEnabled(true);
            labelFillPreview->show();
            updateFrameFillPreview();
            break;
    }
}

void DlgProperty::setupComboFillType()
{
    if (m_brush.style() == Qt::NoBrush)
    {
        comboFillType->setCurrentIndex(0);
        btnFillColor->setEnabled (false);
        labelFillPreview->hide();
        labelFillColor->setEnabled(false);
    }
    else
    {
        comboFillType->setCurrentIndex(1);
        btnFillColor->setEnabled (true);
        labelFillPreview->show();
        labelFillColor->setEnabled(true);
    }
    
    setupBtnFillColor();
    updateFrameFillPreview();
}

void DlgProperty::fillColor ()
{
    QColor color = QColorDialog::getColor();
    m_brush.setColor (color);
    
    setupBtnFillColor();
    updateFrameFillPreview();
}

void DlgProperty::setupBtnFillColor()
{
    // setup btnOutlineColor
    QPixmap pixmap (btnFillColor->iconSize());
    
    if (m_brush.style() == Qt::NoBrush)
        pixmap.fill (Qt::black);
    else
        pixmap.fill (m_brush.color());
    
    QIcon icon (pixmap);
    btnFillColor->setIcon (icon);
}

void DlgProperty::updateFrameFillPreview()
{
        QPixmap pixmap (frameFillPreview->width(), frameFillPreview->height());
        pixmap.fill(m_brush.color());
        
        labelFillPreview->setPixmap (pixmap);

}

void DlgProperty::accept()
{
    if (m_pBox != NULL)
    {
        m_pBox->setTopLeft (m_boxTopLeft);
        m_pBox->setHeight (m_height);
        m_pBox->setWidth (m_width);
        m_pBox->changePen (m_pen);
        m_pBox->changeBrush (m_brush);
        m_pBox->setText (m_strText);
    }
    else if (m_pLine != NULL)
    {
        m_pLine->setText (m_strText);
        m_pLine->changePen (m_pen);
        m_pLine->setPoints(m_lineStartPoint.x(), m_lineStartPoint.y(), m_lineEndPoint.x(), m_lineEndPoint.y());
    }
    
    QDialog::accept();
}
