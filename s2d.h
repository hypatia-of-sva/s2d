#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define FALSE 0
#endif
#ifndef PI
    #define PI    3.14159265358979323846264338327950288   /**< pi */
#endif

#define S2D_VERSION 1

/* generic replacement for stdint.h, assuming 64 bit architecture */
#ifndef HAVE_LIBC
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long long uint64_t, size_t, uintptr_t;
typedef long long int64_t, ptrdiff_t;
#endif

typedef enum s2d_key_t {
    S2D_KEY_0                 =   0,
    S2D_KEY_1                 =   1,
    S2D_KEY_2                 =   2,
    S2D_KEY_3                 =   3,
    S2D_KEY_4                 =   4,
    S2D_KEY_5                 =   5,
    S2D_KEY_6                 =   6,
    S2D_KEY_7                 =   7,
    S2D_KEY_8                 =   8,
    S2D_KEY_9                 =   9,
    
    S2D_KEY_A                 =  10,
    S2D_KEY_B                 =  11,
    S2D_KEY_C                 =  12,
    S2D_KEY_D                 =  13,
    S2D_KEY_E                 =  14,
    S2D_KEY_F                 =  15,
    S2D_KEY_G                 =  16,
    S2D_KEY_H                 =  17,
    S2D_KEY_I                 =  18,
    S2D_KEY_J                 =  19,
    S2D_KEY_K                 =  20,
    S2D_KEY_L                 =  21,
    S2D_KEY_M                 =  22,
    S2D_KEY_N                 =  23,
    S2D_KEY_O                 =  24,
    S2D_KEY_P                 =  25,
    S2D_KEY_Q                 =  26,
    S2D_KEY_R                 =  27,
    S2D_KEY_S                 =  28,
    S2D_KEY_T                 =  29,
    S2D_KEY_U                 =  30,
    S2D_KEY_V                 =  31,
    S2D_KEY_W                 =  32,
    S2D_KEY_X                 =  33,
    S2D_KEY_Y                 =  34,
    S2D_KEY_Z                 =  35,
    
    S2D_KEY_ENTER             =  36,
    S2D_KEY_ESCAPE            =  37,   
    S2D_KEY_BACKSPACE         =  38,
    S2D_KEY_TAB               =  39,
    S2D_KEY_SPACE             =  40,

    S2D_KEY_MINUS             =  41,
    S2D_KEY_EQUAL             =  42,    
    S2D_KEY_LEFT_BRACKET      =  43,
    S2D_KEY_RIGHT_BRACKET     =  44,
    S2D_KEY_BACKSLASH         =  45,
    S2D_KEY_SEMICOLON         =  46,
    S2D_KEY_APOSTROPHE        =  47,
    S2D_KEY_GRAVE_ACCENT      =  48,
    S2D_KEY_COMMA             =  49,
    S2D_KEY_PERIOD            =  50,
    S2D_KEY_SLASH             =  51,

    S2D_KEY_RIGHT             =  52,
    S2D_KEY_LEFT              =  53,
    S2D_KEY_DOWN              =  54,
    S2D_KEY_UP                =  55,

    S2D_KEY_INSERT            =  56,
    S2D_KEY_DELETE            =  57,
    S2D_KEY_HOME              =  58,
    S2D_KEY_END               =  59,
    S2D_KEY_PAGE_UP           =  60,
    S2D_KEY_PAGE_DOWN         =  61,
    S2D_KEY_PRINT_SCREEN      =  62,
    S2D_KEY_PAUSE             =  63,
    S2D_KEY_MENU              =  64,

    S2D_KEY_LEFT_SHIFT        =  65,
    S2D_KEY_LEFT_CONTROL      =  66,
    S2D_KEY_LEFT_ALT          =  67,
    S2D_KEY_LEFT_SUPER        =  68,
    S2D_KEY_RIGHT_SHIFT       =  69,
    S2D_KEY_RIGHT_CONTROL     =  70,
    S2D_KEY_RIGHT_ALT         =  71,
    S2D_KEY_RIGHT_SUPER       =  72,   
    
    S2D_KEY_CAPS_LOCK         =  73,
    S2D_KEY_SCROLL_LOCK       =  74,
    S2D_KEY_NUM_LOCK          =  75,
    
    S2D_KEY_KP_0              =  76,
    S2D_KEY_KP_1              =  77,
    S2D_KEY_KP_2              =  78,
    S2D_KEY_KP_3              =  79,
    S2D_KEY_KP_4              =  80,
    S2D_KEY_KP_5              =  81,
    S2D_KEY_KP_6              =  82,
    S2D_KEY_KP_7              =  83,
    S2D_KEY_KP_8              =  84,
    S2D_KEY_KP_9              =  85,
    S2D_KEY_KP_DECIMAL        =  86,
    S2D_KEY_KP_DIVIDE         =  87,
    S2D_KEY_KP_MULTIPLY       =  88,
    S2D_KEY_KP_SUBTRACT       =  89,
    S2D_KEY_KP_ADD            =  90,
    S2D_KEY_KP_ENTER          =  91,
    S2D_KEY_KP_EQUAL          =  92,
    
    S2D_KEY_F1                =  93,
    S2D_KEY_F2                =  94,
    S2D_KEY_F3                =  95,
    S2D_KEY_F4                =  96,
    S2D_KEY_F5                =  97,
    S2D_KEY_F6                =  98,
    S2D_KEY_F7                =  99,
    S2D_KEY_F8                = 100,
    S2D_KEY_F9                = 101,
    S2D_KEY_F10               = 102,
    S2D_KEY_F11               = 103,
    S2D_KEY_F12               = 104,
    S2D_KEY_F13               = 105,
    S2D_KEY_F14               = 106,
    S2D_KEY_F15               = 107,
    S2D_KEY_F16               = 108,
    S2D_KEY_F17               = 109,
    S2D_KEY_F18               = 110,
    S2D_KEY_F19               = 111,
    S2D_KEY_F20               = 112,
    S2D_KEY_F21               = 113,
    S2D_KEY_F22               = 114,
    S2D_KEY_F23               = 115,
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
typedef struct s2d_datetime_info_t {
    struct {
        int32_t sec, min, hour, mday, mon, year, wday, yday, isdst;
    } local, UTC;
    int64_t unix_time_sec, tv_sec, tv_nsec;
} s2d_datetime_info_t;

typedef void* s2d_sound_t;
typedef void* s2d_music_t;
typedef void* s2d_texture_t;


void s2d_init(int width, int height, int fullscreen, int hide_cursor, char* title, int nr_parallel_sound_effect);
void s2d_quit(void);

/* returns 1 while events are pending, 0 otherwise */
int s2d_event_poll(s2d_event_t* e);

s2d_sound_t s2d_sound_load(char* path);
void s2d_sound_free(s2d_sound_t sound);
void s2d_sound_play(s2d_sound_t sound, int channel);
void s2d_sound_stop(int channel);
void s2d_sound_volume(s2d_sound_t sound, float volume_percentage);
void s2d_sound_channel_volume(int channel, float volume_percentage);

s2d_music_t s2d_music_load(char* path);
void s2d_music_free(s2d_music_t music);
void s2d_music_play(s2d_music_t music, int loop);
void s2d_music_stop(void);
void s2d_music_volume(float volume_percentage);

s2d_texture_t s2d_texture_load(char* path);
void s2d_texture_free(s2d_texture_t tex);
void s2d_texture_dimensions(s2d_texture_t tex, int* out_width, int* out_height);
void s2d_texture_blit_rects(s2d_texture_t tex, int* src_dst_rect_pairs, int nr);
void s2d_texture_blit_mesh(s2d_texture_t tex, int* grid_nrs, int dst_width, int dst_height, int tile_width, int tile_height, int src_tiles_per_row, int origin_x, int origin_y, float scale);

void s2d_clear(void);
void s2d_render(void);

void s2d_wait_ms(uint32_t ms);
int64_t s2d_time_ms_since_init(void);
s2d_datetime_info_t* s2d_current_datetime(void);         /* in libc, not SLD */

/* returns != 0 if file doesn't exist / already exists */
int s2d_file_read(char* path, void** out_data, size_t* out_len);
int s2d_file_write(char* path, void* data, size_t len);
int s2d_file_rename(char* old_path, char* new_path);    /* in libc, not SDL */
int s2d_file_remove(char* path);                        /* in libc, not SDL */

void s2d_log_line(char* line_fmt, ...);

void* s2d_mem_alloc(size_t size, int zero);
void* s2d_mem_realloc(void* old_ptr, size_t new_size);
void s2d_mem_free(void* ptr);
void s2d_mem_zero(void* ptr, size_t size);
void s2d_mem_copy(void* dst, const void* src, size_t size);
int s2d_mem_cmp(const void* a, const void* b, size_t size);

char* s2d_str_new(const char* str_const);
char* s2d_str_concat(char* str, const char* str_const);
size_t s2d_str_len(const char* str);
int s2d_str_cmp(const char* a, const char* b);
char* s2d_format(const char* fmt, ...); /* caller has ownership, to be freed with s2d_mem_free */
void s2d_parse(const char* str, char* fmt, ...);

#define s2d_assert(cond, ...) s2d_assert_func(cond, #cond, __func__, __FILE__, __LINE__, __VA_ARGS__)
void s2d_assert_func(int cond, const char* cond_str, const char* func, const char* file, int line, const char* msg_fmt, ...);
void s2d_exit(int code);                                        /* in libc, not SDL */

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





