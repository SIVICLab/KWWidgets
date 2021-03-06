/*=========================================================================

  Module:    $RCSfile: vtkKWDialog.cxx,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkKWDialog.h"

#include "vtkObjectFactory.h"

#include "vtkKWApplication.h"
#include "vtkKWTkUtilities.h"
#include "vtkKWFrame.h"
#include "vtkKWInternationalization.h"

#include <vtksys/stl/string>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWDialog );
//vtkCxxRevisionMacro(vtkKWDialog, "$Revision: 1.63 $");

//----------------------------------------------------------------------------
vtkKWDialog::vtkKWDialog()
{
  this->Done = vtkKWDialog::StatusCanceled;
  this->Beep = 0;
  this->Modal = 1;
}

//----------------------------------------------------------------------------
int vtkKWDialog::PreInvoke()
{
  this->Done = vtkKWDialog::StatusActive;

  if (!this->IsCreated())
    {
    this->Create();
    }

  if (!this->IsMapped())
    {
    vtkKWTopLevel *master_top = 
      vtkKWTopLevel::SafeDownCast(this->GetMasterWindow());
    if (master_top && !master_top->IsMapped())
      {
      master_top->Display();
      }
    this->Display();
    }

  if (this->Beep)
    {
    vtkKWTkUtilities::Bell(this->GetApplication());
    }

  return 1;
}

//----------------------------------------------------------------------------
void vtkKWDialog::PostInvoke()
{
  if (this->IsMapped())
    {
    // OK(), Cancel() should take care of that, but who knows, if another
    // button was user-created and forgot to Withdraw(), or simply the
    // dialog was closed programmatically by a subclass by setting
    // this->Done and breaking the event-loop in Invoke()

    this->Withdraw(); 
    }
}

//----------------------------------------------------------------------------
int vtkKWDialog::IsUserDoneWithDialog()
{
  return this->Done != vtkKWDialog::StatusActive;
}

//----------------------------------------------------------------------------
int vtkKWDialog::Invoke()
{
  if (!this->PreInvoke())
    {
    return 0;
    }

  this->GetApplication()->RegisterDialogUp(this);

  // Wait for the end

  while (!this->IsUserDoneWithDialog())
    {
    Tcl_DoOneEvent(0);    
    }

  this->GetApplication()->UnRegisterDialogUp(this);

  this->PostInvoke();

  return (this->Done == vtkKWDialog::StatusCanceled ? 0 : 1);
}

//----------------------------------------------------------------------------
void vtkKWDialog::Display()
{
  if (!this->IsCreated())
    {
    this->Create();
    }

  this->Done = vtkKWDialog::StatusActive;
  this->Superclass::Display();
}

//----------------------------------------------------------------------------
void vtkKWDialog::Cancel()
{
  this->Done = vtkKWDialog::StatusCanceled;  
  this->Withdraw();
}

//----------------------------------------------------------------------------
void vtkKWDialog::OK()
{
  this->Done = vtkKWDialog::StatusOK;  
  this->Withdraw();
}

//----------------------------------------------------------------------------
void vtkKWDialog::CreateWidget()
{
  // Check if already created

  if (this->IsCreated())
    {
    vtkErrorMacro(<< this->GetClassName() << " already created");
    return;
    }

  // Call the superclass to create the whole widget

  this->Superclass::CreateWidget();

  this->SetDeleteWindowProtocolCommand(this, "Cancel");


  // Tcl Interactor

  const char *context = k_("Dialog");

  if (!this->GetApplication()->GetReleaseMode())
    {
    this->SetKeyBinding(
      "<Control-t>", this, "DisplayTclInteractor",
      context, ks_("Main Window|Display Tcl interactor"));
    }

  // Error log

  vtksys_stl::string cmd;
  cmd = "DisplayLogDialog {";
  cmd += this->GetTclName();
  cmd += "}";
  this->SetKeyBinding(
    "<Control-Alt-e>", this->GetApplication(), cmd.c_str(),
    context, ks_("Main Window|Display error log"));
}

//----------------------------------------------------------------------------
void vtkKWDialog::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Beep: " << this->GetBeep() << endl;
}

