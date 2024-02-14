# Direct Rendering Example

This is an example showing how to use a Direct Rendering feature of DALi.

Direct Rendering (DR) allows injecting native drawing calls using GL API into
DALi rendering pipeline so developers can execute custom code. Demo shows a bunch
of cubes rendered by the custom GL code.

Environmental variables that can be used with the demo:

EGL_ENABLED (default: 0) - old rendering method using EGL native image as an offscreen

UNSAFE_MODE (default: 1) - direct rendering 'unsafe' mode which executes calls on the
same GL context as DALi rendering (hence is unsafe if not properly cleaned up). If set to 0,
demo will create isolated GL context for custom rendering.

DR_THREAD_ENABLED (default: 0) - threaded rendering. Runs custom rendering code on a separate
thread. This method implicitly creates an offscreen buffer to render into.

MAX_CUBES - allows change number of cubes to render