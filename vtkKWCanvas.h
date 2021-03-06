/*=========================================================================

Copyright (c) 1998-2003 Kitware Inc. 469 Clifton Corporate Parkway,
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
// .NAME vtkKWCanvas - canvas widget
// .SECTION Description
// A simple widget that represents a canvas. 
// .SECTION Thanks
// This work is part of the National Alliance for Medical Image
// Computing (NAMIC), funded by the National Institutes of Health
// through the NIH Roadmap for Medical Research, Grant U54 EB005149.
// Information on the National Centers for Biomedical Computing
// can be obtained from http://nihroadmap.nih.gov/bioinformatics.

#ifndef __vtkKWCanvas_h
#define __vtkKWCanvas_h

#include "vtkKWCoreWidget.h"

class vtkColorTransferFunction;

class KWWidgets_EXPORT vtkKWCanvas : public vtkKWCoreWidget
{
public:
  static vtkKWCanvas* New();
  vtkTypeMacro(vtkKWCanvas,vtkKWCoreWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the width and height of the canvas.
  // No effect before Create() is called.
  virtual void SetWidth(int);
  virtual int GetWidth();
  virtual void SetHeight(int);
  virtual int GetHeight();

  // Description:
  // Add horizontal or vertical gradient (constrained to a rectange).
  // The arguments x1, y1, x2, and y2 give the coordinates of two diagonally
  // opposite corners of the rectangle. 
  virtual int AddHorizontalGradient(vtkColorTransferFunction *ctf, 
                                    int x1, int y1, int x2, int y2, 
                                    const char *tag);
  virtual int AddHorizontalRGBGradient(double r1, double g1, double b1, 
                                       double r2, double g2, double b2, 
                                       int x1, int y1, int x2, int y2, 
                                       const char *tag);
  virtual int AddVerticalGradient(vtkColorTransferFunction *ctf, 
                                  int x1, int y1, int x2, int y2, 
                                  const char *tag);
  virtual int AddVerticalRGBGradient(double r1, double g1, double b1, 
                                     double r2, double g2, double b2, 
                                     int x1, int y1, int x2, int y2, 
                                     const char *tag);

  // Description:
  // Query if the canvas has a tag
  virtual int HasTag(const char *tag);

  // Description:
  // Delete a tag in the canvas
  virtual void DeleteTag(const char *tag);

  // Description:
  // Set/Get the background color of the widget.
  virtual void GetBackgroundColor(double *r, double *g, double *b);
  virtual double* GetBackgroundColor();
  virtual void SetBackgroundColor(double r, double g, double b);
  virtual void SetBackgroundColor(double rgb[3])
    { this->SetBackgroundColor(rgb[0], rgb[1], rgb[2]); };
  
  // Description:
  // Set/Get the highlight thickness, a non-negative value indicating the
  // width of the highlight rectangle to draw around the outside of the
  // widget when it has the input focus.
  virtual void SetHighlightThickness(int);
  virtual int GetHighlightThickness();
  
  // Description:
  // Set/Get the border width, a non-negative value indicating the width
  // of the 3-D border to draw around the outside of the widget (if such
  // a border is being drawn; the Relief option typically determines this).
  virtual void SetBorderWidth(int);
  virtual int GetBorderWidth();
  
  // Description:
  // Set/Get the 3-D effect desired for the widget. 
  // The value indicates how the interior of the widget should appear
  // relative to its exterior. 
  // Valid constants can be found in vtkKWOptions::ReliefType.
  virtual void SetRelief(int);
  virtual int GetRelief();
  virtual void SetReliefToRaised();
  virtual void SetReliefToSunken();
  virtual void SetReliefToFlat();
  virtual void SetReliefToRidge();
  virtual void SetReliefToSolid();
  virtual void SetReliefToGroove();

  // Description:
  // Set/add/remove a binding to all items matching a tag in the canvas widget; 
  // that command is invoked whenever the 'event' is triggered on the tag.
  // SetBinding will replace any old bindings, whereas AddBinding will
  // add the binding to the list of bindings already defined for that event.
  // RemoveBinding can remove a specific binding or all bindings for an event.
  // The 'object' argument is the object that will have the method called on
  // it. The 'method' argument is the name of the method to be called and any
  // arguments in string form. If the object is NULL, the method is still
  // evaluated as a simple command. 
  virtual void SetCanvasBinding(
    const char *tag, const char *event, vtkObject *object, const char *method);
  virtual void SetCanvasBinding(
    const char *tag, const char *event, const char *command);
  virtual const char* GetCanvasBinding(const char *tag, const char *event);
  virtual void AddCanvasBinding(
    const char *tag, const char *event, vtkObject *object, const char *method);
  virtual void AddCanvasBinding(
    const char *tag, const char *event, const char *command);
  virtual void RemoveCanvasBinding(const char *tag, const char *event);
  virtual void RemoveCanvasBinding(
    const char *tag, const char *event, vtkObject *object, const char *method);

  // Description:
  // Update the "enable" state of the object and its internal parts.
  // Depending on different Ivars (this->Enabled, the application's 
  // Limited Edition Mode, etc.), the "enable" state of the object is updated
  // and propagated to its internal parts/subwidgets. This will, for example,
  // enable/disable parts of the widget UI, enable/disable the visibility
  // of 3D widgets, etc.
  virtual void UpdateEnableState();

protected:
  vtkKWCanvas() {};
  ~vtkKWCanvas() {};

  // Description:
  // Create the widget.
  virtual void CreateWidget();

  // Description:
  // Add gradient.
  virtual int AddGradient(vtkColorTransferFunction *ctf, 
                          int x1, int y1, int x2, int y2, 
                          const char *tag,
                          int horizontal);

private:
  vtkKWCanvas(const vtkKWCanvas&); // Not implemented
  void operator=(const vtkKWCanvas&); // Not implemented
};


#endif



