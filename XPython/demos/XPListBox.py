# http://www.xsquawkbox.net/xpsdk/mediawiki/TestWidgets
from enum import IntEnum
try:
    from OpenGL import GL
except ImportError:
    print("OpenGL not found. Do 'python -m pip install PyOpenGL' ... on windows, install as administrator.")
    raise
import xp

xpWidgetClass_ListBox = 10019  # not used


class Prop(IntEnum):
    # Properties greater than xpProperty_UserStart will be treated as Python Objects
    # rather than ints by XPPython3.
    # By convention, properties are numbered:
    # between (widgetClass * 100) + 1000 and (widgetClass * 100) + 1099)

    # This is the item number of the current item, starting at 0.
    ListBoxCurrentItem = 1002900
    # This will add an item to the list box at the end.
    ListBoxAddItem = 1002901
    # This will clear the list box and then add the items.
    ListBoxAddItemsWithClear = 1002902
    # This will clear the list box.
    ListBoxClear = 1002903
    # This will insert an item into the list box at the index.
    ListBoxInsertItem = 1002904
    # This will delete an item from the list box at the index.
    ListBoxDeleteItem = 1002905
    # This stores the pointer to the listbox data.
    ListBoxData = 1002906
    # This stores the max Listbox Items.
    ListBoxMaxListBoxItems = 1002907
    # This stores the highlight state.
    ListBoxHighlighted = 1002908
    # This stores the scrollbar Min.
    ListBoxScrollBarMin = 1002909
    # This stores the scrollbar Max.
    ListBoxScrollBarMax = 1002910
    # This stores the scrollbar SliderPosition.
    ListBoxScrollBarSliderPosition = 1002911
    # This stores the scrollbar ScrollBarPageAmount.
    ListBoxScrollBarPageAmount = 1002912
    # This stores the scrollbar ScrollBarSlop
    ListBoxScrollBarSlop = 1002913


xpMessage_ListBoxItemSelected = 1001900


