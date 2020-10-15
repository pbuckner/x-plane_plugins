# http://www.xsquawkbox.net/xpsdk/mediawiki/TestWidgets
from enum import IntEnum
try:
    from OpenGL import GL
except ImportError:
    print("[XPPython3] OpenGL not found. Use XPPython3 Pip Package Installer to install 'PyOpenGL' package and restart.")
    raise
from XPPython3 import xp

# To use:
# Where you want a list box widget, call
#   widget = XPCreateListBox(left, top, right, bottom, visible, container)
# you'll get a XPListBox instance, NOT a widget id (which will be widget.widgetID, if you ever need it)
#   widget.clear() to clear contents
#   widget.add() to add a line to the end
# The rest is internal.

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


class XPListBox(object):
    def __init__(self, left, top, right, bottom, visible, descriptor, container, autoScroll=True):
        """
        Same parameters as createCustomWidget
        descriptor: Initial entry (can be '')
        autoScroll: True:
                     On input, if we're currently able to view last entry, scroll such that we'll continue
                     to view the last entries ('follow'). Otherwise, it will not scroll the list
                     automatically
                    False:
                     On input, _always_ return to top of list -- user can manually scroll, but
                     as data is added, we scroll back to the top.

        The actual widget is stored in self.widgetID

        Generally, you'll call .add() to add a line (one at at time!) to the scrolling listbox
        or .clear() to clear all lines.

        (interface hasn't been updated to support updating / deleting a subset of lines)
        """
        self.widgetID = xp.createCustomWidget(left, top, right, bottom, visible, descriptor, 0, container, self.listBoxProc)
        self.autoScroll = autoScroll

    def clear(self):
        xp.setWidgetProperty(self.widgetID, Prop.ListBoxClear, 1)

    def add(self, s):
        xp.setWidgetDescriptor(self.widgetID, s)
        xp.setWidgetProperty(self.widgetID, Prop.ListBoxAddItem, 1)

    def listBoxProc(self, message, widget, param1, param2):
        currentItem = 0
        scrollbarMax = 0
        sliderPosition = 0

        try:
            if xp.selectIfNeeded(message, widget, param1, param2, 0):
                return 1
        except SystemError:
            print("Failure in selectIfNeeded for message: {}, {}, {}, {}".format(message, widget, param1, param2))
            raise

        left, top, right, bottom = xp.getWidgetGeometry(widget)
        sliderPosition = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, None)
        scrollbarMin = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarMin, None)
        scrollbarMax = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarMax, None)
        scrollBarPageAmount = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarPageAmount, None)
        currentItem = xp.getWidgetProperty(widget, Prop.ListBoxCurrentItem, None)
        maxListBoxItems = xp.getWidgetProperty(widget, Prop.ListBoxMaxListBoxItems, None)
        highlighted = xp.getWidgetProperty(widget, Prop.ListBoxHighlighted, None)
        listBoxDataObj = xp.getWidgetProperty(widget, Prop.ListBoxData, None)
        scrollBarSlop = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarSlop, None)
        fontWidth, fontHeight, _other = xp.getFontDimensions(xp.Font_Basic)
        listbox_item_height = int(fontHeight * 1.2)

        if message == xp.Msg_Create:
            listBoxDataObj = {'Items': [], 'Lefts': [], 'Rights': []}
            descriptor = xp.getWidgetDescriptor(widget)
            self.listBoxFillWithData(listBoxDataObj, descriptor, (right - left - 20))
            xp.setWidgetProperty(widget, Prop.ListBoxData, listBoxDataObj)

            xp.setWidgetProperty(widget, Prop.ListBoxCurrentItem, currentItem)

            scrollbarMin = 0
            scrollbarMax = len(listBoxDataObj['Items'])
            scrollBarSlop = 0
            highlighted = False
            sliderPosition = scrollbarMax
            maxListBoxItems = int((top - bottom) / listbox_item_height)
            scrollBarPageAmount = maxListBoxItems

            xp.setWidgetProperty(widget, Prop.ListBoxScrollBarMin, scrollbarMin)
            xp.setWidgetProperty(widget, Prop.ListBoxScrollBarMax, scrollbarMax)
            xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSlop, scrollBarSlop)
            xp.setWidgetProperty(widget, Prop.ListBoxHighlighted, highlighted)
            xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, sliderPosition)
            xp.setWidgetProperty(widget, Prop.ListBoxMaxListBoxItems, maxListBoxItems)
            xp.setWidgetProperty(widget, Prop.ListBoxScrollBarPageAmount, scrollBarPageAmount)

            return 1

        if message == xp.Msg_DescriptorChanged:
            return 1

        if message == xp.Msg_PropertyChanged:
            if xp.getWidgetProperty(widget, Prop.ListBoxAddItem, None):

                wasViewingBottom = maxListBoxItems - sliderPosition > 0
                xp.setWidgetProperty(widget, Prop.ListBoxAddItem, 0)  # unset it
                descriptor = xp.getWidgetDescriptor(widget)

                if xp.measureString(xp.Font_Basic, descriptor) > (right - left - 20):
                    charsPer = int((right - left - 20) / fontWidth)
                    for x in range(0, len(descriptor), charsPer):
                        self.listBoxAddItem(listBoxDataObj, descriptor[x:x + charsPer], (right - left - 20))
                else:
                    self.listBoxAddItem(listBoxDataObj, descriptor, (right - left - 20))

                scrollbarMax = len(listBoxDataObj['Items'])
                sliderPosition = scrollbarMax
                xp.setWidgetProperty(widget, Prop.ListBoxScrollBarMax, scrollbarMax)
                if self.autoScroll:
                    if wasViewingBottom and not(maxListBoxItems - sliderPosition > 0):
                        # print('was, but no more, setting to {}'.format(maxListBoxItems - 1))
                        xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, maxListBoxItems - 1)
                    elif wasViewingBottom:
                        # print('was, and still is, setting to: {}'.format(sliderPosition))
                        xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, sliderPosition)
                    else:
                        # print("wasn't, still isn't, set backwards: {}".format(sliderPosition - 1))
                        xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, sliderPosition - 1)
                else:
                    xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, sliderPosition)

            if xp.getWidgetProperty(widget, Prop.ListBoxAddItemsWithClear, None):
                xp.setWidgetProperty(widget, Prop.ListBoxAddItemsWithClear, 0)  # unset it
                descriptor = xp.getWidgetDescriptor(widget)
                self.listBoxClear(listBoxDataObj)
                self.listBoxFillWithData(listBoxDataObj, descriptor, (right - left - 20))
                scrollbarMax = len(listBoxDataObj['Items'])
                sliderPosition = scrollbarMax
                xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, sliderPosition)
                xp.setWidgetProperty(widget, Prop.ListBoxScrollBarMax, scrollbarMax)

            if xp.getWidgetProperty(widget, Prop.ListBoxClear, None):
                xp.setWidgetProperty(widget, Prop.ListBoxClear, 0)
                xp.setWidgetProperty(widget, Prop.ListBoxCurrentItem, 0)
                self.listBoxClear(listBoxDataObj)
                scrollbarMax = len(listBoxDataObj['Items'])
                sliderPosition = scrollbarMax
                xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, sliderPosition)
                xp.setWidgetProperty(widget, Prop.ListBoxScrollBarMax, scrollbarMax)

            if xp.getWidgetProperty(widget, Prop.ListBoxInsertItem, None):
                xp.setWidgetProperty(widget, Prop.ListBoxInsertItem, 0)
                descriptor = xp.getWidgetDescriptor(widget)
                self.listBoxInsertItem(listBoxDataObj, descriptor, (right - left - 20), currentItem)
            if xp.getWidgetProperty(widget, Prop.ListBoxDeleteItem, None):
                xp.setWidgetProperty(widget, Prop.ListBoxDeleteItem, 0)
                if listBoxDataObj['Items'] and len(listBoxDataObj['Items']) > currentItem:
                    self.listBoxDeleteItem(listBoxDataObj, currentItem)

            return 1

        if message == xp.Msg_Draw:
            _x, y = xp.getMouseLocationGlobal()

            xp.drawWindow(left, bottom, right - 20, top, xp.Window_ListView)
            xp.drawTrack(right - 20, bottom, right, top, scrollbarMin, scrollbarMax, sliderPosition, xp.Track_ScrollBar, highlighted)

            xp.setGraphicsState(0, 1, 0, 0, 1, 0, 0)
            GL.glColor4f(1.0, 1.0, 1.0, 1.0)

            xp.setGraphicsState(0, 0, 0, 0, 0, 0, 0)

            # Now draw each item.
            listBoxIndex = scrollbarMax - sliderPosition
            itemNumber = 0

            # print("numItems is {}, maxListBoxItems: {}, sliderPosition = {}, lbIndex= {}: {}".format(
            #     scrollbarMax,
            #     maxListBoxItems,
            #     sliderPosition,
            #     listBoxIndex,
            #     listBoxIndex + maxListBoxItems > scrollbarMax
            # ))

            while itemNumber < maxListBoxItems:
                if listBoxIndex < len(listBoxDataObj['Items']):
                    # Calculate the item rect in global coordinates.
                    itemTop = int(top - (itemNumber * listbox_item_height))
                    itemBottom = int(top - ((itemNumber * listbox_item_height) + listbox_item_height))

                    # If we are hilited, draw the hilite bkgnd.
                    if currentItem == listBoxIndex:
                        SetAlphaLevels(0.25)
                        xp.setGraphicsState(0, 0, 0, 0, 1, 0, 0)
                        SetupAmbientColor(XP_Color.MenuHilite, True)
                        SetAlphaLevels(1.0)
                        GL.glBegin(GL.GL_QUADS)
                        GL.glVertex2i(left, itemTop)
                        GL.glVertex2i(right - 20, itemTop)
                        GL.glVertex2i(right - 20, itemBottom)
                        GL.glVertex2i(left, itemBottom)
                        GL.glEnd()

                    text = SetupAmbientColor(XP_Color.ListText)

                    listBoxWidth = (right - 20) - left
                    fontWidth, fontHeight, _other = xp.getFontDimensions(xp.Font_Basic)
                    maxChars = int(listBoxWidth / fontWidth)
                    buffer = listBoxDataObj['Items'][listBoxIndex][0:maxChars]
                    listBoxIndex += 1
                    xp.drawString(text,
                                  left, itemBottom + 2,
                                  buffer, None, xp.Font_Basic)
                itemNumber += 1
            return 1

        if message == xp.Msg_MouseUp:
            if IN_RECT(MOUSE_X(param1), MOUSE_Y(param1), right - 20, top, right, bottom):
                highlighted = False
                xp.setWidgetProperty(widget, Prop.ListBoxHighlighted, highlighted)

            if IN_RECT(MOUSE_X(param1), MOUSE_Y(param1), left, top, right - 20, bottom):
                if listBoxDataObj['Items']:
                    if currentItem != -1:
                        xp.setWidgetDescriptor(widget, listBoxDataObj['Items'][currentItem])
                    else:
                        xp.setWidgetDescriptor(widget, "")
                    xp.sendMessageToWidget(widget, xpMessage_ListBoxItemSelected, xp.Mode_UpChain, widget, currentItem)
            return 1

        if message == xp.Msg_MouseDown:
            if IN_RECT(MOUSE_X(param1), MOUSE_Y(param1), left, top, right - 20, bottom):
                if listBoxDataObj['Items']:
                    x, y = xp.getMouseLocationGlobal()
                    listBoxDataOffset = self.listBoxGetItemNumber(listBoxDataObj, listbox_item_height, x - left, top - y)
                    if listBoxDataOffset != -1:
                        listBoxDataOffset += scrollbarMax - sliderPosition
                        if listBoxDataOffset < len(listBoxDataObj['Items']):
                            xp.setWidgetProperty(widget, Prop.ListBoxCurrentItem, listBoxDataOffset)

            if IN_RECT(MOUSE_X(param1), MOUSE_Y(param1), right - 20, top, right, bottom):
                tm = xp.getTrackMetrics(right - 20, bottom, right, top, scrollbarMin, scrollbarMax, sliderPosition, xp.Track_ScrollBar)
                scrollbarMin = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarMin, None)
                scrollbarMax = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarMax, None)
                if tm.isVertical:
                    upBtnSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                            right - 20, top, right, top - tm.upBtnSize)
                    downBtnSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                              right - 20, bottom + tm.downBtnSize, right, bottom)
                    upPageSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                             right - 20, (top - tm.upBtnSize), right, (bottom + tm.downBtnSize + tm.downPageSize + tm.thumbSize))
                    downPageSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                               right - 20, (top - tm.upBtnSize - tm.upPageSize - tm.thumbSize), right, (bottom + tm.downBtnSize))
                    thumbSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                            right - 20, (top - tm.upBtnSize - tm.upPageSize),
                                            right, (bottom + tm.downBtnSize + tm.downPageSize))
                else:
                    downBtnSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                              right - 20, top, right - 20 + tm.upBtnSize, bottom)
                    upBtnSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                            right - 20 - tm.downBtnSize, top, right, bottom)
                    downPageSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                               right - 20 + tm.downBtnSize, top,
                                               right - tm.upBtnSize - tm.upPageSize - tm.thumbSize, bottom)
                    upPageSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                             right - 20 + tm.downBtnSize + tm.downPageSize + tm.thumbSize, top,
                                             right - tm.upBtnSize, bottom)
                    thumbSelected = IN_RECT(MOUSE_X(param1), MOUSE_Y(param1),
                                            right - 20 + tm.downBtnSize + tm.downPageSize, top,
                                            right - tm.upBtnSize - tm.upPageSize, bottom)

                if upPageSelected:
                    sliderPosition += scrollBarPageAmount
                    if sliderPosition > scrollbarMax:
                        sliderPosition = scrollbarMax
                    xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, sliderPosition)
                elif downPageSelected:
                    sliderPosition -= scrollBarPageAmount
                    if sliderPosition < scrollbarMin:
                        sliderPosition = scrollbarMin
                    xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, sliderPosition)
                elif upBtnSelected:
                    sliderPosition += 1
                    if sliderPosition > scrollbarMax:
                        sliderPosition = scrollbarMax
                    xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, sliderPosition)
                elif downBtnSelected:
                    sliderPosition -= 1
                    if sliderPosition < scrollbarMin:
                        sliderPosition = scrollbarMin
                    xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, sliderPosition)
                elif thumbSelected:
                    if tm.isVertical:
                        scrollBarSlop = int(bottom + tm.downBtnSize + tm.downPageSize + (tm.thumbSize / 2) - MOUSE_Y(param1))
                    else:
                        scrollBarSlop = int(right - 20 + tm.downBtnSize + tm.downPageSize + (tm.thumbSize / 2) - MOUSE_X(param1))
                    highlighted = True
                    xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSlop, scrollBarSlop)
                    xp.setWidgetProperty(widget, Prop.ListBoxHighlighted, highlighted)
                else:
                    highlighted = False
                    xp.setWidgetProperty(widget, Prop.ListBoxHighlighted, highlighted)
            return 1
        if message == xp.Msg_MouseDrag:
            if IN_RECT(MOUSE_X(param1), MOUSE_Y(param1), right - 20, top, right, bottom):
                tm = xp.getTrackMetrics(right - 20, bottom, right, top, scrollbarMin, scrollbarMax, sliderPosition, xp.Track_ScrollBar)
                scrollbarMin = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarMin, None)
                scrollbarMax = xp.getWidgetProperty(widget, Prop.ListBoxScrollBarMax, None)

                thumbSelected = highlighted
                if thumbSelected:
                    if param1 != 0:
                        if tm.isVertical:
                            y = MOUSE_Y(param1) + scrollBarSlop
                            sliderPosition = round(float(float(y - (bottom + tm.downBtnSize + tm.thumbSize / 2)) / float((top - tm.upBtnSize - tm.thumbSize / 2) - (bottom + tm.downBtnSize + tm.thumbSize / 2))) * scrollbarMax)
                        else:
                            x = MOUSE_X(param1) + scrollBarSlop
                            sliderPosition = round((float)((float)(x - (right - 20 + tm.downBtnSize + tm.thumbSize / 2)) / (float)((right - tm.upBtnSize - tm.thumbSize / 2) - (right - 20 + tm.downBtnSize + tm.thumbSize / 2))) * scrollbarMax)
                    else:
                        sliderPosition = 0

                    if sliderPosition < scrollbarMin:
                        sliderPosition = scrollbarMin
                    if sliderPosition > scrollbarMax:
                        sliderPosition = scrollbarMax
                    xp.setWidgetProperty(widget, Prop.ListBoxScrollBarSliderPosition, sliderPosition)
            return 1
        return 0

    def listBoxGetItemNumber(self, data, listbox_item_height, inX, inY):
        """
        This routine finds the item that is in a given point, or returns -1 if there is none.
        It simply trolls through all the items.
        """
        for n in range(len(data['Items'])):
            if all([inX >= data['Lefts'][n],
                    inX < data['Rights'][n],
                    inY >= (n * listbox_item_height),
                    inY < (n * listbox_item_height) + listbox_item_height]):
                return n
        return -1

    def listBoxFillWithData(self, data, items, width):
        for item in items.split(';'):
            data['Items'].append(item)
            data['Lefts'].append(0)
            data['Rights'].append(width)

    def listBoxAddItem(self, data, buff, width):
        data['Items'].append(buff)
        data['Lefts'].append(0)
        data['Rights'].append(width)

    def listBoxClear(self, data):
        data['Items'] = []
        data['Lefts'] = []
        data['Rights'] = []

    def listBoxInsertItem(self, data, buff, width, item):
        data['Items'].insert(item, buff)
        data['Lefts'].insert(item, 0)
        data['Rights'].insert(item, width)

    def listBoxDeleteItem(self, data, item):
        data['Items'].pop(item)
        data['Lefts'].pop(item)
        data['Rights'].pop(item)


def XPCreateListBox(left, top, right, bottom, visible, container):
    listBox = XPListBox(left, top, right, bottom, visible, '', container)
    return listBox


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


def IN_RECT(x, y, left, top, right, bottom):
    return ((x) >= (left)) and ((x) <= (right)) and ((y) >= (bottom)) and ((y) <= (top))


def MOUSE_X(param1):
    return param1[0]


def MOUSE_Y(param1):
    return param1[1]
