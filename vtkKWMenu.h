/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkKWMenu.h,v $
  Language:  C++
  Date:      $Date: 2000-01-13 22:56:28 $
  Version:   $Revision: 1.1 $

Copyright (c) 1998-1999 Kitware Inc. 469 Clifton Corporate Parkway,
Clifton Park, NY, 12065, USA.

All rights reserved. No part of this software may be reproduced, distributed,
or modified, in any form or by any means, without permission in writing from
Kitware Inc.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================*/
// .NAME vtkKWMenu
// .SECTION Description
// This class is the Menu abstraction for the
// Kitware toolkit. It provides a c++ interface to
// the TK menu widgets used by the Kitware toolkit.

#ifndef __vtkKWMenu_h
#define __vtkKWMenu_h

#include "vtkKWWidget.h"


class VTK_EXPORT vtkKWMenu : public vtkKWWidget
{
public:
  vtkKWMenu();
  ~vtkKWMenu();
  static vtkKWMenu* New();
  const char *GetClassName() {return "vtkKWMenu";};
  
  void Create(vtkKWApplication* app, const char* args);
  
  // Description: 
  // Append a separator to the menu.
  void AddSeparator();
  
  // Description: 
  // Append a sub menu to the current menu.
  void AddCascade(const char* label, vtkKWMenu*, int underline );
  
  // Description: 
  // Append a CheckButton menu item to the current menu.
  void AddCheckButton(const char* label, vtkKWObject* Object, 
		      const char* MethodAndArgString );

  // Description: 
  // Append a standard menu item and command to the current menu.
  void AddCommand(const char* label, vtkKWObject* Object,
		  const char* MethodAndArgString );

  // Description: 
  // Append a radio menu item and command to the current menu.
  // The radio group is specified by the buttonVar value.
  void AddRadioButton(int value, const char* label, const char* buttonVar, 
		      vtkKWObject* Called, 
		      const char* MethodAndArgString);

  // Description:
  // Same as add commands, but insert at a given integer position.
  void InsertSeparator(int position);
  
  void InsertCascade(int position, const char* label,  vtkKWMenu*, int underline  );
  
  
  void InsertCheckButton(int position, 
			 const char* label, vtkKWObject* Object, 
			 const char* MethodAndArgString );
  
  void InsertCommand(int position, const char* label, vtkKWObject* Object,
		     const char* MethodAndArgString );
  
  char* CreateRadioButtonVar(vtkKWObject* Object, const char* varname);
  
  void InsertRadioButton(int value, int position, const char* label, const char* buttonVar, 
			 vtkKWObject* Called, 
			 const char* MethodAndArgString);
  void Invoke(int position);
  void DeleteMenuItem(int position);
  void DeleteMenuItem(const char* menuname);
  int GetIndex(const char* menuname);
  
protected:
  void AddGeneric(const char* addtype, const char* label, vtkKWObject* Object,
		  const char* MethodAndArgString, const char* extra);
  void InsertGeneric(int position, const char* addtype, const char* label, 
		     vtkKWObject* Object,
		     const char* MethodAndArgString, const char* extra);
  
};


#endif


