/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkKWTextProperty.cxx,v $
  Language:  C++
  Date:      $Date: 2002-10-13 22:24:12 $
  Version:   $Revision: 1.4 $

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
#include "vtkKWTextProperty.h"

#include "vtkActor2D.h"
#include "vtkKWApplication.h"
#include "vtkKWChangeColorButton.h"
#include "vtkKWCheckButton.h"
#include "vtkKWEntry.h"
#include "vtkKWLabel.h"
#include "vtkKWPushButton.h"
#include "vtkKWOptionMenu.h"
#include "vtkKWScale.h"
#include "vtkKWTkUtilities.h"
#include "vtkObjectFactory.h"
#include "vtkProperty2D.h"
#include "vtkTextProperty.h"

#define VTK_KW_TEXT_PROP_ARIAL   "Arial"
#define VTK_KW_TEXT_PROP_COURIER "Courier"
#define VTK_KW_TEXT_PROP_TIMES   "Times"

/* 
 * This part was generated by ImageConvert from image:
 *    bold.png (zlib, base64)
 */
#define image_bold_width         12
#define image_bold_height        16
#define image_bold_pixel_size    4
#define image_bold_buffer_length 152

static unsigned char image_bold[] = 
  "eNr7//8/w38aYiBQAuI0IH4HxP+huByIZ0LFzkD56GA3knoYMEYzg5B6ZPF3NFCP7J40PO"
  "ph/gWx7wKxCxH+VULS846A+cgAOZxdSPAvepjiUn8Gh58Jhc8ZJH+hp4dVQNyBlh4EQYr/"
  "0zh9AgBC7tWi";

/* 
 * This part was generated by ImageConvert from image:
 *    italic.png (zlib, base64)
 */
#define image_italic_width         12
#define image_italic_height        16
#define image_italic_pixel_size    4
#define image_italic_buffer_length 140

static unsigned char image_italic[] = 
  "eNr7//8/w38aYyzABYjvAjFIsgOIlQioB4HdUD1gQIT6d0A8k0j1LlC3hBKpvgOqXpBI9W"
  "eg7mcgQr0S1OxyItWnQdUrEal+FXI4EqH+HdS/xKgPRQ9HHOoFoWrOIKcBXOppiQHpUhBU";

/* 
 * This part was generated by ImageConvert from image:
 *    shadow.png (zlib, base64)
 */
#define image_shadow_width         12
#define image_shadow_height        16
#define image_shadow_pixel_size    4
#define image_shadow_buffer_length 384

static unsigned char image_shadow[] = 
  "eNq10s8rRFEYxvFhRmZIzSATXSnNlR/JwsiMsiALyo+sxFqUmhVFdjaSBWv/go2VlY38Bf"
  "4DS6uJsrC8vk89i9PZO/Xpdmeec977vvdmWZbL/pnXOR58fcQN1nLBCvJHznShH8N4wz6q"
  "6EM+yL/gFgmaWMIFTl1jEqUgr7O//P8u5jCCA2xiQTWC/Dy+9Yh4Rl31Ucasz++N+t3Ah/"
  "f84EQZ7xlAd5RXT8t4wq/3XWMIRXQG+XHdYxAruMQn2tjBKApBXjPv8DxVfwZ3rnGFLT1T"
  "NE/NseLaBc/q1XNtIY3m+Y577OHYZ0z4Xr1PB3mdO4ZDnHlWZfe17R6SaD76Btb9vhruYR"
  "GrmIrer1bJ30OKmq+pf+vRPJT9A5tnxYs=";

/* 
 * This part was generated by ImageConvert from image:
 *    copy.png (zlib, base64)
 */
#define image_copy_width         15
#define image_copy_height        16
#define image_copy_pixel_size    4
#define image_copy_buffer_length 124

static unsigned char image_copy[] = 
  "eNqlkwkKACAIBHu6P9+CLjNlpYSlCKZJJAAFj2kFL1nWVpaPWLVSVrnGXtwwb+fFPdO89e"
  "5IeN/kmffsxfaQ997vz3nN7Bfrh3vj2XEvY1+9LPj4Rz+pVM55tQ==";

// ----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWTextProperty);
vtkCxxRevisionMacro(vtkKWTextProperty, "$Revision: 1.4 $");

int vtkKWTextPropertyCommand(ClientData cd, Tcl_Interp *interp,
                      int argc, char *argv[]);

