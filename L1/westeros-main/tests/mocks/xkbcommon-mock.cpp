/*
 * xkbcommon Mock Implementation
 * Copyright 2024 RDK Management
 */
#include <xkbcommon/xkbcommon.h>
#include <cstdlib>
#include <cstring>

// Mock structures
struct xkb_context {
    int dummy;
};

struct xkb_keymap {
    int dummy;
};

struct xkb_state {
    int dummy;
};

// Context functions
xkb_context* xkb_context_new(enum xkb_context_flags flags) {
    (void)flags;
    return new xkb_context();
}

void xkb_context_unref(xkb_context *context) {
    if (context) delete context;
}

xkb_context* xkb_context_ref(xkb_context *context) {
    return context;
}

// Keymap functions
xkb_keymap* xkb_keymap_new_from_names(xkb_context *context,
                                       const struct xkb_rule_names *names,
                                       enum xkb_keymap_compile_flags flags) {
    (void)context;
    (void)names;
    (void)flags;
    return new xkb_keymap();
}

void xkb_keymap_unref(xkb_keymap *keymap) {
    if (keymap) delete keymap;
}

xkb_keymap* xkb_keymap_ref(xkb_keymap *keymap) {
    return keymap;
}

char* xkb_keymap_get_as_string(xkb_keymap *keymap, enum xkb_keymap_format format) {
    (void)keymap;
    (void)format;
    const char *mock_keymap = "xkb_keymap { xkb_keycodes { }; xkb_symbols { }; };";
    return strdup(mock_keymap);
}

xkb_mod_index_t xkb_keymap_mod_get_index(xkb_keymap *keymap, const char *name) {
    (void)keymap;
    (void)name;
    return 0; // Mock index
}

// State functions
xkb_state* xkb_state_new(xkb_keymap *keymap) {
    (void)keymap;
    return new xkb_state();
}

void xkb_state_unref(xkb_state *state) {
    if (state) delete state;
}

xkb_state* xkb_state_ref(xkb_state *state) {
    return state;
}

enum xkb_state_component xkb_state_update_mask(xkb_state *state,
                                                xkb_mod_mask_t depressed_mods,
                                                xkb_mod_mask_t latched_mods,
                                                xkb_mod_mask_t locked_mods,
                                                xkb_layout_index_t depressed_layout,
                                                xkb_layout_index_t latched_layout,
                                                xkb_layout_index_t locked_layout) {
    (void)state;
    (void)depressed_mods;
    (void)latched_mods;
    (void)locked_mods;
    (void)depressed_layout;
    (void)latched_layout;
    (void)locked_layout;
    return (enum xkb_state_component)0;
}

xkb_mod_mask_t xkb_state_serialize_mods(xkb_state *state, enum xkb_state_component components) {
    (void)state;
    (void)components;
    return 0;
}

xkb_layout_index_t xkb_state_serialize_layout(xkb_state *state, enum xkb_state_component components) {
    (void)state;
    (void)components;
    return 0;
}
