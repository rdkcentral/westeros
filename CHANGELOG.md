# Westeros Changelog

---

## [2.1.0] - 24 February 2026

**Tag:** 2.1.0

### Changes
- westeros: add wl_simple_shell popup api
- Fix: vulkan running failure when Ursr update to 25.0.3.
- Resolve coverity issues for westeros

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [2.0.0] - 10 January 2026

**Tag:** Westeros-2.0.0

### Changes
- From this release onwards, Westeros is separated into different repos.
- Westeros-sink reporting pre-seek position after seek, causing playback position jumps on video-only playback
- fix westeros-sink timeCodeFound function
- westeros-soc-brcm: Ignore playback rate 0.25-2.0 when audio is passthrough

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.62] - Oct 28, 2025

**Tag:** Westeros-1.01.62

### Changes
- v4l2: Fix frame dropping boundary condition for seek accuracy
- v4l2: fix compile error on platforms without V4L2_PIX_FMT_AV1 defined
- v4l2: Fix thread race condition causing video decode crashes
- brcm: Increase EOS "safety net" timeout. Currently too short for I-frame only streams (like REW)
- v4l2: update video decode error

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.61] - Sep 30, 2025

**Tag:** Westeros-1.01.61

### Changes
- essos: Blacklist status, fix revoke defect

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.60] - Sep 10, 2025

**Tag:** Westeros-1.01.60

### Changes
- v4l2: Add low-latency-mode for Netflix DPI 7.0 support
- brcm: Fix "NXCLIENT_BAD_SEQUENCE_NUMBER" error when leaving Netflix DolbyVision
- brcm: Add check for stc_channel==0 to reduce error logging during gaming/low latency

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.59] - June 24, 2025

**Tag:** Westeros-1.01.59

### Changes
- if segstart==0 and 1st decoded pts is small, use 0 for base of position
- Fixes for YTS HW Dual Video Tests
- Fix NPLB timeAfterSeek Tests
- v4l2: decoder reset locking and send HLG info to pqserver by fifo
- avoid gint64 overflow when calculating PTS value
- Fix race condition with vidfilter placing the first buffer into playpump vs setStartPTS/Flush
- Retry VideoDecoder_Start() on OUT_OF_MEMORY error
- Device Frozen at App Launch Screen fix
- fix for white line at start of DV test stream
- API to reset first frame event
- essos: fix xkb and wl resources leak
- WesterosSink fails to complete preroll after seek fix
- add linux-explicit-synchronization for westeros
- Fix framestep for NTS VPEEK test
- AAMP L3 Test 2003, 2005, 2006 failures fixes

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.58] - Feb 20, 2025

**Tag:** Westeros-1.01.58

### Changes
- drm: Fix heap corruption resulting from race condition in video disconnect
- sink: Default position to GST_CLOCK_TIME_NONE instead of 0
- drm/sink: Westeros RT Thread Updates
  - Update westeros thread names
  - Allow RT thread priority to be overridden from kernel space
  - Add VSync marker stats
  - FTRACE Debug support controlled via flag
- erm: fix demarsheling of video info in resource request
- all: Cleanup comments containing proprietary device names
- drm: ignore invalid video server rate changes
- sink: Support for video pipeline graph in text form in westerossink
- brcm: prerollrate setting was not correctly honored pre URSR_24; remove prerollrate in URSR_24 so it works the same as previously
- v4l2: configuring h264 output to AFBC only / double write compressed mode
- v4l2: Ensure geometry ends in same state even if window, zoom-mode, afd and pixel aspect-ratio are set (or arrives from stream) in different order.

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.57] - Nov 25, 2024

**Tag:** Westeros-1.01.57

### Changes
- Handle Active Format Descriptor (AFD) in all zoom modes in v4l2
- Robustness improvements during shutdown
- Timecode PTS processing fix
- Remove ALLM management code. Don't enable ALLM in "immediate-output" mode for Brcm
- Essos key repeat updates

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.56] - July 23, 2024

**Tag:** Westeros-1.01.56

### Changes
- Enable frame step on preroll by default in v4l2
- support for assigning surface ownership to virtual embedded compositors
- Disable frame step on preroll by default; Allow to display frame on paused state

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.55] - May 17, 2024

**Tag:** Westeros-1.01.55

### Changes
- all: fix some compile warnings
- sink: fix compiler warnings
- essos: update key repeat — filter repeated key press events for the same key so that they do not terminate essos key repeat which generates key repeat events while a key is pressed until the key is released or a key press for a different key is received
- core, sink: fix compiler warnings
- brcm: adjust decode rate to 4.0 for immediate output; set trickstate rate to 4.0 to allow decoding faster in immediate mode case

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.54] - Apr 17, 2024

**Tag:** Westeros-1.01.54

