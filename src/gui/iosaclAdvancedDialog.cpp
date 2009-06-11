/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

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
#include "utils_no_qt.h"

#include "iosaclAdvancedDialog.h"
#include "SimpleTextEditor.h"
#include "FWWindow.h"
#include "FWBSettings.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/XMLTools.h"

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qstackedwidget.h>
#include <qregexp.h>
#include <qtextedit.h>
#include <qtabwidget.h>
#include <qlistwidget.h>
#include <qlabel.h>
#include <qprocess.h>
#include <qfile.h>

#include <iostream>
#include <sstream>

#include <libxml/xmlmemory.h>

using namespace std;
using namespace libfwbuilder;

iosaclAdvancedDialog::~iosaclAdvancedDialog()
{
    delete m_dialog;
}

iosaclAdvancedDialog::iosaclAdvancedDialog(QWidget *parent,FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::iosaclAdvancedDialog_q;
    m_dialog->setupUi(this);

    obj=o;

    Firewall  *fw=Firewall::cast(obj);
    FWOptions *fwopt=fw->getOptionsObject();
    string compiler=fwopt->getStr("compiler");
    if (compiler=="")
    {
        compiler=Resources::platform_res[fw->getStr("platform")]->getCompiler();
    }
/*
 * On Unix compilers are installed in the standard place and are
 * accessible via PATH. On Windows and Mac they get installed in
 * unpredictable directories and need to be found
 *
 * first, check if user specified an absolute path for the compiler,
 * then check  if compiler is registsred in preferences, and if not,
 * look for it in appRootDir and if it is not there, rely on PATH
 */
#if defined(Q_OS_WIN32) ||  defined(Q_OS_MACX)

    if ( ! QFile::exists( compiler.c_str() ) )
    {
        string ts = string("Compilers/")+compiler;
        QString cmppath = st->getStr( ts.c_str() );
        if (!cmppath.isEmpty()) compiler = cmppath.toLatin1().constData();
        else
        {
            /* try to find compiler in appRootDir. */
            string ts =  getPathToBinary(compiler);
            if ( QFile::exists( ts.c_str() ) )
                compiler = ts;
        }
    }
#endif

    string vers="version_"+obj->getStr("version");
    string platform = obj->getStr("platform");   // should be 'iosacl'

    QString      s;
    QStringList  logLevels;
    QStringList  logLevelMapping;
    logLevelMapping.push_back("");
    logLevelMapping.push_back("");

/* filling pop-down menu and pushing the same strings to the mapping
 * list at the same time so we could use translation
 */
    s=QObject::tr("0 - System Unusable");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("0");

    s=QObject::tr("1 - Take Immediate Action");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("1");

    s=QObject::tr("2 - Critical Condition");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("2");

    s=QObject::tr("3 - Error Message");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("3");

    s=QObject::tr("4 - Warning Message");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("4");

    s=QObject::tr("5 - Normal but significant condition");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("5");

    s=QObject::tr("6 - Informational");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("6");

    s=QObject::tr("7 - Debug Message");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("7");

/* do not need to translate syslog facilities, but will use the same
 * method just in case */

    QStringList syslogFacilities;
    QStringList syslogFacilityMapping;
    syslogFacilities.push_back("");
    syslogFacilityMapping.push_back("");
    syslogFacilityMapping.push_back("");

    syslogFacilities.push_back("LOCAL0");
    syslogFacilityMapping.push_back("LOCAL0");
    syslogFacilityMapping.push_back("16");

    syslogFacilities.push_back("LOCAL1");
    syslogFacilityMapping.push_back("LOCAL1");
    syslogFacilityMapping.push_back("17");

    syslogFacilities.push_back("LOCAL2");
    syslogFacilityMapping.push_back("LOCAL2");
    syslogFacilityMapping.push_back("18");

    syslogFacilities.push_back("LOCAL3");
    syslogFacilityMapping.push_back("LOCAL3");
    syslogFacilityMapping.push_back("19");

    syslogFacilities.push_back("LOCAL4");
    syslogFacilityMapping.push_back("LOCAL4");
    syslogFacilityMapping.push_back("20");

    syslogFacilities.push_back("LOCAL5");
    syslogFacilityMapping.push_back("LOCAL5");
    syslogFacilityMapping.push_back("21");

    syslogFacilities.push_back("LOCAL6");
    syslogFacilityMapping.push_back("LOCAL6");
    syslogFacilityMapping.push_back("22");

    syslogFacilities.push_back("LOCAL7");
    syslogFacilityMapping.push_back("LOCAL7");
    syslogFacilityMapping.push_back("23");

    FWOptions *fwoptions=(Firewall::cast(obj))->getOptionsObject();
    assert(fwoptions!=NULL);

    bool f1=fwoptions->getBool("iosacl_acl_basic");
    bool f2=fwoptions->getBool("iosacl_acl_no_clear");
    bool f3=fwoptions->getBool("iosacl_acl_substitution");
    bool f4=fwoptions->getBool("iosacl_add_clear_statements");

    /*
     * If none of the new iosacl_acl_* options is set and old iosacl_add_clear_statements
     * option is true, set iosacl_acl_basic to true.
     *
     * If old option iosacl_add_clear_statements iss false, set
     * iosacl_acl_no_clear to true
     */
    if (!f1 && !f2 && !f3)
    {
        if ( f4 ) fwoptions->setBool("iosacl_acl_basic",true);
        else fwoptions->setBool("iosacl_acl_no_clear",true);
    }

    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=NULL);

    data.registerOption(m_dialog->ipv4before_2,    fwoptions,
                        "ipv4_6_order",
                        QStringList() <<  "IPv4 before IPv6"
                        << "ipv4_first"
                        << "IPv6 before IPv4"
                        << "ipv6_first"
    );
