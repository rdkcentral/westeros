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
broadcasts and per-frame composition are not logged. No mutex calls, repaints,
Wayland requests or renderer queries are added.

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