### Changes
- brcm: remove ENABLE_LEXPSYNCPROTOCOL from public headers — removed the `#ifdef ENABLE_LEXPSYNCPROTOCOL` from the public `_WstRenderer` structure because it could cause struct misalignment if external renderers don't have ENABLE_LEXPSYNCPROTOCOL set.
- v4l2: fix loss of precision in conversion from nanoseconds to 90KHz
- brcm: adjust decode rate for immediate output — set trickstate rate to 2.0 to allow for overflow recovery and low latency decode without excessive frame dropping.
- core: remove unused local variable

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.53] - Mar 13, 2024

**Tag:** Westeros-1.01.53

### Changes
- v4l2: support use of session sync mode from asink (off by default, enabled at build time with `-DUSE_AUDIOSINK_SESSION_MODE`)
- v4l2: enable DI and NR (NR is disabled when playing progressive movies)
- brcm: Set "videoStarted= FALSE" early in sinkReleaseVideo() to prevent underflow callback lock() race condition
- essos: allow EssAppPlatformDisplayType_waylandExtension on supported platforms
- brcm: adjust decode rate for immediate output — set trickstate rate to 1.4 instead of 1.2 or 2.0 to allow for overflow recovery and low latency decode without excessive frame dropping
- drm: fix false underflow unpausing from frame advance

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.52] - Jan 11, 2024

**Tag:** Westeros-1.01.52

### Changes
- drm: add console command to toggle gfx scaling
- brcm: reduce false underflows on framerate changes
- drm: display gfx centered when actual-size active
- brcm: use decode rate 1.2 for immediate-output
- erm: fix potential crash on client destroy
- v4l2: handle eos prior to capture start
- essos: new api to determine the app platform display type and if extensions should be used
- core: reduce embedded client render latency
- brcm: fix pause at playrates other than 1x
- drm: fix potential video frame leak under heavy cpu load

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.51] - Nov 13, 2023

**Tag:** Westeros-1.01.51

### Changes
- v4l2: support on-the-fly codec changes
- brcm: use all decode mode for < 4x trick speeds
- v4l2: expand codec change support
- v4l2: fix video server pause race condition — if the video server has been paused and there is a seek followed by a transition from paused to playing there was a chance the video output thread would think the video server was not paused thus leaving it paused resulting in black video
- drm: clear frameAdvance on frame pop in play state
- v4l2: process video server messages while paused

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.50] - Oct 13, 2023

**Tag:** Westeros-1.01.50

### Changes
- core: fix default zorder for embedded
- v4l2: send vpc rect updates to video server
- drm: no video fence for kept frames
- essos: fix vector iteration in essFreeInputDevices
- brcm: fix deadlock in sw decoder data probe destruction
- v4l2: svp support updates — add init/term methods, add method to pass caps

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.49] - Sept 13, 2023

**Tag:** Westeros-1.01.49

### Changes
- brcm: adjust buffer release for embedded comp with exp sync
- v4l2: fix initial rpi pixel aspect ratio — for older rpi drivers that require capture setup prior to source change events ensure the initial pixel aspect ratio is applied
- brcm: update decoder for immediate-output — reconfigure video decoder for low latency if the immediate-output property is set after the decoder is configured
- v4l2: protect against double renders from basesink

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.48] - Aug 14, 2023

**Tag:** Westeros-1.01.48

### Changes
- v4l2: ensure frame step is not blocked waiting for preroll
- erm: serialize notification callbacks
- brcm: fix resource preemption thread safety
- test: useful additions to westeros-player: emit timecodes with -T argument, exclude audio with -M argument, show/hide/peek video via touch files
- drm: preserve vfm pause state across flush
- raw: raw sink updates — add support for generic av sync, add support for single buffer NV12 output (enable with `-DUSE_SINGLE_BUFFER_NV12`)
- v4l2: ensure buffer unmap in render error path
- brcm: publish explicit sync header during build
- v4l2: add vframe-source-type property to identify DTV

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.47] - July 13, 2023

**Tag:** Westeros-1.01.47

### Changes
- drm: add hooks for external video stats
- brcm: fix eos underflow hang during preroll
- brcm: fix play resume after frame step
- drm: thread name and priority changes — set names to key threads, add env vars for offload and video connection thread priorities
- v4l2: allow frame step request prior to preroll
- drm: filter connector events from hotplug events
- drm: improve display mode specifier parsing
- drm: process all pending uevent as a batch
- drm: no mode set pending in create native window
- drm: display connection fix — enforce display server connection number limit prior to creating connection thread
- drm: adjust expiry limit for low frame rates
- drm: prevent native fence fd leak — during mode changes it is possible to skip an atomic update which would leak a native fence fd
- drm: prefer hdmi connectors
- drm: resize video if mode size changes while paused

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.46] - June 13, 2023

**Tag:** Westeros-1.01.46

### Changes
- v4l2, raw: relax tolerance for aspect ratio comparisons for zoom
- drm: fix potential crash with frame advance
- essos: conditionally use eglGetPlatformDisplayEXT
- sink, brcm: frame advance changes
- core, essos: fallback to eglGetDisplay if eglGetPlatformDisplayEXT fails

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.45] - May 12, 2023

