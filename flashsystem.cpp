/**************************************************************************
    Lightspark, a free flash player implementation

    Copyright (C) 2009,2010  Alessandro Pignotti (a.pignotti@sssup.it)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include "flashsystem.h"
#include "abc.h"
#include "class.h"

using namespace lightspark;

REGISTER_CLASS_NAME(ApplicationDomain);
REGISTER_CLASS_NAME(Capabilities);
REGISTER_CLASS_NAME(Security);

void Capabilities::sinit(Class_base* c)
{
	c->setConstructor(new Function(_constructor));
	c->setGetterByQName("language","",new Function(_getLanguage));
	c->setVariableByQName("version","",Class<ASString>::getInstanceS("UNIX 10,0,0,0")->obj);
}

ASFUNCTIONBODY(Capabilities,_constructor)
{
	obj->setVariableByQName("playerType","",Class<ASString>::getInstanceS("AVMPlus")->obj);
	return NULL;
}

ASFUNCTIONBODY(Capabilities,_getLanguage)
{
	return Class<ASString>::getInstanceS("en")->obj;
}

void ApplicationDomain::sinit(Class_base* c)
{
	c->setConstructor(new Function(_constructor));
	c->setGetterByQName("currentDomain","",new Function(_getCurrentDomain));
}

void ApplicationDomain::buildTraits(ASObject* o)
{
	o->setVariableByQName("hasDefinition","",new Function(hasDefinition));
	o->setVariableByQName("getDefinition","",new Function(getDefinition));
}

ASFUNCTIONBODY(ApplicationDomain,_constructor)
{
	return NULL;
}

ASFUNCTIONBODY(ApplicationDomain,_getCurrentDomain)
{
	return Class<ApplicationDomain>::getInstanceS()->obj;
}

ASFUNCTIONBODY(ApplicationDomain,hasDefinition)
{
	assert(args && argslen==1);
	const tiny_string& tmp=args[0]->toString();
	tiny_string name,ns;

	stringToQName(tmp,name,ns);

	LOG(LOG_CALLS,"Looking for definition of " << ns << " :: " << name);
	objAndLevel o=getGlobal()->getVariableByQName(name,ns);
	if(o.obj==NULL)
		return abstract_b(false);
	else
	{
		//Check if the object has to be defined
		if(o.obj->getObjectType()==T_DEFINABLE)
		{
			LOG(LOG_CALLS,"We got an object not yet valid");
			Definable* d=static_cast<Definable*>(o.obj);
			d->define(getGlobal());
			o=getGlobal()->getVariableByQName(name,ns);
		}

		if(o.obj->getObjectType()!=T_CLASS)
			return abstract_b(false);

		LOG(LOG_CALLS,"Found definition for " << ns << " :: " << name);
		return abstract_b(true);
	}
}

ASFUNCTIONBODY(ApplicationDomain,getDefinition)
{
	assert(args && argslen==1);
	const tiny_string& tmp=args[0]->toString();
	tiny_string name,ns;

	stringToQName(tmp,name,ns);

	LOG(LOG_CALLS,"Looking for definition of " << ns << " :: " << name);
	objAndLevel o=getGlobal()->getVariableByQName(name,ns);
	assert(o.obj);

	//Check if the object has to be defined
	if(o.obj->getObjectType()==T_DEFINABLE)
	{
		abort();
/*		LOG(LOG_CALLS,"We got an object not yet valid");
		Definable* d=static_cast<Definable*>(o.obj);
		d->define(sys->currentVm->last_context->Global);
		o=sys->currentVm->last_context->Global->getVariableByQName(name,ns,owner);*/
	}

	assert(o.obj->getObjectType()==T_CLASS);

	LOG(LOG_CALLS,"Getting definition for " << ns << " :: " << name);
	return o.obj;
}

void Security::sinit(Class_base* c)
{
	c->setConstructor(NULL);
	c->setVariableByQName("allowDomain","",new Function(undefinedFunction));
}
