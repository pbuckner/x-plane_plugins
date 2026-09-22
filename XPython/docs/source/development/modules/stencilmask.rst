Understanding Stencil Masks
===========================

.. py:currentmodule:: xp

Every stencil call takes the same two parameters, ``bits`` and ``mask``, and they mean
different things depending on which phase you're in. This is key to understanding the rest of
the API.

The stencil buffer is a set of hidden values associated with your window: one 8-BIT [#BIT]_ value for **each pixel**
in your window.

You use this buffer in two phases:

* **Write Stencil Buffer** (:func:`setupStencilMask` / :func:`beginSetupStencilMask`) -- draw lines and polygons to
  record *the shape* of the stencil. Nothing appears on screen; only the footprint of the geometry is
  recorded into the stencil buffer.
* **Test Against Stencil Buffer** (:func:`useStencilMask`) -- draw normally, but each pixel is kept only where the
  stencil buffer matches. Your drawing is confined to the stencil region you recorded earlier.

:func:`clearStencilMask` zeroes the whole buffer: rarely used, but *requires* a call to disable stencil testing first with
``useStencilMask(0, 0)``.      

Reading ``bits`` and ``mask``
-----------------------------

In both phases, ``mask`` selects *which BIT positions you care about* and ``bits`` supplies
*the value* for those positions. BIT positions outside ``mask`` are neither written nor
tested: that is what lets independent shapes share one stencil buffer without interfering.

The two phases apply the same pair of numbers differently:

**Writing.** Every pixel the shape covers gets ``bits`` written into it, but only in the
positions ``mask`` selects::

          BIT#     7   6   5   4   3   2   1   0
                 ---------------------------------
   bits = 0x05 = | 0 | 0 | 0 | 0 | 0 | 1 | 0 | 1 |
   mask = 0x06 = | 0 | 0 | 0 | 0 | 0 | 1 | 1 | 0 |
                 ---------------------------------
   result      = | X | X | X | X | X | 1 | 0 | X |

   1 = set this BIT on
   0 = set this BIT off
   X = leave this BIT unchanged

- BIT# 2 is selected by ``mask`` and set in ``bits``, so it is written as ``1`` into the stencil.
- BIT# 1 is selected by ``mask`` but clear in ``bits``, so it is written as ``0``. (Selecting a BIT and giving it
  a zero value is how you erase.)
- BIT# 0 is set in ``bits`` but *not* selected by ``mask``, so it
  is silently ignored (that is, the value of the stencil is not changed from any previous value).

**Testing.** When using :func:`useStencilMask`, ``bits`` and ``mask`` now describe a comparison.
A pixel is drawn where::

   (stencil & mask) == (bits & mask)

     BIT#     7   6   5   4   3   2   1   0
            ---------------------------------
   bits =   | 0 | 0 | 0 | 0 | 0 | 1 | 0 | 1 |
   mask =   | 0 | 0 | 0 | 0 | 0 | 1 | 1 | 0 |
            ---------------------------------
   mask says draw where:   BIT# 2 is 1  AND  BIT# 1 is 0, regardless of every other bit in stencil

That is, use ``mask`` against both the stencil buffer and provided ``bits``, to select *which BITS
to check*, ignoring all other BITS. If the BIT value in the stencil matches the ``bits`` value provided (modulo ``mask``)
then we'll *draw* the pixel (regardless of whether the matched stencil value is 1 or 0 -- it
has to *match* the provided ``bits``).

So ``bits`` is **a value to match, not a shape identifier**: ``useStencilMask(0x03, 0x03)`` does not mean "shapes 1 and 2",
it means "the pixels where BIT# 0 is set *and* BIT# 1 is set" -- the intersection.

Common patterns
---------------

Assigning each shape its own single BIT keeps the channels independent and composable:

