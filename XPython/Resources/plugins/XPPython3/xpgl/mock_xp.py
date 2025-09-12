from typing import List
import numpy as np
from OpenGL.GL import glGenTextures, glActiveTexture, glBindTexture, GL_TEXTURE_2D, GL_TEXTURE0, glEnable, glDisable, \
    GL_FALSE, GL_TRUE, GL_ALPHA_TEST, GL_BLEND, GL_DEPTH_TEST, glDepthMask


class DummyXP:
    Counter = 1
    Font_Proportional = 0
    Font_Basic = 1
    log = print

    @staticmethod
    def getCycleNumber() -> int:
        return xp.Counter

    @staticmethod
    def generateTextureNumbers(count: int) -> List[int]:
        f = glGenTextures(count)
        if isinstance(f, (int, np.uint32)):
            return [f, ]
        return f

    @staticmethod
    def bindTexture2d(textureID: int, textureUnit: int) -> None:
        glActiveTexture(GL_TEXTURE0 + textureUnit)
        glBindTexture(GL_TEXTURE_2D, textureID)

    @staticmethod
    def setGraphicsState(fog=0, numberTexUnits=0, lighting=0, alphaTesting=0,
                         alphaBlending=0, depthTesting=0, depthWriting=0):
        if fog or lighting:
            raise ValueError("setting fog or lighting in this manner is not supported with recent X-Plane")
        if numberTexUnits:
            glEnable(GL_TEXTURE_2D)
            glActiveTexture(GL_TEXTURE0 + numberTexUnits)
        else:
            glDisable(GL_TEXTURE_2D)
        if alphaTesting:
            glEnable(GL_ALPHA_TEST)
        else:
            glDisable(GL_ALPHA_TEST)
        if alphaBlending:
            glEnable(GL_BLEND)
        else:
            glDisable(GL_BLEND)
        if depthTesting:
            glEnable(GL_DEPTH_TEST)
        else:
            glDisable(GL_DEPTH_TEST)
        if depthWriting:
            glDepthMask(GL_TRUE)
        else:
            glDepthMask(GL_FALSE)


xp = DummyXP()
