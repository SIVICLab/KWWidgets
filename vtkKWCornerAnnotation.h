/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkKWCornerAnnotation.h,v $
  Language:  C++
  Date:      $Date: 2002-11-19 14:33:09 $
  Version:   $Revision: 1.20 $

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
// .NAME vtkKWCornerAnnotation - a corner annotation widget
// .SECTION Description
// A class that provides a UI for vtkCornerAnnotation. User can set the
// text for each corner, set the color of the text, and turn the annotation
// on and off.

#ifndef __vtkKWCornerAnnotation_h
#define __vtkKWCornerAnnotation_h

#include "vtkKWLabeledFrame.h"

class vtkCornerAnnotation;
class vtkKWChangeColorButton;
class vtkKWCheckButton;
class vtkKWFrame;
class vtkKWGenericComposite;
class vtkKWLabel;
class vtkKWScale;
class vtkKWText;
class vtkKWTextProperty;
class vtkKWView;

class VTK_EXPORT vtkKWCornerAnnotation : public vtkKWLabeledFrame
{
public:
  static vtkKWCornerAnnotation* New();
  vtkTypeRevisionMacro(vtkKWCornerAnnotation,vtkKWLabeledFrame);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Displays and/or updates the property ui display
  virtual void ShowProperties();

  // Description:
  // Create the properties object, called by ShowProperties.
  virtual void Create(vtkKWApplication *app, const char* args);

  // Description:
  // Close out and remove any composites prior to deletion.
  virtual void Close();

  // Description:
  // Set/Get the composite that owns this annotation
  void SetView(vtkKWView*);
  vtkGetObjectMacro(View,vtkKWView);

  // Description:
  // Callback functions used by the pro sheet
  virtual void SetCornerText(const char *txt, int corner);
  virtual char *GetCornerText(int i);
  virtual void CornerChanged(int i);
  virtual void OnDisplayCorner();
  virtual void SetVisibility(int i);
  virtual int  GetVisibility();
  vtkBooleanMacro(Visibility,int);
  
  // Description:
  // Chaining method to serialize an object and its superclasses.
  virtual void SerializeSelf(ostream& os, vtkIndent indent);
  virtual void SerializeToken(istream& is,const char token[1024]);
  virtual void SerializeRevision(ostream& os, vtkIndent indent);

  // Description:
  // Change the color of the annotation
  void SetTextColor(float r, float g, float b);
  void SetTextColor(float *rgb)
    { this->SetTextColor(rgb[0], rgb[1], rgb[2]); }
  float *GetTextColor();
  void OnTextColorChangeCallback();

  // Description:
  // Set/Get the maximum line height.
  void SetMaximumLineHeight(float);
  void SetMaximumLineHeightNoTrace(float);
  float GetMaximumLineHeight();
  void MaximumLineHeightCallback();
  void MaximumLineHeightEndCallback();

  // Description:
  // Get at the underlying vtkCornerAnnotationClass
  vtkGetObjectMacro(CornerProp,vtkCornerAnnotation);
  
  // Description:
  // GUI components access
  vtkGetObjectMacro(TextPropertyWidget, vtkKWTextProperty);
  void OnTextChangeCallback();

protected:
  vtkKWCornerAnnotation();
  ~vtkKWCornerAnnotation();

  vtkKWTextProperty *TextPropertyWidget;

  vtkKWCheckButton       *CornerVisibilityButton;

  vtkKWFrame             *CornerTopFrame;
  vtkKWFrame             *CornerBottomFrame;

  vtkKWFrame             *CornerFrame[4];
  vtkKWLabel             *CornerLabel[4];
  vtkKWText              *CornerText[4];
  vtkCornerAnnotation    *CornerProp;
  vtkKWGenericComposite  *CornerComposite;
  vtkKWScale             *MaximumLineHeightScale;

  vtkKWView *View;
private:
  vtkKWCornerAnnotation(const vtkKWCornerAnnotation&); // Not implemented
  void operator=(const vtkKWCornerAnnotation&); // Not Implemented
};


#endif