/* Page "Compiler Options" */

    data.registerOption( m_dialog->outputFileName, fwoptions,
                         "output_file"  );

    data.registerOption( m_dialog->iosacl_acl_basic, fwoptions,
                         "iosacl_acl_basic" );

/*
    data.registerOption( m_dialog->iosacl_acl_alwaysNew, fwoptions,
                         "iosacl_acl_always_new" );
*/

    data.registerOption( m_dialog->iosacl_acl_no_clear, fwoptions,
                         "iosacl_acl_no_clear" );

    data.registerOption( m_dialog->iosacl_acl_substitution, fwoptions,
                         "iosacl_acl_substitution" );

    data.registerOption( m_dialog->iosacl_acl_temp_addr, fwoptions,
                         "iosacl_acl_temp_addr" );

    data.registerOption( m_dialog->iosacl_include_comments, fwoptions,
                         "iosacl_include_comments" );

    data.registerOption( m_dialog->iosacl_use_acl_remarks, fwoptions,
                         "iosacl_use_acl_remarks" );

    data.registerOption( m_dialog->iosacl_regroup_commands, fwoptions,
                         "iosacl_regroup_commands" );

    data.registerOption( m_dialog->iosacl_check_shadowing, fwoptions,
                         "check_shading"  );

    data.registerOption( m_dialog->iosacl_ignore_empty_groups, fwoptions,
                         "ignore_empty_groups" );

    data.registerOption( m_dialog->mgmt_ssh, fwoptions,  "mgmt_ssh"  );
    data.registerOption( m_dialog->mgmt_addr, fwoptions,  "mgmt_addr" );

/* page Installer */

    data.registerOption( m_dialog->user, fwoptions, "admUser");
    data.registerOption( m_dialog->altAddress, fwoptions, "altAddress");
    data.registerOption( m_dialog->sshArgs, fwoptions, "sshArgs");
    data.registerOption( m_dialog->scpArgs, fwoptions, "scpArgs");

    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();

    m_dialog->installScript->setText(pis->getCommand().c_str() );
    m_dialog->installScriptArgs->setText( pis->getArguments().c_str() );


