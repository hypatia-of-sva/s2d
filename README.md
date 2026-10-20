# S2D - Simple 2D Games library

This library is a simple windowing/input, graphics, sound and utility library for 2D games. It is currently implemented in SDL2, including SDL2_mixer and SDL2_image.

## windowing and input

The library creates a single window of fixed dimensions either in windowed or full-screen mode, optionally the default mouse cursor can be disabled. For input, Key press/release and Mouse click/release/move/scroll events are reported through the `s2d_event_poll` function, as well as the press on the window close button and the internal notice that the music has stopped playing.

```c
void s2d_init(int width, int height, bool fullscreen, bool hide_cursor, char* title,
    int nr_parallel_sound_effect);
void s2d_quit(void);

typedef enum s2d_key_t {
    S2D_KEY_0                 =   0,
[...]
    S2D_KEY_F24               = 116,    
    S2D_KEY_LAST              = S2D_KEY_F24,
    S2D_KEY_UNKNOWN           = 255,
    S2D_KEY_MAX_ENUM          = 0xFF
} s2d_key_t;
typedef enum s2d_mouse_button_t {
    S2D_MOUSE_BUTTON_LEFT       = 0,
    S2D_MOUSE_BUTTON_RIGHT      = 1,
    S2D_MOUSE_BUTTON_MIDDLE     = 2,
    S2D_MOUSE_BUTTON_X1         = 3,
    S2D_MOUSE_BUTTON_X2         = 4,
    S2D_MOUSE_BUTTON_LAST       = S2D_MOUSE_BUTTON_X2,
    S2D_MOUSE_BUTTON_UNKNOWN    = 255,
    S2D_MOUSE_BUTTON_MAX_ENUM   = 0xFF
} s2d_mouse_button_t;
typedef enum s2d_event_type_t {
    S2D_EVENT_TYPE_QUIT             = 0,
    S2D_EVENT_TYPE_KEY_PRESS        = 1,
    S2D_EVENT_TYPE_KEY_RELEASE      = 2,
    S2D_EVENT_TYPE_KEY_REPEAT       = 3,
    S2D_EVENT_TYPE_MOUSE_CLICK      = 4,
    S2D_EVENT_TYPE_MOUSE_RELEASE    = 5,   
    S2D_EVENT_TYPE_MOUSE_MOVE       = 6,
    S2D_EVENT_TYPE_MOUSE_SCROLL     = 7,
    S2D_EVENT_TYPE_MUSIC_FINISHED   = 8,
    S2D_EVENT_TYPE_UNKNOWN          = 0xFF,
    S2D_EVENT_TYPE_MAX_ENUM         = 0xFF
} s2d_event_type_t;
typedef struct s2d_event_t {
    s2d_event_type_t type;
    union {
        s2d_key_t key;
        s2d_mouse_button_t button;
        struct { int x, y; } pos;
    } value;
} s2d_event_t;
int s2d_event_poll(s2d_event_t* e);  /* returns 1 while events are pending, 0 otherwise */
```

## graphics

The library gives facilities to load textures and blit them on screen, by using it either as a free-form texture map to select UV rectangles out of to map onto XY rectangles - as 8 ints in order u,v,delta_u,delta_v,x,y,delta_x,delta_y - or to use it as a tile-map to create a mesh from with each tile of the mesh being equivalent to an index of the tile map. Before these functions, the screen needs to get cleared, and after it, the render needs to be completed with the `s2d_render` function.

```c
typedef void* s2d_texture_t;
s2d_texture_t s2d_texture_load(char* path);
void s2d_texture_free(s2d_texture_t tex);
void s2d_texture_dimensions(s2d_texture_t tex, int* out_width, int* out_height);
void s2d_texture_blit_rects(s2d_texture_t tex, int* src_dst_rect_pairs, int nr);
void s2d_texture_blit_mesh(s2d_texture_t tex, int* grid_nrs, int src_tile_width,
    int src_tile_height, int src_tiles_per_row, int dst_origin_x, int dst_origin_y,
    int dst_width, int dst_height, int dst_tile_width, int dst_tile_height)
void s2d_clear(void);
void s2d_render(void);
```

## sound

There are two kinds of sound that can be used analogously: a) sound effects, that need to be placed on a channel. The maximum number of such sound effects is set at initialization; b) music, which only has one channel, but is otherwise very similar. It has the extra feature of having a loop option and giving an event notice when it's done playing, so another track can be lined up. (This is heavily based on SDL2_mixer.)

