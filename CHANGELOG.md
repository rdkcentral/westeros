# Changelog

Consolidated release details generated from docs/relnotes.

## 2.1.2
- Date: 23 June 2026
- Tag/Revision: 2.1.2
- Highlights:
  - rpi: fix eglBindWaylandDisplayWL error
  - Coverity fix changes
  - Add ChangeLog files in the westeros repositories

## 2.1.1
- Date: 27 April 2026
- Tag/Revision: 2.1.1
- Highlights:
  - Add the changes needed for open-sourcing

## 2.1.0
- Date: 24 February 2026
- Tag/Revision: 2.1.0
- Highlights:
  - westeros: add wl_simple_shell popup api
  - Fix: vulkan running failure when Ursr update to 25.0.3.
  - Resolve coverity issues for westeros

## 2.0.0
- Date: 10 January 2026
- Tag/Revision: Westeros-2.0.0
- Highlights:
  - Westeros-sink reporting pre-seek position after seek, causing playback position jumps on video-only playback
  - fix westeros-sink timeCodeFound function
  - westeros-soc-brcm: Ignore playback rate 0.25-2.0 when audio is passthrough

## 1.01.62
- Date: Oct 28, 2025
- Tag/Revision: Westeros-1.01.62
- Highlights:
  - v4l2: Fix frame dropping boundary condition for seek accuracy
  - v4l2: fix compile error on platforms without V4L2_PIX_FMT_AV1 defined
  - v4l2:Fix thread race condition causing video decode crashes

## 1.01.61
- Date: Sep 30, 2025
- Tag/Revision: Westeros-1.01.61
- Highlights:
  - essos: Blacklist status, fix revoke defect

## 1.01.60
- Date: Sep 10, 2025
- Tag/Revision: Westeros-1.01.60
- Highlights:
  - v4l2: Add low-latency-mode for Netflix DPI 7.0 support
  - brcm: Fix "NXCLIENT_BAD_SEQUENCE_NUMBER" error when leaving Netflix DolbyVision
  - brcm: Add check for stc_channel==0 to reduce error logging during gaming/low latency

## 1.01.59
- Date: June 24, 2025
- Tag/Revision: Westeros-1.01.59
- Highlights:
  - if segstart==0 and 1st decoded pts is small, use 0 for base of position
  - Fixes for YTS HW Dual Video Tests
  - Fix NPLB timeAfterSeek Tests

## 1.01.58
- Date: Feb 20, 2025
- Tag/Revision: Westeros-1.01.58
- Highlights:
  - drm: Fix heap corruption resulting from race condition in video disconnect
  - sink: Default position to GST_CLOCK_TIME_NONE instead of 0
  - drm/sink: Westeros RT Thread Updates

## 1.01.57
- Date: Nov 25, 2024
- Tag/Revision: Westeros-1.01.57
- Highlights:
  - Handle Active Format Descriptor (AFD) in all zoom modes in v4l2
  - Robustness improvements during shutdown
  - Timecode PTS processing fix

## 1.01.56
- Date: July 23, 2024
- Tag/Revision: Westeros-1.01.56
- Highlights:
  - Enable frame step on preroll by default in v4l2
  - support for assigning surface ownership to virtual embedded compositors
  - Disable frame step on preroll by default Allow to display frame on paused state

## 1.01.55
- Date: May 17, 2024
- Tag/Revision: Westeros-1.01.55
- Highlights:
  - all: fix some compile warnings
  - sink: fix compiler warnings
  - essos: update key repeat

## 1.01.54
- Date: Apr 17, 2024
- Tag/Revision: Westeros-1.01.54
- Highlights:
  - brcm: remove ENABLE_LEXPSYNCPROTOCOL from public headers
  - v4l2: fix loss of precision in conversion from nanoseconds to 90KHz
  - brcm: adjust decode rate for immediate output

## 1.01.53
- Date: Mar 13, 2024
- Tag/Revision: Westeros-1.01.53
- Highlights:
  - v4l2: support use of session sync mode from asink
  - v4l2: enable DI and NR.
  - brcm: Set "videoStarted= FALSE" early in sinkReleaseVideo() to prevent underflow callback lock() race condition

## 1.01.52
- Date: Jan 11, 2024
- Tag/Revision: Westeros-1.01.52
- Highlights:
  - drm: add console command to toggle gfx scaling
  - brcm: reduce false underflows on framerate changes
  - drm: display gfx centered when actual-size active

## 1.01.51
- Date: Nov 13, 2023
- Tag/Revision: Westeros-1.01.51
- Highlights:
  - v4l2: support on-the-fly codec changes
  - brcm: use all decode mode for < 4x trick speeds
  - v4l2: expand codec change support

## 1.01.50
- Date: Oct 13, 2023
- Tag/Revision: Westeros-1.01.50
- Highlights:
  - core: fix default zorder for embedded
  - v4l2: send vpc rect updates to video server
  - drm: no video fence for kept frames