**Tag:** Westeros-1.01.45

### Changes
- v4l2: remove capture workaround for newer rpi drivers
- drm: update video show/hide when playing but with no queued frames
- brcm: add support for explicit synchronization — disabled by default; enable at build time with `--enable-lexpsyncprotocol` during configure; required to be enabled with use of Broadcom supplied wayland-egl in place of wayland-egl-bnxs

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.44] - Apr 13, 2023

**Tag:** Westeros-1.01.44

### Changes
- brcm: ignore unexpected extra segment event
- v4l2: send keep frame setting on ready-to-null — if the stop-keep-frame property has been explicitly set then on the ready-to-null transition connect to the video server to update the new stop-keep-frame value
- raw: add support for tunnelled operation
- v4l2: logging update — add more detail to the verbose logs for VIDIOC_G_SELECTION and VIDIOC_S_SELECTION ioctls
- v4l2: fix locking in soc_render for dmabuf
- brcm, v4l2, raw: shorten thread names
- drm: fix commit error on display disable

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.43] - Mar 13, 2023

**Tag:** Westeros-1.01.43

### Changes
- test: add test cases for sink stats query
- drm: iterate all planes during termination
- drm: avoid inappropriate use of join
- drm: prevent buffer leak with keep last frame

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.42] - Feb 13, 2023

**Tag:** Westeros-1.01.42

### Changes
- v4l2: add support for VIDIOC_S_EXT_CTRLS — on Amlogic, use VIDIOC_S_EXT_CTRLS instead of VIDIOC_S_PARM when the Linux version is >= 5.15
- brcm: game mode updates — add support for HDMI ALLM; improve suppression of video underflows with immediate-output; don't alter decode rate when using eGame latency mode
- drm, v4l2: add eos msg to video server — add an eos msg that westerossink can send to the westeros-soc-drm video server to indicate eos, enabling the video server to flush the display queue in case there is a start threshold keeping the frames from displaying
- drm: fix potential use of null ptr
- brcm: allow HDR source-follow to be disabled (disabled by default at build time; enable by defining `NEXUS_HAVE_HDMI_OVERRIDE_MATRIX`)
- essos: add runtime logging verbosity control — set log level with env var `ESSOS_DEBUG=level` where level is 0-4
- drm: make refresh lock available with any driver
- essos: fix event loss during hot plug handling
- brcm: fix eotf handling with gst 1.18
- sink: log gstreamer version

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.41] - Jan 13, 2023

**Tag:** Westeros-1.01.41

### Changes
- brcm: log sdk version from sink
- brcm: Do NEXUS_SimpleVideoDecoder_SetTrickState at pause/resume — video only stream will not pause by NEXUS_SimpleStcChannel_Freeze since it is in vsync mode (tsm off)
- drm, v4l2: allow video show/hide while paused
- drm: only set mode from env on initial window creation
- drm: fix locking for offload thread — fix thread safety issue between the offload thread and the disconnection of a video server client thread

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.40] - Dec 14, 2022

**Tag:** Westeros-1.01.40

### Changes
- sink: don't clear eosEventSeen on eos post
- drm: don't set avsync rate on unpause unless we've previously set it to non-unity
- drm: add support for video fence (enable at build time with `-DDRM_USE_VIDEO_FENCE`)
- erm: serialize transactions by resource type

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.39] - Nov 14, 2022

**Tag:** Westeros-1.01.39

### Changes
- erm: fix thread safety issue in EssRMgrDestroy
- test: fixes for running automated tests on ARM64 Linux
- sink: add display-name property
- v4l2: fix guarding of a non-generic call
- sink: add stats log — to enable define `WESTEROS_SINK_STATS_LOG=N` in the player environment where N is the desired log interval in milliseconds

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.38] - Oct 14, 2022

**Tag:** Westeros-1.01.38

### Changes
- brcm: reduce memory usage — don't pre-allocate capture surfaces if secure video is used without secure graphics
- brcm: low latency improvements — add latency logging; push new video frame PTS and systemtime to vidfilter; add event to tell vidfilter to do lowlatency; enable earlyPictureDeliveryMode, disable mtgAllowed/captureMode
- v4l2: ensure dv flags get reset — when new caps arrive, ensure dv flags are reset if the dv structures are not present
- brcm: return VideoDynamicRangeMode to eTrackInput after DV stream
- brcm: correct surface visibility query
- Expose surface scaling via the simpleshell
- brcm: use NEXUS_VideoDecoderLowLatencyMode_eGaming with immediate-output if available
- v4l2: generic avsync update — allow eos detection in cases where audio is of shorter duration than video

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.37] - Sept 14, 2022

**Tag:** Westeros-1.01.37

### Changes
- v4l2: amlogic: intercept instant rate change requests — Amlogic audio sink handles instant rate change requests; Westeros sink should intercept it as well to avoid subsequent GST_EVENT_INSTANT_RATE_SYNC_TIME

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.36] - August 27, 2022

**Tag:** Westeros-1.01.36