```c
typedef void* s2d_sound_t;
s2d_sound_t s2d_sound_load(char* path);
void s2d_sound_free(s2d_sound_t sound);
void s2d_sound_play(s2d_sound_t sound, int channel);
void s2d_sound_stop(int channel);
void s2d_sound_volume(s2d_sound_t sound, float volume_percentage);
void s2d_sound_channel_volume(int channel, float volume_percentage);

typedef void* s2d_music_t;
s2d_music_t s2d_music_load(char* path);
void s2d_music_free(s2d_music_t music);
void s2d_music_play(s2d_music_t music, bool loop);
void s2d_music_stop(void);
void s2d_music_volume(float volume_percentage);
```

## utilities

There are a number of utility function in S2D, that can be used to replace libc (and thus are useful when you want to only link to SDL, not libc, on windows; tho currently, this is not possible due to some functions still being implemented in libc, as indicated with comments). They can be divided into the following groups and are mostly self-explanatory:

### basic types and macros

```c
#define true 1
#define false 0
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t, bool;
typedef unsigned long long uint64_t, size_t, uintptr_t;
typedef long long int64_t, ptrdiff_t;
```

### time/date functions:

```c
void s2d_wait_ms(uint32_t ms);
int64_t s2d_time_ms_since_init(void);

typedef struct s2d_datetime_info_t {
    struct {
        int32_t sec, min, hour, mday, mon, year, wday, yday, isdst;
    } local, UTC;
    int64_t unix_time_sec, tv_sec, tv_nsec;
} s2d_datetime_info_t;
s2d_datetime_info_t* s2d_current_datetime(void);         /* in libc, not SLD */
```

### i/o functions:

```c
void s2d_log_line(char* line_fmt, ...);
/* returns != 0 if file doesn't exist / already exists */
int s2d_file_read(char* path, void** out_data, size_t* out_len);
int s2d_file_write(char* path, void* data, size_t len);
int s2d_file_rename(char* old_path, char* new_path);    /* in libc, not SDL */
int s2d_file_remove(char* path);                        /* in libc, not SDL */
```

### allocation functions:

```c
void* s2d_mem_alloc(size_t size, bool zero);
void* s2d_mem_realloc(void* old_ptr, size_t new_size);
void s2d_mem_free(void* ptr);
void s2d_mem_zero(void* ptr, size_t size);
void s2d_mem_copy(void* dst, const void* src, size_t size);
int s2d_mem_cmp(const void* a, const void* b, size_t size);
```

### string functions:

```c
char* s2d_str_new(const char* str_const);
char* s2d_str_concat(char* str, const char* str_const);
size_t s2d_str_len(const char* str);
int s2d_str_cmp(const char* a, const char* b);
char* s2d_format(const char* fmt, ...); /* caller has ownership, to be freed with s2d_mem_free */
void s2d_parse(const char* str, char* fmt, ...);
```

### assert and exit:

```c
#define s2d_assert(cond, msg, ...) s2d_assert_func(cond, #cond, __func__, __FILE__, __LINE__, \
    msg __VA_OPT__(,) __VA_ARGS__)
void s2d_assert_func(bool cond, const char* cond_str, const char* func, const char* file, int line,
    const char* msg_fmt, ...);
void s2d_exit(int code);                                        /* in libc, not SDL */
```

### math library:

```c
#define PI 3.14159265358979323846264338327950288
double s2d_acos(double x);
double s2d_asin(double x);
double s2d_atan(double x);
double s2d_cos(double x);
double s2d_sin(double x);
double s2d_atan2(double y, double x);
double s2d_tan(double x);
double s2d_exp(double x);
double s2d_log(double x);
double s2d_loglO(double x);
double s2d_pow(double x, double y);
double s2d_sqrt(double x);
double s2d_ceil(double x);
double s2d_fabs(double x);
double s2d_floor(double x);
double s2d_fmod(double x, double y);
int32_t s2d_abs(int32_t j);
int32_t s2d_imax(int32_t i, int32_t j);
int32_t s2d_imin(int32_t i, int32_t j);
double s2d_fmax(double x, double y);
double s2d_fmin(double x, double y);
/* better than libc, but not cryptographically safe. Seeded with true entropy at init.
   written in libc, not SDL, based on public domain xoshiro256++ and splitmix64 algorithms
   (See https://prng.di.unimi.it/, https://dl.acm.org/doi/10.1145/3460772) */
uint64_t s2d_rand(void);                        /* in own code, not SDL */
```