## 1.01.49
- Date: Sept 13, 2023
- Tag/Revision: Westeros-1.01.49
- Highlights:
  - brcm: adjust buffer release for embedded comp with exp sync
  - v4l2: fix initial rpi pixel aspect ratio
  - brcm: update decoder for immediate-output

## 1.01.48
- Date: Aug 14, 2023
- Tag/Revision: Westeros-1.01.48
- Highlights:
  - v4l2: ensure frame step is not blocked waiting for preroll
  - erm: serialize notification callbacks
  - brcm: fix resource preemption thread safety

## 1.01.47
- Date: July 13, 2023
- Tag/Revision: Westeros-1.01.47
- Highlights:
  - drm: add hooks for external video stats
  - brcm: fix eos underflow hang during preroll
  - brcm: fix play resume after frame step

## 1.01.46
- Date: June 13, 2023
- Tag/Revision: Westeros-1.01.46
- Highlights:
  - v4l2, raw: relax tolerance for aspect ratio comparisons for zoom
  - drm: fix potential crash with frame advance
  - essos: conditionally use eglGetPlatformDisplayEXT

## 1.01.45
- Date: May 12, 2023
- Tag/Revision: Westeros-1.01.45
- Highlights:
  - v4l2: remove capture workaround for newer rpi drivers
  - drm: update video show/hide when playing but with no queued frames
  - brcm: add support for explicit synchronization

## 1.01.44
- Date: Apr 13, 2023
- Tag/Revision: Westeros-1.01.44
- Highlights:
  - brcm: ignore unexpected extra segment event
  - v4l2: send keep frame setting on ready-to-null
  - raw: add support for tunnelled operation

## 1.01.43
- Date: Mar 13, 2023
- Tag/Revision: Westeros-1.01.43
- Highlights:
  - test: add test cases for sink stats query
  - drm: iterate all planes during termination
  - drm: avoid inappropriate use of join

## 1.01.42
- Date: Feb 13, 2023
- Tag/Revision: Westeros-1.01.42
- Highlights:
  - v4l2: add support for VIDIOC_S_EXT_CTRLS
  - brcm: game mode updates
  - drm, v4l2: add eos msg to video server

## 1.01.41
- Date: Jan 13, 2023
- Tag/Revision: Westeros-1.01.41
- Highlights:
  - brcm: log sdk version from sink
  - brcm: Do NEXUS_SimpleVideoDecoder_SetTrickState at pause/resume
  - drm,v4l2: allow video show/hide while paused

## 1.01.40
- Date: Dec 14, 2022
- Tag/Revision: Westeros-1.01.40
- Highlights:
  - sink: don't clear eosEventSeen on eos post
  - drm: don't set avsync rate on unpause
  - drm: add support for video fence

## 1.01.39
- Date: Nov 14, 2022
- Tag/Revision: Westeros-1.01.39
- Highlights:
  - erm: fix thread safety issue in EssRMgrDestroy
  - test: fixes for running automated tests on ARM64 Linux
  - sink: add display-name property

## 1.01.38
- Date: Oct 14, 2022
- Tag/Revision: Westeros-1.01.38
- Highlights:
  - brcm: reduce memory usage
  - brcm: low latency improvements
  - v4l2: ensure dv flags get reset

## 1.01.37
- Date: Sept 14, 2022
- Tag/Revision: Westeros-1.01.37
- Highlights:
  - v4l2: amlogic: intercept instant rate change requests

## 1.01.36
- Date: August 27, 2022
- Tag/Revision: Westeros-1.01.36
- Highlights:
  - v4l2: ensure changes to stop-keep-frame prop are sent to video server
  - drm: generic avsync updates
  - sink: don't reset segment start position on duplicate segment event

## 1.01.35
- Date: July 27, 2022
- Tag/Revision: Westeros-1.01.35
- Highlights:
  - drm,v4l2,generic avsync: pause video scheduling in amaster mode when audio is not rendering
  - v4l2: use video color enum values
  - sink: ignore extra segment events

## 1.01.34
- Date: June 28, 2022
- Tag/Revision: Westeros-1.01.34
- Highlights:
  - drm: update video rect when playing but with no queued frames
  - v4l2: adjust frame counts on sync mode change
  - drm: improve locking for video plane allocation

## 1.01.33
- Date: May 27, 2022
- Tag/Revision: Westeros-1.01.33
- Highlights:
  - v4l2: adjust eos deadlock prevention in preroll
  - v4l2: keep vpc surface on ERM revoke when keep last frame active
  - sink: fix formatting for event log

## 1.01.32
- Date: April 26, 2022
- Tag/Revision: Westeros-1.01.32
- Highlights:
  - v4l2: avoid deadlock if eos detected during preroll
  - drm, v4l2, raw: support client-side playback rate
  - drm, v4l2: add support for holding last frame on stop