.. list-table::
   :header-rows: 1
   :widths: 35 30 35

   * - Goal
     - Write
     - Test
   * - Draw inside a shape
     - ``setupStencilMask(0x01, 0x01)``  e.g., draw a star
     - ``useStencilMask(0x01, 0x01)`` e.g., paint where there's a 1 in BIT#0
   * - Draw *outside* a shape (inverse clip)
     - ``setupStencilMask(0x01, 0x01)``
     - ``useStencilMask(0x00, 0x01)`` e.g., paint where there's a 0 in BIT#0
   * - Two independent shapes
     - two calls to :func:`setupStencilMask`, with ``(0x01, 0x01)``, then ``(0x02, 0x02)``
     - ``useStencilMask`` with ``(0x01, 0x01)``,  or ``(0x02, 0x02)``
   * - Intersection of both
     - as above
     - ``useStencilMask(0x03, 0x03)`` e.g., where there's a 1 in both BIT#0 and BIT#1
   * - Inside A but *not* B (knockout)
     - as above
     - ``useStencilMask(0x01, 0x03)`` e.g., where there's a 1 in BIT#0 and 0 in BIT#1
   * - Nested clipping, up to 8 levels
     - one BIT per level
     - test the accumulated pattern

Note the inverse clip: it costs nothing extra. The same recorded shape gives you both "inside"
and "outside" simply by testing against 0 with the same mask.

Writing several shapes
++++++++++++++++++++++

A single stencil buffer can hold many shapes at once, each in its own bit. The ``mask`` value
causes only selected bits to be written, leaving the other bits untouched::

  with xp.setupStencilMask(0x01, 0x01):
      # record the square's footprint in BIT# 0; BIT# 1 and 2 are untouched
      drawSquare()

  with xp.setupStencilMask(0x02, 0x02):
      # record the circle in BIT# 1; 0 and 2 are untouched
      drawCircle()

  with xp.setupStencilMask(0x04, 0x04):
      # record the triangle in BIT# 2; 0 and 1 are untouched
      drawTriangle()

Those three shapes can now be tested individually or in combination::

  xp.useStencilMask(0x01, 0x01)
  fillRect()   # confined to the square

  xp.useStencilMask(0x02, 0x02)
  fillRect()   # confined to the circle

  xp.useStencilMask(0x03, 0x03)
  fillRect()   # only where BOTH BIT# 0 and  1 are set: square <intersects> circle

  xp.useStencilMask(0x01, 0x03)
  fillRect()   # square, but NOT where it overlaps the circle

  xp.useStencilMask(0x00, 0x07)
  fillRect()   # outside all three shapes

Erasing part of the buffer
++++++++++++++++++++++++++

:func:`clearStencilMask` is all-or-nothing. To clear selected BITs over *part* of the
buffer, write with ``bits=0`` and a ``mask`` naming the BITs to reset::

  with xp.setupStencilMask(0, 0x05):
      # BIT# 0 and 2 are selected, and given the value zero
      drawTriangle()

The erase reaches **only the pixels the new shape covers**. Above, the shape drawn is the same
triangle that was recorded into BIT# 2, so that channel is wiped completely and
``useStencilMask(0x04, 0x04)`` afterwards matches nothing. BIT# 0 (the square) is cleared only where
the triangle overlapped it, so ``useStencilMask(0x01, 0x01)`` now draws the square minus its
intersection with the triangle. Had you drawn some *other* shape here, only the pixels under
that shape would have been affected.

Why two parameters?
-------------------

The test is an equality match, so it can express AND and NOT but never OR. There is no
reference/mask pair that means "inside A **or** B" -- a single equality test cannot match two
different patterns. Multi-BIT writes are how you precompute, at record time, the things the
test cannot ask for later.

The cases where a multi-BIT ``bits``/``mask`` pair earns its keep:

#. **Precomputing a union.** If you need "inside either shape," record a dedicated union
   channel while you are already drawing each shape::

    with xp.setupStencilMask(BIT_A | BIT_UNION, BIT_A | BIT_UNION):  # bits=0x09, mask=0x09
        drawSquare()
    with xp.setupStencilMask(BIT_B | BIT_UNION, BIT_B | BIT_UNION):  # bits=0x0A, mask=0x0A
        drawCircle()
    ...
    xp.useStencilMask(BIT_UNION, BIT_UNION)                          # either shape, one test

   This costs nothing extra -- the geometry was being drawn anyway.