vtkCxxSetObjectMacro(vtkKWTextProperty, Actor2D, vtkActor2D);

// ----------------------------------------------------------------------------
vtkKWTextProperty::vtkKWTextProperty()
{
  this->TextProperty = NULL;
  this->Actor2D = NULL;

  this->ShowLabel = 0;
  this->Label = vtkKWLabel::New();

  this->ShowColor = 1;
  this->ChangeColorButton = vtkKWChangeColorButton::New();

  this->ShowFontFamily = 1;
  this->FontFamilyOptionMenu = vtkKWOptionMenu::New();

  this->ShowStyles = 1;
  this->StylesFrame = vtkKWWidget::New();
  this->BoldCheckButton = vtkKWCheckButton::New();
  this->ItalicCheckButton = vtkKWCheckButton::New();
  this->ShadowCheckButton = vtkKWCheckButton::New();

  this->ShowOpacity = 1;
  this->OpacityScale = vtkKWScale::New();

  this->ShowHorizontalJustification = 1;
  this->HorizontalJustificationOptionMenu = vtkKWOptionMenu::New();

  this->ShowVerticalJustification = 1;
  this->VerticalJustificationOptionMenu = vtkKWOptionMenu::New();

  this->OnChangeCommand = NULL;
  this->OnColorChangeCommand = NULL;

  this->ShowCopy = 0;
  this->CopyButton = vtkKWPushButton::New();
}