def XPListBoxProc(message, widget, param1, param2):
    CurrentItem = 0
    Max = 0
    SliderPosition = 0

    try:
        if xp.selectIfNeeded(message, widget, param1, param2, 0):
            return 1
    except SystemError:
        print("Failure in selectIfNeeded for message: {}, {}, {}, {}".format(message, widget, param1, param2))
        raise

    Left, Top, Right, Bottom = xp.getWidgetGeometry(widget)
    SliderPosition = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, None)
    Min = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarMin, None)
    Max = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarMax, None)
    ScrollBarPageAmount = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarPageAmount, None)
    CurrentItem = xp.getWidgetProperty(widget, Prop.ListBoxCurrentItem, None)
    MaxListBoxItems = xp.getWidgetProperty(widget, Prop.ListBoxMaxListBoxItems, None)
    Highlighted = xp.getWidgetProperty(widget, Prop.ListBoxHighlighted, None)
    ListBoxDataObj = xp.getWidgetProperty(widget, Prop.ListBoxData, None)
    ScrollBarSlop = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarSlop, None)
    FontWidth, FontHeight, _other = xp.getFontDimensions(xp.Font_Basic)
    listbox_item_height = int(FontHeight * 1.2)

    if message == xp.Msg_Create:
        ListBoxDataObj = {'Items': [], 'Lefts': [], 'Rights': []}
        descriptor = xp.getWidgetDescriptor(widget)
        XPListBoxFillWithData(ListBoxDataObj, descriptor, (Right - Left - 20))
        xp.setWidgetProperty(widget, Prop.ListBoxData, ListBoxDataObj)

        xp.setWidgetProperty(widget, Prop.ListBoxCurrentItem, CurrentItem)

        Min = 0
        Max = len(ListBoxDataObj['Items'])
        ScrollBarSlop = 0
        Highlighted = False
        SliderPosition = Max
        MaxListBoxItems = int((Top - Bottom) / listbox_item_height)
        ScrollBarPageAmount = MaxListBoxItems

        xp.setWidgetProperty(widget, Prop.ListBoxScrollBarMin, Min)
        xp.setWidgetProperty(widget, Prop.ListBoxScrollBarMax, Max)
        xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSlop, ScrollBarSlop)
        xp.setWidgetProperty(widget, Prop.ListBoxHighlighted, Highlighted)
        xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, SliderPosition)
        xp.setWidgetProperty(widget, Prop.ListBoxMaxListBoxItems, MaxListBoxItems)
        xp.setWidgetProperty(widget, Prop.ListBoxScrollBarPageAmount, ScrollBarPageAmount)

        return 1

    if message == xp.Msg_DescriptorChanged:
        return 1

    if message == xp.Msg_PropertyChanged:
        if xp.getWidgetProperty(widget, Prop.ListBoxAddItem, None):
            xp.setWidgetProperty(widget, Prop.ListBoxAddItem, 0)  # unset it
            descriptor = xp.getWidgetDescriptor(widget)
            XPListBoxAddItem(ListBoxDataObj, descriptor, (Right - Left - 20))
            Max = len(ListBoxDataObj['Items'])
            SliderPosition = Max
            xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, SliderPosition)
            xp.setWidgetProperty(widget, Prop.ListBoxScrollBarMax, Max)

        if xp.getWidgetProperty(widget, Prop.ListBoxAddItemsWithClear, None):
            xp.setWidgetProperty(widget, Prop.ListBoxAddItemsWithClear, 0)  # unset it
            descriptor = xp.getWidgetDescriptor(widget)
            XPListBoxClear(ListBoxDataObj)
            XPListBoxFillWithData(ListBoxDataObj, descriptor, (Right - Left - 20))
            Max = len(ListBoxDataObj['Items'])
            SliderPosition = Max
            xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, SliderPosition)
            xp.setWidgetProperty(widget, Prop.ListBoxScrollBarMax, Max)

        if xp.getWidgetProperty(widget, Prop.ListBoxClear, None):
            xp.setWidgetProperty(widget, Prop.ListBoxClear, 0)
            xp.setWidgetProperty(widget, Prop.ListBoxCurrentItem, 0)
            XPListBoxClear(ListBoxDataObj)
            Max = len(ListBoxDataObj['Items'])
            SliderPosition = Max
            xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, SliderPosition)
            xp.setWidgetProperty(widget, Prop.ListBoxScrollBarMax, Max)

        if xp.getWidgetProperty(widget, Prop.ListBoxInsertItem, None):
            xp.setWidgetProperty(widget, Prop.ListBoxInsertItem, 0)
            descriptor = xp.getWidgetDescriptor(widget)
            XPListBoxInsertItem(ListBoxDataObj, descriptor, (Right - Left - 20), CurrentItem)

        if xp.getWidgetProperty(widget, Prop.ListBoxDeleteItem, None):
            xp.setWidgetProperty(widget, Prop.ListBoxDeleteItem, 0)
            if ListBoxDataObj['Items'] and len(ListBoxDataObj['Items']) > CurrentItem:
                XPListBoxDeleteItem(ListBoxDataObj, CurrentItem)

        return 1

    if message == xp.Msg_Draw:
        _x, y = xp.getMouseLocationGlobal()

        xp.drawWindow(Left, Bottom, Right - 20, Top, xp.Window_ListView)
        xp.drawTrack(Right - 20, Bottom, Right, Top, Min, Max, SliderPosition, xp.Track_ScrollBar, Highlighted)

        xp.setGraphicsState(0, 1, 0, 0, 1, 0, 0)
        GL.glColor4f(1.0, 1.0, 1.0, 1.0)

        xp.setGraphicsState(0, 0, 0, 0, 0, 0, 0)

        # Now draw each item.
        ListBoxIndex = Max - SliderPosition
        ItemNumber = 0
        while ItemNumber < MaxListBoxItems:
            if ListBoxIndex < len(ListBoxDataObj['Items']):
                # Calculate the item rect in global coordinates.
                ItemTop = int(Top - (ItemNumber * listbox_item_height))
                ItemBottom = int(Top - ((ItemNumber * listbox_item_height) + listbox_item_height))

                # If we are hilited, draw the hilite bkgnd.
                if CurrentItem == ListBoxIndex:
                    SetAlphaLevels(0.25)
                    xp.setGraphicsState(0, 0, 0, 0, 1, 0, 0)
                    SetupAmbientColor(XP_Color.MenuHilite, True)
                    SetAlphaLevels(1.0)
                    GL.glBegin(GL.GL_QUADS)
                    GL.glVertex2i(Left, ItemTop)
                    GL.glVertex2i(Right - 20, ItemTop)
                    GL.glVertex2i(Right - 20, ItemBottom)
                    GL.glVertex2i(Left, ItemBottom)
                    GL.glEnd()

                text = SetupAmbientColor(XP_Color.ListText)

                ListBoxWidth = (Right - 20) - Left
                FontWidth, FontHeight, _other = xp.getFontDimensions(xp.Font_Basic)
                MaxChars = int(ListBoxWidth / FontWidth)
                Buffer = ListBoxDataObj['Items'][ListBoxIndex][0:MaxChars]
                ListBoxIndex += 1
                xp.drawString(text,
                              Left, ItemBottom + 2,
                              Buffer, None, xp.Font_Basic)
            ItemNumber += 1
        return 1

    if message == xp.Msg_MouseUp:
        if IN_RECT(MOUSE_X(param1), MOUSE_Y(param1), Right - 20, Top, Right, Bottom):
            Highlighted = False
            xp.setWidgetProperty(widget, Prop.ListBoxHighlighted, Highlighted)

        if IN_RECT(MOUSE_X(param1), MOUSE_Y(param1), Left, Top, Right - 20, Bottom):
            if ListBoxDataObj['Items']:
                if CurrentItem != -1:
                    xp.setWidgetDescriptor(widget, ListBoxDataObj['Items'][CurrentItem])
                else:
                    xp.setWidgetDescriptor(widget, "")
                xp.sendMessageToWidget(widget, xpMessage_ListBoxItemSelected, xp.Mode_UpChain, widget, CurrentItem)
        return 1

    if message == xp.Msg_MouseDown:
        if IN_RECT(MOUSE_X(param1), MOUSE_Y(param1), Left, Top, Right - 20, Bottom):
            if ListBoxDataObj['Items']:
                x, y = xp.getMouseLocationGlobal()
                ListBoxDataOffset = XPListBoxGetItemNumber(ListBoxDataObj, listbox_item_height, x - Left, Top - y)
                if ListBoxDataOffset != -1:
                    ListBoxDataOffset += Max - SliderPosition
                    if ListBoxDataOffset < len(ListBoxDataObj['Items']):
                        xp.setWidgetProperty(widget, Prop.ListBoxCurrentItem, ListBoxDataOffset)

        if IN_RECT(MOUSE_X(param1), MOUSE_Y(param1), Right - 20, Top, Right, Bottom):
            tm = xp.getTrackMetrics(Right - 20, Bottom, Right, Top, Min, Max, SliderPosition, xp.Track_ScrollBar)
            Min = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarMin, None)
            Max = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarMax, None)
            if tm.isVertical:
                UpBtnSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                        Right - 20, Top, Right, Top - tm.upBtnSize)
                DownBtnSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                          Right - 20, Bottom + tm.downBtnSize, Right, Bottom)
                UpPageSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                         Right - 20, (Top - tm.upBtnSize), Right, (Bottom + tm.downBtnSize + tm.downPageSize + tm.thumbSize))
                DownPageSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                           Right - 20, (Top - tm.upBtnSize - tm.upPageSize - tm.thumbSize), Right, (Bottom + tm.downBtnSize))
                ThumbSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                        Right - 20, (Top - tm.upBtnSize - tm.upPageSize),
                                        Right, (Bottom + tm.downBtnSize + tm.downPageSize))
            else:
                DownBtnSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                          Right - 20, Top, Right - 20 + tm.upBtnSize, Bottom)
                UpBtnSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                        Right - 20 - tm.downBtnSize, Top, Right, Bottom)
                DownPageSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                           Right - 20 + tm.downBtnSize, Top,
                                           Right - tm.upBtnSize - tm.upPageSize - tm.thumbSize, Bottom)
                UpPageSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                         Right - 20 + tm.downBtnSize + tm.downPageSize + tm.thumbSize, Top,
                                         Right - tm.upBtnSize, Bottom)
                ThumbSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                        Right - 20 + tm.downBtnSize + tm.downPageSize, Top,
                                        Right - tm.upBtnSize - tm.upPageSize, Bottom)

            if UpPageSelected:
                SliderPosition += ScrollBarPageAmount
                if SliderPosition > Max:
                    SliderPosition = Max
                xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, SliderPosition)
            elif DownPageSelected:
                SliderPosition -= ScrollBarPageAmount
                if SliderPosition < Min:
                    SliderPosition = Min
                xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, SliderPosition)
            elif UpBtnSelected:
                SliderPosition += 1
                if SliderPosition > Max:
                    SliderPosition = Max
                xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, SliderPosition)
            elif DownBtnSelected:
                SliderPosition -= 1
                if SliderPosition < Min:
                    SliderPosition = Min
                xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, SliderPosition)
            elif ThumbSelected:
                if tm.isVertical:
                    ScrollBarSlop = int(Bottom + tm.downBtnSize + tm.downPageSize +
                                        (tm.thumbSize / 2) - MOUSE_Y(param1))
                else:
                    ScrollBarSlop = int(Right - 20 + tm.downBtnSize + tm.downPageSize +
                                        (tm.thumbSize / 2) - MOUSE_X(param1))
                Highlighted = True
                xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSlop, ScrollBarSlop)
                xp.setWidgetProperty(widget, Prop.ListBoxHighlighted, Highlighted)
            else:
                Highlighted = False
                xp.setWidgetProperty(widget, Prop.ListBoxHighlighted, Highlighted)
        return 1
    if message == xp.Msg_MouseDrag:
        if IN_RECT(MOUSE_X(param1), MOUSE_Y(param1), Right - 20, Top, Right, Bottom):
            tm = xp.getTrackMetrics(Right - 20, Bottom, Right, Top, Min, Max, SliderPosition, xp.Track_ScrollBar)
            Min = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarMin, None)
            Max = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarMax, None)

            ThumbSelected = Highlighted
            if ThumbSelected:
                if param1 != 0:
                    if tm.isVertical:
                        y = MOUSE_Y(param1) + ScrollBarSlop
                        SliderPosition = round(float(float(y - (Bottom + tm.downBtnSize + tm.thumbSize / 2)) /
                                                     float((Top - tm.upBtnSize - tm.thumbSize / 2) - (Bottom + tm.downBtnSize + tm.thumbSize / 2))) * Max)
                    else:
                        x = MOUSE_X(param1) + ScrollBarSlop
                        SliderPosition = round((float)((float)(x - (Right - 20 + tm.downBtnSize + tm.thumbSize / 2)) / (float)((Right - tm.upBtnSize - tm.thumbSize / 2) - (Right - 20 + tm.downBtnSize + tm.thumbSize / 2))) * Max)
                else:
                    SliderPosition = 0

                if SliderPosition < Min:
                    SliderPosition = Min
                if SliderPosition > Max:
                    SliderPosition = Max
                xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, SliderPosition)
        return 1
    return 0