/* page "Prolog/Epilog" */
    data.registerOption( m_dialog->iosacl_prolog_script, fwoptions,
                         "iosacl_prolog_script"  );

    data.registerOption( m_dialog->iosacl_epilog_script, fwoptions,
                         "iosacl_epilog_script"  );

/* page Logging */

    data.registerOption(m_dialog->generate_logging_commands, fwoptions,
                        "iosacl_generate_logging_commands");

    data.registerOption(m_dialog->syslog_host, fwoptions, "iosacl_syslog_host");

    m_dialog->syslog_facility->clear();
    m_dialog->syslog_facility->addItems( syslogFacilities );
    data.registerOption( m_dialog->syslog_facility,      fwoptions,
                         "iosacl_syslog_facility", syslogFacilityMapping);

    m_dialog->logging_trap_level->clear();
    m_dialog->logging_trap_level->addItems(logLevels);

    data.registerOption( m_dialog->logging_trap_level,   fwoptions,
                         "iosacl_logging_trap_level", logLevelMapping);

    data.registerOption(m_dialog->logging_timestamp, fwoptions,
                        "iosacl_logging_timestamp");

    data.registerOption(m_dialog->logging_buffered, fwoptions,
                        "iosacl_logging_buffered");

    m_dialog->logging_buffered_level->clear();
    m_dialog->logging_buffered_level->addItems(logLevels);
    data.registerOption( m_dialog->logging_buffered_level, fwoptions,
                         "iosacl_logging_buffered_level", logLevelMapping);

    data.registerOption(m_dialog->logging_console, fwoptions,
                        "iosacl_logging_console");

    m_dialog->logging_console_level->clear();
    m_dialog->logging_console_level->addItems(logLevels);
    data.registerOption( m_dialog->logging_console_level,fwoptions,
                         "iosacl_logging_console_level", logLevelMapping);

    data.loadAll();
    scriptACLModeChanged();
    toggleGenerateLogging();
}

/*
 * store all data in the object
 */
void iosaclAdvancedDialog::accept()
{
    FWOptions *options=(Firewall::cast(obj))->getOptionsObject();
    assert(options!=NULL);

    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=NULL);

    data.saveAll();

//    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();
//    pis->setCommand( installScript->text() );
//    pis->setArguments( installScriptArgs->text() );

    const InetAddr *mgmt_addr = Firewall::cast(obj)->getManagementAddress();
    if (mgmt_addr)
        mgmt->setAddress(*mgmt_addr);

    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();
    pis->setCommand( m_dialog->installScript->text().toLatin1().constData() );
    pis->setArguments( m_dialog->installScriptArgs->text().toLatin1().constData() );

    mw->updateLastModifiedTimestampForAllFirewalls(obj);
    QDialog::accept();
}

void iosaclAdvancedDialog::reject()
{
    QDialog::reject();
}

void iosaclAdvancedDialog::editProlog()
{
    SimpleTextEditor edt(this,
                         m_dialog->iosacl_prolog_script->toPlainText(),
                          true, tr( "Script Editor" ) );
    if ( edt.exec() == QDialog::Accepted )
        m_dialog->iosacl_prolog_script->setText( edt.text() );
}

void iosaclAdvancedDialog::editEpilog()
{
    SimpleTextEditor edt(this,
                         m_dialog->iosacl_epilog_script->toPlainText(),
                          true, tr( "Script Editor" ) );
    if ( edt.exec() == QDialog::Accepted )
        m_dialog->iosacl_epilog_script->setText( edt.text() );
}

void iosaclAdvancedDialog::scriptACLModeChanged()
{
    m_dialog->iosacl_acl_temp_lbl->setEnabled(
        m_dialog->iosacl_acl_substitution->isChecked());
    m_dialog->iosacl_acl_temp_addr->setEnabled(
        m_dialog->iosacl_acl_substitution->isChecked());
}

void iosaclAdvancedDialog::toggleGenerateLogging()
{
    m_dialog->syslog_controls->setEnabled(
        m_dialog->generate_logging_commands->isChecked());
    m_dialog->other_logging_controls->setEnabled(
        m_dialog->generate_logging_commands->isChecked());
}



