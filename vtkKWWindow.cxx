/*=========================================================================

  Module:    $RCSfile: vtkKWWindow.cxx,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkKWWindow.h"

#include "vtkKWApplication.h"
#include "vtkKWCheckButton.h"
#include "vtkKWEvent.h"
#include "vtkKWFrame.h"
#include "vtkKWIcon.h"
#include "vtkKWLabel.h"
#include "vtkKWLoadSaveDialog.h"
#include "vtkKWMenu.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWMostRecentFilesManager.h"
#include "vtkKWNotebook.h"
#include "vtkKWProgressGauge.h"
#include "vtkKWSplitFrame.h"
#include "vtkKWTclInteractor.h"
#include "vtkKWTkUtilities.h"
#include "vtkKWToolbar.h"
#include "vtkKWToolbarSet.h"
#include "vtkKWUserInterfaceNotebookManager.h"
#include "vtkObjectFactory.h"

#include <kwsys/SystemTools.hxx>

#define VTK_KW_HIDE_MAIN_PANEL_LABEL "Hide Left Panel" 
#define VTK_KW_SHOW_MAIN_PANEL_LABEL "Show Left Panel"
#define VTK_KW_WINDOW_DEFAULT_WIDTH 900
#define VTK_KW_WINDOW_DEFAULT_HEIGHT 700

vtkCxxRevisionMacro(vtkKWWindow, "$Revision: 1.239 $");

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWWindow );

int vtkKWWindowCommand(ClientData cd, Tcl_Interp *interp,
                       int argc, char *argv[]);

//----------------------------------------------------------------------------
vtkKWWindow::vtkKWWindow()
{
  // Menus

  this->FileMenu              = vtkKWMenu::New();
  this->HelpMenu              = vtkKWMenu::New();
  this->EditMenu              = NULL;
  this->ViewMenu              = NULL;
  this->WindowMenu            = NULL;

  // Toolbars

  this->Toolbars               = vtkKWToolbarSet::New();
  this->ToolbarsVisibilityMenu = NULL; 
  this->MenuBarSeparatorFrame  = vtkKWFrame::New();

  // Main split panel

  this->MainSplitFrame        = vtkKWSplitFrame::New();

  // Status frame

  this->StatusFrameSeparator  = vtkKWFrame::New();
  this->StatusFrame           = vtkKWFrame::New();
  this->StatusLabel           = vtkKWLabel::New();
  this->StatusImage           = vtkKWLabel::New();

  this->ProgressFrame         = vtkKWFrame::New();
  this->ProgressGauge         = vtkKWProgressGauge::New();

  this->TrayFrame             = vtkKWFrame::New();
  this->TrayImageError        = vtkKWLabel::New();

  this->MainNotebook          = vtkKWNotebook::New();

  this->TclInteractor         = NULL;

  this->CommandFunction       = vtkKWWindowCommand;

  this->PrintTargetDPI        = 100.0;
  this->SupportHelp           = 1;
  this->SupportPrint          = 1;
  this->PromptBeforeClose     = 0;

  this->MostRecentFilesManager = vtkKWMostRecentFilesManager::New();

  this->SetWindowClass("KitwareWidget");

  this->ScriptExtension       = NULL;
  this->SetScriptExtension(".tcl");

  this->ScriptType            = NULL;
  this->SetScriptType("Tcl");
}

//----------------------------------------------------------------------------
vtkKWWindow::~vtkKWWindow()
{
  this->PrepareForDelete();

  if (this->TclInteractor)
    {
    this->TclInteractor->Delete();
    this->TclInteractor = NULL;
    }

  if (this->MainNotebook)
    {
    this->MainNotebook->Delete();
    this->MainNotebook = NULL;
    }

  if (this->FileMenu)
    {
    this->FileMenu->Delete();
    this->FileMenu = NULL;
    }

  if (this->HelpMenu)
    {
    this->HelpMenu->Delete();
    this->HelpMenu = NULL;
    }

  if (this->Toolbars)
    {
    this->Toolbars->Delete();
    this->Toolbars = NULL;
    }

  if (this->MenuBarSeparatorFrame)
    {
    this->MenuBarSeparatorFrame->Delete();
    this->MenuBarSeparatorFrame = NULL;
    }

  if (this->MainSplitFrame)
    {
    this->MainSplitFrame->Delete();
    this->MainSplitFrame = NULL;
    }

  if (this->StatusFrameSeparator)
    {
    this->StatusFrameSeparator->Delete();
    this->StatusFrameSeparator = NULL;
    }

  if (this->StatusFrame)
    {
    this->StatusFrame->Delete();
    this->StatusFrame = NULL;
    }

  if (this->StatusImage)
    {
    this->StatusImage->Delete();
    this->StatusImage = NULL;
    }

  if (this->StatusLabel)
    {
    this->StatusLabel->Delete();
    this->StatusLabel = NULL;
    }

  if (this->ProgressFrame)
    {
    this->ProgressFrame->Delete();
    this->ProgressFrame = NULL;
    }

  if (this->ProgressGauge)
    {
    this->ProgressGauge->Delete();
    this->ProgressGauge = NULL;
    }

  if (this->TrayFrame)
    {
    this->TrayFrame->Delete();
    this->TrayFrame = NULL;
    }

  if (this->TrayImageError)
    {
    this->TrayImageError->Delete();
    this->TrayImageError = NULL;
    }

  if (this->EditMenu)
    {
    this->EditMenu->Delete();
    this->EditMenu = NULL;
    }

  if (this->ViewMenu)
    {
    this->ViewMenu->Delete();
    this->ViewMenu = NULL;
    }

  if (this->WindowMenu)
    {
    this->WindowMenu->Delete();
    this->WindowMenu = NULL;
    }

  if (this->ToolbarsVisibilityMenu)
    {
    this->ToolbarsVisibilityMenu->Delete();
    this->ToolbarsVisibilityMenu = NULL;
    }

  if (this->MostRecentFilesManager)
    {
    this->MostRecentFilesManager->Delete();
    this->MostRecentFilesManager = NULL;
    }

  this->SetScriptExtension(NULL);
  this->SetScriptType(NULL);
}

//----------------------------------------------------------------------------
void vtkKWWindow::Create(vtkKWApplication *app, const char *args)
{
  // Check if already created

  if (this->IsCreated())
    {
    vtkErrorMacro("class already created");
    return;
    }

  // Call the superclass to create the whole widget

  this->Superclass::Create(app, args);

  const char *wname = this->GetWidgetName();
  kwsys_stl::string cmd;
  kwsys_stl::string label;
  
  this->SetIconName(app->GetPrettyName());

  // Menu : File

  this->FileMenu->SetParent(this->GetMenu());
  this->FileMenu->SetTearOff(0);
  this->FileMenu->Create(app, NULL);

  this->GetMenu()->AddCascade("File", this->FileMenu, 0);

  if (this->SupportPrint)
    {
    this->FileMenu->AddCommand(
      VTK_KW_PRINT_OPTIONS_MENU_LABEL, this, "PrintOptionsCallback", 4);
    this->FileMenu->AddSeparator();
    }

  this->FileMenu->AddCommand("Close", this, "Close", 0);
  this->FileMenu->AddCommand("Exit", this->GetApplication(), "Exit", 1);

  this->MostRecentFilesManager->SetApplication(app);

  // Menu : Window

  this->GetWindowMenu()->AddCommand(
    VTK_KW_HIDE_MAIN_PANEL_LABEL, this, "MainPanelVisibilityCallback", 1);

  // Menu : Help

  this->HelpMenu->SetParent(this->GetMenu());
  this->HelpMenu->SetTearOff(0);
  this->HelpMenu->Create(app, NULL);

  if (this->SupportHelp)
    {
    this->GetMenu()->AddCascade("Help", this->HelpMenu, 0);
    }

  cmd = "DisplayHelpDialog ";
  cmd += this->GetTclName();
  this->HelpMenu->AddCommand(
    "Help", this->GetApplication(), cmd.c_str(), 0);

  if (app->HasCheckForUpdates())
    {
    this->HelpMenu->AddCommand("Check For Updates", app, "CheckForUpdates", 0);
    }

  this->HelpMenu->AddSeparator();
  label = "About ";
  label += app->GetPrettyName();
  cmd = "DisplayAboutDialog ";
  cmd += this->GetTclName();
  this->HelpMenu->AddCommand(
    label.c_str(), this->GetApplication(), cmd.c_str(), 0);

  // Menubar separator

  this->MenuBarSeparatorFrame->SetParent(this);  
#if defined(_WIN32)
  this->MenuBarSeparatorFrame->Create(app, "-height 2 -bd 1 -relief groove");
#else
  this->MenuBarSeparatorFrame->Create(app, "-height 2 -bd 1 -relief sunken");
#endif

  this->Script("pack %s -side top -fill x -pady 2",
               this->MenuBarSeparatorFrame->GetWidgetName());

  // Toolbars

  this->Toolbars->SetParent(this);  
  this->Toolbars->Create(app, NULL);
  this->Toolbars->ShowBottomSeparatorOn();
  this->Toolbars->SynchronizeToolbarsVisibilityWithRegistryOn();
  this->Toolbars->SetToolbarVisibilityChangedCommand(
    this, "ToolbarVisibilityChangedCallback");
  this->Toolbars->SetNumberOfToolbarsChangedCommand(
    this, "NumberOfToolbarsChangedCallback");

  // Split frame

  this->MainSplitFrame->SetParent(this);
  this->MainSplitFrame->Create(app);

#if 1
  this->Script("pack %s -side top -fill both -expand t",
               this->MainSplitFrame->GetWidgetName());
#endif

  // Restore Window Geometry

  if (app->GetSaveWindowGeometry())
    {
    this->RestoreWindowGeometry();
    }
  else
    {
    this->SetSize(VTK_KW_WINDOW_DEFAULT_WIDTH, VTK_KW_WINDOW_DEFAULT_HEIGHT);
    }

  // Create the notebook

  this->MainNotebook->SetParent(this->GetMainPanelFrame());
  this->MainNotebook->Create(app, NULL);

  this->Script("pack %s -pady 0 -padx 0 -fill both -expand yes -anchor n",
               this->MainNotebook->GetWidgetName());

  this->MainNotebook->AlwaysShowTabsOn();

  // Status frame

  this->StatusFrame->SetParent(this);
  this->StatusFrame->Create(app, NULL);
  
  this->Script("pack %s -side bottom -fill x -pady 0",
               this->StatusFrame->GetWidgetName());
  
  // Status frame separator

  this->StatusFrameSeparator->SetParent(this);
  this->StatusFrameSeparator->Create(app, "-height 2 -bd 1");
#if defined(_WIN32)
  this->StatusFrameSeparator->ConfigureOptions("-relief groove");
#else
  this->StatusFrameSeparator->ConfigureOptions("-relief sunken");
#endif

  this->Script("pack %s -side bottom -fill x -pady 2",
               this->StatusFrameSeparator->GetWidgetName());
  
  // Status frame : image

  this->StatusImage->SetParent(this->StatusFrame);
  this->StatusImage->Create(app, "-relief sunken -bd 1");

  this->UpdateStatusImage();

  this->Script("pack %s -side left -anchor c -ipadx 1 -ipady 1 -fill y", 
               this->StatusImage->GetWidgetName());

  // Status frame : label

  this->StatusLabel->SetParent(this->StatusFrame);
  this->StatusLabel->Create(app, "-relief sunken -bd 0 -padx 3 -anchor w");

  this->Script("pack %s -side left -padx 1 -expand yes -fill both",
               this->StatusLabel->GetWidgetName());

  // Status frame : progress frame

  this->ProgressFrame->SetParent(this->StatusFrame);
  this->ProgressFrame->Create(app, "-relief sunken -bd 1");

  this->Script("pack %s -side left -padx 0 -fill y", 
               this->ProgressFrame->GetWidgetName());

  // Status frame : progress frame : gauge

  this->ProgressGauge->SetParent(this->ProgressFrame);
  this->ProgressGauge->SetLength(200);
  this->ProgressGauge->SetHeight(
    vtkKWTkUtilities::GetPhotoHeight(this->StatusImage) - 4);
  this->ProgressGauge->Create(app, NULL);

  this->Script("pack %s -side right -padx 2 -pady 2",
               this->ProgressGauge->GetWidgetName());

  // Status frame : tray frame

  this->TrayFrame->SetParent(this->StatusFrame);
  this->TrayFrame->Create(app, "-relief sunken -bd 1");

  this->Script(
   "pack %s -side left -ipadx 0 -ipady 0 -padx 0 -pady 0 -fill both",
   this->TrayFrame->GetWidgetName());

  // Status frame : tray frame : error image

  this->TrayImageError->SetParent(this->TrayFrame);
  this->TrayImageError->Create(app, NULL);

  this->TrayImageError->SetImageOption(vtkKWIcon::ICON_SMALLERRORRED);
  
  this->TrayImageError->SetBind(this, "<Button-1>", "ErrorIconCallback");

  // Printer settings

  if (this->GetApplication()->HasRegistryValue(
        2, "RunTime", VTK_KW_PRINT_TARGET_DPI_REG_KEY))
    {
    this->SetPrintTargetDPI(
      this->GetApplication()->GetFloatRegistryValue(
        2, "RunTime", VTK_KW_PRINT_TARGET_DPI_REG_KEY));
    }
  
  // If we have a User Interface Manager, it's time to create it

  vtkKWUserInterfaceManager *uim = this->GetUserInterfaceManager();
  if (uim && !uim->IsCreated())
    {
    uim->Create(app);
    }

  vtkKWUserInterfaceNotebookManager *uim_nb = 
    vtkKWUserInterfaceNotebookManager::SafeDownCast(uim);
  if (uim_nb)
    {
    if (this->GetApplication()->HasRegistryValue(
          2, "RunTime", VTK_KW_ENABLE_GUI_DRAG_AND_DROP_REG_KEY))
      {
      uim_nb->SetEnableDragAndDrop(
        this->GetApplication()->GetIntRegistryValue(
          2, "RunTime", VTK_KW_ENABLE_GUI_DRAG_AND_DROP_REG_KEY));
      }
    }

  // Udpate the enable state

  this->UpdateEnableState();
}

//----------------------------------------------------------------------------
void vtkKWWindow::PrepareForDelete()
{
  // Have reference to this object:
  // this->Menu
  // this->MenuBarSeparatorFrame
  // this->Toolbars
  // this->MainSplitFrame
  // this->StatusFrameSeparator
  // this->StatusFrame

  if (this->TclInteractor )
    {
    this->TclInteractor->SetMasterWindow(NULL);
    this->TclInteractor->Delete();
    this->TclInteractor = NULL;
    }
}

//----------------------------------------------------------------------------
vtkKWFrame* vtkKWWindow::GetMainPanelFrame()
{
  return this->MainSplitFrame ? this->MainSplitFrame->GetFrame1() : NULL;
}

//----------------------------------------------------------------------------
vtkKWFrame* vtkKWWindow::GetViewFrame()
{
  return this->MainSplitFrame ? this->MainSplitFrame->GetFrame2() : NULL;
}

//----------------------------------------------------------------------------
int vtkKWWindow::DisplayCloseDialog()
{
  vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
  dialog->SetStyleToYesNo();
  dialog->SetMasterWindow(this);
  dialog->SetOptions(
    vtkKWMessageDialog::QuestionIcon | 
    vtkKWMessageDialog::Beep | 
    vtkKWMessageDialog::YesDefault);
  dialog->Create(this->GetApplication(), NULL);
  dialog->SetText("Are you sure you want to close this window?");
  dialog->SetTitle("Close");
  int ret = dialog->Invoke();
  dialog->Delete();
  return ret;
}

//----------------------------------------------------------------------------
int vtkKWWindow::Close()
{
  // If a dialog is still up, complain and bail
  // This should be fixed, since we don't know here if the dialog that is
  // up is something that was created by this instance, and not by another
  // window instance (in the later case, it would be safe to close)

  if (this->GetApplication()->GetDialogUp())
    {
    this->Script("bell");
    return 0;
    }

  // Prompt confirmation if needed

  if (this->PromptBeforeClose && !this->DisplayCloseDialog())
    {
    return 0;
    }

  // Save its geometry

  if (this->GetApplication()->GetSaveWindowGeometry())
    {
    this->SaveWindowGeometry();
    }

  // Save print settings

  this->GetApplication()->SetRegistryValue(
    2, "RunTime", VTK_KW_PRINT_TARGET_DPI_REG_KEY, "%lf", 
    this->PrintTargetDPI);

  // Remove this window from the application. 
  // It is likely that the application will exit if there are no more windows.

  return this->GetApplication()->RemoveWindow(this);
}

//----------------------------------------------------------------------------
void vtkKWWindow::SaveWindowGeometry()
{
  if (this->IsCreated())
    {
    kwsys_stl::string geometry = this->GetGeometry();
    this->GetApplication()->SetRegistryValue(
      2, "Geometry", VTK_KW_WINDOW_GEOMETRY_REG_KEY, "%s", geometry.c_str());

    this->GetApplication()->SetRegistryValue(
      2, "Geometry", VTK_KW_MAIN_PANEL_SIZE_REG_KEY, "%d", 
      this->MainSplitFrame->GetFrame1Size());
    }
}

//----------------------------------------------------------------------------
void vtkKWWindow::RestoreWindowGeometry()
{
  if (this->IsCreated())
    {
    if (this->GetApplication()->HasRegistryValue(
          2, "Geometry", VTK_KW_WINDOW_GEOMETRY_REG_KEY))
      {
      char geometry[40];
      if (this->GetApplication()->GetRegistryValue(
            2, "Geometry", VTK_KW_WINDOW_GEOMETRY_REG_KEY, geometry))
        {
        this->SetGeometry(geometry);
        }
      }
    else
      {
      this->SetSize(VTK_KW_WINDOW_DEFAULT_WIDTH, VTK_KW_WINDOW_DEFAULT_HEIGHT);
      }

    if (this->GetApplication()->HasRegistryValue(
          2, "Geometry", VTK_KW_MAIN_PANEL_SIZE_REG_KEY))
      {
      int reg_size = this->GetApplication()->GetIntRegistryValue(
        2, "Geometry", VTK_KW_MAIN_PANEL_SIZE_REG_KEY);
      if (reg_size >= this->MainSplitFrame->GetFrame1MinimumSize())
        {
        this->MainSplitFrame->SetFrame1Size(reg_size);
        }
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWWindow::Render()
{
}

//----------------------------------------------------------------------------
void vtkKWWindow::SetStatusText(const char *text)
{
  this->StatusLabel->SetText(text);
}

//----------------------------------------------------------------------------
const char *vtkKWWindow::GetStatusText()
{
  return this->StatusLabel->GetText();
}

//----------------------------------------------------------------------------
vtkKWMenu *vtkKWWindow::GetEditMenu()
{
  if (!this->EditMenu)
    {
    this->EditMenu = vtkKWMenu::New();
    }

  if (!this->EditMenu->IsCreated() && this->GetMenu() && this->IsCreated())
    {
    this->EditMenu->SetParent(this->GetMenu());
    this->EditMenu->SetTearOff(0);
    this->EditMenu->Create(this->GetApplication(), NULL);
    this->GetMenu()->InsertCascade(1, "Edit", this->EditMenu, 0);
    }
  
  return this->EditMenu;
}

//----------------------------------------------------------------------------
vtkKWMenu *vtkKWWindow::GetViewMenu()
{
  if (!this->ViewMenu)
    {
    this->ViewMenu = vtkKWMenu::New();
    }

  if (!this->ViewMenu->IsCreated() && this->GetMenu() && this->IsCreated())
    {
    this->ViewMenu->SetParent(this->GetMenu());
    this->ViewMenu->SetTearOff(0);
    this->ViewMenu->Create(this->GetApplication(), NULL);
    this->GetMenu()->InsertCascade(
      1 + (this->EditMenu ? 1 : 0), "View", this->ViewMenu, 0);
    }

  return this->ViewMenu;
}

//----------------------------------------------------------------------------
vtkKWMenu *vtkKWWindow::GetWindowMenu()
{
  if (!this->WindowMenu)
    {
    this->WindowMenu = vtkKWMenu::New();
    }

  if (!this->WindowMenu->IsCreated() && this->GetMenu() && this->IsCreated())
    {
    this->WindowMenu->SetParent(this->GetMenu());
    this->WindowMenu->SetTearOff(0);
    this->WindowMenu->Create(this->GetApplication(), NULL);
    this->GetMenu()->InsertCascade(
      1 + (!this->EditMenu ? 1 : 0), "Window", this->WindowMenu, 0);
    }
  
  return this->WindowMenu;
}

//----------------------------------------------------------------------------
vtkKWMenu *vtkKWWindow::GetToolbarsVisibilityMenu()
{
  if (!this->ToolbarsVisibilityMenu)
    {
    this->ToolbarsVisibilityMenu = vtkKWMenu::New();
    }

  if (!this->ToolbarsVisibilityMenu->IsCreated() && 
      this->GetWindowMenu() &&
      this->IsCreated())
    {
    this->ToolbarsVisibilityMenu->SetParent(this->GetWindowMenu());
    this->ToolbarsVisibilityMenu->SetTearOff(0);
    this->ToolbarsVisibilityMenu->Create(this->GetApplication(), NULL);
    this->GetWindowMenu()->InsertCascade(
      2, " Toolbars", this->ToolbarsVisibilityMenu, 1, 
      "Set Toolbars Visibility");
    }
  
  return this->ToolbarsVisibilityMenu;
}

//----------------------------------------------------------------------------
void vtkKWWindow::NumberOfToolbarsChangedCallback()
{
  this->Toolbars->PopulateToolbarsVisibilityMenu(
    this->GetToolbarsVisibilityMenu());
}

//----------------------------------------------------------------------------
void vtkKWWindow::ToolbarVisibilityChangedCallback()
{
  this->Toolbars->UpdateToolbarsVisibilityMenu(
    this->GetToolbarsVisibilityMenu());

  this->UpdateToolbarState();
}

//----------------------------------------------------------------------------
int vtkKWWindow::GetMainPanelVisibility()
{
  return (this->MainSplitFrame && 
          this->MainSplitFrame->GetFrame1Visibility() ? 1 : 0);
}

//----------------------------------------------------------------------------
void vtkKWWindow::SetMainPanelVisibility(int arg)
{
  if (arg == this->GetMainPanelVisibility())
    {
    return;
    }

  if (this->MainSplitFrame)
    {
    this->MainSplitFrame->SetFrame1Visibility(arg);
    }

  this->UpdateMenuState();
}

//----------------------------------------------------------------------------
void vtkKWWindow::MainPanelVisibilityCallback()
{
  this->SetMainPanelVisibility(!this->GetMainPanelVisibility());
}

//----------------------------------------------------------------------------
void vtkKWWindow::LoadScript()
{
  vtkKWLoadSaveDialog* load_dialog = vtkKWLoadSaveDialog::New();
  this->GetApplication()->RetrieveDialogLastPathRegistryValue(
    load_dialog, "LoadScriptLastPath");
  load_dialog->SetParent(this);
  load_dialog->Create(this->GetApplication(), NULL);
  load_dialog->SaveDialogOff();
  load_dialog->SetTitle("Load Script");
  load_dialog->SetDefaultExtension(this->ScriptExtension);

  kwsys_stl::string filetypes;
  filetypes += "{{";
  filetypes += this->ScriptType;
  filetypes += " Scripts} {";
  filetypes += this->ScriptExtension;
  filetypes += "}} {{All Files} {.*}}";
  load_dialog->SetFileTypes(filetypes.c_str());

  int enabled = this->GetEnabled();
  this->SetEnabled(0);

  if (load_dialog->Invoke() && 
      load_dialog->GetFileName() && 
      strlen(load_dialog->GetFileName()) > 0)
    {
    if (!kwsys::SystemTools::FileExists(load_dialog->GetFileName()))
      {
      vtkWarningMacro("Unable to open script file!");
      }
    else
      {
      this->GetApplication()->SaveDialogLastPathRegistryValue(
        load_dialog, "LoadScriptLastPath");
      this->LoadScript(load_dialog->GetFileName());
      }
    }

  this->SetEnabled(enabled);
  load_dialog->Delete();
}

//----------------------------------------------------------------------------
void vtkKWWindow::LoadScript(const char *filename)
{
  this->GetApplication()->LoadScript(filename);
}

//----------------------------------------------------------------------------
void vtkKWWindow::InsertRecentFilesMenu(
  int pos, vtkKWObject *target, const char *label, int underline)
{
  if (!this->IsCreated() || !label || !this->FileMenu || 
      !this->MostRecentFilesManager)
    {
    return;
    }

  // Create the sub-menu if not done already

  vtkKWMenu *mrf_menu = this->MostRecentFilesManager->GetMenu();
  if (!mrf_menu->IsCreated())
    {
    mrf_menu->SetParent(this->FileMenu);
    mrf_menu->SetTearOff(0);
    mrf_menu->Create(this->GetApplication(), NULL);
    }

  // Remove the menu if already there (in case that function was used to
  // move the menu)

  if (this->FileMenu->HasItem(label))
    {
    this->FileMenu->DeleteMenuItem(label);
    }

  this->FileMenu->InsertCascade(pos, label, mrf_menu, underline);

  // Fill the recent files vector with recent files stored in registry
  // this will also update the menu

  this->MostRecentFilesManager->SetDefaultTargetObject(target);
  this->MostRecentFilesManager->LoadFilesFromRegistry();
}

//----------------------------------------------------------------------------
void vtkKWWindow::AddRecentFile(const char *name, 
                                vtkKWObject *target,
                                const char *command)
{  
  if (this->MostRecentFilesManager)
    {
    this->MostRecentFilesManager->AddFile(name, target, command);
    this->MostRecentFilesManager->SaveFilesToRegistry();
    }
}

//----------------------------------------------------------------------------
int vtkKWWindow::GetFileMenuInsertPosition()
{
  if (!this->IsCreated())
    {
    return 0;
    }

  // First find the print-related menu commands

  if (this->GetFileMenu()->HasItem(VTK_KW_PRINT_OPTIONS_MENU_LABEL))
    {
    return this->GetFileMenu()->GetIndex(VTK_KW_PRINT_OPTIONS_MENU_LABEL);
    }

  // Otherwise find Close or Exit if Close was removed

  if (this->GetFileMenu()->HasItem("Close"))
    {
    return this->GetFileMenu()->GetIndex("Close");  
    }

  if (this->GetFileMenu()->HasItem("Exit"))
    {
    return this->GetFileMenu()->GetIndex("Exit");  
    }

  return this->HelpMenu->GetNumberOfItems();
}

//----------------------------------------------------------------------------
int vtkKWWindow::GetHelpMenuInsertPosition()
{
  if (!this->IsCreated())
    {
    return 0;
    }

  // Find about

  if (this->HelpMenu->HasItem("About*"))
    {
    return this->HelpMenu->GetIndex("About*") - 1;
    }

  return this->HelpMenu->GetNumberOfItems();
}

//----------------------------------------------------------------------------
void vtkKWWindow::WarningMessage(const char* message)
{
  vtkKWMessageDialog::PopupMessage(
    this->GetApplication(), this, "Warning",
    message, vtkKWMessageDialog::WarningIcon);
  this->SetErrorIcon(vtkKWWindow::ERROR_ICON_RED);
  this->InvokeEvent(vtkKWEvent::WarningMessageEvent, (void*)message);
}

//----------------------------------------------------------------------------
void vtkKWWindow::ErrorMessage(const char* message)
{
  vtkKWMessageDialog::PopupMessage(
    this->GetApplication(), this, "Error",
    message, vtkKWMessageDialog::ErrorIcon);
  this->SetErrorIcon(vtkKWWindow::ERROR_ICON_RED);
  this->InvokeEvent(vtkKWEvent::ErrorMessageEvent, (void*)message);
}

//----------------------------------------------------------------------------
void vtkKWWindow::SetErrorIcon(int s)
{
  if (!this->TrayImageError || !this->TrayImageError->IsCreated())
    {
    return;
    }

  if (s > vtkKWWindow::ERROR_ICON_NONE) 
    {
    this->Script("pack %s -fill both -ipadx 4 -expand yes", 
                 this->TrayImageError->GetWidgetName());
    if (s == vtkKWWindow::ERROR_ICON_RED)
      {
      this->TrayImageError->SetImageOption(vtkKWIcon::ICON_SMALLERRORRED);
      }
    else if (s == vtkKWWindow::ERROR_ICON_BLACK)
      {
      this->TrayImageError->SetImageOption(vtkKWIcon::ICON_SMALLERROR);
      }
    }
  else
    {
    this->Script("pack forget %s", this->TrayImageError->GetWidgetName());
    }
}

//----------------------------------------------------------------------------
void vtkKWWindow::ErrorIconCallback()
{
  this->SetErrorIcon(vtkKWWindow::ERROR_ICON_BLACK);
}

//----------------------------------------------------------------------------
char* vtkKWWindow::GetTitle()
{
  if (!this->Title && 
      this->GetApplication() && 
      this->GetApplication()->GetName())
    {
    return this->GetApplication()->GetName();
    }
  return this->Title;
}

//----------------------------------------------------------------------------
void vtkKWWindow::DisplayTclInteractor()
{
  if ( ! this->TclInteractor )
    {
    this->TclInteractor = vtkKWTclInteractor::New();
    kwsys_stl::string title;
    if (this->GetTitle())
      {
      title += this->GetTitle();
      title += " : ";
      }
    title += "Command Prompt";
    this->TclInteractor->SetTitle(title.c_str());
    this->TclInteractor->SetMasterWindow(this);
    this->TclInteractor->Create(this->GetApplication(), NULL);
    }
  
  this->TclInteractor->Display();
}

//----------------------------------------------------------------------------
void vtkKWWindow::UpdateStatusImage()
{
  if (!this->StatusImage || !this->StatusImage->IsCreated())
    {
    return;
    }

  // Create an empty image for now. It will be modified later on

  kwsys_stl::string image_name(
    this->Script("%s cget -image", this->StatusImage->GetWidgetName()));
  if (!image_name.size() || !*image_name.c_str())
    {
    image_name = this->Script("image create photo");
    }

  this->Script("%s configure "
               "-image %s "
               "-fg white -bg white "
               "-highlightbackground white -highlightcolor white "
               "-highlightthickness 0 "
               "-padx 0 -pady 0", 
               this->StatusImage->GetWidgetName(),
               image_name.c_str());
}

//----------------------------------------------------------------------------
void vtkKWWindow::Update()
{
  // Update the whole interface

  if (this->GetUserInterfaceManager())
    {
    // Redundant Update() here, since we call UpdateEnableState(), which as 
    // a side effect will update each panel (see UpdateEnableState())
    // this->GetUserInterfaceManager()->Update();
    }

  // Make sure everything is enable/disable accordingly

  this->UpdateEnableState();
}

//----------------------------------------------------------------------------
void vtkKWWindow::UpdateToolbarState()
{
  if (!this->Toolbars)
    {
    return;
    }

  int flat_frame;
  if (this->GetApplication()->HasRegistryValue(
    2, "RunTime", VTK_KW_TOOLBAR_FLAT_FRAME_REG_KEY))
    {
    flat_frame = this->GetApplication()->GetIntRegistryValue(
      2, "RunTime", VTK_KW_TOOLBAR_FLAT_FRAME_REG_KEY);
    }
  else
    {
    flat_frame = vtkKWToolbar::GetGlobalFlatAspect();
    }

  int flat_buttons;
  if (this->GetApplication()->HasRegistryValue(
    2, "RunTime", VTK_KW_TOOLBAR_FLAT_BUTTONS_REG_KEY))
    {
    flat_buttons = this->GetApplication()->GetIntRegistryValue(
      2, "RunTime", VTK_KW_TOOLBAR_FLAT_BUTTONS_REG_KEY);
    }
  else
    {
    flat_buttons = vtkKWToolbar::GetGlobalWidgetsFlatAspect();
    }

  this->Toolbars->SetToolbarsFlatAspect(flat_frame);
  this->Toolbars->SetToolbarsWidgetsFlatAspect(flat_buttons);

  // The split frame packing mechanism is so weird that I will have
  // to unpack the toolbar frame myself in case it's empty, otherwise
  // the middle frame won't claim the space used by the toolbar frame

  if (this->Toolbars->IsCreated())
    {
    if (this->Toolbars->GetNumberOfVisibleToolbars())
      {
      this->Script(
        "pack %s -padx 0 -pady 0 -side top -fill x -expand no -after %s",
        this->Toolbars->GetWidgetName(),
        this->MenuBarSeparatorFrame->GetWidgetName());
      this->Toolbars->PackToolbars();
      }
    else
      {
      this->Toolbars->Unpack();
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWWindow::UpdateEnableState()
{
  this->Superclass::UpdateEnableState();

  // Update the toolbars

  if (this->Toolbars)
    {
    this->PropagateEnableState(this->Toolbars);
    this->UpdateToolbarState();
    }

  // Update the notebook

  this->PropagateEnableState(this->MainNotebook);

  // Update all the user interface panels

  if (this->GetUserInterfaceManager())
    {
    this->GetUserInterfaceManager()->SetEnabled(this->GetEnabled());
    // As a side effect, SetEnabled() call an Update() on the panel, 
    // which will call an UpdateEnableState() too,
    // this->GetUserInterfaceManager()->UpdateEnableState();
    // this->GetUserInterfaceManager()->Update();
    }

  // Update the Tcl interactor

  this->PropagateEnableState(this->TclInteractor);

  // Update the window element

  this->PropagateEnableState(this->MainSplitFrame);
  this->PropagateEnableState(this->StatusFrame);
  this->PropagateEnableState(this->MenuBarSeparatorFrame);

  // Do not disable the status image, it has not functionality attached 
  // anyway, and is used to display the application logo: disabling it 
  // only makes it look fairly ugly/dithered.
  // this->PropagateEnableState(this->StatusImage);

  // Given the state, can we close or not ?

  this->SetDeleteWindowProtocolCommand(
    this, this->GetEnabled() ? 
    "Close" : "SetStatusText \"Can not close while UI is disabled\"");

  // Update the menus

  this->UpdateMenuState();
}

//----------------------------------------------------------------------------
void vtkKWWindow::UpdateMenuState()
{
  this->PropagateEnableState(this->FileMenu);
  this->PropagateEnableState(this->EditMenu);
  this->PropagateEnableState(this->ViewMenu);
  this->PropagateEnableState(this->WindowMenu);
  this->PropagateEnableState(this->HelpMenu);
  this->PropagateEnableState(this->ToolbarsVisibilityMenu);

  // Most Recent Files

  if (this->MostRecentFilesManager)
    {
    this->PropagateEnableState(this->MostRecentFilesManager->GetMenu());
    this->MostRecentFilesManager->UpdateMenuStateInParent();
    }

  // Update the About entry, since the pretty name also depends on the
  // limited edition mode

  if (this->HelpMenu)
    {
    kwsys_stl::string about_command = "DisplayAbout ";
    about_command +=  this->GetTclName();
    int pos = this->HelpMenu->GetIndexOfCommand(
      this->GetApplication(), about_command.c_str());
    if (pos >= 0)
      {
      kwsys_stl::string label("-label {About ");
      label += this->GetApplication()->GetPrettyName();
      label += "}";
      this->HelpMenu->ConfigureItem(pos, label.c_str());
      }
    }

  if (this->WindowMenu)
    {
    kwsys_stl::string label("-label {");
    label += this->GetMainPanelVisibility()
      ? VTK_KW_HIDE_MAIN_PANEL_LABEL : VTK_KW_SHOW_MAIN_PANEL_LABEL;
    label += "}";
    this->WindowMenu->ConfigureItem(0, label.c_str());
    }
}

//----------------------------------------------------------------------------
void vtkKWWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Menu: " << this->GetMenu() << endl;
  os << indent << "FileMenu: " << this->GetFileMenu() << endl;
  os << indent << "HelpMenu: " << this->GetHelpMenu() << endl;
  os << indent << "MainNotebook: " << this->GetMainNotebook() << endl;
  os << indent << "MainSplitFrame: " << this->GetMainSplitFrame() << endl;
  os << indent << "PrintTargetDPI: " << this->GetPrintTargetDPI() << endl;
  os << indent << "ProgressGauge: " << this->GetProgressGauge() << endl;
  os << indent << "PromptBeforeClose: " << this->GetPromptBeforeClose() 
     << endl;
  os << indent << "ScriptExtension: " << this->GetScriptExtension() << endl;
  os << indent << "ScriptType: " << this->GetScriptType() << endl;
  os << indent << "SupportHelp: " << this->GetSupportHelp() << endl;
  os << indent << "SupportPrint: " << this->GetSupportPrint() << endl;
  os << indent << "StatusFrame: " << this->GetStatusFrame() << endl;
  os << indent << "WindowClass: " << this->GetWindowClass() << endl;  
  os << indent << "TclInteractor: " << this->GetTclInteractor() << endl;
  os << indent << "Toolbars: " << this->GetToolbars() << endl;
}


