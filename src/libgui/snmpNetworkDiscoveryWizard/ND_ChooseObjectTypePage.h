/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  This program is free software which we release under the GNU General Public
  License. You may redistribute and/or modify this program under the terms
  of that license as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  To get a copy of the GNU General Public License, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef __ND_CHOOSEOBJECTTYPEPAGE_H_
#define __ND_CHOOSEOBJECTTYPEPAGE_H_

#include "ui_nd_chooseobjecttypepage_q.h"
#include "ObjectDescriptor.h"


class ND_ChooseObjectTypePage : public QWizardPage
{
    Q_OBJECT;

    Ui::ND_ChooseObjectTypePage_q *m_dialog;
    ObjectDescriptorList *objects;
    QStringList *objectsToUse;
    
public:
    ND_ChooseObjectTypePage(QWidget *parent);
    virtual ~ND_ChooseObjectTypePage() {}

    virtual void initializePage();

    void fillTypeChangingList();

    void changeTargetObject(const QString &buf);
    
public slots:
    void typeAddress();
    void typeHost();
    void typeFirewall();

};


#endif