### Changes
- v4l2: ensure changes to stop-keep-frame prop are sent to video server
- drm: generic avsync updates — fix spurious underflow when audio halted; add missing av progression drop log
- sink: don't reset segment start position on duplicate segment event
- brcm: fix operation of legacy server-play-speed property
- sink: add locking for wayland resource release
- erm: increase client msg buffer size
- v4l2: amlogic: use resmen to reserve svp memory

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.35] - July 27, 2022

**Tag:** Westeros-1.01.35

### Changes
- drm, v4l2, generic avsync: pause video scheduling in amaster mode when audio is not rendering
- v4l2: use video color enum values
- sink: ignore extra segment events — defend against extra segment events sent by basesrc in pipelines using appsrc elements when performing trick play
- drm: add support for AV_SYNC_MODE_VIDEO_MONO
- v4l2: use window rect for vpc geometry updates on source res change
- v4l2: add support for new Amlogic double write modes

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.34] - June 28, 2022

**Tag:** Westeros-1.01.34

### Changes
- drm: update video rect when playing but with no queued frames
- v4l2: adjust frame counts on sync mode change
- drm: improve locking for video plane allocation
- v4l2: use dw 1:1 for film grain and interlaced h265
- v4l2: initialize structs with memset
- raw, v4l2: use 64 bit offsets for DRM_IOCTL_MODE_MAP_DUMB
- brcm, raw, v4l2: prevent redundant surface commits on rectangle changes
- v4l2: improve defense against flushless sync mode changes
- v4l2: allow vpc scaling when playing but no new frames
- erm: use CLIENT_IDENTIFIER if ESSRMGR_APPID is not defined
- brcm, v4l2: support mastering-display-info in gstreamer 1.18 and later

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.33] - May 27, 2022

**Tag:** Westeros-1.01.33

### Changes
- v4l2: adjust eos deadlock prevention in preroll
- v4l2: keep vpc surface on ERM revoke when keep last frame active
- sink: fix formatting for event log
- brcm: fix spurious underflows with useImmediateOutput — filter emitUnderflow with useImmediateOutput to remove underflow messages/logging flood
- drm: fix errors from video positioned off screen while paused
- v4l2, raw, brcm: add "stats" property
- test: fix some test case issues
- drm: fix mutex locking order for rect update

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.32] - April 26, 2022

**Tag:** Westeros-1.01.32

### Changes
- v4l2: avoid deadlock if eos detected during preroll
- drm, v4l2, raw: support client-side playback rate
- drm, v4l2: add support for holding last frame on stop — set sink property `stop-keep-frame=true` to have the last frame continue to display after the pipeline stops instead of displaying black
- v4l2: fix cases of truncated data when codec data is in caps
- v4l2: don't reset stop-keep-frame on return to ready state
- sink: re-request resources on ready to paused if revoked
- erm: add black list — mechanism to prevent certain entities from requesting resources at certain times; appid assigned via env var `ESSRMGR_APPID`
- test: fix some video play position checks
- erm: fix possible bad appid
- v4l2: fix race condition between dequeue and decoder teardown

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.31] - March 24, 2022

**Tag:** Westeros-1.01.31

### Changes
- v4l2: add cast for build issue — fix in 64 bit RPi4
- sink: retry obtaining compositor
- brcm: add some null checks in sink
- v4l2: fix potential crash — join wayland dispatch thread prior to tearing down output buffers
- test: fixes/improvements to automated test code
- drm: fix errors from video positioned offscreen
- v4l2: add locking around input buffer creation
- v4l2: add support for VIDIOC_CROPCAP to check pixel-aspect-ratio
- erm: uds: fix thread safety issue in some requests
- drm: fix potential glitch on video client close
- v4l2: add some memory reduction build options (`-DWESTEROS_SINK_LOW_MEM_DWMODE`, `-DWESTEROS_SINK_LOW_MEM_BUFFERS`)

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.30] - Feb 22, 2022

**Tag:** Westeros-1.01.30

### Changes
- v4l2: add support for mjpeg
- brcm: restore tsm mode with audio dependent clock
- sink: fix leak after ERM revoke — fixes potential resource leaks when a sink element is disposed of after having had its resources revoked by ERM
- essos: fix crash in wl_egl_window_create for platforms with weston
- brcm: release capture buffers on ERM revoke
- brcm: fix position error from decoder pts status rollover
- v4l2: fix log message
- brcm: fix false rollover detection
- essos: make new eglGetDisplay call conditional — present only if HAVE_WESTEROS is not defined; fixes a regression in another platform caused by this change
- v4l2: reduce eos count down
- brcm: sink updates — 1080i fallback to SDR; use first pts rather than first pts passed callback

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.29] - Jan 27, 2022

**Tag:** Westeros-1.01.29