def XPCreateListBox(left, top, right, bottom, visible, descriptor, container):
    return xp.createCustomWidget(left, top, right, bottom, visible, descriptor, 0, container, XPListBoxProc)


def XPListBoxGetItemNumber(ListBoxData, listbox_item_height, inX, inY):
    """
    This routine finds the item that is in a given point, or returns -1 if there is none.
    It simply trolls through all the items.
    """
    for n in range(len(ListBoxData['Items'])):
        if all([inX >= ListBoxData['Lefts'][n],
                inX < ListBoxData['Rights'][n],
                inY >= (n * listbox_item_height),
                inY < (n * listbox_item_height) + listbox_item_height]):
            return n
    return -1


def XPListBoxFillWithData(ListBoxData, items, width):
    for item in items.split(';'):
        ListBoxData['Items'].append(item)
        ListBoxData['Lefts'].append(0)
        ListBoxData['Rights'].append(width)


def XPListBoxAddItem(ListBoxData, Buffer, width):
    ListBoxData['Items'].append(Buffer)
    ListBoxData['Lefts'].append(0)
    ListBoxData['Rights'].append(width)


def XPListBoxClear(ListBoxData):
    ListBoxData['Items'] = []
    ListBoxData['Lefts'] = []
    ListBoxData['Rights'] = []