## 1.01.31
- Date: March 24, 2022
- Tag/Revision: Westeros-1.01.31
- Highlights:
  - v4l2: add cast for build issue
  - sink: retry obtaining compositor
  - brcm: add some null checks in sink

## 1.01.30
- Date: Feb 22, 2022
- Tag/Revision: Westeros-1.01.30
- Highlights:
  - v4l2: add support for mjpeg
  - brcm: restore tsm mode with audio dependent clock
  - sink: fix leak after ERM revoke

## 1.01.29
- Date: Jan 27, 2022
- Tag/Revision: Westeros-1.01.29
- Highlights:
  - v4l2: fix eos issue for streams starting without timestamps
  - raw: fix missing return values
  - v4l2: add support for codec_data in caps

## 1.01.28
- Date: Dec 16, 2021
- Tag/Revision: Westeros-1.01.28
- Highlights:
  - v4l2: ensure driver events are checked after last frame decoded
  - brcm: intial support for instant rate change
  - drm: eliminate a source of video latency

## 1.01.27
- Date: Nov 11, 2021
- Tag/Revision: Westeros-1.01.27
- Highlights:
  - v4l2: clear prerollBuffer on flush
  - Fix compilation error
  - Add --enable-essosresmgrfree configure option

## 1.01.26
- Date: Aug 9, 2021
- Tag/Revision: Westeros-1.01.26
- Highlights:
  - drm: allow refresh lock on rpi
  - raw: update vpc geometry on aspect/zoom changes
  - v4l2, raw: fix crash without compositor connection

## 1.01.25
- Date: June 21, 2021
- Tag/Revision: Westeros-1.01.25
- Highlights:
  - v4l2: updates
  - drm: ensure video frame widths are even
  - raw: release preroll lock while paused

## 1.01.24
- Date: May 11, 2021
- Tag/Revision: Westeros-1.01.24
- Highlights:
  - v4l2: Apply new pixel aspect ratio after resolution change
  - drm: ensure received dmabuf fd's have O_CLOEXEC set
  - v4l2: fix potential crash getting input buffers

## 1.01.23
- Date: March 22, 2021
- Tag/Revision: Westeros-1.01.23
- Highlights:
  - sink: v4l2: raw: increase use of essos resource manager
  - drm: fix uninitialized variable
  - drm: allow eglCreateContext to be called prior to eglCreateDisplay

## 1.01.22
- Date: Jan 28, 2021
- Tag/Revision: Westeros-1.01.22
- Highlights:
  - Avoid event thread safety issue introduced in Wayland 1.17.91
  - brcm: fix slow pause/resume transition
  - v4l2: use gst-video to parse colorimetry if available

## 1.01.21
- Date: Nov 11, 2020
- Tag/Revision: Westeros-1.01.21
- Highlights:
  - Limit audience for mode msgs with virtutual embedded compositor
  - Video updates
  - v4l2 updates

## 1.01.20
- Date: Oct 1, 2020
- Tag/Revision: Westeros-1.01.20
- Highlights:
  - Support building Essos without Westeros
  - v4l2+drm updates
  - v4l2: fix potential hang with teardown with no media playing

## 1.01.19
- Date: July 17, 2020
- Tag/Revision: Westeros-1.01.19
- Highlights:
  - v4l1+drm updates
  - drm use vblank time from drmWaitVBlank reply
  - v4l2 updates

## 1.01.18
- Date: June 10, 2020
- Tag/Revision: Westeros-1.01.18
- Highlights:
  - Add icegdl support files
  - Accommodate newer versions of mesa
  - Initial support for secure video on AmLogic-Meson

## 1.01.17
- Date: Apr 16, 2020
- Tag/Revision: Westeros-1.01.17
- Highlights:
  - V4l2 and drm updates plus automated tests
  - Improve essos app ability to control EGL
  - Essos updates

## 1.01.16
- Date: Mar 6, 2020
- Tag/Revision: Westeros-1.01.16
- Highlights:
  - Fix compilation error
  - Fix handle leak in drm video server
  - Changes for building with Yocto 3.1

## 1.01.15
- Date: Feb 19, 2020
- Tag/Revision: Westeros-1.01.15
- Highlights:
  - Fix typo breaking rpi build
  - Video hole punch fix and related tests
  - Expand automated test to drm+v4l2

## 1.01.14
- Date: Jan 28, 2020
- Tag/Revision: Westeros-1.01.14
- Highlights:
  - Update method of detecting if secure video is enabled
  - EOS fixes
  - Ensure window size set for embedded

## 1.01.13
- Date: Jan 8, 2020
- Tag/Revision: 7df197ef3141e6d6218b8d11229d4db334ce2142
- Highlights:
  - Add test case for shm with repeater
  - Remove stop hang defence as root cause is fixed
  - Mode setting enhancements