### Changes
- v4l2: fix eos issue for streams starting without timestamps
- raw: fix missing return values
- v4l2: add support for codec_data in caps
- erm: make revoke timeout configurable
- erm: update missed timeouts to configured value
- drm: clear avsync video_config struct
- v4l2, raw: fix vpc positioning for textures — November 2021 commit fixed positioning on the video plane but broke positioning of textures
- essos: fix possible hang in essProcessRunWaylandEventLoopOnce
- erm: fix race condition — fix a race condition that can result in the first resource request of a context using the wrong revoke timeout

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.28] - Dec 16, 2021

**Tag:** Westeros-1.01.28

### Changes
- v4l2: ensure driver events are checked after last frame decoded
- brcm: initial support for instant rate change
- drm: eliminate a source of video latency — perform avsync initialization earlier to avoid introducing an additional one vertical period latency
- v4l2, raw: use window rect for vpc geometry updates
- drm: fix offload queue full resource leak
- drm: update amlogic decoder config
- erm: avoid allocating large state in stack
- v4l2: respect user set avsync mode
- drm, v4l2: fix a cause of missing eos — prevent video server status messages that pertain to events prior to a flush from updating state after the flush
- drm, v4l2: add immediate-output — add support for low latency video which can be enabled by setting the immediate-output westerossink property to true
- drm: fix display server startup race condition
- drm, v4l2, raw: add policy control for zoom — two policies: `global-zoom-active`, `allow-4k-zoom`; configurable via env vars `WESTEROS_GL_GLOBAL_ZOOM_ACTIVE` and `WESTEROS_GL_ALLOW_4K_ZOOM`; also settable/queryable via westeros-gl-console commands

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.27] - Nov 11, 2021

**Tag:** Westeros-1.01.27

### Changes
- v4l2: clear prerollBuffer on flush
- Fix compilation error
- Add `--enable-essosresmgrfree` configure option — allows building essos library without the essosrmgr library
- Add generating essos pkg-config file
- v4l2: fix eos when playback starts with out-of-segment frames
- v4l2: refine resolution change processing
- brcm: use first frame cb to improve drop counts
- sink: use raw PTS to track SEI timecodes
- essos: allow loop throttling for direct linux — event loop throttling for direct linux operation (non-wayland) can be disabled by defining `ESSOS_DIRECT_NO_EVENT_LOOP_THROTTLE`
- drm, v4l2: updates to reduce frame drops at 60 fps
- v4l2: report unthresholded queued-frames
- v4l2, raw: apply stretch and zoom to 16:9 content
- drm: adjust frame queue locking
- drm: flush offload queue on video connection destroy
- v4l2: don't expect last frame flag on rpi v4l2 res changes
- v4l2: add support for AV1 with Dolby Vision

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.26] - Aug 9, 2021

**Tag:** Westeros-1.01.26

### Changes
- drm: allow refresh lock on rpi
- raw: update vpc geometry on aspect/zoom changes
- v4l2, raw: fix crash without compositor connection
- sink: brcm: fix race condition for start video
- essos: avoid unnecessary dispatch call
- v4l2: add support for auto dw mode
- sink: brcm, v4l2: use g_error_new_literal instead of g_error_new
- drm, v4l2 updates — use v4l2 decoder stop command and eos events; adjust underflow detection when applied_rate is not 1.0; ensure vmaster mode used when applied_rate is not 1.0
- drm: auto frm updates — for Amlogic SOC interpret modes rate 47 as 47.95 and 59 as 59.94
- drm: updates for rpi — ensure primary plane is used for graphics; use zpos to place primary plane above video
- drm, v4l2, raw: support altering video rect while paused
- core: update dependencies for linux dmabuf protocol
- v4l2: force local position reporting if applied_rate is not 1.0
- drm: fix mode resource blob leak
- v4l2: use bit masking to check for event types

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.25] - June 21, 2021

**Tag:** Westeros-1.01.25

### Changes
- v4l2: updates — update aml parm structures; update buffer margin policy for mpeg2; skip capture buffers with V4L2_BUF_FLAG_LAST flag set and bytesused set to 0
- drm: ensure video frame widths are even
- raw: release preroll lock while paused
- brcm: update stc freeze based on state
- v4l2: adjust calculation of queued_frames
- v4l2: estimate frame rate if not supplied in caps
- drm: update video server frame cropping
- v4l2 updates — fix setting zoom none as a local override; handle multiple decoded frames waiting to dequeue when POLLPRI is signaled
- Try to address some race conditions and resource connection leak
- erm, sink: add resource activity state tracking
- erm: add options to sample app for resource state
- Address some race conditions on resource revocation — ERM: capture EssRMgrRequest by value; wait for revoke notifications completion. Sink: additional locking to address some revocation scenarios
- v4l2: check and log dequeue errors

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.24] - May 11, 2021

**Tag:** Westeros-1.01.24

