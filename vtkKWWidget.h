/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkKWWidget.h,v $
  Language:  C++
  Date:      $Date: 2003-04-03 20:20:14 $
  Version:   $Revision: 1.47 $

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
// .NAME vtkKWWidget - superclass of KW widgets
// .SECTION Description
// This class is the superclass of all UI based objects in the
// Kitware toolkit. It contains common methods such as specifying
// the parent widget, generating and returning the Tcl widget name
// for an instance, and managing children. It overrides the 
// Unregister method to handle circular reference counts between
// child and parent widgets.

#ifndef __vtkKWWidget_h
#define __vtkKWWidget_h

#include "vtkKWObject.h"

class vtkKWWindow;
class vtkKWWidgetCollection;

//BTX
template<class DataType> class vtkLinkedList;
template<class DataType> class vtkLinkedListIterator;
//ETX

class VTK_EXPORT vtkKWWidget : public vtkKWObject
{
public:
  static vtkKWWidget* New();
  vtkTypeRevisionMacro(vtkKWWidget,vtkKWObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Create a Tk widget
  void Create(vtkKWApplication *app,const char *name, const char *args);
  int IsCreated() { return (this->Application != 0); }

  // Description:
  // Get the name of the underlying tk widget being used
  // the parent should be set before calling this method.
  const char *GetWidgetName();
  vtkKWWidget *GetChildWidgetWithName(const char *);

  // Description:
  // Set/Get the parent widget for this widget
  void SetParent(vtkKWWidget *p);
  vtkGetObjectMacro(Parent,vtkKWWidget);

  // Description:
  // Add/Remove/Get a child to this Widget
  void AddChild(vtkKWWidget *w);
  void RemoveChild(vtkKWWidget *w);
  vtkGetObjectMacro(Children,vtkKWWidgetCollection);
  
  // Description::
  // Override Unregister since widgets have loops.
  virtual void UnRegister(vtkObjectBase *o);

  // Description:
  // Get the net reference count of this widget. That is the
  // reference count of this widget minus its children.
  virtual int  GetNetReferenceCount();

  // Description:
  // A method to set callback functions on objects.  The first argument is
  // the KWObject that will have the method called on it.  The second is the
  // name of the method to be called and any arguments in string form.
  // The calling is done via TCL wrappers for the KWObject.
  virtual void SetCommand( vtkKWObject* Object, 
                           const char* MethodAndArgString);

  // Description:
  // Get the string type of a widget.
  virtual const char* GetType();
  
  // Description:
  // A method to set binding on the object.
  // This method sets binding:
  // bind this->GetWidgetName() event { object->GetTclName() command }
  void SetBind(vtkKWObject* object, const char *event, const char *command);

  // Description:
  // A method to set binding on the object.
  // This method sets binding:
  // bind this->GetWidgetName() event { command }  
  void SetBind(const char *event, const char *command);

  // Description:
  // A method to set binding on the object.
  // This method sets binding:
  // bind this->GetWidgetName() event { widget command }  
  void SetBind(const char *event, const char *widget, const char *command);

  // Description:
  // A method to set binding on the object.
  // This method sets binding:
  // bind all event { widget command }  
  void SetBindAll(const char *event, const char *widget, const char *command);

  // Description:
  // A method to set binding on the object.
  // This method sets binding:
  // bind all event { command }  
  void SetBindAll(const char *event, const char *command);

  // Description:
  // Set or get enabled state.
  virtual void SetEnabled(int);
  vtkBooleanMacro(Enabled, int);
  vtkGetMacro(Enabled, int);

  // Description:
  // Set focus to this widget.
  void Focus();

  // Description: a method to create a callback string from a KWObject.
  // The caller is resposible for deleting the returned string.  
  char* CreateCommand(vtkKWObject* Object, const char* MethodAndArgString);

  // Description:
  // Get the containing vtkKWWindow for this Widget if there is one.
  // NOTE: this may return NULL if the Widget is not in a window.
  vtkKWWindow* GetWindow();

  // Description:
  // Setting this string enables balloon help for this widget.
  virtual void SetBalloonHelpString(const char *str);
  vtkGetStringMacro(BalloonHelpString);

  // Description:
  // Convenience method to Set/Get the current background and foreground colors
  // of the widget (either using 0 -> 255 int, or normalized 0.0 -> 1.0 float).
  void GetBackgroundColor(int *r, int *g, int *b);
  void SetBackgroundColor(int r, int g, int b);
  void GetBackgroundColor(float *r, float *g, float *b);
  void SetBackgroundColor(float r, float g, float b);
  void GetForegroundColor(int *r, int *g, int *b);
  void SetForegroundColor(int r, int g, int b);
  void GetForegroundColor(float *r, float *g, float *b);
  void SetForegroundColor(float r, float g, float b);
  
  // Description:
  // Query if widget has Tk configuration option, and get the option as int
  int HasConfigurationOption(const char* option);
  int GetConfigurationOptionAsInt(const char* option);
  
  // Description:
  // Query if widget is "alive" (i.e. is created and 'winfo exists')
  int IsAlive();
  
  // Description:
  // Query if widget is mapped (on screen)
  int IsMapped();
  
  // Description:
  // Query if widget is packed
  int IsPacked();
  int GetNumberOfPackedChildren();

  // Description:
  // Unpack widget, unpack siblings (slave's of parent widget), unpack children
  void Unpack();
  void UnpackSiblings();
  void UnpackChildren();
  
  // Description:
  // Adjusts the placement of the baloon help
  vtkSetMacro(BalloonHelpJustification,int);
  vtkGetMacro(BalloonHelpJustification,int);  
  void SetBalloonHelpJustificationToLeft(){
    this->SetBalloonHelpJustification(0);};
  void SetBalloonHelpJustificationToRight(){
    this->SetBalloonHelpJustification(2);};
  
  // Description:
  // These method are for supporting tracing the widgets activity.
  // GetChildWidget is used only for initializing the widget relative
  // to its parent.  The trace name has to be set by the parent
  // for GetChildWidget to work.
  vtkSetStringMacro(TraceName);
  vtkGetStringMacro(TraceName);
  vtkKWWidget *GetChildWidgetWithTraceName(const char* traceName);

  // Description:
  // Tracing support specific to widgets.  This method will initialize
  // a widget useing its parent as reference.  This method returns 1
  // if the widget was initialized successfully.  This widget needs
  // a TraceName unique between the children of the parent in order for
  // this method to work.  The parent also has to be able to be initialized.
  // The "file" arguments extends the functinality to saving state
  // Into a tcl script.  I need a way to store "TraceInitialized"
  // for each file.
  virtual int InitializeTrace(ofstream* file);

  // Description:
  // Enable/disable Drag and Drop.
  virtual void SetEnableDragAndDrop(int);
  vtkBooleanMacro(EnableDragAndDrop, int);
  vtkGetMacro(EnableDragAndDrop, int);

  // Description:
  // Add/Query/Remove a Drag & Drop target. 
  virtual int AddDragAndDropTarget(vtkKWWidget *target);
  virtual int RemoveDragAndDropTarget(vtkKWWidget *target);
  virtual int HasDragAndDropTarget(vtkKWWidget *target);
  virtual int GetNumberOfDragAndDropTargets();

  // Description:
  // Set a Drag & Drop target callbacks/commands.
  // You have to add a target before settings its commands.
  // The StartCommand of all targets is called when Drag & Drop is initiated.
  // The PerformCommand of all targets is called while Drag & Drop is performed.
  // The EndCommand of all targets that contain the drop coordinates is called
  // when Drag & Drop is ended
  // Note that the each command is passed the absolute/screen (x,y) mouse 
  // coordinates, the current widget and the DragAndDropAnchor (which are the
  // same most of the times), i.e. the last 4 parameters are: int, int, 
  // vtkKWWidget*, vtkKWWidget*). Additionally, EndCommand is passed a 5th 
  // parameter, the target (vtkKWWidget *).
  virtual int SetDragAndDropStartCommand(
    vtkKWWidget *target, vtkKWObject *object, const char *method);
  virtual int SetDragAndDropPerformCommand(
    vtkKWWidget *target, vtkKWObject *object, const char *method);
  virtual int SetDragAndDropEndCommand(
    vtkKWWidget *target, vtkKWObject *object, const char *method);

  // Description:
  // Set/Get the Drag and Drop anchor. This is the actual widget (or part of)
  // that the user drags and drops. It defaults to the current widget (this),
  // but can be used to specify that a sub-part of the widget is the real
  // anchor.
  virtual void SetDragAndDropAnchor(vtkKWWidget*);
  vtkGetObjectMacro(DragAndDropAnchor, vtkKWWidget);

  // Description:
  // Drag and Drop callbacks
  virtual void DragAndDropStartCallback(int x, int y);
  virtual void DragAndDropPerformCallback(int x, int y);
  virtual void DragAndDropEndCallback(int x, int y);

protected:
  vtkKWWidget();
  ~vtkKWWidget();
  virtual void SerializeRevision(ostream& os, vtkIndent indent);

  char *WidgetName;
  vtkKWWidget *Parent;
  vtkKWWidgetCollection *Children; 
  int DeletingChildren;

  //BTX
  friend class vtkKWFrame;
  //ETX
  vtkSetStringMacro(WidgetName);

  // Ballon help
  char  *BalloonHelpString;
  int   BalloonHelpJustification;
  int   BalloonHelpInitialized;
  void  SetUpBalloonHelpBindings();
  
  // We need a unique way to get the widget from the parent.  This
  // is unfortunate, but necessary.  With out this name set, the
  // trace cannot be initialized for this widget.
  char *TraceName;
  int Enabled;

  // Update the enable state. This should propagate similar calls to the
  // internal widgets.
  virtual void UpdateEnableState();

  // Drag and Drop

  //BTX

  class DragAndDropTarget
  {
  public:
    vtkKWWidget *Target;
    char *StartCommand;
    char *PerformCommand;
    char *EndCommand;

    void SetStartCommand(const char*);
    void SetEndCommand(const char*);
    void SetPerformCommand(const char*);

    DragAndDropTarget();
    ~DragAndDropTarget();
  };

  typedef vtkLinkedList<DragAndDropTarget*> DragAndDropTargetsContainer;
  typedef vtkLinkedListIterator<DragAndDropTarget*> DragAndDropTargetsContainerIterator;
  DragAndDropTargetsContainer *DragAndDropTargets;

  DragAndDropTarget* GetDragAndDropTarget(vtkKWWidget *target);

  //ETX

  int EnableDragAndDrop;
  vtkKWWidget *DragAndDropAnchor;

  virtual void SetDragAndDropBindings();
  virtual void RemoveDragAndDropBindings();
  virtual void DeleteDragAndDropTargets();

private:
  vtkKWWidget(const vtkKWWidget&); // Not implemented
  void operator=(const vtkKWWidget&); // Not implemented
};


#endif


