/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkKWScale.cxx,v $
  Language:  C++
  Date:      $Date: 2002-10-23 21:31:28 $
  Version:   $Revision: 1.34 $

Copyright (c) 2000-2001 Kitware Inc. 469 Clifton Corporate Parkway,
Clifton Park, NY, 12065, USA.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither the name of Kitware nor the names of any contributors may be used
   to endorse or promote products derived from this software without specific 
   prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "vtkKWScale.h"

#include "vtkKWEntry.h"
#include "vtkKWApplication.h"
#include "vtkKWPushButton.h"
#include "vtkKWTkUtilities.h"
#include "vtkObjectFactory.h"


// ---------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWScale );
vtkCxxRevisionMacro(vtkKWScale, "$Revision: 1.34 $");

int vtkKWScaleCommand(ClientData cd, Tcl_Interp *interp,
                      int argc, char *argv[]);

/* 
 * This part was generated by ImageConvert from image:
 *    arrow.png (zlib, base64)
 */
#define image_arrow_width         4
#define image_arrow_height        7
#define image_arrow_pixel_size    4
#define image_arrow_buffer_length 40

static unsigned char image_arrow[] = 
  "eNpjYGD4z4AK/jOgiv1HE/uPB+PSDwcAlQUP8Q==";

// ---------------------------------------------------------------------------
vtkKWScale::vtkKWScale()
{
  this->CommandFunction = vtkKWScaleCommand;

  this->Command      = NULL;
  this->StartCommand = NULL;
  this->EndCommand   = NULL;
  this->EntryCommand = NULL;

  this->Value      = 0;
  this->Resolution = 1;
  this->EntryResolution = 2;
  this->Range[0]   = 0;
  this->Range[1]   = 1;  

  this->Scale           = vtkKWWidget::New();
  this->Entry           = NULL;
  this->Label           = NULL;
  this->TopLevel        = NULL;
  this->PopupPushButton = NULL;

  this->DisplayEntryAndLabelOnTop = 1;
  this->PopupScale = 0;
  this->ExpandEntry = 0;
}

// ---------------------------------------------------------------------------
vtkKWScale::~vtkKWScale()
{
  if (this->Command)
    {
    delete [] this->Command;
    this->Command = NULL;
    }

  if (this->StartCommand)
    {
    delete [] this->StartCommand;
    this->StartCommand = NULL;
    }

  if (this->EndCommand)
    {
    delete [] this->EndCommand;
    this->EndCommand = NULL;
    }

  if (this->EntryCommand)
    {
    delete [] this->EntryCommand;
    this->EntryCommand = NULL;
    }

  if (this->Scale)
    {
    this->Scale->Delete();
    this->Scale = NULL;
    }

  if (this->Entry)
    {
    this->Entry->Delete();
    this->Entry = NULL;
    }

  if (this->Label)
    {
    this->Label->Delete();
    this->Label = NULL;
    }

  if (this->TopLevel)
    {
    this->TopLevel->Delete();
    this->TopLevel = NULL;
    }

  if (this->PopupPushButton)
    {
    this->PopupPushButton->Delete();
    this->PopupPushButton = NULL;
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::Create(vtkKWApplication *app, const char *args)
{
  const char *wname;

  // Must set the application

  if (this->Application)
    {
    vtkErrorMacro("Scale already created");
    return;
    }

  this->SetApplication(app);

  // Create the frame that will embend all widgets

  wname = this->GetWidgetName();
  this->Script("frame %s -bd 0", wname);

  // If we need the scale to popup, create the toplevel and the pushbutton

  if (this->PopupScale)
    {
    this->TopLevel = vtkKWWidget::New();
    this->TopLevel->Create(
      app, "toplevel", "-bg black -bd 2 -relief flat");
    this->Script("wm overrideredirect %s 1", 
                 this->TopLevel->GetWidgetName());
    this->Script("wm withdraw %s", 
                 this->TopLevel->GetWidgetName());

    this->PopupPushButton = vtkKWPushButton::New();
    this->PopupPushButton->SetParent(this);
    this->PopupPushButton->Create(app, "-padx 0 -pady 0");

    ostrstream arrow;
    arrow << this->PopupPushButton->GetWidgetName() << ".arrowimg" << ends;
    if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                       arrow.str(), 
                                       image_arrow, 
                                       image_arrow_width, 
                                       image_arrow_height, 
                                       image_arrow_pixel_size,
                                       image_arrow_buffer_length,
                                       this->PopupPushButton->GetWidgetName()))
      {
      vtkWarningMacro(<< "Error creating photo (arrow)");
      this->PopupPushButton->SetLabel(">");
      }
    else
      {
      this->Script("%s configure -image %s", 
                   this->PopupPushButton->GetWidgetName(), arrow.str());
      }
    arrow.rdbuf()->freeze(0);

    this->Scale->SetParent(this->TopLevel);
    }
  else
    {
    this->Scale->SetParent(this);
    }

  // Create the scale

  this->Scale->Create(
    app, "scale", "-orient horizontal -showvalue no -bd 2");

  this->Script(
    "%s configure -resolution %f -highlightthickness 0 -from %f -to %f %s",
    this->Scale->GetWidgetName(), 
    this->Resolution,
    this->Range[0], this->Range[1],
    (args ? args : ""));

  this->PackWidget();
  this->Bind();
}