### Changes
- v4l2: Apply new pixel aspect ratio after resolution change
- drm: ensure received dmabuf fd's have O_CLOEXEC set
- v4l2: fix potential crash getting input buffers
- v4l2: add a stack for managing pixel-aspect-ratio
- v4l2: check pts valid in wstChain
- v4l2, drm: add support for amlogic msync
- raw: add missing locking
- drm: fix sync leak on mode change
- v4l2: extend pts interpolation to non-interlaced content
- v4l2: report QOS every 120 decoded frames
- Fix compilation errors
- Add missing dependencies: libdrm and gbm
- Fix configure premature error caused by AC_MSG_ERROR
- Fix compilation error (cast from void* to uint32_t loses precision)
- Fix cast warning (cast to pointer from integer of different size)
- essos resource manager updates — two implementations now available: shared memory (default) and unix domain sockets (enable with `-DUSE_ESSRMGR_UDS_IMPL`); the uds impl uses a centralized resource server

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.23] - March 22, 2021

**Tag:** Westeros-1.01.23

### Changes
- sink, v4l2, raw: increase use of essos resource manager — runtime disabled by default; enable with env var `WESTEROS_SINK_USE_ESSRMGR=1`
- drm: fix uninitialized variable
- drm: allow eglCreateContext to be called prior to eglCreateDisplay
- essos: add audio to resource manager
- drm: revert adjustment of video plane order — remove change affecting AmLogic video plane order
- v4l2: fix definition of low_latency_mode in aml_vdec_cfg_infos
- drm, sink, v4l2, raw: updates — v4l2 and raw sink now send content frame rate to video server; update underflow logic to work with amlogic avsync module enabled
- rpi: updates — fix potential uninitialized var use in wstGLSetupDisplay; fix potential failure to release mutex
- drm: add frame rate matching mode — runtime disabled by default; enable with westeros-gl-console command `set auto-frm-mode 1`
- drm: clear underflow condition after it is reported
- sink: fix resource release on shutdown — fix incorrect call to EssRMgrReleaseResource on element shutdown for case where resource has been already revoked
- v4l2: adjust hdr parameter handling for amlogic
- v4l2: adjust locking in wstSinkSocStopVideo
- drm: add westeros-gl-console helper lib
- essos: fix async grant notifications for audio
- drm: fix leak of avsync struct vframe

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.22] - Jan 28, 2021

**Tag:** Westeros-1.01.22

### Changes
- Avoid event thread safety issue introduced in Wayland 1.17.91
- brcm: fix slow pause/resume transition
- v4l2: use gst-video to parse colorimetry if available
- Video updates — add support for SEI time codes; time code processing and signal generation disabled by default but can be enabled via the `enable-timecode` property
- Support querying formats and modifiers for EGL_EXT_image_dma_buf_import_modifiers
- Get egl display with eglGetCurrentDisplay in embedded renderer
- v4l2: correct surface size on transition to graphics path
- Virtual embedded updates — add ability to bind externally launched clients to specific virtual embedded compositor instances; add unbound client listener; add bind client api
- v4l2+drm updates — add support for frame advance (not yet supported by amlogic avsync module)
- test: add video position tests for drm
- video updates — add a 'raw' video backend for westerossink; assorted fixes for raw sink
- video updates — post bus message for decode error for brcm and v4l2
- v4l2: add support for AV1 codec

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.21] - Nov 11, 2020

**Tag:** Westeros-1.01.21

### Changes
- Limit audience for mode msgs with virtual embedded compositor
- Video updates — add Essos resource manager for coordinated use of video resources; integrate use of resource manager into westeros-sink; add software video decode capability to westeros-sink. Use of resource manager is runtime disabled by default (`WESTEROS_SINK_USE_ESSRMGR=1`). Software decode support disabled at build time by default, enable with `-DENABLE_SW_DECODE`.
- v4l2 updates — adjust stop sequence to stop unwanted logs; fix buffer leak on flush with dma-buf input; fix possible crash on pause to ready transition; fix memory leak on eos thread exit; fix size of msg buffer for session info
- Video updates — extend software decode support to v4l2 platform; fixes and improvements to software decode operation
- drm: change to m4 configure macro dir
- Video updates — separate Essos resource manager into libessosrmgr.so; sinks link with libessosrmgr.so rather than libessos.so
- v4l2 westeros-sink: change to m4 configure macro dir
- Fix race condition on decoder release by owner
- v4l2: fix potential crash during flush
- Add support for zwp_linux_dmabuf_v1 — disabled by default, enable with `--enable-ldbprotocol` during configure
- Update license
- v4l2: fix potential crash on tear down

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.20] - Oct 1, 2020

**Tag:** Westeros-1.01.20

