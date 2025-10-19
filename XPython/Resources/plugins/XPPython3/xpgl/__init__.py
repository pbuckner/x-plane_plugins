# pylint: disable=unused-import
# flake8: noqa
##################################
# The file triangulate.py is from https://github.com/yaugenst/triangulation/tree/master
# which is subject to the following MIT License
#
# MIT License
#
# Copyright (c) 2018 mrbaozi
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
from .lines import drawLine, drawPolyLine, setLinePattern
from .images import loadImage, drawTexture, drawRotatedTexture, drawRotatedTextureCenter
from .text import drawText, loadFont, measureText
from .transformations import graphicsContext, translateContext, saveGraphicsContext, restoreGraphicsContext, \
    setTranslateTransform, setRotateTransform, setScaleTransform
from .mask import drawMaskStart, drawMaskEnd, drawUnderMask, maskContext

from .xpgl_utils import clear, report
from .colors import Colors, RGBColor
from .shapes import drawTriangle, drawRectangle, drawFrame, drawPolygon, drawCircle, drawArcLine, drawArc