// ----------------------------------------------------------------------------
vtkKWTextProperty::~vtkKWTextProperty()
{
  this->SetTextProperty(NULL);
  this->SetActor2D(NULL);

  this->Label->Delete();
  this->Label = NULL;

  this->ChangeColorButton->Delete();
  this->ChangeColorButton = NULL;

  this->FontFamilyOptionMenu->Delete();
  this->FontFamilyOptionMenu = NULL;

  this->StylesFrame->Delete();
  this->StylesFrame = NULL;
  this->BoldCheckButton->Delete();
  this->BoldCheckButton = NULL;
  this->ItalicCheckButton->Delete();
  this->ItalicCheckButton = NULL;
  this->ShadowCheckButton->Delete();
  this->ShadowCheckButton = NULL;

  this->OpacityScale->Delete();
  this->OpacityScale = NULL;

  this->HorizontalJustificationOptionMenu->Delete();
  this->HorizontalJustificationOptionMenu = NULL;

  this->VerticalJustificationOptionMenu->Delete();
  this->VerticalJustificationOptionMenu = NULL;

  this->SetOnChangeCommand(NULL);
  this->SetOnColorChangeCommand(NULL);

  this->CopyButton->Delete();
  this->CopyButton = NULL;
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::Create(vtkKWApplication *app)
{
  // Set application

  if (this->Application)
    {
    vtkErrorMacro("vtkKWTextProperty already created");
    return;
    }
  this->SetApplication(app);

  // Create the top level widget

  const char *wname = this->GetWidgetName();
  this->Script("frame %s -borderwidth 0 -relief flat", wname);

  // Label

  this->Label->SetParent(this);
  this->Label->Create(this->Application, "");

  // Color

  this->ChangeColorButton->SetParent(this);
  this->ChangeColorButton->SetText("");
  this->ChangeColorButton->Create(this->Application, "");
  this->ChangeColorButton->SetCommand(this, "ChangeColorButtonCallback");
  this->ChangeColorButton->SetBalloonHelpString("Select the text color.");

  // Font Family

  this->FontFamilyOptionMenu->SetParent(this);
  this->FontFamilyOptionMenu->Create(
    this->Application, "-width 7 -indicator 0 -padx 0 -pady 0");
  this->FontFamilyOptionMenu->SetBalloonHelpString("Select the font family.");
  this->FontFamilyOptionMenu->AddEntryWithCommand(
    VTK_KW_TEXT_PROP_ARIAL, this, "FontFamilyOptionMenuCallback");
  this->FontFamilyOptionMenu->AddEntryWithCommand(
    VTK_KW_TEXT_PROP_COURIER, this, "FontFamilyOptionMenuCallback");
  this->FontFamilyOptionMenu->AddEntryWithCommand(
    VTK_KW_TEXT_PROP_TIMES, this, "FontFamilyOptionMenuCallback");

  // Styles (bold, italic, shadow)

  this->StylesFrame->SetParent(this);
  this->StylesFrame->Create(
    this->Application, "frame", "-borderwidth 0 -relief flat");

  const char *styles_options = "-indicator 0 -padx 0 -pady 0";
  this->BoldCheckButton->SetParent(this->StylesFrame);
  this->BoldCheckButton->Create(this->Application, styles_options);
  this->BoldCheckButton->SetCommand(this, "BoldCheckButtonCallback");
  this->BoldCheckButton->SetBalloonHelpString("Select bold style.");
  ostrstream bold;
  bold << this->BoldCheckButton->GetWidgetName() << ".boldimg" << ends;
  this->Script("image create photo %s", bold.str());
  if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                     bold.str(), 
                                     image_bold, 
                                     image_bold_width, 
                                     image_bold_height, 
                                     image_bold_pixel_size,
                                     image_bold_buffer_length,
                                     this->BoldCheckButton->GetWidgetName()))
    {
    vtkWarningMacro(<< "Error creating photo (bold)");
    this->BoldCheckButton->SetText("Bold");
    }
  else
    {
    this->Script("%s configure -image %s", 
                 this->BoldCheckButton->GetWidgetName(), bold.str());
    }
  bold.rdbuf()->freeze(0);
  ostrstream bold_s;
  bold_s << this->BoldCheckButton->GetWidgetName() << ".bold_simg" << ends;
  this->Script("image create photo %s", bold_s.str());
  if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                     bold_s.str(), 
                                     image_bold, 
                                     image_bold_width, 
                                     image_bold_height, 
                                     image_bold_pixel_size,
                                     image_bold_buffer_length,
                                     this->BoldCheckButton->GetWidgetName(),
                                     "-selectcolor"))
    {
    vtkWarningMacro(<< "Error creating photo (bold)");
    }
  else
    {
    this->Script("%s configure -selectimage %s", 
                 this->BoldCheckButton->GetWidgetName(), bold_s.str());
    }
  bold_s.rdbuf()->freeze(0);

  this->ItalicCheckButton->SetParent(this->StylesFrame);
  this->ItalicCheckButton->Create(this->Application,  styles_options);
  this->ItalicCheckButton->SetCommand(this, "ItalicCheckButtonCallback");
  this->ItalicCheckButton->SetBalloonHelpString("Select italic style.");
  ostrstream italic;
  italic << this->ItalicCheckButton->GetWidgetName() << ".italicimg" << ends;
  this->Script("image create photo %s", italic.str());
  if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                     italic.str(), 
                                     image_italic, 
                                     image_italic_width, 
                                     image_italic_height, 
                                     image_italic_pixel_size,
                                     image_italic_buffer_length,
                                     this->ItalicCheckButton->GetWidgetName()))
    {
    vtkWarningMacro(<< "Error creating photo (italic)");
    this->ItalicCheckButton->SetText("Italic");
    }
  else
    {
    this->Script("%s configure -image %s", 
                 this->ItalicCheckButton->GetWidgetName(), italic.str());
    }
  italic.rdbuf()->freeze(0);
  ostrstream italic_s;
  italic_s << this->ItalicCheckButton->GetWidgetName() 
           << ".italic_simg" << ends;
  this->Script("image create photo %s", italic_s.str());
  if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                     italic_s.str(), 
                                     image_italic, 
                                     image_italic_width, 
                                     image_italic_height, 
                                     image_italic_pixel_size,
                                     image_italic_buffer_length,
                                     this->ItalicCheckButton->GetWidgetName(),
                                     "-selectcolor"))
    {
    vtkWarningMacro(<< "Error creating photo (italic)");
    }
  else
    {
    this->Script("%s configure -selectimage %s", 
                 this->ItalicCheckButton->GetWidgetName(), italic_s.str());
    }
  italic_s.rdbuf()->freeze(0);

  this->ShadowCheckButton->SetParent(this->StylesFrame);
  this->ShadowCheckButton->Create(this->Application,  styles_options);
  this->ShadowCheckButton->SetCommand(this, "ShadowCheckButtonCallback");
  this->ShadowCheckButton->SetBalloonHelpString("Select shadow style.");
  ostrstream shadow;
  shadow << this->ShadowCheckButton->GetWidgetName() << ".shadowimg" << ends;
  this->Script("image create photo %s", shadow.str());
  if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                     shadow.str(), 
                                     image_shadow, 
                                     image_shadow_width, 
                                     image_shadow_height, 
                                     image_shadow_pixel_size,
                                     image_shadow_buffer_length,
                                     this->ShadowCheckButton->GetWidgetName()))
    {
    vtkWarningMacro(<< "Error creating photo (shadow)");
    this->ShadowCheckButton->SetText("Shadow");
    }
  else
    {
    this->Script("%s configure -image %s", 
                 this->ShadowCheckButton->GetWidgetName(), shadow.str());
    }
  shadow.rdbuf()->freeze(0);
  ostrstream shadow_s;
  shadow_s << this->ShadowCheckButton->GetWidgetName() 
           << ".shadow_simg" << ends;
  this->Script("image create photo %s", shadow_s.str());
  if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                     shadow_s.str(), 
                                     image_shadow, 
                                     image_shadow_width, 
                                     image_shadow_height, 
                                     image_shadow_pixel_size,
                                     image_shadow_buffer_length,
                                     this->ShadowCheckButton->GetWidgetName(),
                                     "-selectcolor"))
    {
    vtkWarningMacro(<< "Error creating photo (shadow)");
    }
  else
    {
    this->Script("%s configure -selectimage %s", 
                 this->ShadowCheckButton->GetWidgetName(), shadow_s.str());
    }
  shadow_s.rdbuf()->freeze(0);

  this->Script("pack %s %s %s -side left -fill both",
               this->BoldCheckButton->GetWidgetName(),
               this->ItalicCheckButton->GetWidgetName(),
               this->ShadowCheckButton->GetWidgetName());

  // Opacity

  this->OpacityScale->SetParent(this);
  this->OpacityScale->PopupScaleOn();
  this->OpacityScale->SetResolution(0.01);
  this->OpacityScale->SetRange(0.0, 1.0);
  this->OpacityScale->Create(this->Application, "");
  this->OpacityScale->DisplayEntry();
  this->OpacityScale->GetEntry()->SetWidth(4);
  this->OpacityScale->SetCommand(this, "OpacityScaleCallback");
  this->OpacityScale->SetEndCommand(this, "OpacityScaleEndCallback");
  this->OpacityScale->SetEntryCommand(this, "OpacityScaleEndCallback");
  this->OpacityScale->SetBalloonHelpString("Select the text opacity.");

  // Copy button

  this->CopyButton->SetParent(this);
  this->CopyButton->Create(this->Application, "");
  ostrstream copy;
  copy << this->CopyButton->GetWidgetName() << ".copyimg" << ends;
  this->Script("image create photo %s", copy.str());
  if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                     copy.str(), 
                                     image_copy, 
                                     image_copy_width, 
                                     image_copy_height, 
                                     image_copy_pixel_size,
                                     image_copy_buffer_length,
                                     this->CopyButton->GetWidgetName()))
    {
    vtkWarningMacro(<< "Error creating photo (copy)");
    this->CopyButton->SetLabel("Copy");
    }
  else
    {
    this->Script("%s configure -image %s", 
                 this->CopyButton->GetWidgetName(), copy.str());
    }
  copy.rdbuf()->freeze(0);

  // Pack

  this->Script("grid %s %s %s %s %s %s -sticky news -padx 1",
               this->Label->GetWidgetName(),
               this->ChangeColorButton->GetWidgetName(),
               this->FontFamilyOptionMenu->GetWidgetName(),
               this->StylesFrame->GetWidgetName(),
               this->OpacityScale->GetWidgetName(),
               this->CopyButton->GetWidgetName());

  // Update

  this->UpdateInterface();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateInterface()
{
  this->UpdateLabel();
  this->UpdateColorButton();
  this->UpdateFontFamilyOptionMenu();
  this->UpdateStylesFrame();
  this->UpdateOpacityScale();
  this->UpdateCopyButton();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetTextProperty(vtkTextProperty *_arg)
{
  if (this->TextProperty == _arg)
    {
    return;
    }

  this->TextProperty = _arg;
  
  if (this->TextProperty != NULL) 
    { 
    this->UpdateInterface();
    }

  this->Modified();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShowLabel(int _arg)
{
  if (this->ShowLabel == _arg)
    {
    return;
    }
  this->ShowLabel = _arg;
  this->Modified();

  this->UpdateLabel();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateLabel()
{
  if (this->Label->IsCreated())
    {
    this->Script("grid %s %s",
                 (this->ShowLabel ? "" : "remove"), 
                 this->Label->GetWidgetName());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShowColor(int _arg)
{
  if (this->ShowColor == _arg)
    {
    return;
    }
  this->ShowColor = _arg;
  this->Modified();

  this->UpdateColorButton();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetColor(float r, float g, float b) 
{
  if (this->TextProperty)
    {
    this->TextProperty->SetColor(r, g, b);
    }

  this->UpdateColorButton();

  this->AddTraceEntry("$kw(%s) SetColor %f %f %f", this->GetTclName(), r,g,b);

  if (this->OnColorChangeCommand)
    {
    this->Script("eval %s", this->OnColorChangeCommand);
    }

  if (this->OnChangeCommand)
    {
    this->Script("eval %s", this->OnChangeCommand);
    }
}

// ----------------------------------------------------------------------------
float* vtkKWTextProperty::GetColor() 
{
  int use_actor_color = 0;
  if (!this->TextProperty)
    {
    use_actor_color = 1;
    }
  else
    {
    // This test is done to maintain backward compatibility (see
    // vtkOpenGL...TextMapper). The default vtkTextProperty color is
    // -1, -1, -1 so that the mappers know that they have to use
    // the actor's color instead.
    float *rgb = this->TextProperty->GetColor();
    if (rgb[0] < 0.0 && rgb[1] < 0.0 && rgb[2] < 0.0)
      {
      use_actor_color = 1;
      }
    }

  if (use_actor_color && this->Actor2D && this->Actor2D->GetProperty())
    {
    return this->Actor2D->GetProperty()->GetColor();
    }
  else if (this->TextProperty)
    {
    return this->TextProperty->GetColor();
    }
  return 0;
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateColorButton()
{
  if (this->ChangeColorButton->IsCreated() && this->TextProperty)
    {
    this->ChangeColorButton->SetColor(this->GetColor());

    this->Script("grid %s %s",
                 (this->ShowColor ? "" : "remove"), 
                 this->ChangeColorButton->GetWidgetName());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::ChangeColorButtonCallback(float r, float g, float b) 
{
  this->SetColor(r, g, b);
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShowFontFamily(int _arg)
{
  if (this->ShowFontFamily == _arg)
    {
    return;
    }
  this->ShowFontFamily = _arg;
  this->Modified();

  this->UpdateFontFamilyOptionMenu();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetFontFamily(int v) 
{
  if (this->TextProperty)
    {
    this->TextProperty->SetFontFamily(v);
    }

  this->UpdateFontFamilyOptionMenu();

  this->AddTraceEntry("$kw(%s) SetFontFamily %d", this->GetTclName(), v);

  if (this->OnChangeCommand)
    {
    this->Script("eval %s", this->OnChangeCommand);
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateFontFamilyOptionMenu()
{
  if (this->FontFamilyOptionMenu->IsCreated() && this->TextProperty)
    {
    switch (this->TextProperty->GetFontFamily())
      {
      case VTK_ARIAL:
        this->FontFamilyOptionMenu->SetValue(VTK_KW_TEXT_PROP_ARIAL);
        break;
      case VTK_COURIER:
        this->FontFamilyOptionMenu->SetValue(VTK_KW_TEXT_PROP_COURIER);
        break;
      case VTK_TIMES:
        this->FontFamilyOptionMenu->SetValue(VTK_KW_TEXT_PROP_TIMES);
        break;
      }
    this->Script("grid %s %s",
                 (this->ShowFontFamily ? "" : "remove"), 
                 this->FontFamilyOptionMenu->GetWidgetName());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::FontFamilyOptionMenuCallback() 
{
  if (this->FontFamilyOptionMenu->IsCreated())
    {
    char *value = this->FontFamilyOptionMenu->GetValue();
    if (!strcmp(value, VTK_KW_TEXT_PROP_ARIAL))
      {
      this->SetFontFamily(VTK_ARIAL);
      }
    else if (!strcmp(value, VTK_KW_TEXT_PROP_COURIER))
      {
      this->SetFontFamily(VTK_COURIER);
      }
    else if (!strcmp(value, VTK_KW_TEXT_PROP_TIMES))
      {
      this->SetFontFamily(VTK_TIMES);
      }
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShowStyles(int _arg)
{
  if (this->ShowStyles == _arg)
    {
    return;
    }
  this->ShowStyles = _arg;
  this->Modified();

  this->UpdateStylesFrame();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateStylesFrame()
{
  if (this->StylesFrame->IsCreated())
    {
    this->Script("grid %s %s",
                 (this->ShowStyles ? "" : "remove"), 
                 this->StylesFrame->GetWidgetName());
    }

  this->UpdateBoldCheckButton();
  this->UpdateItalicCheckButton();
  this->UpdateShadowCheckButton();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetBold(int v) 
{
  if (this->TextProperty)
    {
    this->TextProperty->SetBold(v);
    }

  this->UpdateBoldCheckButton();

  this->AddTraceEntry("$kw(%s) SetBold %d", this->GetTclName(), v);

  if (this->OnChangeCommand)
    {
    this->Script("eval %s", this->OnChangeCommand);
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateBoldCheckButton()
{
  if (this->BoldCheckButton->IsCreated() && this->TextProperty)
    {
    this->BoldCheckButton->SetState(this->TextProperty->GetBold());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::BoldCheckButtonCallback() 
{
  if (this->BoldCheckButton->IsCreated())
    {
    this->SetBold(this->BoldCheckButton->GetState());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetItalic(int v) 
{
  if (this->TextProperty)
    {
    this->TextProperty->SetItalic(v);
    }

  this->UpdateItalicCheckButton();

  this->AddTraceEntry("$kw(%s) SetItalic %d", this->GetTclName(), v);

  if (this->OnChangeCommand)
    {
    this->Script("eval %s", this->OnChangeCommand);
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateItalicCheckButton()
{
  if (this->ItalicCheckButton->IsCreated() && this->TextProperty)
    {
    this->ItalicCheckButton->SetState(this->TextProperty->GetItalic());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::ItalicCheckButtonCallback() 
{
  if (this->ItalicCheckButton->IsCreated())
    {
    this->SetItalic(this->ItalicCheckButton->GetState());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShadow(int v) 
{
  if (this->TextProperty)
    {
    this->TextProperty->SetShadow(v);
    }

  this->UpdateShadowCheckButton();

  this->AddTraceEntry("$kw(%s) SetShadow %d", this->GetTclName(), v);

  if (this->OnChangeCommand)
    {
    this->Script("eval %s", this->OnChangeCommand);
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateShadowCheckButton()
{
  if (this->ShadowCheckButton->IsCreated() && this->TextProperty)
    {
    this->ShadowCheckButton->SetState(this->TextProperty->GetShadow());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::ShadowCheckButtonCallback() 
{
  if (this->ShadowCheckButton->IsCreated())
    {
    this->SetShadow(this->ShadowCheckButton->GetState());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShowOpacity(int _arg)
{
  if (this->ShowOpacity == _arg)
    {
    return;
    }
  this->ShowOpacity = _arg;
  this->Modified();

  this->UpdateOpacityScale();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetOpacityNoTrace(float v) 
{
  if (this->TextProperty)
    {
    this->TextProperty->SetOpacity(v);
    }

  this->UpdateOpacityScale();

  if (this->OnChangeCommand)
    {
    this->Script("eval %s", this->OnChangeCommand);
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetOpacity(float v) 
{
  this->SetOpacityNoTrace(v);
  this->AddTraceEntry("$kw(%s) SetOpacity %f", this->GetTclName(), v);
}

// ----------------------------------------------------------------------------
float vtkKWTextProperty::GetOpacity() 
{
  int use_actor_opacity = 0;
  if (!this->TextProperty)
    {
    use_actor_opacity = 1;
    }
  else
    {
    // This test is done to maintain backward compatibility (see
    // vtkOpenGL...TextMapper). The default vtkTextProperty opacity is
    // -1 so that the mappers know that they have to use
    // the actor's opacity instead.
    if (this->TextProperty->GetOpacity() < 0.0)
      {
      use_actor_opacity = 1;
      }
    }

  if (use_actor_opacity && this->Actor2D && this->Actor2D->GetProperty())
    {
    return this->Actor2D->GetProperty()->GetOpacity();
    }
  else if (this->TextProperty)
    {
    return this->TextProperty->GetOpacity();
    }
  return 0.0;
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateOpacityScale()
{
  if (this->OpacityScale->IsCreated() && this->TextProperty)
    {
    this->OpacityScale->SetValue(this->GetOpacity());
    this->Script("grid %s %s",
                 (this->ShowOpacity ? "" : "remove"), 
                 this->OpacityScale->GetWidgetName());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::OpacityScaleCallback() 
{
  if (this->OpacityScale->IsCreated())
    {
    this->SetOpacityNoTrace(this->OpacityScale->GetValue());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::OpacityScaleEndCallback() 
{
  if (this->OpacityScale->IsCreated())
    {
    this->SetOpacity(this->OpacityScale->GetValue());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShowHorizontalJustification(int _arg)
{
  if (this->ShowHorizontalJustification == _arg)
    {
    return;
    }
  this->ShowHorizontalJustification = _arg;
  this->Modified();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShowVerticalJustification(int _arg)
{
  if (this->ShowVerticalJustification == _arg)
    {
    return;
    }
  this->ShowVerticalJustification = _arg;
  this->Modified();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShowCopy(int _arg)
{
  if (this->ShowCopy == _arg)
    {
    return;
    }
  this->ShowCopy = _arg;
  this->Modified();

  this->UpdateCopyButton();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateCopyButton()
{
  if (this->CopyButton->IsCreated())
    {
    this->Script("grid %s %s",
                 (this->ShowCopy ? "" : "remove"), 
                 this->CopyButton->GetWidgetName());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::CopyValuesFrom(vtkKWTextProperty *widget)
{
  if (widget)
    {
    vtkTextProperty *tprop = widget->GetTextProperty();
    if (tprop)
      {
      this->SetColor(widget->GetColor());
      this->SetFontFamily(tprop->GetFontFamily());
      this->SetBold(tprop->GetBold());
      this->SetItalic(tprop->GetItalic());
      this->SetShadow(tprop->GetShadow());
      this->SetOpacity(widget->GetOpacity());
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWTextProperty::SaveInTclScript(ofstream *file, 
                                        const char *tcl_name)
{
  static int instance_count = 0;

  const char *name;
  char buffer[128];
  if (tcl_name)
    {
    name = tcl_name;
    }
  else
    {
    sprintf(buffer, "TextProperty%d", instance_count++);
    name = buffer;
    *file << "vtkTextProperty " << name << endl;
    }

  vtkTextProperty *tprop = this->TextProperty;
  if (tprop)
    {
    float *rgb = this->GetColor();
    *file << "\t" << name << " SetColor " 
          << rgb[0] << " "  << rgb[1] << " "  << rgb[2] << endl;
    *file << "\t" << name << " SetFontFamily " 
          << tprop->GetFontFamily() << endl;
    *file << "\t" << name << " SetBold " << tprop->GetBold() << endl;
    *file << "\t" << name << " SetItalic " << tprop->GetItalic() << endl;
    *file << "\t" << name << " SetShadow " << tprop->GetShadow() << endl;
    *file << "\t" << name << " SetOpacity " << this->GetOpacity() << endl;
    }
}

//----------------------------------------------------------------------------
void vtkKWTextProperty::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  if (this->TextProperty)
    {
    os << indent << "TextProperty:\n";
    this->TextProperty->PrintSelf(os, indent.GetNextIndent());
    }
  else
    {
    os << indent << "TextProperty: None" << endl;
    }
  if (this->Actor2D)
    {
    os << indent << "Actor2D:\n";
    this->Actor2D->PrintSelf(os, indent.GetNextIndent());
    }
  else
    {
    os << indent << "Actor2D: None" << endl;
    }
  os << indent << "ShowColor: " << (this->ShowColor ? "On" : "Off") << endl;
  os << indent << "ShowFontFamily: " 
     << (this->ShowFontFamily ? "On" : "Off") << endl;
  os << indent << "ShowStyles: " 
     << (this->ShowStyles ? "On" : "Off") << endl;
  os << indent << "ShowOpacity: " 
     << (this->ShowOpacity ? "On" : "Off") << endl;
  os << indent << "ShowHorizontalJustification: " 
     << (this->ShowHorizontalJustification ? "On" : "Off") << endl;
  os << indent << "ShowVerticalJustification: " 
     << (this->ShowVerticalJustification ? "On" : "Off") << endl;
  os << indent << "ShowCopy: " << (this->ShowCopy ? "On" : "Off") << endl;
  os << indent << "OnChangeCommand: " 
     << (this->OnChangeCommand ? this->OnChangeCommand : "None") << endl;
  os << indent << "OnColorChangeCommand: " 
     << (this->OnColorChangeCommand ? this->OnColorChangeCommand : "None") << endl;
  os << indent << "CopyButton: " << this->CopyButton << endl;
  os << indent << "Label: " << this->Label << endl;
}
