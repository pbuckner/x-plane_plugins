from typing import Tuple, Dict, NewType
RGBColor = NewType('RGBColor', Tuple[float, float, float])

Colors: Dict[str, RGBColor] = {
    'aquamarine': RGBColor((0.439216, 0.858824, 0.576471)),
    'bakers choc': RGBColor((0.36, 0.20, 0.09)),
    'black': RGBColor((0, 0, 0)),
    'blue violet': RGBColor((0.62352, 0.372549, 0.623529)),
    'blue': RGBColor((0, 0, 1.)),
    'brass': RGBColor((0.71, 0.65, 0.26)),
    'bright gold': RGBColor((0.85, 0.85, 0.10)),
    'bronze': RGBColor((0.55, 0.47, 0.14)),
    'bronze2': RGBColor((0.65, 0.49, 0.24)),
    'brown': RGBColor((0.647059, 0.164706, 0.164706)),
    'cadet blue': RGBColor((0.372549, 0.623529, 0.623529)),
    'cool copper': RGBColor((0.85, 0.53, 0.10)),
    'copper': RGBColor((0.72, 0.45, 0.20)),
    'coral': RGBColor((1.0, 0.498039, 0.0)),
    'cornflower blue': RGBColor((0.258824, 0.258824, 0.435294)),
    'cyan': RGBColor((0, 1., 1.)),
    'dark brown': RGBColor((0.36, 0.25, 0.20)),
    'dark green copper': RGBColor((0.29, 0.46, 0.43)),
    'dark green': RGBColor((0.184314, 0.309804, 0.184314)),
    'dark olive green': RGBColor((0.309804, 0.309804, 0.184314)),
    'dark orchid': RGBColor((0.6, 0.196078, 0.8)),
    'dark purple': RGBColor((0.53, 0.12, 0.47)),
    'dark slate blue': RGBColor((0.419608, 0.137255, 0.556863)),
    'dark slate gray': RGBColor((0.184314, 0.309804, 0.309804)),
    'dark slate grey': RGBColor((0.184314, 0.309804, 0.309804)),
    'dark tan': RGBColor((0.59, 0.41, 0.31)),
    'dark turquoise': RGBColor((0.439216, 0.576471, 0.858824)),
    'dark wood': RGBColor((0.52, 0.37, 0.26)),
    'dim gray': RGBColor((0.329412, 0.329412, 0.329412)),
    'dim grey': RGBColor((0.329412, 0.329412, 0.329412)),
    'dusty rose': RGBColor((0.52, 0.39, 0.39)),
    'feldspar': RGBColor((0.82, 0.57, 0.46)),
    'firebrick': RGBColor((0.556863, 0.137255, 0.137255)),
    'flesh': RGBColor((0.96, 0.80, 0.69)),
    'forest green': RGBColor((0.137255, 0.556863, 0.137255)),
    'gold': RGBColor((0.8, 0.498039, 0.196078)),
    'goldenrod': RGBColor((0.858824, 0.858824, 0.439216)),
    'gray': RGBColor((0.752941, 0.752941, 0.752941)),
    'gray05': RGBColor((0.05, 0.05, 0.05)),
    'gray10': RGBColor((0.10, 0.10, 0.10)),
    'gray15': RGBColor((0.15, 0.15, 0.15)),
    'gray20': RGBColor((0.20, 0.20, 0.20)),
    'gray25': RGBColor((0.25, 0.25, 0.25)),
    'gray30': RGBColor((0.30, 0.30, 0.30)),
    'gray35': RGBColor((0.35, 0.35, 0.35)),
    'gray40': RGBColor((0.40, 0.40, 0.40)),
    'gray45': RGBColor((0.45, 0.45, 0.45)),
    'gray50': RGBColor((0.50, 0.50, 0.50)),
    'gray55': RGBColor((0.55, 0.55, 0.55)),
    'gray60': RGBColor((0.60, 0.60, 0.60)),
    'gray65': RGBColor((0.65, 0.65, 0.65)),
    'gray70': RGBColor((0.70, 0.70, 0.70)),
    'gray75': RGBColor((0.75, 0.75, 0.75)),
    'gray80': RGBColor((0.80, 0.80, 0.80)),
    'gray85': RGBColor((0.85, 0.85, 0.85)),
    'gray90': RGBColor((0.90, 0.90, 0.90)),
    'gray95': RGBColor((0.95, 0.95, 0.95)),
    'green copper': RGBColor((0.32, 0.49, 0.46)),
    'green yellow': RGBColor((0.576471, 0.858824, 0.439216)),
    'green': RGBColor((0, 1.0, 0)),
    'greenish yellow': RGBColor((.5, 1., 0)),
    'grey': RGBColor((0.752941, 0.752941, 0.752941)),
    'hunters green': RGBColor((0.13, 0.37, 0.31)),
    'indian red': RGBColor((0.309804, 0.184314, 0.184314)),
    'khaki': RGBColor((0.623529, 0.623529, 0.372549)),
    'light blue': RGBColor((.74902, .847059, 0.847059)),
    'light gray': RGBColor((0.658824, 0.658824, 0.658824)),
    'light green': RGBColor((.5, 1., .5)),
    'light grey': RGBColor((0.658824, 0.658824, 0.658824)),
    'light purple': RGBColor((0.87, 0.58, 0.98)),
    'light steel blue': RGBColor((0.560784, 0.560784, 0.737255)),
    'light wood': RGBColor((0.91, 0.76, 0.65)),
    'lime green': RGBColor((0.196078, 0.8, 0.196078)),
    'magenta': RGBColor((1., 0, 1.)),
    'mandarin orange': RGBColor((0.89, 0.47, 0.20)),
    'maroon': RGBColor((0.556863, 0.137255, 0.419608)),
    'medium aquamarine': RGBColor((0.196078, 0.8, 0.6)),
    'medium blue': RGBColor((0.196078, 0.196078, 0.8)),
    'medium forest green': RGBColor((0.419608, 0.556863, 0.137255)),
    'medium goldenrod': RGBColor((0.917647, 0.917647, 0.678431)),
    'medium orchid': RGBColor((0.576471, 0.439216, 0.858824)),
    'medium purple': RGBColor((0.73, 0.16, 0.96)),
    'medium sea green': RGBColor((0.258824, 0.435294, 0.258824)),
    'medium slate blue': RGBColor((0.482, .408, .933)),
    'medium spring green': RGBColor((0, .980, .604)),
    'medium turquoise': RGBColor((0.439216, 0.858824, 0.858824)),
    'medium violet red': RGBColor((0.858824, 0.439216, 0.576471)),
    'medium wood': RGBColor((0.65, 0.50, 0.39)),
    'midnight blue': RGBColor((0.184314, 0.184314, 0.309804)),
    'navy blue': RGBColor((0.137255, 0.137255, 0.556863)),
    'navy': RGBColor((0.137255, 0.137255, 0.556863)),
    'neon blue': RGBColor((0.30, 0.30, 1.00)),
    'neon pink': RGBColor((1.00, 0.43, 0.78)),
    'new midnight blue': RGBColor((0.00, 0.00, 0.61)),
    'new tan': RGBColor((0.92, 0.78, 0.62)),
    'old gold': RGBColor((0.81, 0.71, 0.23)),
    'orange red': RGBColor((1.0, 0.25, 0)),
    'orange': RGBColor((1, 0.5, 0.0)),
    'orchid': RGBColor((0.858824, 0.439216, 0.858824)),
    'pale green': RGBColor((0.560784, 0.737255, 0.560784)),
    'pink': RGBColor((0.737255, 0.560784, 0.560784)),
    'plum': RGBColor((0.917647, 0.678431, 0.917647)),
    'quartz': RGBColor((0.85, 0.85, 0.95)),
    'red': RGBColor((1., 0, 0)),
    'rich blue': RGBColor((0.35, 0.35, 0.67)),
    'salmon': RGBColor((0.435294, 0.258824, 0.258824)),
    'scarlet': RGBColor((0.55, 0.09, 0.09)),
    'sea green': RGBColor((0.137255, 0.556863, 0.419608)),
    'semi sweet choc': RGBColor((0.42, 0.26, 0.15)),
    'sienna': RGBColor((0.556863, 0.419608, 0.137255)),
    'silver': RGBColor((0.90, 0.91, 0.98)),
    'sky blue': RGBColor((0.196078, 0.6, 0.8)),
    'slate blue': RGBColor((0, 0.498039, 1.0)),
    'spicy pink': RGBColor((1.00, 0.11, 0.68)),
    'spring green': RGBColor((0, 1.0, 0.498039)),
    'steel blue': RGBColor((0.137255, 0.419608, 0.556863)),
    'summer sky': RGBColor((0.22, 0.69, 0.87)),
    'tan': RGBColor((0.858824, 0.576471, 0.439216)),
    'thistle': RGBColor((0.847059, 0.74902, 0.847059)),
    'turquoise': RGBColor((0.678431, 0.917647, 0.917647)),
    'very dark brown': RGBColor((0.35, 0.16, 0.14)),
    'very light gray': RGBColor((0.80, 0.80, 0.80)),
    'very light grey': RGBColor((0.80, 0.80, 0.80)),
    'very light purple': RGBColor((0.94, 0.81, 0.99)),
    'violet red': RGBColor((0.8, 0.196078, 0.6)),
    'violet': RGBColor((0.309804, 0.184314, 0.309804)),
    'wheat': RGBColor((0.847059, 0.847059, 0.74902)),
    'white': RGBColor((1., 1., 1.)),
    'yellow green': RGBColor((0.6, 0.8, 0.196078)),
    'yellow': RGBColor((1., 1., 0)),
}
