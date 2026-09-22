#####################################
# Opt-in per-frame profiling shared by xprenderer.py and pgrenderer.py.
#
# Off by default. Turn it on by setting PROFILE = True at the top of either
# renderer (they are independent, so you can profile one path at a time).
#
# What it measures: CPU time spent in the renderer, in the plugin's Python. It
# says NOTHING about GPU cost -- there is no timer-query access from Python, so a
# path that submits quickly may still be the slower one on the card. Use the
# whole-sim dataref sim/time/framerate_period for that side of the question.
#
# Costs nothing when off: every call site is guarded by `if self._prof:`.

import time
from typing import Any, Dict, List, Optional

from XPPython3 import xp

# Keep at most this many samples per phase, so a long session cannot grow without
# bound. Old samples are dropped, so a report describes recent frames.
MAX_SAMPLES = 4000


def _percentile(orderedSamples: List[float], fraction: float) -> float:
    if not orderedSamples:
        return 0.0
    idx = int(round(fraction * (len(orderedSamples) - 1)))
    return orderedSamples[idx]


class Profiler:
    """Accumulates named phase timings and logs a summary every reportFrames."""

    def __init__(self: Any, label: str, reportFrames: int = 300, warmupFrames: int = 120) -> None:
        self.label = label
        self.reportFrames = reportFrames
        self.warmupFrames = warmupFrames
        self.frames = 0
        self.phases: Dict[str, List[float]] = {}
        # Uncapped call count per phase. self.phases[] is trimmed to MAX_SAMPLES, so
        # len() saturates and cannot be used to work out calls-per-frame.
        self.counts: Dict[str, int] = {}
        self.vtxTotal = 0
        self.idxTotal = 0
        self.skipped = 0          # draw lists dropped (e.g. uint16 overflow)

    @staticmethod
    def now() -> float:
        return time.perf_counter()

    def add(self: Any, phase: str, started: float) -> None:
        """Record elapsed time since `started` (a value from now())."""
        self.addElapsed(phase, time.perf_counter() - started)

    def addElapsed(self: Any, phase: str, elapsed: float) -> None:
        """Record an already-measured duration, for costs accumulated in a loop."""
        if self.frames < self.warmupFrames:
            return                # skip first-frame/atlas-upload noise
        samples = self.phases.setdefault(phase, [])
        samples.append(elapsed)
        self.counts[phase] = self.counts.get(phase, 0) + 1
        if len(samples) > MAX_SAMPLES:
            del samples[:len(samples) - MAX_SAMPLES]

    def frame(self: Any, vtxCount: int = 0, idxCount: int = 0, skipped: int = 0) -> None:
        """Call once per rendered frame, after the phases have been recorded."""
        self.frames += 1
        if self.frames <= self.warmupFrames:
            return
        self.vtxTotal += vtxCount
        self.idxTotal += idxCount
        self.skipped += skipped
        if (self.frames - self.warmupFrames) % self.reportFrames == 0:
            self.report()

    def report(self: Any) -> None:
        counted = self.frames - self.warmupFrames
        if counted <= 0:
            return
        vtxPerFrame = self.vtxTotal / counted
        idxPerFrame = self.idxTotal / counted
        xp.log(f"[xp_imgui prof] {self.label}: {counted} frames, "
               f"{vtxPerFrame:.0f} vtx/frame, {idxPerFrame:.0f} idx/frame"
               + (f", {self.skipped} draw list(s) SKIPPED" if self.skipped else ""))
        for phase in sorted(self.phases):
            samples = sorted(self.phases[phase])
            n = len(samples)
            mean = sum(samples) / n
            # samples are seconds; 1e9 gives nanoseconds. NOTE n is the number of
            # SAMPLES, which for phases timed inside the per-draw-list loop is
            # (lists per frame) x frames -- so a phase mean is per LIST while
            # 'total' is per FRAME. Multiply a phase mean by n/counted to compare
            # it against total.
            perFrame = mean * self.counts.get(phase, n) / counted
            perVtx = (perFrame / vtxPerFrame * 1e9) if vtxPerFrame else 0.0
            xp.log(f"[xp_imgui prof]   {phase:<10} n={n:<5} "
                   f"mean={mean * 1000:.3f}ms  p50={_percentile(samples, 0.50) * 1000:.3f}ms  "
                   f"p95={_percentile(samples, 0.95) * 1000:.3f}ms  "
                   f"max={samples[-1] * 1000:.3f}ms  "
                   f"[{perFrame * 1000:.3f}ms/frame, {perVtx:.1f}ns/vtx]")

    def reset(self: Any) -> None:
        self.frames = 0
        self.phases.clear()
        self.counts.clear()
        self.vtxTotal = self.idxTotal = self.skipped = 0


def makeProfiler(enabled: bool, label: str) -> Optional[Profiler]:
    """Returns a Profiler, or None when profiling is off (the guard callers test)."""
    return Profiler(label) if enabled else None
