/*
 * GStreamer Stub Header for L1 Testing
 * This is a minimal stub to allow compilation without GStreamer
 */
#ifndef __GST_GST_H__
#define __GST_GST_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// GLib type definitions
typedef unsigned int guint;
typedef unsigned int guint32;
typedef int gint;
typedef bool gboolean;
typedef float gfloat;
typedef double gdouble;
typedef void* gpointer;
typedef const void* gconstpointer;
typedef unsigned long gulong;
typedef unsigned long long guint64;
typedef long long gint64;
typedef char gchar;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#define G_SOURCE_REMOVE FALSE
#define G_SOURCE_CONTINUE TRUE

// Forward declarations
typedef struct _GstElement GstElement;
typedef struct _GstBus GstBus;
typedef struct _GstMessage GstMessage;
typedef struct _GstCaps GstCaps;
typedef struct _GstBuffer GstBuffer;
typedef struct _GstSample GstSample;
typedef struct _GstEvent GstEvent;
typedef struct _GMainLoop GMainLoop;
typedef struct _GMainContext GMainContext;
typedef struct _GObject GObject;

// GError structure
typedef struct _GError {
  guint32 domain;
  gint code;
  gchar *message;
} GError;

// Enums
typedef enum {
   GST_STATE_VOID_PENDING = 0,
   GST_STATE_NULL = 1,
   GST_STATE_READY = 2,
   GST_STATE_PAUSED = 3,
   GST_STATE_PLAYING = 4
} GstState;

typedef enum {
   GST_STATE_CHANGE_FAILURE = 0,
   GST_STATE_CHANGE_SUCCESS = 1,
   GST_STATE_CHANGE_ASYNC = 2,
   GST_STATE_CHANGE_NO_PREROLL = 3
} GstStateChangeReturn;

typedef enum {
   GST_MESSAGE_UNKNOWN = 0,
   GST_MESSAGE_EOS = 1,
   GST_MESSAGE_ERROR = 2,
   GST_MESSAGE_WARNING = 3,
   GST_MESSAGE_INFO = 4,
   GST_MESSAGE_STATE_CHANGED = 5,
   GST_MESSAGE_BUFFERING = 6,
   GST_MESSAGE_ANY = -1
} GstMessageType;

typedef enum {
   GST_SEEK_FLAG_NONE = 0,
   GST_SEEK_FLAG_FLUSH = 1,
   GST_SEEK_FLAG_ACCURATE = 2,
   GST_SEEK_FLAG_KEY_UNIT = 4,
   GST_SEEK_FLAG_SEGMENT = 8
} GstSeekFlags;

typedef enum {
   GST_SEEK_TYPE_NONE = 0,
   GST_SEEK_TYPE_SET = 1,
   GST_SEEK_TYPE_CUR = 2,
   GST_SEEK_TYPE_END = 3
} GstSeekType;

typedef enum {
   GST_FORMAT_UNDEFINED = 0,
   GST_FORMAT_DEFAULT = 1,
   GST_FORMAT_BYTES = 2,
   GST_FORMAT_TIME = 3,
   GST_FORMAT_BUFFERS = 4,
   GST_FORMAT_PERCENT = 5
} GstFormat;

typedef int64_t GstClockTime;

// GCallback: Use void* return to allow any function pointer type in C++
#ifdef __cplusplus
// In C++, use reinterpret_cast in the macro instead of changing typedef
typedef void (*GCallback)();
#else
typedef void (*GCallback)();
#endif

#define GST_SECOND ((GstClockTime)1000000000)
#define GST_MSECOND ((GstClockTime)1000000)
#define GST_CLOCK_TIME_NONE ((GstClockTime)-1)

// Wayland output mode flag
#define WL_OUTPUT_MODE_CURRENT 0x1

// Cast macros
#define GST_ELEMENT(obj) ((GstElement*)(obj))
#define GST_BIN(obj) ((GstElement*)(obj))
#define GST_PIPELINE(obj) ((GstElement*)(obj))
#define GST_OBJECT(obj) ((void*)(obj))
#define G_OBJECT(obj) ((GObject*)(obj))

// G_CALLBACK: Use reinterpret_cast in C++ to handle function pointer conversion
#ifdef __cplusplus
#define G_CALLBACK(f) (reinterpret_cast<GCallback>(f))
#else
#define G_CALLBACK(f) ((GCallback)(f))
#endif

#define GST_MESSAGE_TYPE(msg) (((GstMessage*)(msg))->type)
#define GST_TIME_FORMAT "lu"
#define GST_TIME_ARGS(t) ((unsigned long)(t))

// GstMessage structure
struct _GstMessage {
  GstMessageType type;
  void *src;
  guint32 seqnum;
};

// Stub function declarations
void gst_init(int *argc, char **argv[]);
GstElement* gst_element_factory_make(const char *factoryname, const char *name);
GstElement* gst_pipeline_new(const char *name);
GstBus* gst_element_get_bus(GstElement *element);
GstBus* gst_pipeline_get_bus(GstElement *pipeline);
GstStateChangeReturn gst_element_set_state(GstElement *element, GstState state);
GstStateChangeReturn gst_element_get_state(GstElement *element, GstState *state, GstState *pending, GstClockTime timeout);
gboolean gst_element_seek(GstElement *element, double rate, GstFormat format, GstSeekFlags flags,
                      GstSeekType start_type, gint64 start, GstSeekType end_type, gint64 end);
gboolean gst_element_query_position(GstElement *element, GstFormat format, gint64 *cur);
gboolean gst_element_query_duration(GstElement *element, GstFormat format, gint64 *duration);
gboolean gst_element_send_event(GstElement *element, GstEvent *event);
void gst_object_unref(void *object);
void gst_object_ref(void *object);
GstMessage* gst_bus_timed_pop_filtered(GstBus *bus, GstClockTime timeout, GstMessageType types);
guint gst_bus_add_watch(GstBus *bus, GCallback func, gpointer user_data);
void gst_message_unref(GstMessage *message);
GstMessageType gst_message_type(GstMessage *message);
void gst_message_parse_error(GstMessage *message, GError **gerror, gchar **debug);
void gst_message_parse_state_changed(GstMessage *message, GstState *oldstate, GstState *newstate, GstState *pending);
gboolean gst_bin_add(GstElement *bin, GstElement *element);
gboolean gst_element_link(GstElement *src, GstElement *dest);
GstEvent* gst_event_new_step(GstFormat format, guint64 amount, gdouble rate, gboolean flush, gboolean intermediate);

// GObject functions
void g_object_set(gpointer object, const char *first_property_name, ...);
void g_object_get(gpointer object, const char *first_property_name, ...);
gulong g_signal_connect(gpointer instance, const char *detailed_signal, GCallback c_handler, gpointer data);

// GLib stubs
GMainLoop* g_main_loop_new(GMainContext *context, gboolean is_running);
void g_main_loop_run(GMainLoop *loop);
void g_main_loop_quit(GMainLoop *loop);
void g_main_loop_unref(GMainLoop *loop);
gboolean g_main_context_iteration(GMainContext *context, gboolean may_block);
guint g_timeout_add(guint interval, GCallback function, gpointer data);
gboolean g_source_remove(guint tag);
void g_print(const gchar *format, ...);
void g_free(gpointer mem);
void g_error_free(GError *error);

#ifdef __cplusplus
}
#endif

#endif /* __GST_GST_H__ */
