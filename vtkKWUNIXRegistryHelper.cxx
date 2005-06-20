/*=========================================================================

  Module:    $RCSfile: vtkKWUNIXRegistryHelper.cxx,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkKWUNIXRegistryHelper.h"

#include "vtkObjectFactory.h"

#include <vtksys/stl/string>
#include <vtksys/stl/map>

#ifdef VTK_USE_ANSI_STDLIB
#define VTK_IOS_NOCREATE 
#else
#define VTK_IOS_NOCREATE | ios::nocreate
#endif

#define BUFFER_SIZE 8192

vtkStandardNewMacro( vtkKWUNIXRegistryHelper );
vtkCxxRevisionMacro(vtkKWUNIXRegistryHelper, "$Revision: 1.5 $");

//----------------------------------------------------------------------------
//****************************************************************************
class vtkKWUNIXRegistryHelperInternals
{
public:
  typedef vtksys_stl::map<vtksys_stl::string, vtksys_stl::string> StringToStringMap;
  StringToStringMap EntriesMap;
};
//****************************************************************************
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkKWUNIXRegistryHelper::vtkKWUNIXRegistryHelper()
{
  this->Internals = new vtkKWUNIXRegistryHelperInternals;
  this->SubKey  = 0;
}

//----------------------------------------------------------------------------
vtkKWUNIXRegistryHelper::~vtkKWUNIXRegistryHelper()
{
  delete this->Internals;
}


//----------------------------------------------------------------------------
int vtkKWUNIXRegistryHelper::OpenInternal(const char *toplevel,
                                              const char *subkey, 
                                              int readonly)
{  
  int res = 0;
  int cc;
  ostrstream str;
  if ( !getenv("HOME") )
    {
    return 0;
    }
  str << getenv("HOME") << "/." << toplevel << "rc" << ends;
  if ( readonly == vtkKWRegistryHelper::ReadWrite )
    {
    ofstream ofs( str.str(), ios::out|ios::app );
    str.rdbuf()->freeze(0);
    if ( ofs.fail() )
      {
      return 0;
      }
    ofs.close();
    }

  ifstream *ifs = new ifstream(str.str(), ios::in VTK_IOS_NOCREATE);
  str.rdbuf()->freeze(0);
  if ( !ifs )
    {
    return 0;
    }
  if ( ifs->fail())
    {
    delete ifs;
    return 0;
    }

  res = 1;
  char buffer[BUFFER_SIZE];
  while( !ifs->fail() )
    {
    int found = 0;
    ifs->getline(buffer, BUFFER_SIZE);
    if ( ifs->fail() || ifs->eof() )
      {
      break;
      }
    char *line = this->Strip(buffer);
    if ( *line == '#'  || *line == 0 )
      {
      // Comment
      continue;
      }   
    int linelen = static_cast<int>(strlen(line));
    for ( cc = 0; cc < linelen; cc++ )
      {
      if ( line[cc] == '=' )
        {
        char *key = new char[ cc+1 ];
        strncpy( key, line, cc );
        key[cc] = 0;
        char *value = line + cc + 1;
        char *nkey = this->Strip(key);
        char *nvalue = this->Strip(value);
        this->Internals->EntriesMap[nkey] = nvalue;
        this->Empty = 0;
        delete [] key;
        found = 1;      
        break;
        }
      }
    }
  ifs->close();
  this->SetSubKey( subkey );
  delete ifs;
  return res;
}

//----------------------------------------------------------------------------
int vtkKWUNIXRegistryHelper::CloseInternal()
{
  int res = 0;
  if ( !this->Changed )
    {
    this->Internals->EntriesMap.erase(
      this->Internals->EntriesMap.begin(),
      this->Internals->EntriesMap.end());
    this->Empty = 1;
    this->SetSubKey(0);
    return 1;
    }

  ostrstream str;
  if ( !getenv("HOME") )
    {
    return 0;
    }
  str << getenv("HOME") << "/." << this->GetTopLevel() << "rc" << ends;
  ofstream *ofs = new ofstream(str.str(), ios::out);
  str.rdbuf()->freeze(0);
  if ( !ofs )
    {
    return 0;
    }
  if ( ofs->fail())
    {
    delete ofs;
    return 0;
    }
  *ofs << "# This file is automatically generated by the application" << endl
       << "# If you change any lines or add new lines, note that all" << endl
       << "# coments and empty lines will be deleted. Every line has" << endl
       << "# to be in format: " << endl
       << "# key = value" << endl
       << "#" << endl;

  if ( !this->Internals->EntriesMap.empty() )
    {
    vtkKWUNIXRegistryHelperInternals::StringToStringMap::iterator it;
    for ( it = this->Internals->EntriesMap.begin();
      it != this->Internals->EntriesMap.end();
      ++ it )
      {
      *ofs << it->first.c_str() << " = " << it->second.c_str()<< endl;
      }
    }
  this->Internals->EntriesMap.erase(
    this->Internals->EntriesMap.begin(),
    this->Internals->EntriesMap.end());
  ofs->close();
  delete ofs;
  res = 1;
  this->SetSubKey(0);
  this->Empty = 1;
  return res;
}

//----------------------------------------------------------------------------
int vtkKWUNIXRegistryHelper::ReadValueInternal(const char *skey,
                                                   char *value)

{
  int res = 0;
  char *key = this->CreateKey( skey );
  if ( !key )
    {
    return 0;
    }
  vtkKWUNIXRegistryHelperInternals::StringToStringMap::iterator it
    = this->Internals->EntriesMap.find(key);
  if ( it != this->Internals->EntriesMap.end() )
    {
    strcpy(value, it->second.c_str());
    res = 1;
    }
  delete [] key;
  return res;
}

//----------------------------------------------------------------------------
int vtkKWUNIXRegistryHelper::DeleteKeyInternal(const char* vtkNotUsed(key))
{
  int res = 0;
  return res;
}

//----------------------------------------------------------------------------
int vtkKWUNIXRegistryHelper::DeleteValueInternal(const char *skey)
{
  char *key = this->CreateKey( skey );
  if ( !key )
    {
    return 0;
    }
  this->Internals->EntriesMap.erase(key);
  delete [] key;
  return 1;
}

//----------------------------------------------------------------------------
int vtkKWUNIXRegistryHelper::SetValueInternal(const char *skey, 
                                                  const char *value)
{
  char *key = this->CreateKey( skey );
  if ( !key )
    {
    return 0;
    }
  this->Internals->EntriesMap[key] = value;
  delete [] key;
  return 1;
}

//----------------------------------------------------------------------------
char *vtkKWUNIXRegistryHelper::CreateKey( const char *key )
{
  char *newkey;
  if ( !this->SubKey || !key )
    {
    return 0;
    }
  int len = strlen(this->SubKey) + strlen(key) + 1;
  newkey = new char[ len+1 ] ;
  sprintf(newkey, "%s\\%s", this->SubKey, key);
  return newkey;
}

//----------------------------------------------------------------------------
void vtkKWUNIXRegistryHelper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}