#. **Selectively erasing channels**, as above. This is the API's only partial clear.

#. **Encoding an ID instead of flags.** If regions are *mutually exclusive* -- layer numbers,
   priority bands, which of N instruments owns a pixel -- spend the byte as a small integer
   rather than as flags: ``setupStencilMask(id, 0x0F)`` to record, ``useStencilMask(id,
   0x0F)`` to test. Eight BITs buys 256 exclusive IDs versus 8 independent flags. Flags are
   right when regions overlap; IDs when they cannot.

#. **Marking one shape as a member of several categories.** If a shape is simultaneously
   "clickable" and "highlighted" and "clipped," one write with all three BITS beats drawing
   the same geometry three times.

The rule of thumb: **flags** when regions overlap and you need to test them independently, a
**packed value** when they are mutually exclusive, and **multi-BIT writes** whenever the test
you will want later is an OR -- since that is the one thing equality cannot do.

Pitfalls
--------

**Keep** ``bits`` **a subset of** ``mask``. For example, ``setupStencilMask(0x03, 0x01)`` writes only BIT#
0; the ``0x02`` you asked for is dropped because ``mask`` does not select it. X-Plane will report
this as an error and ignore it.

**Only 8 BITs are significant** (values ``0x00`` -- ``0xff``). Higher BITs are reported as an error.

``mask=0`` **when writing, records nothing** -- no BIT position is selected, so the call has no
effect at all.

``mask=0`` **when testing, passes everywhere** -- with nothing to compare, every pixel matches
and no clipping occurs. This reads as "the stencil feature is broken/disabled." As a special
case ``bits=0, mask=0`` disables stencil testing.

**Color and alpha are ignored while recording.** Only the rasterized footprint of the geometry
matters. A fully transparent fill records exactly the same shape as an opaque one. The color
argument is still required by :func:`polygon`'s signature; it simply goes nowhere.

Worked example
--------------

See ``samples/PI_PGStencil.py`` for a working example: it exercises the whole API
in one window, recording two overlapping squares into independent channels (``0x01`` and ``0x02``) and a
five-pointed star into a third (``0x04``), then paints four regions by drawing a *full-window*
polygon four times -- so the stencil test is the only thing confining each one:

.. image:: /images/panel_stencil.png
    :width: 500px

Square A is painted red, square B green, and their overlap yellow -- three different colors
from three full-window polygons, distinguished only by which bits each test matched. The star
is painted by nobody: it shows the bare window background, lighter than the dark gray "outside"
region that *was* painted. The white outlines are drawn last, after stencil testing is disabled with
``useStencilMask(0, 0)``, so a mispainted region reads unambiguously as a stencil failure
rather than as misplaced geometry.

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - Test
     - Region painted
   * - ``useStencilMask(0x00, 0x07)``
     - neither square, and not the star
   * - ``useStencilMask(0x01, 0x05)``
     - all of A, minus the star
   * - ``useStencilMask(0x02, 0x06)``
     - all of B, minus the star
   * - ``useStencilMask(0x03, 0x07)``
     - the overlap of A and B, minus the star

The star is a *knockout*: every test widens its ``mask`` to include the star BIT while leaving
that bit 0 in ``bits``, which reads as "...and the star BIT must be clear." No pass can paint
where the star was recorded, so it becomes a hole punched cleanly through all four regions.
This costs no extra drawing pass and no change to any geometry.

The same picture could be produced without a third channel by erasing instead: record the star
with ``bits=0, mask=0x03`` to clear A and B inside it, leaving the four original tests
unchanged. The extra-channel version is used because it keeps the star inspectable as its own
region afterwards, whereas erasing destroys that information. The erase form scales better when
you are short on BITs.

----

.. [#BIT]
   We'll use capitalized 'BIT' to refer to one or more of the 8-bits in the stencil buffer for
   a particular pixel. This is to avoid confusion with the lower-case parameter ``bits`` which is a *value*
   to set, or to match, used by the functions.
