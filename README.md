# westeros
Wayland Compositor

Westeros is a light-weight Wayland compositor library. It uses the Wayland protocols, and is designed 
to be compatible with applications built to use Wayland compositors. It implements a library that 
enables an application to create one or more Wayland displays. It supports the creation of normal, nested, and
embedded wayland compositors.  A normal compositor displays its composited output to the screen, while a nested
compositor sends its composited output to another compositor as a client surface.  An embedded compositor allows 
the application that has created the embedded Wayland compositor to incorporate its composited output into the 
applications UI.  This allows for easy integration of the UI of external third party applications into an 
applications's UI.

## Simple-shell visibility diagnostics

Temporary `cc_test` messages are written to stderr and flushed immediately.
Capture the compositor service with `StandardError=journal` (and
`StandardOutput=journal` for existing stdout messages), then filter a complete
reproduction with `journalctl -b -o short-monotonic --no-pager | grep cc_test`.
Build and deploy these Westeros sources; rebuilding Subtec alone does not include
the compositor diagnostics. No Subtec client instrumentation is required.

The trace includes the requesting client's PID and surface ID, the target's
cached state, renderer/repeater selection, nested-to-upstream surface ID mapping,
visibility-status recipients, VPC hardware-path transitions and the embedded
fast-renderer/surface mapping. Generic status queries, geometry/opacity/z-order
broadcasts are not logged. No mutex calls, repaints, Wayland requests, GL queries,
delays or renderer queries are added. Diagnostic output itself adds some overhead.

Each actual SimpleShell visibility transition arms a numbered composition trace
window (`cc_test compose arm`). This includes caption show/hide and possible
subsequent UI/video visibility requests, without relying on fixed surface IDs.
Repeated requests with unchanged shell visibility do not rearm the window.
Each compositor owner sharing that context logs its next **six** composition calls,
including hidden/not-ready/failed calls. A newer transition supersedes an unfinished
window; multiple transitions before an owner's next call coalesce to the latest
window number. Virtual-composition temporary hide/restore calls never arm a window.
`frame=.../6`, `remaining`, and `exhausted=1` identify the capture boundary. An arm
without subsequent compose logs means no composition was observed for that window
(or the window was superseded), not that rendering succeeded.

Within a window, `compose begin/surface/temp_hide/temp_restore/end` identify the
context, display, composing owner, surface owner, shell state and renderer pointer.
`scene_skip` identifies `WstHints_hidden`. `vpc surface/rect` records current/next
hardware path, pending transitions, and whether a rectangle was returned, directly
hole-punched, or omitted (other owner, graphics path, or no size override). Hardware
rectangles held during rotation/pending graphics transitions are identified.
`compose end` reports the existing API result, whether `needHolePunch` was written,
and the final returned rectangles, including any supplied by the caller.
`embedded hole_punch_submit` records the actual GL clear submission's rectangle,
viewport and computed scissor using already-queried state, not GPU completion.

`embedded scene` identifies embedded GL versus fast delegation; actual fast-path
changes are also logged outside windows. `embedded surface` records each GL
draw/skip decision and the available image, CPU-memory and texture state.
`embedded draw_submit` records geometry, effective alpha and textures after
preparation, immediately before the shader draw helper. Fast delegation logs
describe the handoff, not platform-renderer draw decisions. Match `surface` in
renderer logs to `renderSurface` in compositor logs; renderer objects have no
protocol surface IDs. Visibility, buffers and a draw submission alone do not prove
pixel contents, successful GPU execution, hardware-plane state, or scanout.

Build/deploy the compositor library and embedded renderer modules
together: temporary trace-scope fields were appended to the internal `WstRenderer`
interface. A proprietary wrapper must reach this instrumented embedded renderer
for `embedded scene` messages to appear; wrapper/driver internals are not traced.
Trace scope uses the existing composition mutex; only the diagnostic window number
is atomically published from the unlocked SimpleShell callback. Existing locking
and rendering decisions are unchanged.

Embedded renderer visibility calls can occur every frame during virtual
composition. That trace is capped at 16 lines per render surface over its
lifetime; `lastTrace=1` marks exhaustion. Reproduce from a fresh compositor
process to capture early transitions. Simple-shell visibility requests remain
logged after this cap, as do VPC path changes. Their volume depends on the
client's request/transition rate; the overall trace is not globally capped.

Correlate by process, context/shell pointers and surface IDs; IDs are local to
each compositor. `shellVisible` is cached shell state, not hardware-plane
visibility. VPC status deliberately reports visible even when the graphics
surface is hidden; the target's `vpcStatusForcedVisible` records that distinction.
`set_visible end` means the synchronous handler returned, not that scanout
completed.
Platform-specific renderer/driver internals are not included in this reference
tree; these logs narrow the handoff but do not prove hardware-plane state.

---
# Copyright and license

Westeros is licensed under the Apache License, Version 2.0. Licensing is described in LICENSE and NOTICE, in
the top level directory.