// ---------------------------------------------------------------------------
void vtkKWScale::DisplayEntry()
{
  if (this->Entry && this->Entry->IsCreated())
    {
    return;
    }

  this->Entry = vtkKWEntry::New();
  this->Entry->SetParent(this);
  this->Entry->Create(this->Application, "-width 10");
  this->UpdateEntryResolution();
  this->Entry->SetValue(this->GetValue(), this->EntryResolution);

  this->PackWidget();
  this->Bind();
}

// ---------------------------------------------------------------------------
void vtkKWScale::DisplayLabel(const char *label)
{
  if (this->Label && this->Label->IsCreated())
    {
    this->Script("%s configure -text {%s}",
                 this->Label->GetWidgetName(), label);
    return;
    }

  this->Label = vtkKWWidget::New();
  this->Label->SetParent(this);
  ostrstream temp;
  temp << "-text {" << label << "}" << ends;
  this->Label->Create(this->Application, "label", temp.str());
  temp.rdbuf()->freeze(0);

  this->PackWidget();
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetDisplayEntryAndLabelOnTop(int arg)
{
  if (this->DisplayEntryAndLabelOnTop == arg)
    {
    return;
    }

  this->DisplayEntryAndLabelOnTop = arg;
  this->Modified();

  this->PackWidget();
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetExpandEntry(int arg)
{
  if (this->ExpandEntry == arg)
    {
    return;
    }

  this->ExpandEntry = arg;
  this->Modified();

  this->PackWidget();
}

// ---------------------------------------------------------------------------
void vtkKWScale::PackWidget()
{
  if (this->Scale && this->Scale->IsCreated())
    {
    if (this->DisplayEntryAndLabelOnTop && !this->PopupScale)
      {
      this->Script("pack %s -side bottom -fill x -expand yes -pady 0 -padx 0",
                   this->Scale->GetWidgetName());
      }
    else
      {
      this->Script("pack %s -side left -fill x -expand yes -pady 0 -padx 0",
                   this->Scale->GetWidgetName());
      }
    }

  if (this->Label && this->Label->IsCreated())
    {
    this->Script("pack forget %s", this->Label->GetWidgetName());
    if (this->DisplayEntryAndLabelOnTop || this->PopupScale)
      {
      this->Script("pack %s -side left -padx 0 -fill y", 
                   this->Label->GetWidgetName());
      }
    else
      {
      this->Script("pack %s -side left -padx 0 -fill y -before %s", 
                   this->Label->GetWidgetName(), 
                   this->Scale->GetWidgetName());
      }
    }

  if (this->Entry && this->Entry->IsCreated())
    {
    this->Script("pack forget %s", this->Entry->GetWidgetName());
    if (this->PopupScale)
      {
      this->Script("pack %s -side left -padx 0 %s", 
                   this->Entry->GetWidgetName(),
                   (this->ExpandEntry ? "-fill both -expand t" : "-fill y"));
      }
    else
      {
      if (this->DisplayEntryAndLabelOnTop)
        {
        this->Script("pack %s -side right -padx 0 -fill y", 
                     this->Entry->GetWidgetName());
        }
      else
        {
        this->Script("pack %s -side right -padx 0 -fill y -after %s", 
                     this->Entry->GetWidgetName(), 
                     this->Scale->GetWidgetName());
        }
      }
    }

  if (this->PopupScale && 
      this->PopupPushButton && this->PopupPushButton->IsCreated())
    {
    this->Script("pack forget %s", this->PopupPushButton->GetWidgetName());
    if (this->Label || this->Entry)
      {
      this->Script("pack %s -side left -padx 1 -fill y -ipadx 1 -after %s", 
                   this->PopupPushButton->GetWidgetName(),
                   (this->Entry ? this->Entry->GetWidgetName() 
                                : this->Label->GetWidgetName()));
      }
    else
      {
      this->Script("pack %s -side left -padx 1 -fill y -ipadx 1", 
                   this->PopupPushButton->GetWidgetName());
      }
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::Bind()
{
  if (this->Scale && this->Scale->IsCreated())
    {
    this->Script("bind %s <ButtonPress> {%s InvokeStartCommand}",
                 this->Scale->GetWidgetName(), this->GetTclName());
    this->Script("bind %s <ButtonRelease> {%s InvokeEndCommand}",
                 this->Scale->GetWidgetName(), this->GetTclName());

    // If in popup mode, leaving the toplevel will withdraw it, unless 
    // the user is interacting with the scale.

    if (this->PopupScale &&
        this->TopLevel && this->TopLevel->IsCreated())
      {
      this->Script("bind %s <Leave> {%s WithdrawPopupScaleCallback}",
                   this->TopLevel->GetWidgetName(), this->GetTclName());
      this->Script("bind %s <ButtonPress> {+bind %s <Leave> {}}",
                   this->Scale->GetWidgetName(), this->TopLevel->GetWidgetName());
      this->Script("bind %s <ButtonRelease> "
                   "{+bind %s <Leave> {%s WithdrawPopupScaleCallback}}",
                   this->Scale->GetWidgetName(), 
                   this->TopLevel->GetWidgetName(), 
                   this->GetTclName());
      }
    
    this->Scale->SetCommand(this, "ScaleValueChanged");
    }

  if (this->Entry && this->Entry->IsCreated())
    {
    this->Script("bind %s <Return> {%s EntryValueChanged}",
                 this->Entry->GetWidgetName(), this->GetTclName());
    this->Script("bind %s <FocusOut> {%s EntryValueChanged}",
                 this->Entry->GetWidgetName(), this->GetTclName());
    }

  if (this->PopupScale && 
      this->PopupPushButton && this->PopupPushButton->IsCreated())
    {
    this->Script("bind %s <ButtonPress> {%s DisplayPopupScaleCallback}",
                 this->PopupPushButton->GetWidgetName(), this->GetTclName());
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::UnBind()
{
  if (this->Scale && this->Scale->IsCreated())
    {
    this->Script("bind %s <ButtonPress>", 
                 this->Scale->GetWidgetName());
    this->Script("bind %s <ButtonRelease>", 
                 this->Scale->GetWidgetName());
    this->Script("%s configure -command {}",
                 this->Scale->GetWidgetName());
    }

  if (this->Entry && this->Entry->IsCreated())
    {
    this->Script("bind %s <Return>",
                 this->Entry->GetWidgetName());
    this->Script("bind %s <FocusOut>",
                 this->Entry->GetWidgetName());
    }

  if (this->PopupScale && 
      this->PopupPushButton && this->PopupPushButton->IsCreated())
    {
    this->Script("bind %s <ButtonPress>",
                 this->PopupPushButton->GetWidgetName());
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::DisplayPopupScaleCallback()
{
  if (!this->PopupScale ||
      !this->PopupPushButton || !this->PopupPushButton->IsCreated() ||
      !this->TopLevel || !this->TopLevel->IsCreated() ||
      !this->Scale || !this->Scale->IsCreated())
    {
    return;
    }

  // Get the position of the mouse, the position and size of the push button,
  // the size of the scale.

  this->Script("concat "
               " [winfo pointerx %s] [winfo pointery %s]" 
               " [winfo rooty %s] [winfo height %s]"
               " [%s coords %f]"
               " [winfo x %s] [winfo y %s]",
               this->GetWidgetName(), 
               this->GetWidgetName(),
               this->PopupPushButton->GetWidgetName(), 
               this->PopupPushButton->GetWidgetName(),
               this->Scale->GetWidgetName(), this->Value,
               this->Scale->GetWidgetName(), this->Scale->GetWidgetName());
  
  int x, y, py, ph, scx, scy, sx, sy;
  sscanf(this->Application->GetMainInterp()->result, 
         "%d %d %d %d %d %d %d %d", 
         &x, &y, &py, &ph, &scx, &scy, &sx, &sy);
 
  // Place the scale so that the slider is coincident with the x mouse position
  // and just below the push button
  
  x -= sx + scx;

  if (py <= y && y <= (py + ph -1))
    {
    y = py + ph - 3;
    }
  else
    {
    y -= sy + scy;
    }

  this->Script("wm geometry %s +%d+%d",
               this->TopLevel->GetWidgetName(), x, y);
  this->Script("update");
  this->Script("wm deiconify %s", 
               this->TopLevel->GetWidgetName());
  this->Script("raise %s", 
               this->TopLevel->GetWidgetName());
}

// ---------------------------------------------------------------------------
void vtkKWScale::WithdrawPopupScaleCallback()
{
  if (!this->PopupScale ||
      !this->TopLevel || !this->TopLevel->IsCreated() ||
      !this->Scale || !this->Scale->IsCreated())
    {
    return;
    }

  // Withdraw the popup

  this->Script("wm withdraw %s",
               this->TopLevel->GetWidgetName());
}

// ---------------------------------------------------------------------------
void vtkKWScale::UpdateEntryResolution()
{
  if (fabs(this->Resolution) >= 1.0)
    {
    this->EntryResolution = 0;
    }
  else if (this->IsCreated())
    {
    // Trick here: use the 'expr' Tcl command to display the shortest
    // representation of the floating point number this->Resolution.
    // sprintf would be of no help here.
    const char *res = this->Script("expr %f", fabs(this->Resolution));
    char *pos = strchr(res, '.');
    if (pos)
      {
      this->EntryResolution = strlen(res) - (pos - res) - 1;
      }
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetResolution(float r)
{
  if (this->Resolution == r)
    {
    return;
    }

  this->Resolution = r;
  this->Modified();

  if (this->Scale && this->Scale->IsCreated())
    {
    this->Script("%s configure -resolution %f",
                 this->Scale->GetWidgetName(), r);
    }

  if (this->Entry && this->Entry->IsCreated())
    {
    this->UpdateEntryResolution();
    this->Entry->SetValue(this->Value, this->EntryResolution);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetValue(float num)
{
  if (this->Value == num)
    {
    return;
    }

  this->Value = num;
  this->Modified();

  if (this->Scale && this->Scale->IsCreated())
    {
    this->Script("%s set %f", 
                 this->Scale->GetWidgetName(), num);
    }
  
  if (this->Entry && this->Entry->IsCreated())
    {
    this->Entry->SetValue(this->Value, this->EntryResolution);
    }

  if (this->Command)
    {
    this->Script("eval %s", this->Command);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetRange(float min, float max)
{
  if (this->Range[0] == min && this->Range[1] == max)
    {
    return;
    }

  this->Range[0] = min;
  this->Range[1] = max;
  this->Modified();

  if (this->Scale && this->Scale->IsCreated())
    {
    this->Script("%s configure -from %f -to %f",
                 this->Scale->GetWidgetName(), min, max);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::ScaleValueChanged(float num)
{
  this->SetValue(num);
}

// ---------------------------------------------------------------------------
void vtkKWScale::EntryValueChanged()
{
  float value = this->Entry->GetValueAsFloat();
  float old_value = this->GetValue();
  this->SetValue(value);

  if (value != old_value && this->EntryCommand)
    {
    this->Script("eval %s", this->EntryCommand);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::InvokeStartCommand()
{
  if (this->StartCommand)
    {
    this->Script("eval %s", this->StartCommand);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::InvokeEndCommand()
{
  if (this->EndCommand)
    {
    this->Script("eval %s", this->EndCommand);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::InvokeEntryCommand()
{
  if (this->EntryCommand)
    {
    this->Script("eval %s",this->EntryCommand);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetStartCommand(vtkKWObject* Object, 
                                 const char * MethodAndArgString)
{
  if (this->StartCommand)
    {
    delete [] this->StartCommand;
    }

  if (!Object)
    {
    return;
    }

  ostrstream command;
  command << Object->GetTclName() << " " << MethodAndArgString << ends;
  this->StartCommand = command.str();
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetEndCommand(vtkKWObject* Object, 
                               const char * MethodAndArgString)
{
  if (this->EndCommand)
    {
    delete [] this->EndCommand;
    }

  if (!Object)
    {
    return;
    }

  ostrstream command;
  command << Object->GetTclName() << " " << MethodAndArgString << ends;
  this->EndCommand = command.str();
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetEntryCommand(vtkKWObject* Object, 
                                 const char * MethodAndArgString)
{
  if (this->EntryCommand)
    {
    delete [] this->EntryCommand;
    }

  if (!Object)
    {
    return;
    }

  ostrstream command;
  command << Object->GetTclName() << " " << MethodAndArgString << ends;
  this->EntryCommand = command.str();
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetCommand(vtkKWObject* Object, 
                            const char *CommandString)
{
  if (this->Command)
    {
    delete [] this->Command;
    }

  if (!Object)
    {
    return;
    }

  ostrstream command;
  command << Object->GetTclName() << " " << CommandString << ends;
  this->Command = command.str();
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetBalloonHelpString(const char *string)
{
  this->Scale->SetBalloonHelpString(string);

  if (this->Entry)
    {
    this->Entry->SetBalloonHelpString(string);
    }

  if (this->Label)
    {
    this->Label->SetBalloonHelpString(string);
    }

  if (this->PopupScale && this->PopupPushButton)
    {
    ostrstream temp;
    temp << string << " (press this button to display a scale)" << ends;
    this->PopupPushButton->SetBalloonHelpString(temp.str());
    temp.rdbuf()->freeze(0);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetBalloonHelpJustification(int j)
{
  this->Scale->SetBalloonHelpJustification(j);

  if (this->Entry)
    {
    this->Entry->SetBalloonHelpJustification(j);
    }

  if (this->Label)
    {
    this->Label->SetBalloonHelpJustification(j);
    }

  if (this->PopupScale && this->PopupPushButton)
    {
    this->PopupPushButton->SetBalloonHelpJustification(j);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetEnabled(int e)
{
  if (this->Enabled == e)
    {
    return;
    }

  this->Enabled = e;
  this->Modified();

  if (this->Entry && this->Entry->IsCreated())
    {
    this->Entry->SetEnabled(e);
    }

  if (this->Label && this->Label->IsCreated())
    {
    this->Label->SetEnabled(e);
    }

  if (this->Scale->IsCreated())
    {
    this->Scale->SetEnabled(e);
    }

  if (this->PopupScale && 
      this->PopupPushButton && this->PopupPushButton->IsCreated())
    {
    this->PopupPushButton->SetEnabled(e);
    }
}

//----------------------------------------------------------------------------
void vtkKWScale::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Value: " << this->Value << endl;
  os << indent << "Resolution: " << this->Resolution << endl;
  os << indent << "Range: " << this->Range[0] << "..." <<  this->Range[1] << endl;
  os << indent << "Label: " << this->Label << endl;
  os << indent << "Entry: " << this->Entry << endl;
  os << indent << "PopupPushButton: " << this->PopupPushButton << endl;
  os << indent << "DisplayEntryAndLabelOnTop: " 
     << (this->DisplayEntryAndLabelOnTop ? "On" : "Off") << endl;
  os << indent << "PupupScale: " 
     << (this->PopupScale ? "On" : "Off") << endl;
  os << indent << "ExpandEntry: " 
     << (this->ExpandEntry ? "On" : "Off") << endl;
}
