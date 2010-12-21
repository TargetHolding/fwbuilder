/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id$

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


#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "platforms.h"
#include "ProjectPanel.h"

#include "FWBTree.h"
#include "CustomServiceDialog.h"
#include "FWBSettings.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/Resources.h"

#include <memory>

#include <qlineedit.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <QUndoStack>

#include <iostream>

#include "FWWindow.h"

using namespace libfwbuilder;
using namespace std;

CustomServiceDialog::CustomServiceDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::CustomServiceDialog_q;
    m_dialog->setupUi(this);
    obj=NULL;
}

CustomServiceDialog::~CustomServiceDialog()
{
    delete m_dialog;
}

void CustomServiceDialog::getHelpName(QString *str)
{
    *str = "CustomServiceDialog";
}

void CustomServiceDialog::loadFWObject(FWObject *o)
{
    obj=o;
    CustomService *s = dynamic_cast<CustomService*>(obj);
    assert(s!=NULL);

    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

/* fill in m_dialog->platform */
    m_dialog->platform->clear();

    int cp=0;
    QString default_platform = 
        st->value(SETTINGS_PATH_PREFIX"/CustomService/Platform").toString();

    QMap<QString,QString> platforms = getAllPlatforms();
    QMap<QString,QString>::iterator i;
    for (i=platforms.begin(); i!=platforms.end(); i++,cp++)
    {
//        cerr << "m_dialog->platform: key=" << i.key()
//             << "  data=" << i.data() << endl;

/*
 * here i.key is m_dialog->platform m_dialog->code ( "ipf", "ipfw",
 * "iptables", "pf") while i.data is human readable name ("ipfilter",
 * "PF" )
 */
        platformReverseMap[i.value()] = i.key();

        m_dialog->platform->addItem(i.value());
        if (default_platform=="") default_platform = i.key();
        if (default_platform==i.key()) m_dialog->platform->setCurrentIndex(cp);

        string platform_code = i.key().toStdString();

        allCodes[i.key()] = QString(s->getCodeForPlatform(platform_code).c_str());
    }

    fillDialogInputFields();

    QString protocol = s->getProtocol().c_str();
    if (protocol == "") protocol = "any";

    m_dialog->protocol->clear();
    m_dialog->protocol->addItem("any");
    m_dialog->protocol->addItem("tcp");
    m_dialog->protocol->addItem("udp");
    m_dialog->protocol->addItem("icmp");
    m_dialog->protocol->addItem("ipv6-icmp");

    bool standard_protocol = false;
    int proto_index = 0;
    for (; proto_index < m_dialog->protocol->count(); ++proto_index)
    {
        if (protocol == m_dialog->protocol->itemText(proto_index))
        {
            m_dialog->protocol->setCurrentIndex(proto_index);
            standard_protocol = true;
            break;
        }
    }
    if (!standard_protocol)
    {
        m_dialog->protocol->addItem(protocol);
        m_dialog->protocol->setCurrentIndex(proto_index);
    }

    int af = s->getAddressFamily();
    if (af == AF_INET6)
        m_dialog->ipv6->setChecked(true);
    else
        m_dialog->ipv4->setChecked(true);

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);

//    m_dialog->platform->setEnabled(!o->isReadOnly());

    m_dialog->code->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->code);


    init=false;
}

void CustomServiceDialog::changed()
{
    if (!init)
    {
        QString pl = platformReverseMap[m_dialog->platform->currentText()];
        allCodes[pl] = m_dialog->code->text().toUtf8().constData();
    }
    BaseObjectDialog::changed();
}

void CustomServiceDialog::validate(bool *res)
{
    *res=true;
    if (!validateName(this,obj,m_dialog->obj_name->text()))
    {
        *res=false;
        return;
    }
}



void CustomServiceDialog::platformChanged()
{
    init=true;
    fillDialogInputFields();
    init=false;
//    changed();
}

void CustomServiceDialog::applyChanges()
{
    std::auto_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    CustomService *s = dynamic_cast<CustomService*>(new_state);
    assert(s!=NULL);

    string oldname = obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    string commText = string(
        m_dialog->comment->toPlainText().toUtf8().constData());
    new_state->setComment( commText );
    QMap<QString,QString> platforms = getAllPlatforms();
    QMap<QString,QString>::iterator i;
    for (i=platforms.begin(); i!=platforms.end(); i++)
    {
        QString platform = i.key();
        QString code = allCodes[platform];
        s->setCodeForPlatform( platform.toUtf8().constData(),
                               string(code.toUtf8().constData()));
    }
    QString protocol = m_dialog->protocol->lineEdit()->text();
    s->setProtocol(string(protocol.toUtf8().constData()));
    int af = (m_dialog->ipv6->isChecked()) ? AF_INET6 : AF_INET;
    s->setAddressFamily(af);

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
    
}

void CustomServiceDialog::fillDialogInputFields()
{
    QString npl = platformReverseMap[m_dialog->platform->currentText()];
    showPlatform = npl;
    st->setValue(SETTINGS_PATH_PREFIX"/CustomService/Platform", showPlatform);
    m_dialog->code->setText(allCodes[showPlatform]);
}