def XPListBoxInsertItem(ListBoxData, Buffer, Width, CurrentItem):
    ListBoxData['Items'].insert(CurrentItem, Buffer)
    ListBoxData['Lefts'].insert(CurrentItem, 0)
    ListBoxData['Rights'].insert(CurrentItem, Width)


def XPListBoxDeleteItem(ListBoxData, CurrentItem):
    ListBoxData['Items'].pop(CurrentItem)
    ListBoxData['Lefts'].pop(CurrentItem)
    ListBoxData['Rights'].pop(CurrentItem)


# /************************************************************************
#  *  X-PLANE UI INFRASTRUCTURE CODE
#  ************************************************************************
#  *
#  * This code helps provde an x-plane compatible look.  It is copied from
#  * the source code from the widgets DLL; someday listyboxes will be part of
#  * this, so our listboxes are written off of the same APIs.
#  *
#  */
# Enums for x-plane native colors.
class XP_Color(IntEnum):
    MenuDarkTinge = 0
    MenuBkgnd = 1
    MenuHilite = 2
    MenuLiteTinge = 3
    MenuText = 4
    MenuTextDisabled = 5
    SubTitleText = 6
    TabFront = 7
    TabBack = 8
    CaptionText = 9
    ListText = 10
    GlassText = 11
    Count = 12


