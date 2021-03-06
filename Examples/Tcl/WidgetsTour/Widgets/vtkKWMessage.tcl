proc vtkKWMessageEntryPoint {parent win} {

  set app [$parent GetApplication] 

  set lorem_ipsum "Lorem ipsum dolor sit amet consectetuer adipiscing elit. Nunc felis. Nulla gravida. Aliquam erat volutpat. Mauris accumsan quam non sem. Sed commodo magna quis bibendum lacinia elit turpis iaculis augue eget hendrerit elit dui vel elit.\n\nInteger ante eros auctor eu dapibus ac ultricies vitae lacus. Fusce accumsan mauris. Morbi felis. Class aptent taciti sociosqu ad litora torquent per conubia nostra per inceptos hymenaeos. Maecenas convallis imperdiet nunc."

  # -----------------------------------------------------------------------

  # Create a message

  set message1 [vtkKWMessage New]
  $message1 SetParent $parent
  $message1 Create
  $message1 SetText $lorem_ipsum
  $message1 SetWidth 400
  $message1 SetBalloonHelpString \
    "A message. The width is explicitly set to a given number of pixels"

  pack [$message1 GetWidgetName] -side top -anchor nw -expand n -padx 2 -pady 2

  # -----------------------------------------------------------------------

  # Create another message right justify it

  set message2 [vtkKWMessage New]
  $message2 SetParent $parent
  $message2 Create
  $message2 SetText $lorem_ipsum
  $message2 SetJustificationToLeft
  $message2 SetAnchorToEast
  $message2 SetAspectRatio 200
  $message2 SetBackgroundColor 0.7 0.7 0.95
  $message2 SetBorderWidth 2
  $message2 SetReliefToGroove
  $message2 SetBalloonHelpString \
    "Another message no explicit width is specified but the aspect ratio\
    is set so that the text is twice as wide as it is tall left-justified,\
    and anchored to the right side of its full length if packed to fill\
    that horizontal space."

  pack [$message2 GetWidgetName] -side top -anchor nw -expand n -fill x -padx 2 -pady 6

  # -----------------------------------------------------------------------

  # Create another message with a label this time

  set message4 [vtkKWMessageWithLabel New]
  $message4 SetParent $parent
  $message4 Create
  [$message4 GetLabel] SetImageToPredefinedIcon 61
  [$message4 GetWidget] SetText $lorem_ipsum
  $message4 SetBalloonHelpString \
    "This is a vtkKWMessageWithLabel i.e. a message associated to a\
    label that can be positioned around the message. This can be used for\
    example to prefix a message with a small icon to emphasize its meaning.\
    Predefined icons include warning info error etc."

  pack [$message4 GetWidgetName] -side top -anchor nw -expand n -padx 2 -pady 6
}

proc vtkKWMessageGetType {} {
  return "TypeCore"
}
