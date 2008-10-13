/*=========================================================================

  Module:    vtkKWStartupPageWidget.cxx,v

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkKWStartupPageWidget.h"

#include "vtkKWApplication.h"
#include "vtkKWCanvas.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWInternationalization.h"
#include "vtkKWMostRecentFilesManager.h"
#include "vtkKWTkUtilities.h"
#include "vtkKWIcon.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

#include <vtksys/ios/sstream>
#include <vtksys/stl/string>

vtkCxxRevisionMacro(vtkKWStartupPageWidget, "1.21");
vtkStandardNewMacro(vtkKWStartupPageWidget);

#define VTK_KW_SPW_OPEN "opentag"

//----------------------------------------------------------------------------
class vtkKWStartupPageWidgetInternals
{
public:
  vtksys_stl::string ScheduleRedrawTimerId;

  int LastRedrawSize[2];

  char TextFont[1024];
  char TextColor[20];
  char SelectedTextColor[20];
  char TextShadowColor[20];
  char MostRecentFilesFont[1024];

  char HintFont[1024];
  char HintColor[20];
  char HintShadowColor[20];

  vtkKWStartupPageWidgetInternals() 
    {
      this->LastRedrawSize[0] = 0;
      this->LastRedrawSize[1] = 0;
    }
};

//----------------------------------------------------------------------------
vtkKWStartupPageWidget::vtkKWStartupPageWidget()
{
  this->Internals = new vtkKWStartupPageWidgetInternals;

  this->GradientColor1[0]    = 21.0 / 255.0;
  this->GradientColor1[1]    = 27.0 / 255.0;
  this->GradientColor1[2]    = 26.0 / 255.0;

  this->GradientColor2[0]    = 70.0 / 255.0;
  this->GradientColor2[1]    = 81.0 / 255.0;
  this->GradientColor2[2]    = 80.0 / 255.0;

  this->TextColor[0]         = 255.0 / 255.0;
  this->TextColor[1]         = 255.0 / 255.0;
  this->TextColor[2]         = 255.0 / 255.0;

  this->SelectedTextColor[0] = 255.0 / 255.0;
  this->SelectedTextColor[1] = 249.0 / 255.0;
  this->SelectedTextColor[2] =  85.0 / 255.0;

  this->HintColor[0]         = 120.0 / 255.0;
  this->HintColor[1]         = 120.0 / 255.0;
  this->HintColor[2]         = 120.0 / 255.0;

  this->SupportDoubleClick     = 1;
  this->SupportDrop            = 1;
  this->SupportMostRecentFiles = 1;
  this->SupportOpen            = 1;

  this->OpenIcon            = NULL;
  this->DoubleClickIcon     = NULL;
  this->DropIcon            = NULL;
  this->MostRecentFilesIcon = NULL;

  this->OpenCommand            = NULL;
  this->DropCommand            = NULL;
  this->DoubleClickCommand     = NULL;

  this->StartupPageCanvas      = vtkKWCanvas::New();

  this->MostRecentFilesManager = NULL;
}

//----------------------------------------------------------------------------
vtkKWStartupPageWidget::~vtkKWStartupPageWidget()
{
  delete this->Internals;
  this->Internals = NULL;

  // Commands

  if (this->OpenCommand)
    {
    delete [] this->OpenCommand;
    this->OpenCommand = NULL;
    }

  if (this->DropCommand)
    {
    delete [] this->DropCommand;
    this->DropCommand = NULL;
    }

  if (this->DoubleClickCommand)
    {
    delete [] this->DoubleClickCommand;
    this->DoubleClickCommand = NULL;
    }

  // GUI

  if (this->StartupPageCanvas)
    {
    this->StartupPageCanvas->Delete();
    this->StartupPageCanvas = NULL;
    }

  if (this->OpenIcon)
    {
    this->OpenIcon->Delete();
    this->OpenIcon = NULL;
    }

  if (this->DoubleClickIcon)
    {
    this->DoubleClickIcon->Delete();
    this->DoubleClickIcon = NULL;
    }

  if (this->DropIcon)
    {
    this->DropIcon->Delete();
    this->DropIcon = NULL;
    }

  if (this->MostRecentFilesIcon)
    {
    this->MostRecentFilesIcon->Delete();
    this->MostRecentFilesIcon = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::CreateWidget()
{
  // Check if already created

  if (this->IsCreated())
    {
    vtkErrorMacro(<< this->GetClassName() << " already created");
    return;
    }

  // Call the superclass to create the whole widget

  this->Superclass::CreateWidget();

  // Create the Hue/Sat canvas

  this->StartupPageCanvas->SetParent(this);
  this->StartupPageCanvas->Create();
  this->StartupPageCanvas->SetHeight(0);
  this->StartupPageCanvas->SetWidth(0);
  this->StartupPageCanvas->SetBorderWidth(0);

  this->Script("pack %s -fill both -expand 1", 
               this->StartupPageCanvas->GetWidgetName());

  // Update

  this->UpdateInternalCanvasBindings();
  this->UpdateInternalCanvasColors();
  this->UpdateInternalCanvasFonts();
  this->UpdateInternalCanvasIcons();

  this->Update();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::Update()
{
  this->UpdateEnableState();

  this->Redraw();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::AddCallbackCommandObservers()
{
  this->Superclass::AddCallbackCommandObservers();

  if (this->MostRecentFilesManager)
    {
    this->AddCallbackCommandObserver(
      this->MostRecentFilesManager, 
      vtkKWMostRecentFilesManager::MenuHasChangedEvent);
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::RemoveCallbackCommandObservers()
{
  this->Superclass::RemoveCallbackCommandObservers();

  if (this->MostRecentFilesManager)
    {
    this->RemoveCallbackCommandObserver(
      this->MostRecentFilesManager, 
      vtkKWMostRecentFilesManager::MenuHasChangedEvent);
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::UpdateInternalCanvasBindings()
{
  this->StartupPageCanvas->AddCanvasBinding(
    "open", "<ButtonPress-1>", this, "OpenCallback");

  this->SetDropFileBinding(NULL, this->DropCommand);

  this->StartupPageCanvas->AddBinding(
    "<Double-1>", this, "DoubleClickCallback");

  this->AddCallbackCommandObservers();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::UpdateInternalCanvasFonts()
{
  Tcl_Interp *interp = this->GetApplication()->GetMainInterp();

  int tcl_major = 0, tcl_minor = 0, tcl_patch_level = 0;
  Tcl_GetVersion(&tcl_major, &tcl_minor, &tcl_patch_level, NULL);

  const char *base_font = (tcl_major < 8 || (tcl_major == 8 && tcl_minor < 5)) 
    ? "fixed" : "TkDefaultFont";

  char base_font_bold[1024];
  vtkKWTkUtilities::ChangeFontWeightToBold(
    interp, base_font, base_font_bold);
  
  vtkKWTkUtilities::ChangeFontSize(
    interp, base_font_bold, 14, this->Internals->TextFont);
  
  vtkKWTkUtilities::ChangeFontSize(
    interp, base_font, 10, this->Internals->MostRecentFilesFont);

  vtkKWTkUtilities::ChangeFontSize(
    interp, base_font, 8, this->Internals->HintFont);
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::UpdateInternalCanvasColors()
{
  double average, shadow;

  // Text

  sprintf(this->Internals->TextColor, "#%02x%02x%02x", 
          (int)(this->TextColor[0] * 255.0),
          (int)(this->TextColor[1] * 255.0),
          (int)(this->TextColor[2] * 255.0));

  sprintf(this->Internals->SelectedTextColor, "#%02x%02x%02x", 
          (int)(this->SelectedTextColor[0] * 255.0),
          (int)(this->SelectedTextColor[1] * 255.0),
          (int)(this->SelectedTextColor[2] * 255.0));

  average = 
    (this->TextColor[0] + this->TextColor[1] + this->TextColor[2]) / 3.0;
  shadow = average > 0.5 ? 0.0 : 1.0;

  sprintf(this->Internals->TextShadowColor, "#%02x%02x%02x", 
          (int)(shadow * 255.0),
          (int)(shadow * 255.0),
          (int)(shadow * 255.0));

  // Hint

  sprintf(this->Internals->HintColor, "#%02x%02x%02x", 
          (int)(this->HintColor[0] * 255.0),
          (int)(this->HintColor[1] * 255.0),
          (int)(this->HintColor[2] * 255.0));

  average = 
    (this->HintColor[0] + this->HintColor[1] + this->HintColor[2]) / 3.0;
  shadow = average > 0.3 ? 0.0 : 1.0;

  sprintf(this->Internals->HintShadowColor, "#%02x%02x%02x", 
          (int)(shadow * 255.0),
          (int)(shadow * 255.0),
          (int)(shadow * 255.0));
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::UpdateInternalCanvasIcons()
{
  if (!this->OpenIcon)
    {
    this->OpenIcon = vtkKWIcon::New();
    this->OpenIcon->SetImage(
      vtkKWIcon::IconNuvola48x48FilesystemsFolderBlue);
    }

  if (!this->DoubleClickIcon)
    {
    this->DoubleClickIcon = vtkKWIcon::New();
    this->DoubleClickIcon->SetImage(
      vtkKWIcon::IconNuvola48x48DevicesMouse);
    }

  if (!this->DropIcon)
    {
    this->DropIcon = vtkKWIcon::New();
    this->DropIcon->SetImage(
      vtkKWIcon::IconNuvola48x48AppsDownloadManager);
    }

  if (!this->MostRecentFilesIcon)
    {
    this->MostRecentFilesIcon = vtkKWIcon::New();
    this->MostRecentFilesIcon->SetImage(
      vtkKWIcon::IconNuvola48x48ActionsHistory);
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetOpenIcon(vtkKWIcon *arg)
{
  if (arg)
    {
    if (!this->OpenIcon)
      {
      this->OpenIcon = vtkKWIcon::New();
      }
    this->OpenIcon->SetImage(arg);
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetOpenIconToPredefinedIcon(int arg)
{
  if (arg)
    {
    if (!this->OpenIcon)
      {
      this->OpenIcon = vtkKWIcon::New();
      }
    this->OpenIcon->SetImage(arg);
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetDoubleClickIcon(vtkKWIcon *arg)
{
  if (arg)
    {
    if (!this->DoubleClickIcon)
      {
      this->DoubleClickIcon = vtkKWIcon::New();
      }
    this->DoubleClickIcon->SetImage(arg);
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetDoubleClickIconToPredefinedIcon(int arg)
{
  if (arg)
    {
    if (!this->DoubleClickIcon)
      {
      this->DoubleClickIcon = vtkKWIcon::New();
      }
    this->DoubleClickIcon->SetImage(arg);
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetDropIcon(vtkKWIcon *arg)
{
  if (arg)
    {
    if (!this->DropIcon)
      {
      this->DropIcon = vtkKWIcon::New();
      }
    this->DropIcon->SetImage(arg);
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetDropIconToPredefinedIcon(int arg)
{
  if (arg)
    {
    if (!this->DropIcon)
      {
      this->DropIcon = vtkKWIcon::New();
      }
    this->DropIcon->SetImage(arg);
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetMostRecentFilesIcon(vtkKWIcon *arg)
{
  if (arg)
    {
    if (!this->MostRecentFilesIcon)
      {
      this->MostRecentFilesIcon = vtkKWIcon::New();
      }
    this->MostRecentFilesIcon->SetImage(arg);
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetMostRecentFilesIconToPredefinedIcon(int arg)
{
  if (arg)
    {
    if (!this->MostRecentFilesIcon)
      {
      this->MostRecentFilesIcon = vtkKWIcon::New();
      }
    this->MostRecentFilesIcon->SetImage(arg);
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetMostRecentFilesManager(
  vtkKWMostRecentFilesManager *arg)
{
  if (this->MostRecentFilesManager == arg)
    {
    return;
    }

  this->MostRecentFilesManager = arg;
  this->Modified();

  this->AddCallbackCommandObservers();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::Redraw()
{
  if (!this->StartupPageCanvas || !this->StartupPageCanvas->IsAlive())
    {
    return;
    }

  int canv_width, canv_height;
  if (!vtkKWTkUtilities::GetWidgetSize(
        this->StartupPageCanvas, &canv_width, &canv_height))
    {
    return;
    }

  // Too small or same size ?

  if (canv_width <= 5 || canv_height <= 5 ||
      (this->Internals->LastRedrawSize[0] == canv_width &&
       this->Internals->LastRedrawSize[1] == canv_height))
    {
    return;
    }

  this->Internals->LastRedrawSize[0] = canv_width;
  this->Internals->LastRedrawSize[1] = canv_height;

  int center_x = canv_width / 2;
  int x = (int)(0.10 * canv_width);
  int y = (int)(0.10 * canv_height);
  int interspace = (int)(0.15 * canv_height);

  vtksys_ios::ostringstream tk_cmd;

  const char *canv = this->StartupPageCanvas->GetWidgetName();

  // Gradient

  this->CanvasDeleteTag(canv, "gradient");

  this->StartupPageCanvas->AddHorizontalRGBGradient(
    this->GradientColor1[0], this->GradientColor1[1], this->GradientColor1[2], 
    this->GradientColor2[0], this->GradientColor2[1], this->GradientColor2[2], 
    0, 0, canv_width - 1, canv_height - 1, "gradient");

  this->StartupPageCanvas->SetBackgroundColor(this->GradientColor2);
    
  tk_cmd << canv << " lower gradient all" << endl;

  // Open file

  if (this->SupportOpen)
    {
    this->AddSectionToCanvas(
      tk_cmd, 
      x, y, 
      this->OpenIcon,
      ks_("Startup Web Page|Open File"), 
      this->Internals->TextFont,
      ks_("...by clicking on this button or by using the \"File -> Open File\" menu."), 
      this->Internals->HintFont,
      "open");
    y += interspace;
    }

  // Double Click

  if (this->SupportDoubleClick)
    {
    this->AddSectionToCanvas(
      tk_cmd, 
      x, y, 
      this->DoubleClickIcon,
      ks_("Startup Web Page|Double Click"), 
      this->Internals->TextFont,
      ks_("...anywhere in this area to open a file."), 
      this->Internals->HintFont,
      "doubleclick");
    y += interspace;
    }

  // Drag & Drop

  if (this->SupportDrop)
    {
    this->AddSectionToCanvas(
      tk_cmd, 
      x, y, 
      this->DropIcon,
      ks_("Startup Web Page|Drag & Drop"), 
      this->Internals->TextFont,
      ks_("...any file in this area to open it."), 
      this->Internals->HintFont,
      "drop");
    y += interspace;
    }

  // Most Recent Files

  if (this->SupportMostRecentFiles)
    {
    this->AddMostRecentFilesSectionToCanvas(
      tk_cmd,
      x, y);
    }
  
  this->Script(tk_cmd.str().c_str());
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::AddMostRecentFilesSectionToCanvas(
  ostream &tk_cmd, 
  int x, int y)
{
  this->AddSectionToCanvas(
      tk_cmd, 
      x, y, 
      this->MostRecentFilesIcon,
      ks_("Startup Web Page|Open Recent File"), 
      this->Internals->TextFont,
      ks_("...by selecting a file below  or by using the \"File -> Open Recent File\" menu."), 
      this->Internals->HintFont,
      "recent");
  
  
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::AddSectionToCanvas(
  ostream &tk_cmd, 
  int x, int y, 
  vtkKWIcon *icon,
  const char *text, const char *text_font, 
  const char *hint, const char *hint_font,
  const char *tag)
{
  const char *canv = this->StartupPageCanvas->GetWidgetName();

  // Open file

  if (!this->CanvasHasTag(canv, tag))
    {
    tk_cmd << canv << " create text 0 0 -anchor sw"
           << " -fill " << this->Internals->TextShadowColor
           << " -text {" << text << "}"
           << " -font {" << text_font << "}"
           << " -tags {" << tag << "textshadow_id " << tag << "}" 
           << endl;
    tk_cmd << canv << " create text 0 0 -anchor sw"
           << " -fill " << this->Internals->TextColor
           << " -text {" << text << "}"
           << " -font {" << text_font << "}"
           << " -tags {" << tag << "text_id " << tag << "}" 
           << endl;
    tk_cmd << canv << " create text 0 0 -anchor nw"
           << " -fill " << this->Internals->HintShadowColor
           << " -text {" << hint << "}"
           << " -font {" << hint_font << "}"
           << " -tags {" << tag << "hintshadow_id " << tag << "}" 
           << endl;
    tk_cmd << canv << " create text 0 0 -anchor nw"
           << " -fill " << this->Internals->HintColor
           << " -text {" << hint << "}"
           << " -font {" << hint_font << "}"
           << " -tags {" << tag << "hint_id " << tag << "}" 
           << endl;

    if (icon)
      {
      vtksys_stl::string img_name(canv);
      img_name += tag;
      img_name += "icon";
      if (vtkKWTkUtilities::UpdatePhotoFromIcon(
            this->GetApplication(), img_name.c_str(), icon))
        {
        tk_cmd << canv << " create image 0 0 -anchor center"
               << " -image " << img_name.c_str()
               << " -tags {" << tag << "icon_id " << tag << "}" 
               << endl;
        }
      }

    // Bindings

    vtksys_stl::string command("HighlightSectionCallback ");
    command += tag;

    vtksys_stl::string highlight_command(command);
    highlight_command += " 1";
    this->StartupPageCanvas->AddCanvasBinding(
      tag, "<Enter>", this, highlight_command.c_str());

    vtksys_stl::string no_highlight_command(command);
    no_highlight_command += " 0";
    this->StartupPageCanvas->AddCanvasBinding(
      tag, "<Leave>", this, no_highlight_command.c_str());
    }

  if (icon)
    {
    tk_cmd << canv << " coords " << tag << "icon_id " 
           << x << " " << y - 4 << endl;
    x += (int)(0.5 * icon->GetWidth()) + 20;
    }

  tk_cmd << canv << " coords " << tag << "text_id " 
         << x << " " << y << endl
         << canv << " coords " << tag << "textshadow_id " 
         << x + 2 << " " << y + 2 << endl
         << canv << " coords " << tag << "hint_id " 
         << x << " " << y << endl
         << canv << " coords " << tag << "hintshadow_id " 
         << x + 2 << " " << y + 2 << endl;
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::HighlightSectionCallback(
  const char *tag, int flag)
{
  const char *canv = this->StartupPageCanvas->GetWidgetName();
  vtksys_stl::string command(canv);
  command += " itemconfigure ";
  command += tag;
  command += "text_id -fill ";
  command += 
    (flag ? this->Internals->SelectedTextColor : this->Internals->TextColor);
  this->Script(command.c_str());
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::OpenCallback()
{
  this->InvokeOpenCommand();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::DoubleClickCallback()
{
  this->InvokeDoubleClickCommand();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::RedrawCallback()
{
  if (!this->GetApplication() || 
      this->GetApplication()->GetInExit() ||
      !this->IsAlive())
    {
    return;
    }

  this->Redraw();
  this->Internals->ScheduleRedrawTimerId = "";
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::ScheduleRedraw()
{
  // Already scheduled

  if (this->Internals->ScheduleRedrawTimerId.size())
    {
    return;
    }

  this->Internals->ScheduleRedrawTimerId =
    this->Script(
      "after idle {catch {%s RedrawCallback}}", this->GetTclName());
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::Bind()
{
  if (!this->IsCreated())
    {
    return;
    }

  if (this->StartupPageCanvas && this->StartupPageCanvas->IsAlive())
    {
    this->StartupPageCanvas->SetBinding(
      "<Configure>", this, "ConfigureCallback");
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::UnBind()
{
  if (!this->IsCreated())
    {
    return;
    }

  if (this->StartupPageCanvas && this->StartupPageCanvas->IsAlive())
    {
    this->StartupPageCanvas->RemoveBinding("<Configure>");
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::ConfigureCallback()
{
  this->ScheduleRedraw();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetSupportOpen(int arg)
{
  if (this->SupportOpen == arg)
    {
    return;
    }

  this->SupportOpen = arg;

  this->Modified();

  this->ScheduleRedraw();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetSupportDoubleClick(int arg)
{
  if (this->SupportDoubleClick == arg)
    {
    return;
    }

  this->SupportDoubleClick = arg;

  this->Modified();

  this->ScheduleRedraw();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetSupportDrop(int arg)
{
  if (this->SupportDrop == arg)
    {
    return;
    }

  this->SupportDrop = arg;

  this->Modified();

  this->ScheduleRedraw();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetSupportMostRecentFiles(int arg)
{
  if (this->SupportMostRecentFiles == arg)
    {
    return;
    }

  this->SupportMostRecentFiles = arg;

  this->Modified();

  this->ScheduleRedraw();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetGradientColor1(
  double r, double g, double b)
{
  if ((r < 0.0 || r > 1.0 ||
       g < 0.0 || g > 1.0 ||
       b < 0.0 || b > 1.0) ||
      (r == this->GradientColor1[0] &&
       g == this->GradientColor1[1] &&
       b == this->GradientColor1[2]))
    {
    return;
    }

  this->GradientColor1[0] = r;
  this->GradientColor1[1] = g;
  this->GradientColor1[2] = b;

  this->Modified();

  this->UpdateInternalCanvasColors();
  this->ScheduleRedraw();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetGradientColor2(
  double r, double g, double b)
{
  if ((r < 0.0 || r > 1.0 ||
       g < 0.0 || g > 1.0 ||
       b < 0.0 || b > 1.0) ||
      (r == this->GradientColor2[0] &&
       g == this->GradientColor2[1] &&
       b == this->GradientColor2[2]))
    {
    return;
    }

  this->GradientColor2[0] = r;
  this->GradientColor2[1] = g;
  this->GradientColor2[2] = b;

  this->Modified();

  this->UpdateInternalCanvasColors();
  this->ScheduleRedraw();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetTextColor(
  double r, double g, double b)
{
  if ((r < 0.0 || r > 1.0 ||
       g < 0.0 || g > 1.0 ||
       b < 0.0 || b > 1.0) ||
      (r == this->TextColor[0] &&
       g == this->TextColor[1] &&
       b == this->TextColor[2]))
    {
    return;
    }

  this->TextColor[0] = r;
  this->TextColor[1] = g;
  this->TextColor[2] = b;

  this->Modified();

  this->UpdateInternalCanvasColors();
  this->ScheduleRedraw();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetSelectedTextColor(
  double r, double g, double b)
{
  if ((r < 0.0 || r > 1.0 ||
       g < 0.0 || g > 1.0 ||
       b < 0.0 || b > 1.0) ||
      (r == this->SelectedTextColor[0] &&
       g == this->SelectedTextColor[1] &&
       b == this->SelectedTextColor[2]))
    {
    return;
    }

  this->SelectedTextColor[0] = r;
  this->SelectedTextColor[1] = g;
  this->SelectedTextColor[2] = b;

  this->Modified();

  this->UpdateInternalCanvasColors();
  this->ScheduleRedraw();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetHintColor(
  double r, double g, double b)
{
  if ((r < 0.0 || r > 1.0 ||
       g < 0.0 || g > 1.0 ||
       b < 0.0 || b > 1.0) ||
      (r == this->HintColor[0] &&
       g == this->HintColor[1] &&
       b == this->HintColor[2]))
    {
    return;
    }

  this->HintColor[0] = r;
  this->HintColor[1] = g;
  this->HintColor[2] = b;

  this->Modified();

  this->UpdateInternalCanvasColors();
  this->ScheduleRedraw();
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetOpenCommand(
  vtkObject *object, const char *method)
{
  this->SetObjectMethodCommand(
    &this->OpenCommand, object, method);
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::InvokeOpenCommand()
{
  this->InvokeObjectMethodCommand(this->OpenCommand);
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetDropCommand(
  vtkObject *object, const char *method)
{
  this->SetObjectMethodCommand(
    &this->DropCommand, object, method);

  if (this->IsCreated())
    {
    this->SetDropFileBinding(NULL, this->DropCommand);
    }
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::SetDoubleClickCommand(
  vtkObject *object, const char *method)
{
  this->SetObjectMethodCommand(
    &this->DoubleClickCommand, object, method);
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::InvokeDoubleClickCommand()
{
  this->InvokeObjectMethodCommand(this->DoubleClickCommand);
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::UpdateEnableState()
{
  this->Superclass::UpdateEnableState();

  this->PropagateEnableState(this->StartupPageCanvas);

  if (this->GetEnabled())
    {
    this->Bind();
    }
  else
    {
    this->UnBind();
    }
}

//----------------------------------------------------------------------------
int vtkKWStartupPageWidget::CanvasHasTag(const char *canvas, const char *tag)
{
  if (!this->IsCreated() || !canvas || !tag || !*tag)
    {
    return 0;
    }
  
  return atoi(this->Script("llength [%s find withtag %s]", canvas, tag));
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::CanvasDeleteTag(const char *canvas, const char *tag)
{
  if (!this->IsCreated() || !canvas || !tag || !*tag)
    {
    return;
    }
  
  this->Script("%s delete %s", canvas, tag);
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::ProcessCallbackCommandEvents(vtkObject *caller,
                                                          unsigned long event,
                                                          void *calldata)
{
  if (caller == this->MostRecentFilesManager)
    {
    switch (event)
      {
      case vtkKWMostRecentFilesManager::MenuHasChangedEvent:
        break;
      }
    }

  this->Superclass::ProcessCallbackCommandEvents(caller, event, calldata);
}

//----------------------------------------------------------------------------
void vtkKWStartupPageWidget::PrintSelf(
  ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "SupportOpen: "
     << (this->SupportOpen ? "On" : "Off") << endl;
  os << indent << "SupportDoubleClick: "<< this->SupportDoubleClick << endl;
  os << indent << "SupportDrop: "<< this->SupportDrop << endl;
  os << indent << "SupportMostRecentFiles: "
     << this->SupportMostRecentFiles << endl;
  os << indent << "GradientColor1: (" 
     << this->GradientColor1[0] << ", "
     << this->GradientColor1[1] << ", "
     << this->GradientColor1[2] << ") " << endl;
  os << indent << "GradientColor2: (" 
     << this->GradientColor2[0] << ", "
     << this->GradientColor2[1] << ", "
     << this->GradientColor2[2] << ") " << endl;
  os << indent << "TextColor: (" 
     << this->TextColor[0] << ", "
     << this->TextColor[1] << ", "
     << this->TextColor[2] << ") " << endl;
  os << indent << "SelectedTextColor: (" 
     << this->SelectedTextColor[0] << ", "
     << this->SelectedTextColor[1] << ", "
     << this->SelectedTextColor[2] << ") " << endl;
  os << indent << "HintColor: (" 
     << this->HintColor[0] << ", "
     << this->HintColor[1] << ", "
     << this->HintColor[2] << ") " << endl;
  os << indent << "StartupPageCanvas: ";
  if (this->StartupPageCanvas)
    {
    os << endl;
    this->StartupPageCanvas->PrintSelf(os, indent.GetNextIndent());
    }
  else
    {
    os << "None" << endl;
    }
}