### Changes
- Support building Essos without Westeros — add `EXTRA_OECONF = "--enable-essoswesterosfree"` to a westeros.bbappend
- v4l2+drm updates — improve support for av sync of video as texture on platforms that support simultaneous use of a v4l2 capture buffer for both texture and display to a drm video plane (currently enabled for AmLogic only)
- v4l2: fix potential hang with teardown with no media playing
- v4l2: add support for force-aspect-ratio property
- drm uevent hotplug — add support for HDMI hotplug event processing via uevent; runtime disabled by default (`WESTEROS_GL_USE_UEVENT_HOTPLUG=1`, but only for Amlogic Meson)
- v4l2: fix vertical positioning with force-aspect-ratio
- v4l2: treat x,y from force-aspect-ratio as offsets for rectangle property
- v4l2: process v4l2 events while paused
- drm: add support for connector color space property
- v4l2: remove flush from resolution change
- v4l2: fix setting of num_planes for capture format
- v4l2: join dispatch thread prior to exit from wstSinkSocStopVideo
- drm: create connection threads as detached
- v4l2: act on every non-initial source change event
- Input keys metadata processing for IR
- Add meta key modifier
- v4l2: use presence of audio sink in sync mode determination
- v4l2+drm updates — add status msg from soc-drm to sink giving position and drop count; sink position driven by soc-drm position; support dynamically changing sync mode; fix overflow error in pts conversion
- brcm: exclude out-of-segment frames from drop count

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.19] - July 17, 2020

**Tag:** Westeros-1.01.19

### Changes
- v4l2+drm updates — accept out-of-segment data after a seek to provide context to the v4l2 decoder but start display with first in-segment frame; sink passes pts value to video server; drm refresh thread now uses drmWaitVBlank (can be disabled with `WESTEROS_GL_NO_VBLANK=1`); improvements to video rate control
- drm: use vblank time from drmWaitVBlank reply
- v4l2 updates — make conversion from nanoseconds to 90KHz consistent; rate control handles discontinuities from decoder
- Fix resource cleanup when hiding video plane and frame import error
- Fix vpc for non-repeating nested composition
- v4l2+drm updates — add 'memory:DMABuf' to sink caps; secure video updates for AmLogic; round up odd video frame height in video server
- essos updates — fix wayland-egl detection on amlogic; add log showing supported operating modes
- Reset base times when freeing overlay plane
- Fix rate setup for immediate output
- v4l2+drm updates — fix flash on video stream resolution change; fix potential bad amlogic dw mode after stream resolution change; fix improper queuing of v4l2 output buffers; fix sink position reporting after seek
- drm: prevent bad video flip on client disconnect
- v4l2+drm updates — restructure westeros-gl video server to use VideoFrameManager; add display server and westeros-gl-console; initial support for amlogic av sync module (enable with `WESTEROS_GL_USE_AVLOGIC_AVSYNC=1`); allow sink to freerun with `WESTEROS_SINK_USE_FREERUN=1`
- Clear flag after thread join
- Ensure listeners called when mode set by create native window
- Add missing resize for nested renderer

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.18] - June 10, 2020

**Tag:** Westeros-1.01.18

### Changes
- Add icegdl support files
- Accommodate newer versions of mesa
- Initial support for secure video on AmLogic-Meson
- Use QoS to signal dropped frames
- v4l2 updates — re-subscribe for events after hard decoder reset; avoid double rendering of buffer during preroll; retain reference on input dma-buf gstbuffer until dequeued from driver
- Fix crash on video server over subscribe — handle asynchronous frame updates at various rates; update display at mode rate; video server informs clients of display refresh rate; sink drops frames if video frame rate exceeds display rate; improve sink pause/resume; automatic screen refresh on HDMI hot plug; rely on gstreamer for AV sync; fixes for some sources of tearing; sink uses /dev/dri/renderD128 for SVP
- v4l2: fix gem allocation for amlogic dw mode 0
- v4l2 updates — increase input buffer sizes; fix sequencing of Amlogic svp decoder configuration
- Move gamepad from joystick to evdev api
- drm+v4l2 updates — remove reference to unneeded header file; add compilation guards for references to newer V4L2 formats
- v4l2 updates — adjusting management of Amlogic dw mode; move to 4M input buffers
- Fix alpha application with WstHints_applyTransform

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.17] - Apr 16, 2020

**Tag:** Westeros-1.01.17

### Changes
- V4l2 and drm updates plus automated tests — drm fixes for issues found when rapidly starting and stopping video server; v4l2 setup input buffers upon receipt of first input buffer; v4l2 fix preroll, seeking, frame stepping; add a number of drm/v4l2 specific automated tests
- Improve essos app ability to control EGL — add EssContextSetEGLSurfaceAttributes, EssContextGetEGLSurfaceAttributes methods
- Essos updates — add EssContextDestroyNativeWindow; don't do internal EGL setup if app is doing external EGL
- Sink restructuring — move wayland resource acquisition/release from init/term to state transition; break out of wayland dispatch to prevent hang
- Essos updates — fix some names; don't perform platform term in EssContextStop; only run event loop if context running in EssContextRunEventLoopOnce
- Sink updates for auto-plugging
- Ensure sink rectangle property acted on first time — updates are done regardless for the first instance of setting the property
- Revert to previous behavior in EssContextRunEventLoopOnce
- Restructure essos — divide API into app and system; add explicit keyRepeat method to key listener
- Remove duplicate API declaration for EssContextSetName
- Fix video connection initialization error — faulty initialization of a field holding a file descriptor
- Sink updates for brcm — add property `immediate-output` as alias for `camera-latency`; add property `force-aspect-ratio` for use by playbin; clean up some unneeded newlines in logs
- Drm updates — use output fence in atomic updates; handle devices that return null connector when HDMI cable is unplugged

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.16] - Mar 6, 2020

