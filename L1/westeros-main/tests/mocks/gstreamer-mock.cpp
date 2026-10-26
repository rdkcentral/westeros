/*
 * GStreamer Stub Implementation for L1 Testing
 * This provides minimal stub implementations to allow compilation without GStreamer
 */
#include "gst/gst.h"
#include <stdio.h>
#include <stdarg.h>

// Stub implementations that do nothing or return safe defaults
void gst_init(int *argc, char **argv[]) {
    (void)argc;
    (void)argv;
}

GstElement* gst_element_factory_make(const char *factoryname, const char *name) {
    (void)factoryname;
    (void)name;
    return nullptr;
}

GstElement* gst_pipeline_new(const char *name) {
    (void)name;
    return nullptr;
}

GstBus* gst_element_get_bus(GstElement *element) {
    (void)element;
    return nullptr;
}

GstBus* gst_pipeline_get_bus(GstElement *pipeline) {
    (void)pipeline;
    return nullptr;
}

GstStateChangeReturn gst_element_set_state(GstElement *element, GstState state) {
    (void)element;
    (void)state;
    return GST_STATE_CHANGE_SUCCESS;
}

GstStateChangeReturn gst_element_get_state(GstElement *element, GstState *state, GstState *pending, GstClockTime timeout) {
    (void)element;
    (void)timeout;
    if (state) *state = GST_STATE_NULL;
    if (pending) *pending = GST_STATE_VOID_PENDING;
    return GST_STATE_CHANGE_SUCCESS;
}

gboolean gst_element_seek(GstElement *element, double rate, GstFormat format, GstSeekFlags flags,
                      GstSeekType start_type, gint64 start, GstSeekType end_type, gint64 end) {
    (void)element;
    (void)rate;
    (void)format;
    (void)flags;
    (void)start_type;
    (void)start;
    (void)end_type;
    (void)end;
    return FALSE;
}

gboolean gst_element_query_position(GstElement *element, GstFormat format, gint64 *cur) {
    (void)element;
    (void)format;
    if (cur) *cur = 0;
    return FALSE;
}

gboolean gst_element_query_duration(GstElement *element, GstFormat format, gint64 *duration) {
    (void)element;
    (void)format;
    if (duration) *duration = 0;
    return FALSE;
}

gboolean gst_element_send_event(GstElement *element, GstEvent *event) {
    (void)element;
    (void)event;
    return FALSE;
}

void gst_object_unref(void *object) {
    (void)object;
}

void gst_object_ref(void *object) {
    (void)object;
}

GstMessage* gst_bus_timed_pop_filtered(GstBus *bus, GstClockTime timeout, GstMessageType types) {
    (void)bus;
    (void)timeout;
    (void)types;
    return nullptr;
}

guint gst_bus_add_watch(GstBus *bus, GCallback func, gpointer user_data) {
    (void)bus;
    (void)func;
    (void)user_data;
    return 0;
}

void gst_message_unref(GstMessage *message) {
    (void)message;
}

GstMessageType gst_message_type(GstMessage *message) {
    (void)message;
    return GST_MESSAGE_UNKNOWN;
}

void gst_message_parse_error(GstMessage *message, GError **gerror, gchar **debug) {
    (void)message;
    if (gerror) *gerror = nullptr;
    if (debug) *debug = nullptr;
}

void gst_message_parse_state_changed(GstMessage *message, GstState *oldstate, GstState *newstate, GstState *pending) {
    (void)message;
    if (oldstate) *oldstate = GST_STATE_NULL;
    if (newstate) *newstate = GST_STATE_NULL;
    if (pending) *pending = GST_STATE_VOID_PENDING;
}

gboolean gst_bin_add(GstElement *bin, GstElement *element) {
    (void)bin;
    (void)element;
    return FALSE;
}

gboolean gst_element_link(GstElement *src, GstElement *dest) {
    (void)src;
    (void)dest;
    return FALSE;
}

GstEvent* gst_event_new_step(GstFormat format, guint64 amount, gdouble rate, gboolean flush, gboolean intermediate) {
    (void)format;
    (void)amount;
    (void)rate;
    (void)flush;
    (void)intermediate;
    return nullptr;
}

// GObject functions
void g_object_set(gpointer object, const char *first_property_name, ...) {
    (void)object;
    (void)first_property_name;
    // Variadic function - just ignore all arguments
}

void g_object_get(gpointer object, const char *first_property_name, ...) {
    (void)object;
    (void)first_property_name;
    // Variadic function - just ignore all arguments
}

gulong g_signal_connect(gpointer instance, const char *detailed_signal, GCallback c_handler, gpointer data) {
    (void)instance;
    (void)detailed_signal;
    (void)c_handler;
    (void)data;
    return 0;
}

// GLib stubs
GMainLoop* g_main_loop_new(GMainContext *context, gboolean is_running) {
    (void)context;
    (void)is_running;
    return nullptr;
}

void g_main_loop_run(GMainLoop *loop) {
    (void)loop;
}

void g_main_loop_quit(GMainLoop *loop) {
    (void)loop;
}

void g_main_loop_unref(GMainLoop *loop) {
    (void)loop;
}

gboolean g_main_context_iteration(GMainContext *context, gboolean may_block) {
    (void)context;
    (void)may_block;
    return FALSE;
}

guint g_timeout_add(guint interval, GCallback function, gpointer data) {
    (void)interval;
    (void)function;
    (void)data;
    return 1; // Return a non-zero timer ID
}

gboolean g_source_remove(guint tag) {
    (void)tag;
    return TRUE;
}

void g_print(const gchar *format, ...) {
    (void)format;
    // Variadic function - just ignore all arguments
}

void g_free(gpointer mem) {
    (void)mem;
}

void g_error_free(GError *error) {
    (void)error;
}
