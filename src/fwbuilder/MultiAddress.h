/* 

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@fwbuilder.org>

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


#ifndef __GEN_MULTIADDRESS_HH_FLAG__
#define __GEN_MULTIADDRESS_HH_FLAG__

#include <fwbuilder/FWObject.h>
#include <fwbuilder/Address.h>
#include <fwbuilder/ObjectGroup.h>
#include <vector>



namespace libfwbuilder
{

class MultiAddress : public ObjectGroup
{
    private:
      
    public:

    DECLARE_FWOBJECT_SUBTYPE(MultiAddress);

    MultiAddress();
    MultiAddress(const FWObject *root,bool prepopulate);
    virtual ~MultiAddress();

    virtual std::string getSourceName();
    virtual void setSourceName(const std::string& source_name);
    virtual void loadFromSource() throw(FWException);

    bool isCompileTime() const;
    bool isRunTime() const;
    void setCompileTime(const bool b);
    void setRunTime(const bool b); 

    virtual bool validateChild(FWObject *o);
};

/*
 * compilers assume that object that appear in Src, Dst, OSrc, ODst
 * are inherited from class Address and use this in many
 * places. MultiAddress is derived from ObjectGroup to simplify
 * processing in compile-time mode, which creates lots of problems
 * with it when it is configured in run-time mode. To simplify things,
 * we'll use class MultiAddressRunTime which is derived from
 * Address. We'll replace MultiAddress objects with run-time mode with
 * objects of this class in a special rule processor
 *
 * Objects of this class are never stored in the data file and do not
 * appear in DTD; they are only used in compilers.
 */
class MultiAddressRunTime : public Address
{
    std::string subst_type_name;
    std::string source_name;
    bool        run_time;

public:

    DECLARE_FWOBJECT_SUBTYPE(MultiAddressRunTime);

    MultiAddressRunTime();
    MultiAddressRunTime(MultiAddress *maddr);

    std::string getSourceName() const { return source_name; }
    std::string getSubstitutionTypeName() const { return subst_type_name; }

    bool isCompileTime() const { return !run_time; }
    bool isRunTime() const { return run_time; }
};

}


#endif