**Tag:** Westeros-1.01.16

### Changes
- Fix compilation error — brcm: allow building with new Broadcom SDK versions where NXPL_CreateCompatiblePixmap no longer exists while still supporting older SDK versions
- Fix handle leak in drm video server — drm: use DRM_IOCTL_GEM_CLOSE to release handles obtained from drmPrimeFDToHandle
- Changes for building with Yocto 3.1 — fix build issues with Yocto 3.1
- V4l2 updates — handle caps received prior to opening driver; fix munumap issue; fix sync issue on shutdown
- Ensure capture thread is stopped — brcm: if capture thread is started and the transition to paused state doesn't complete it needs to be stopped before the element is finalized
- Adjust locking in v4l2 sink — fix some crashes
- Enhance drm emulation for videoserver automated testing — expand drm emulation in automated test to support testing of westeros video server
- v4l2 update: fix input format selection from caps
- Query and log drm version
- Remove unconditional position log

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.15] - Feb 19, 2020

**Tag:** Westeros-1.01.15

### Changes
- Fix typo breaking rpi build — rpi (userland+omx): fix typo in code causing a build failure
- Video hole punch fix and related tests — sink: fix some video issues with video alpha hole punching and update related test cases
- Expand automated test to drm+v4l2 — expanded automated test system to include emulation for testing configurations based on KMS+drm+v4l2
- V4l2 updates — fix max frame size detection and make sink caps based on format support discovered from v4l2 driver
- Drm updates — support zpos plane attribute; prefer primary plane for graphics; video server adjusts frame size based on mode and gfx res; suppress video server with `WESTEROS_GL_NO_VIDEOSERVER=1`; no native fence if video server used
- Add support for v4l2 events — sink (v4l2): add support to v4l2 sink for using v4l2 events
- Fixes for brcm video positioning at various resolutions — core compositor, sink: fixes for video positioning with VPC bridge
- Update method of detecting if secure video is enabled — brcm: update the method used to determine at runtime if a Broadcom SOC device has SVP (secure video path) enabled
- Fix brcm video show property and add test case — sink (brcm): fix operation of the show-video-window property
- Fix video scaling with emb output size with vpc bridge — core compositor: remove code breaking vpc propagation of output size when running in a bridged configuration

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.14] - Jan 28, 2020

**Tag:** Westeros-1.01.14

### Changes
- Update method of detecting if secure video is enabled — brcm: update the method used to determine at runtime if a Broadcom SOC device has SVP (secure video path) enabled
- EOS fixes — sink: don't generate eos during underflow without eos from upstream and ensure synchronous transition from PAUSED to PLAYING after preroll; add automated test case to cover this scenario
- Ensure window size set for embedded — reference app: fix bug where window size defaults to zero if no --window-size command line argument was supplied and running as embedded compositor
- Video positioning fixes and related tests — core compositor, sink: fix video positioning in vpc bridge configuration when running at various output resolutions; add automated test case covering 23 video positioning scenarios
- Use NV12M for multiplane when available — v4l2: preferentially use NV12M format for video on v4l2 devices which report support for this capture format

### Dependencies
- wayland >= 1.6.0
- libxkbcommon >= 0.8.3
- xkeyboard-config >= 2.18
- gstreamer >= 1.10.4
- EGL >= 1.4
- GLES >= 2.0

---

## [1.01.13] - Jan 8, 2020

**Revision hash:** 7df197ef3141e6d6218b8d11229d4db334ce2142

### Changes
- Add test case for shm with repeater — a new test case added to the automated unit test system to cover client rendering with Wayland shared memory across a repeating compositor
- Remove stop hang defence as root cause is fixed — core compositor: a bug whereby the compositor display update timer can be set to a huge value upon system time change was fixed in version 1.01.12; this change removes some defensive code that was temporarily added before the root cause was known
- Mode setting enhancements — essos: provide EssContextSetDisplayMode and add convenience `--window-size` cmdline arg to reference compositor; the EssContextSetDisplayMode API uses the new westeros-gl WstGLSetDisplayMode API when available (on devices using KMS+DRM)
- Mode setting enhancements — drm: add WstGLSetDisplayMode for explicit mode setting and enhance ability for graphics resolution to differ from display resolution; supports mode string format `[wx]h[p|i][[x]r]` (e.g. `1920x1080i60`, `720p`, `1080i`, `3840x2160`, `1920x1080x24`); enables HDMI output at 4K while rendering graphics at lower resolution like 1080p
- Sink gets display size from compositor — sink: binds to wl_output in order to get display size information from the compositor; eliminates the need for any downstream patching related to default screen size
- Act on rectangle property only when values change — sink: sink element will now only act on its 'rectangle' property if the provided values differ from the current values
- Avoid possible null dereference on error path
