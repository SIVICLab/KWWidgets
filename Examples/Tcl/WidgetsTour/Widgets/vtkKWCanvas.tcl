proc vtkKWCanvasEntryPoint {parent win} {

  set app [$parent GetApplication] 

  # Create a canvas

  set canvas1 [vtkKWCanvas New]
  $canvas1 SetParent $parent
  $canvas1 Create
  $canvas1 SetWidth 400
  $canvas1 SetHeight 200
  $canvas1 SetBorderWidth 2
  $canvas1 SetReliefToGroove
  $canvas1 SetBackgroundColor 0.4 0.6 0.9

  pack [$canvas1 GetWidgetName] -side top -anchor nw -expand n -padx 2 -pady 2
  
  # Add some gradients

  $canvas1 AddVerticalRGBGradient 0.0  0.0  0.54  0.25  0.41  0.88  10  20  20  190  "gradient1"

  $canvas1 AddHorizontalRGBGradient 1.0  1.0  0.0  1.0  0.0  0.0  20  10  390  20  "gradient2"

  # There is no C++ API at the moment to access Tk's Canvas functions,
  # so let's just use Tk in the example:

  set wname [$canvas1 GetWidgetName] 

  $wname create arc 10 10 90 90 -start 20 -extent 120 -width 1

  $wname create line 45 55 140 150 -width 1 -fill #223344
  $wname create line 41 59 120 160 -width 2 -fill #445566
  $wname create line 37 63 100 170 -width 3 -fill #667788
  $wname create line 33 67 80  180 -width 4 -fill #8899AA
  $wname create line 29 71 60  190 -width 3 -fill #AABBCC
  $wname create line 25 75 40  200 -width 2 -fill #CCDDEE
  $wname create line 21 79 20  190 -width 1 -fill #EEFFFF

  $wname create oval 160 10 340 100 -outline #22BB33 -fill red -width 2

  $wname create polygon 360 80 380 100 350 130 390 160 -outline green

  $wname create rectangle 150 80 320 180 -outline #666666 -width 3

  # TODO: add a canvas with scrollbars
}

proc vtkKWCanvasGetType {} {
  return "TypeCore"
}
