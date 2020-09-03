# https://developer.x-plane.com/code-sample-type/xplm300-sdk/map/
import os
from OpenGL import GL
from XPLMProcessing import XPLMGetElapsedTime
from XPLMGraphics import XPLMSetGraphicsState
# definitions
# from XPLMMap import xplm_MapStyle_VFR_Sectional, xplm_MapStyle_IFR_LowEnroute, xplm_MapStyle_IFR_HighEnroute
from XPLMMap import xplm_MapLayer_Markings, xplm_MapOrientation_Map
from XPLMMap import XPLM_MAP_USER_INTERFACE

# functions, tested
from XPLMMap import XPLMCreateMapLayer, XPLMRegisterMapCreationHook, XPLMMapExists
from XPLMMap import XPLMMapProject, XPLMDrawMapIconFromSheet, XPLMDrawMapLabel, XPLMMapUnproject, XPLMMapScaleMeter
from XPLMMap import XPLMMapGetNorthHeading  # this is the "mapping angle", an aspect of the projection, Unrelated to 'rotation' of the map (documentation bug)


# functions not tested -- it appears DestroyMapLayer is called even when map view is shifted, that
# would appear to be incorrect.
from XPLMMap import XPLMDestroyMapLayer


SAMPLE_IMG = "Resources/plugins/map-sample-image.png"