kXPlaneColorNames = [
    "sim/graphics/colors/menu_dark_rgb",
    "sim/graphics/colors/menu_bkgnd_rgb",
    "sim/graphics/colors/menu_hilite_rgb",
    "sim/graphics/colors/menu_lite_rgb",
    "sim/graphics/colors/menu_text_rgb",
    "sim/graphics/colors/menu_text_disabled_rgb",
    "sim/graphics/colors/subtitle_text_rgb",
    "sim/graphics/colors/tab_front_rgb",
    "sim/graphics/colors/tab_back_rgb",
    "sim/graphics/colors/caption_text_rgb",
    "sim/graphics/colors/list_text_rgb",
    "sim/graphics/colors/glass_text_rgb"
]


# This array contains the resolved datarefs
gColorRefs = []


def SetupAmbientColor(inColorID, immediate=False):
    """
    This routine sets up a color from the above table.  Pass
    in a float[3] to get the color; pass in NULL to have the
    OpenGL color be set immediately.
    """
    # If we're running the first time, resolve all of our datarefs just once.
    if not gColorRefs:
        for n in range(XP_Color.Count):
            gColorRefs.append(xp.findDataRef(kXPlaneColorNames[n]))

    target = []
    xp.getDatavf(gColorRefs[inColorID], target, 0, 3)

    # If the user passed NULL, set the color now using the alpha level.
    if immediate:
        target.append(gAlphaLevel)
        GL.glColor4fv(target)
    return target[0:3]


# Current alpha levels to blit at.
gAlphaLevel = 1.0


def SetAlphaLevels(inAlphaLevel):
    # Just remember alpha levels for later.
    global gAlphaLevel
    gAlphaLevel = inAlphaLevel


def IN_RECT(x, y, l, t, r, b):
    return ((x) >= (l)) and ((x) <= (r)) and ((y) >= (b)) and ((y) <= (t))


def MOUSE_X(param1):
    return param1[0]


def MOUSE_Y(param1):
    return param1[1]
