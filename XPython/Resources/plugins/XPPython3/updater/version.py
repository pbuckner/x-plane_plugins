from typing import Tuple
from XPPython3.utils.version import StrictVersion
import re


class VersionUnknownException(ValueError):
    pass


class Version(StrictVersion):
    """
    Extract first thing which looks like a version number from
    provided string and convert using StrictVersion.

     1) Version number is first extracted, as the first "word" with leading number, through to first space'
         '3.0.1 Release'         -> '3.0.1'
         '3.0.1a1 Beta'          -> '3.0.1a1'
         'Release 3.0.1a1 Beta'  -> '3.0.1a1'
         'Version-3 Beta 2'      -> '3'

     2) If version number ends with letter, we append '0' to it. This permits us to use step 3
         '3.0.1'   -> '3.0.1'
         '3.0.1a'  -> '3.0.1a0'
         '3.0.1a1' -> '3.0.1a1'

     3) Comparison uses distutils.version.StrictVersion (note that pre-release letter is _only_ a or b):
       * dot separated Major, Minor, Patch values:
            3.0
            3.1
            3.1.10
       * number are compared numerically, missing patch value is 'zero' (Major and Minor are required)
            3.0.0
            3.1.0
            3.1.10
       * optional 'pre-release' after version number consists of letter 'a' or 'b' _with_ a number
            3.0a1
            3.1a2
            3.1.0a11
            3.1.10a1
            3.1.10a2
            3.1.10a10
       * 'pre-release' are less than release of same number
            3.1.9
            3.1.10a1
            3.1.10
    """
    def __init__(self, vstring=None):
        try:
            vstring = re.search(r'\d[\.\dab]*', vstring)[0]
        except TypeError as e:
            raise VersionUnknownException from e
            # vstring = '0.0'
        if vstring[-1].isalpha():
            vstring += '0'
        super(Version, self).__init__(vstring)


def calc_update(try_beta: bool, current_version: str, stable_version: str, beta_version: str) -> Tuple[bool, str]:
    """
    returns tuple (up-to-date: T/F, change to: version)
    """
    try:
        current = Version(current_version)
        stable = Version(stable_version)
        if try_beta:
            try:
                beta = Version(beta_version)
            except VersionUnknownException:
                beta = Version('0.0')
    except VersionUnknownException:
        return (False, "Unknown")
    if try_beta:
        if beta > current:
            if stable > beta:
                return (False, stable_version)
            return (False, beta_version)
        if beta == current and stable < beta:
            return (True, beta_version)
        if stable > current:
            return (False, stable_version)
        return (True, current_version)
    if stable > current:
        return (False, stable_version)
    if stable == current:
        return (True, current_version)
    # If i have a beta (any beta), go with stable_version
    # otherwise, stick with what I have.
    if current.prerelease:
        # you have beta
        return (False, stable_version)
    return (True, current_version)