class PythonInterface():
    def __init__(self):
        self.Sig = "xppython3.PI_Map"
        self.Name = "Map Demos"
        self.Desc = "Map layer drawing example"

        self.s_num_cached_coords = 0
        self.s_cached_x_coords = []
        self.s_cached_y_coords = []
        self.s_cached_lon_coords = []
        self.s_cached_lat_coords = []
        self.g_layer = None
        self.s_icon_width = None  # normally set in prep_cache, we use this as a sentinal to determine if prep_cache has been called
        self.lastReported = 0  # used to periodically report "North" position in map

    def XPluginStart(self):
        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        if self.g_layer:
            XPLMDestroyMapLayer(self.g_layer)

    def XPluginEnable(self):
        if not os.path.exists(SAMPLE_IMG):
            print("Missing sample image, required for this test. Looking for: {}".format(SAMPLE_IMG))
            return 0

        if XPLMMapExists(XPLM_MAP_USER_INTERFACE):
            # map already exists in enable, creating layer immediately
            self.createOurMapLayer(XPLM_MAP_USER_INTERFACE, None)
        else:
            # map does not exist in enable, will create layer using callback in MapCreationHook
            pass

        XPLMRegisterMapCreationHook(self.createOurMapLayer, None)
        return 1

    def XPluginDisable(self):
        pass

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        pass

    def createOurMapLayer(self, mapIdentifier, refcon):
        if (not self.g_layer and mapIdentifier == XPLM_MAP_USER_INTERFACE):
            # Map exists, creating layer
            self.mapRefCon = []
            self.name = "Markings"
            self.prep_cacheCB = self.prep_cache
            self.draw_markingsCB = self.draw_markings
            self.draw_marking_labelsCB = self.draw_marking_labels
            self.draw_marking_iconsCB = self.draw_marking_icons
            self.will_be_deletedCB = self.will_be_deleted
            self.params = (XPLM_MAP_USER_INTERFACE,
                           xplm_MapLayer_Markings,
                           self.will_be_deletedCB,
                           self.prep_cacheCB,
                           self.draw_markingsCB,
                           self.draw_marking_iconsCB,
                           self.draw_marking_labelsCB,
                           1,  # showUiToggle
                           self.name,
                           self.mapRefCon)
            self.g_layer = XPLMCreateMapLayer(self.params)

    def draw_markings(self, layer, inMapBoundsLeftTopRightBottom, zoomRatio,
                      mapUnitsPerUserInterfaceUnit, mapStyle, projection, refcon):
        XPLMSetGraphicsState(0, 0, 0, 0, 1, 1, 0)
        GL.glColor3f(0, 1, 0)  # green

        if not self.s_icon_width:
            return

        half_width = self.s_icon_width / 2
        half_height = half_width * 0.6667  # our images are in 3:2 aspect raio, so the height is 2/3 the width
        for coord in range(self.s_num_cached_coords):
            x = self.s_cached_x_coords[coord]
            y = self.s_cached_y_coords[coord]
            if coord_in_rect(x, y, inMapBoundsLeftTopRightBottom):
                GL.glBegin(GL.GL_LINE_LOOP)
                GL.glVertex2f(x - half_width, y + half_height)
                GL.glVertex2f(x + half_width, y + half_height)
                GL.glVertex2f(x + half_width, y - half_height)
                GL.glVertex2f(x - half_width, y - half_height)
                GL.glEnd()

    def draw_marking_icons(self, layer, inMapBoundsLeftTopRightBottom, zoomRatio,
                           mapUnitsPerUserInterfaceUnit, mapStyle, projection, refcon):
        if XPLMGetElapsedTime() - self.lastReported > 5:
            ###
            # ! comment out this -- calling XPLMMapGetNorthHeading near 'shift' in map layer causes crash in sim
            # midpoint_x = (inMapBoundsLeftTopRightBottom[0] + inMapBoundsLeftTopRightBottom[2]) / 2
            # midpoint_y = (inMapBoundsLeftTopRightBottom[1] + inMapBoundsLeftTopRightBottom[3]) / 2
            # north = XPLMMapGetNorthHeading(projection, midpoint_x, midpoint_y)
            # print("North is {:.1f}".format(north))
            self.lastReported = XPLMGetElapsedTime()

        for coord in range(self.s_num_cached_coords):
            x = self.s_cached_x_coords[coord]
            y = self.s_cached_y_coords[coord]
            if coord_in_rect(x, y, inMapBoundsLeftTopRightBottom):
                if coord % 2:
                    XPLMDrawMapIconFromSheet(layer,
                                             SAMPLE_IMG,
                                             0, 0,
                                             2, 2,
                                             x, y,
                                             xplm_MapOrientation_Map,
                                             0,
                                             self.s_icon_width)
                else:
                    XPLMDrawMapIconFromSheet(layer,
                                             SAMPLE_IMG,
                                             1, 1,
                                             2, 2,
                                             x, y,
                                             xplm_MapOrientation_Map,
                                             0,
                                             self.s_icon_width)

    def draw_marking_labels(self, layer, inMapBoundsLeftTopRightBottom, zoomRatio,
                            mapUnitsPerUserInterfaceUnit, mapStyle, projection, inRefcon):
        if zoomRatio >= 18:  # don't lable when zoomed too far out.. everything will run together
            for coord in range(self.s_num_cached_coords):
                x = self.s_cached_x_coords[coord]
                y = self.s_cached_y_coords[coord]
                if coord_in_rect(x, y, inMapBoundsLeftTopRightBottom):
                    scratch_buffer = '{:0.2f} / {:0.2f} Lat/Lon'.format(self.s_cached_lat_coords[coord],
                                                                        self.s_cached_lon_coords[coord])
                    icon_bottom = y - (self.s_icon_width / 2)
                    # top of the text will touch the bottom of the icon
                    text_center_y = icon_bottom - (mapUnitsPerUserInterfaceUnit * icon_bottom / 2)
                    XPLMDrawMapLabel(layer, scratch_buffer, x, text_center_y, xplm_MapOrientation_Map, 0)

    def prep_cache(self, layer, inTotalMapBoundsLeftTopRightBottom, projection, refcon):
        # if not inTotalMapBoundsLeftTopRightBottom:
        #     inTotalMapBoundsLeftTopRightBottom = (-1.67747962474823, 1.5166040658950806, 1.67747962474823, -1.4840505123138428)
        self.s_num_cached_coords = 0
        self.s_cached_x_coords = []
        self.s_cached_y_coords = []
        self.s_cached_lat_coords = []
        self.s_cached_lon_coords = []
        projection_error = False
        for lon in range(-180, 180):
            for lat in range(-80, 80):  # MapProject != MapUnproject as we get near the poles
                offset = .25
                try:
                    x, y = XPLMMapProject(projection, lat + offset, lon + offset)
                except:
                    print("Project failed for {}, {}".format(lat + offset, lon + offset))
                    return
                inbounds = coord_in_rect(x, y, inTotalMapBoundsLeftTopRightBottom)
                if inbounds:
                    self.s_cached_x_coords.append(x)
                    self.s_cached_y_coords.append(y)
                    self.s_cached_lon_coords.append(lon + offset)
                    self.s_cached_lat_coords.append(lat + offset)
                    self.s_num_cached_coords += 1

                    # While we're here, test that (x, y) -> (lat, lon) -> (x, y)
                    # MapUnproject (x,y) -> (lat, lon) is the inverse of MapProject
                    # only when (x,y) is within bounds of the current projection.
                    # Globally, a particular (x, y) will map to multiple different (lat, lon),
                    # though only one is "correct".
                    new_lat, new_lon = XPLMMapUnproject(projection, x, y)
                    new_x, new_y = XPLMMapProject(projection, new_lat, new_lon)
                    # (allowing for floating point fuzz)
                    if not projection_error and (abs(x - new_x) > .00001 or abs(y - new_y) > .00001):
                        print('Unproject error x,y: ({}, {}) vs ({}, {})'.format(x, y, new_x, new_y))
                        unprojected_lat, unprojected_lon = XPLMMapUnproject(projection, new_x, new_y)
                        print('Unproject error lat,lon: ({}, {}) vs ({}, {})'.format(new_lat, new_lon,
                                                                                     unprojected_lat, unprojected_lon))

        midpoint_x = (inTotalMapBoundsLeftTopRightBottom[0] + inTotalMapBoundsLeftTopRightBottom[2]) / 2
        midpoint_y = (inTotalMapBoundsLeftTopRightBottom[1] + inTotalMapBoundsLeftTopRightBottom[3]) / 2

        self.north = XPLMMapGetNorthHeading(projection, midpoint_x, midpoint_y)

        self.s_icon_width = XPLMMapScaleMeter(projection, midpoint_x, midpoint_y) * 5000

    def will_be_deleted(self, layer, refcon):
        if layer == self.g_layer:
            self.g_layer = None


def coord_in_rect(x, y, bounds_ltrb):
    return (x >= bounds_ltrb[0]) and (x < bounds_ltrb[2]) and (y >= bounds_ltrb[3]) and (y < bounds_ltrb[1])
