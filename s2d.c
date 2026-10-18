#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef HAVE_LIBC
#define HAVE_LIBC
#endif
#include "s2d.h"

static struct {
    int is_init;
    int nr_parallel_sound_effects;
    int music_finished;
    uint64_t prng_state[4];
    s2d_datetime_info_t datetime;
    SDL_Renderer *renderer;
    SDL_Window *window;
} _s2d_data = {0};



/* true entropy generation, like /dev/random, for the seeding of the PRNG */

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
int32_t RtlGenRandom(void* RandomBuffer, uint32_t RandomBufferLength);
#else
int getentropy(void* buffer, size_t length);
#endif

static uint64_t _s2d_entropy_uint64(void) {
    uint64_t val;
    int success;
    
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
    success = RtlGenRandom(&val, sizeof(uint64_t));
#else /* linux / glibc */
    success = (getentropy(&val, sizeof(uint64_t)) != -1);
#endif

    if(success) {
        return val;
    } else {
        return 0;
    }
}


/* PRNG library for fast generation:
    Based on the public domain xoshiro256++ and splitmix64 algorithms 
    written in 2019/2015 by David Blackman and Sebastiano Vigna (vigna@acm.org)
    Original Code from: https://prng.di.unimi.it/
    https://prng.di.unimi.it/xoshiro256plusplus.c
    https://prng.di.unimi.it/splitmix64.c
    Paper: https://dl.acm.org/doi/10.1145/3460772
    Preprint version: https://vigna.di.unimi.it/ftp/papers/ScrambledLinear.pdf
*/
static void _s2d_prng_seed(uint64_t seed) {
    /* 4 rounds of splitmix64 with seed as input state */
    for(int i = 0; i < 4; i++) {
        uint64_t z = (seed += 0x9e3779b97f4a7c15);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
        z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
        _s2d_data.prng_state[i] = z ^ (z >> 31);
    }
}
static uint64_t _s2d_prng_rotl(uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}
uint64_t s2d_rand(void) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    /* 1 round of xoshiro256++ */

    const uint64_t result = _s2d_prng_rotl(_s2d_data.prng_state[0] + _s2d_data.prng_state[3], 23) + _s2d_data.prng_state[0];

    const uint64_t t = _s2d_data.prng_state[1] << 17;

    _s2d_data.prng_state[2] ^= _s2d_data.prng_state[0];
    _s2d_data.prng_state[3] ^= _s2d_data.prng_state[1];
    _s2d_data.prng_state[1] ^= _s2d_data.prng_state[2];
    _s2d_data.prng_state[0] ^= _s2d_data.prng_state[3];

    _s2d_data.prng_state[2] ^= t;

    _s2d_data.prng_state[3] = _s2d_prng_rotl(_s2d_data.prng_state[3], 45);

    return result;
}

void s2d_exit(int code) {
    exit(code);
}


int s2d_file_rename(char* old_path, char* new_path) {
    return rename(old_path, new_path);
}
int s2d_file_remove(char* path) {
    return remove(path);
}


s2d_datetime_info_t* s2d_current_datetime(void) {
    struct tm* tm_struct; 
    struct timespec ts_struct;
    time_t timer;
    int code;
    
    timer = time(NULL);
    s2d_assert(timer != (time_t)-1, "Couldn't read out timer value!");
    _s2d_data.datetime.unix_time_sec = (int64_t) timer;

    tm_struct = localtime(&timer);
    s2d_assert(tm_struct != NULL, "Couldn't read out local time!");
    _s2d_data.datetime.local.sec   = tm_struct[0].tm_sec;
    _s2d_data.datetime.local.min   = tm_struct[0].tm_min;
    _s2d_data.datetime.local.hour  = tm_struct[0].tm_hour;
    _s2d_data.datetime.local.mday  = tm_struct[0].tm_mday;
    _s2d_data.datetime.local.mon   = tm_struct[0].tm_mon;
    _s2d_data.datetime.local.year  = tm_struct[0].tm_year;
    _s2d_data.datetime.local.wday  = tm_struct[0].tm_wday;
    _s2d_data.datetime.local.yday  = tm_struct[0].tm_yday;
    _s2d_data.datetime.local.isdst = tm_struct[0].tm_isdst;
    
    tm_struct = gmtime(&timer);
    s2d_assert(tm_struct != NULL, "Couldn't read out UTC time!");
    _s2d_data.datetime.UTC.sec   = tm_struct[0].tm_sec;
    _s2d_data.datetime.UTC.min   = tm_struct[0].tm_min;
    _s2d_data.datetime.UTC.hour  = tm_struct[0].tm_hour;
    _s2d_data.datetime.UTC.mday  = tm_struct[0].tm_mday;
    _s2d_data.datetime.UTC.mon   = tm_struct[0].tm_mon;
    _s2d_data.datetime.UTC.year  = tm_struct[0].tm_year;
    _s2d_data.datetime.UTC.wday  = tm_struct[0].tm_wday;
    _s2d_data.datetime.UTC.yday  = tm_struct[0].tm_yday;
    _s2d_data.datetime.UTC.isdst = tm_struct[0].tm_isdst;
    
    code = timespec_get(&ts_struct, TIME_UTC);
    s2d_assert(code == TIME_UTC, "Couldn't read out timespec struct!");
    _s2d_data.datetime.tv_sec = ts_struct.tv_sec;
    _s2d_data.datetime.tv_nsec = ts_struct.tv_nsec;

    return &_s2d_data.datetime;
}

double s2d_acos(double x)   { return SDL_acos(x); }
double s2d_asin(double x)   { return SDL_asin(x); }
double s2d_atan(double x)   { return SDL_atan(x); }
double s2d_cos(double x)    { return SDL_cos(x); }
double s2d_sin(double x)    { return SDL_sin(x); }
double s2d_atan2(double y, double x)   { return SDL_atan2(y,x); }
double s2d_tan(double x)    { return SDL_tan(x); }
double s2d_exp(double x)    { return SDL_exp(x); }
double s2d_log(double x)    { return SDL_log(x); }
double s2d_loglO(double x)  { return SDL_log10(x); }
double s2d_pow(double x, double y)    { return SDL_pow(x,y); }
double s2d_sqrt(double x)   { return SDL_sqrt(x); }
double s2d_ceil(double x)   { return SDL_ceil(x); }
double s2d_fabs(double x)   { return SDL_fabs(x); }
double s2d_floor(double x)  { return SDL_floor(x); }
double s2d_fmod(double x, double y)    { return SDL_fmod(x,y); }
int32_t s2d_abs(int32_t j)   { return SDL_abs(j); }
int32_t s2d_imax(int32_t i, int32_t j) { return (i < j)? j : i; }
int32_t s2d_imin(int32_t i, int32_t j) { return (i < j)? i : j; }
double s2d_fmax(double x, double y) { return (x < y)? y : x; }
double s2d_fmin(double x, double y) { return (x < y)? x : y; }

void s2d_log_line(char* line_fmt, ...) {
    va_list ap;
    va_start(ap, line_fmt);
    (void) SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, line_fmt, ap);
    va_end(ap);
}
char* s2d_format(const char* fmt, ...) {
    char* ptr = NULL; va_list ap;
    va_start(ap, fmt);
    (void) SDL_vasprintf(&ptr, fmt, ap);
    va_end(ap);
    return ptr;
}
void s2d_parse(const char* str, char* fmt, ...) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    va_list ap;
    va_start(ap, fmt);
    (void) SDL_vsscanf(str, fmt, ap);
    va_end(ap);
}

void s2d_assert_func(int cond, const char* cond_str, const char* func, const char* file, int line, const char* msg_fmt, ...) {
    if(!cond) {
        /* we do an allocation here, and not as a parameter to this functions msg,
           so that we only allocate the msg string if the condition is actually false */
        char* msg = NULL; va_list ap;
        va_start(ap, msg_fmt);
        (void) SDL_vasprintf(&msg, msg_fmt, ap);
        va_end(ap);
        char* warning_text = s2d_format(
            "%s\n\n"
            "Technical Details:\n"
            "Failed condition:\n"
            "        %s\n"
            "Integer value of the condition: %i\n"
            "Place: in function %s, in file %s, line %i\n"
            "Please report this mistake to the developers!\n",
            msg, cond_str, cond, func, file, line);
        s2d_mem_free(msg);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Assertion failed!", warning_text, _s2d_data.window);
        /* we also print it to the terminal for permanence and easier copy-paste */
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, warning_text);
        s2d_mem_free(warning_text);
        s2d_exit(cond);
    }
}


static void _s2d_music_finished_callback(void) {
    _s2d_data.music_finished = 1;
}
void s2d_init(int width, int height, int fullscreen, int hide_cursor, char* title, int nr_parallel_sound_effects) {
    int rendererFlags, windowFlags, code, imgFlags;
    
    s2d_assert(!_s2d_data.is_init, "S2D Library was tried to be initialized twice!");
    s2d_assert(title != NULL, "S2D Library initialized without valid window title!");

    rendererFlags = SDL_RENDERER_ACCELERATED;
    windowFlags = 0;
    imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;

    code = SDL_Init(SDL_INIT_VIDEO);
    s2d_assert(code == 0, "Couldn't initialize SDL: %s\n", SDL_GetError());

    _s2d_data.window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, windowFlags);
    s2d_assert(_s2d_data.window != NULL, "Failed to open %d x %d window: %s\n", width, height, SDL_GetError());
        
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    _s2d_data.renderer = SDL_CreateRenderer(_s2d_data.window, -1, rendererFlags);
    s2d_assert(_s2d_data.renderer != NULL, "Failed to create renderer: %s\n", SDL_GetError());
    
    if(fullscreen) {
        code = SDL_SetWindowFullscreen(_s2d_data.window, SDL_WINDOW_FULLSCREEN);
        s2d_assert(code == 0, "Couldn't set SDL Window to fullscreen: %s\n", SDL_GetError());
    }
    
    code = IMG_Init(imgFlags);
    s2d_assert(code == imgFlags, "Couldn't initialize SDL Image: %s\n", IMG_GetError());
    
    SDL_SetRenderDrawColor(_s2d_data.renderer, 255, 255, 255, 255);
    
    if(hide_cursor) {
        SDL_ShowCursor(0);
    }

    //maybe make these parameters too?
    code = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
    s2d_assert(code == 0, "Couldn't initialize SDL Mixer\n");

    Mix_AllocateChannels(nr_parallel_sound_effects);
    Mix_HookMusicFinished(&_s2d_music_finished_callback);
    
    _s2d_prng_seed(_s2d_entropy_uint64());
    
    _s2d_data.nr_parallel_sound_effects = nr_parallel_sound_effects;
    _s2d_data.is_init = 1;
}

void s2d_quit(void) {
    s2d_assert(_s2d_data.is_init, "S2D Library was tried to quit before initialized!");
    
    SDL_DestroyRenderer(_s2d_data.renderer);
    SDL_DestroyWindow(_s2d_data.window);
    SDL_Quit();
    
    SDL_memset(&_s2d_data, 0, sizeof(_s2d_data));
}

static s2d_key_t _s2d_scancode_to_key(SDL_Scancode code) {
    switch(code) {
    case SDL_SCANCODE_0            :  return S2D_KEY_0            ; break;  
    case SDL_SCANCODE_1            :  return S2D_KEY_1            ; break;  
    case SDL_SCANCODE_2            :  return S2D_KEY_2            ; break;  
    case SDL_SCANCODE_3            :  return S2D_KEY_3            ; break;  
    case SDL_SCANCODE_4            :  return S2D_KEY_4            ; break;  
    case SDL_SCANCODE_5            :  return S2D_KEY_5            ; break;  
    case SDL_SCANCODE_6            :  return S2D_KEY_6            ; break;  
    case SDL_SCANCODE_7            :  return S2D_KEY_7            ; break;  
    case SDL_SCANCODE_8            :  return S2D_KEY_8            ; break;  
    case SDL_SCANCODE_9            :  return S2D_KEY_9            ; break;  
    case SDL_SCANCODE_A            :  return S2D_KEY_A            ; break;  
    case SDL_SCANCODE_B            :  return S2D_KEY_B            ; break;  
    case SDL_SCANCODE_C            :  return S2D_KEY_C            ; break;  
    case SDL_SCANCODE_D            :  return S2D_KEY_D            ; break;  
    case SDL_SCANCODE_E            :  return S2D_KEY_E            ; break;  
    case SDL_SCANCODE_F            :  return S2D_KEY_F            ; break;  
    case SDL_SCANCODE_G            :  return S2D_KEY_G            ; break;  
    case SDL_SCANCODE_H            :  return S2D_KEY_H            ; break;  
    case SDL_SCANCODE_I            :  return S2D_KEY_I            ; break;  
    case SDL_SCANCODE_J            :  return S2D_KEY_J            ; break;  
    case SDL_SCANCODE_K            :  return S2D_KEY_K            ; break;  
    case SDL_SCANCODE_L            :  return S2D_KEY_L            ; break;  
    case SDL_SCANCODE_M            :  return S2D_KEY_M            ; break;  
    case SDL_SCANCODE_N            :  return S2D_KEY_N            ; break;  
    case SDL_SCANCODE_O            :  return S2D_KEY_O            ; break;  
    case SDL_SCANCODE_P            :  return S2D_KEY_P            ; break;  
    case SDL_SCANCODE_Q            :  return S2D_KEY_Q            ; break;  
    case SDL_SCANCODE_R            :  return S2D_KEY_R            ; break;  
    case SDL_SCANCODE_S            :  return S2D_KEY_S            ; break;  
    case SDL_SCANCODE_T            :  return S2D_KEY_T            ; break;  
    case SDL_SCANCODE_U            :  return S2D_KEY_U            ; break;  
    case SDL_SCANCODE_V            :  return S2D_KEY_V            ; break;  
    case SDL_SCANCODE_W            :  return S2D_KEY_W            ; break;  
    case SDL_SCANCODE_X            :  return S2D_KEY_X            ; break;  
    case SDL_SCANCODE_Y            :  return S2D_KEY_Y            ; break;      
    case SDL_SCANCODE_Z            :  return S2D_KEY_Z            ; break;      
    case SDL_SCANCODE_RETURN       :  return S2D_KEY_ENTER        ; break;          
    case SDL_SCANCODE_ESCAPE       :  return S2D_KEY_ESCAPE       ; break;      
    case SDL_SCANCODE_BACKSPACE    :  return S2D_KEY_BACKSPACE    ; break;      
    case SDL_SCANCODE_TAB          :  return S2D_KEY_TAB          ; break;      
    case SDL_SCANCODE_SPACE        :  return S2D_KEY_SPACE        ; break;      
    case SDL_SCANCODE_MINUS        :  return S2D_KEY_MINUS        ; break; 
    case SDL_SCANCODE_EQUALS       :  return S2D_KEY_EQUAL        ; break;     
    case SDL_SCANCODE_LEFTBRACKET  :  return S2D_KEY_LEFT_BRACKET ; break; 
    case SDL_SCANCODE_RIGHTBRACKET :  return S2D_KEY_RIGHT_BRACKET; break; 
    case SDL_SCANCODE_BACKSLASH    :  return S2D_KEY_BACKSLASH    ; break; 
    case SDL_SCANCODE_SEMICOLON    :  return S2D_KEY_SEMICOLON    ; break; 
    case SDL_SCANCODE_APOSTROPHE   :  return S2D_KEY_APOSTROPHE   ; break; 
    case SDL_SCANCODE_GRAVE        :  return S2D_KEY_GRAVE_ACCENT ; break; 
    case SDL_SCANCODE_COMMA        :  return S2D_KEY_COMMA        ; break; 
    case SDL_SCANCODE_PERIOD       :  return S2D_KEY_PERIOD       ; break; 
    case SDL_SCANCODE_SLASH        :  return S2D_KEY_SLASH        ; break; 
    case SDL_SCANCODE_RIGHT        :  return S2D_KEY_RIGHT        ; break; 
    case SDL_SCANCODE_LEFT         :  return S2D_KEY_LEFT         ; break; 
    case SDL_SCANCODE_DOWN         :  return S2D_KEY_DOWN         ; break; 
    case SDL_SCANCODE_UP           :  return S2D_KEY_UP           ; break; 
    case SDL_SCANCODE_INSERT       :  return S2D_KEY_INSERT       ; break; 
    case SDL_SCANCODE_DELETE       :  return S2D_KEY_DELETE       ; break; 
    case SDL_SCANCODE_HOME         :  return S2D_KEY_HOME         ; break; 
    case SDL_SCANCODE_END          :  return S2D_KEY_END          ; break; 
    case SDL_SCANCODE_PAGEUP       :  return S2D_KEY_PAGE_UP      ; break; 
    case SDL_SCANCODE_PAGEDOWN     :  return S2D_KEY_PAGE_DOWN    ; break; 
    case SDL_SCANCODE_PRINTSCREEN  :  return S2D_KEY_PRINT_SCREEN ; break; 
    case SDL_SCANCODE_PAUSE        :  return S2D_KEY_PAUSE        ; break; 
    case SDL_SCANCODE_MENU         :  return S2D_KEY_MENU         ; break; 
    case SDL_SCANCODE_LSHIFT       :  return S2D_KEY_LEFT_SHIFT   ; break;  
    case SDL_SCANCODE_LCTRL        :  return S2D_KEY_LEFT_CONTROL ; break;  
    case SDL_SCANCODE_LALT         :  return S2D_KEY_LEFT_ALT     ; break;  
    case SDL_SCANCODE_LGUI         :  return S2D_KEY_LEFT_SUPER   ; break;  
    case SDL_SCANCODE_RSHIFT       :  return S2D_KEY_RIGHT_SHIFT  ; break;  
    case SDL_SCANCODE_RCTRL        :  return S2D_KEY_RIGHT_CONTROL; break;  
    case SDL_SCANCODE_RALT         :  return S2D_KEY_RIGHT_ALT    ; break;  
    case SDL_SCANCODE_RGUI         :  return S2D_KEY_RIGHT_SUPER  ; break;     
    case SDL_SCANCODE_CAPSLOCK     :  return S2D_KEY_CAPS_LOCK    ; break;     
    case SDL_SCANCODE_SCROLLLOCK   :  return S2D_KEY_SCROLL_LOCK  ; break;     
    case SDL_SCANCODE_NUMLOCKCLEAR :  return S2D_KEY_NUM_LOCK     ; break;     
    case SDL_SCANCODE_KP_0         :  return S2D_KEY_KP_0         ; break;     
    case SDL_SCANCODE_KP_1         :  return S2D_KEY_KP_1         ; break;     
    case SDL_SCANCODE_KP_2         :  return S2D_KEY_KP_2         ; break;     
    case SDL_SCANCODE_KP_3         :  return S2D_KEY_KP_3         ; break;     
    case SDL_SCANCODE_KP_4         :  return S2D_KEY_KP_4         ; break;     
    case SDL_SCANCODE_KP_5         :  return S2D_KEY_KP_5         ; break;     
    case SDL_SCANCODE_KP_6         :  return S2D_KEY_KP_6         ; break;     
    case SDL_SCANCODE_KP_7         :  return S2D_KEY_KP_7         ; break;     
    case SDL_SCANCODE_KP_8         :  return S2D_KEY_KP_8         ; break;     
    case SDL_SCANCODE_KP_9         :  return S2D_KEY_KP_9         ; break;     
    case SDL_SCANCODE_KP_PERIOD    :  return S2D_KEY_KP_DECIMAL   ; break;     
    case SDL_SCANCODE_KP_DIVIDE    :  return S2D_KEY_KP_DIVIDE    ; break;     
    case SDL_SCANCODE_KP_MULTIPLY  :  return S2D_KEY_KP_MULTIPLY  ; break;     
    case SDL_SCANCODE_KP_MINUS     :  return S2D_KEY_KP_SUBTRACT  ; break;     
    case SDL_SCANCODE_KP_PLUS      :  return S2D_KEY_KP_ADD       ; break;     
    case SDL_SCANCODE_KP_ENTER     :  return S2D_KEY_KP_ENTER     ; break;     
    case SDL_SCANCODE_KP_EQUALS    :  return S2D_KEY_KP_EQUAL     ; break;     
    case SDL_SCANCODE_F1           :  return S2D_KEY_F1           ; break;     
    case SDL_SCANCODE_F2           :  return S2D_KEY_F2           ; break;     
    case SDL_SCANCODE_F3           :  return S2D_KEY_F3           ; break;     
    case SDL_SCANCODE_F4           :  return S2D_KEY_F4           ; break;     
    case SDL_SCANCODE_F5           :  return S2D_KEY_F5           ; break;     
    case SDL_SCANCODE_F6           :  return S2D_KEY_F6           ; break;     
    case SDL_SCANCODE_F7           :  return S2D_KEY_F7           ; break;     
    case SDL_SCANCODE_F8           :  return S2D_KEY_F8           ; break;     
    case SDL_SCANCODE_F9           :  return S2D_KEY_F9           ; break;     
    case SDL_SCANCODE_F10          :  return S2D_KEY_F10          ; break;     
    case SDL_SCANCODE_F11          :  return S2D_KEY_F11          ; break;     
    case SDL_SCANCODE_F12          :  return S2D_KEY_F12          ; break;     
    case SDL_SCANCODE_F13          :  return S2D_KEY_F13          ; break;     
    case SDL_SCANCODE_F14          :  return S2D_KEY_F14          ; break;     
    case SDL_SCANCODE_F15          :  return S2D_KEY_F15          ; break;     
    case SDL_SCANCODE_F16          :  return S2D_KEY_F16          ; break;     
    case SDL_SCANCODE_F17          :  return S2D_KEY_F17          ; break;     
    case SDL_SCANCODE_F18          :  return S2D_KEY_F18          ; break;     
    case SDL_SCANCODE_F19          :  return S2D_KEY_F19          ; break;     
    case SDL_SCANCODE_F20          :  return S2D_KEY_F20          ; break;     
    case SDL_SCANCODE_F21          :  return S2D_KEY_F21          ; break;     
    case SDL_SCANCODE_F22          :  return S2D_KEY_F22          ; break;     
    case SDL_SCANCODE_F23          :  return S2D_KEY_F23          ; break;     
    case SDL_SCANCODE_F24          :  return S2D_KEY_F24          ; break;     
    default                        :  return S2D_KEY_UNKNOWN      ; break;
    }
    return S2D_KEY_UNKNOWN;
}
static s2d_mouse_button_t _s2d_button_map(Uint8 button) {
    switch(button) {
    case SDL_BUTTON_LEFT  : return S2D_MOUSE_BUTTON_LEFT   ; break;
    case SDL_BUTTON_MIDDLE: return S2D_MOUSE_BUTTON_MIDDLE ; break;
    case SDL_BUTTON_RIGHT : return S2D_MOUSE_BUTTON_RIGHT  ; break;
    case SDL_BUTTON_X1    : return S2D_MOUSE_BUTTON_X1     ; break;
    case SDL_BUTTON_X2    : return S2D_MOUSE_BUTTON_X2     ; break;
    default               : return S2D_MOUSE_BUTTON_UNKNOWN; break;
    }
    return S2D_MOUSE_BUTTON_UNKNOWN;
}
int s2d_event_poll(s2d_event_t* e) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    s2d_assert(e != NULL, "S2D event polled with invalid event data store address!");

    SDL_Event ev;

    s2d_mem_zero(e, sizeof(s2d_event_t));
    
    if(_s2d_data.music_finished) {
        _s2d_data.music_finished = 0;
        e[0].type = S2D_EVENT_TYPE_MUSIC_FINISHED;
        return 1;
    }
    
    if(SDL_PollEvent(&ev)) {
        switch (ev.type) {
            case SDL_QUIT:
                e[0].type = S2D_EVENT_TYPE_QUIT;
                break;
                
            case SDL_KEYDOWN:
                if(ev.key.repeat == 0) {
                    e[0].type = S2D_EVENT_TYPE_KEY_PRESS;
                } else {
                    e[0].type = S2D_EVENT_TYPE_KEY_REPEAT;
                }
                e[0].value.key = _s2d_scancode_to_key(ev.key.keysym.scancode);
                break;

            case SDL_KEYUP:
                if(ev.key.repeat == 0) {
                    e[0].type = S2D_EVENT_TYPE_KEY_RELEASE;
                } else {
                    e[0].type = S2D_EVENT_TYPE_KEY_REPEAT;
                }
                e[0].value.key = _s2d_scancode_to_key(ev.key.keysym.scancode);
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                e[0].type = S2D_EVENT_TYPE_MOUSE_CLICK;
                e[0].value.button = _s2d_button_map(ev.button.button);
                break;
                
            case SDL_MOUSEBUTTONUP:
                e[0].type = S2D_EVENT_TYPE_MOUSE_RELEASE;
                e[0].value.button = _s2d_button_map(ev.button.button);
                break;
                
            case SDL_MOUSEMOTION:
                e[0].type = S2D_EVENT_TYPE_MOUSE_MOVE;
                e[0].value.pos.x = ev.motion.x;
                e[0].value.pos.y = ev.motion.y;
                break;
                
            case SDL_MOUSEWHEEL:
                e[0].type = S2D_EVENT_TYPE_MOUSE_SCROLL;
                e[0].value.pos.x = ev.wheel.x;
                e[0].value.pos.y = ev.wheel.y;
                break;

            default:
                e[0].type = S2D_EVENT_TYPE_UNKNOWN;
                break;
        }
        return 1;
    }

    return 0;
}

s2d_sound_t s2d_sound_load(char* path) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    s2d_assert(path != NULL, "S2D Sound loaded without file name!");
    return Mix_LoadWAV(path);
}
void s2d_sound_play(s2d_sound_t sound, int channel) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    s2d_assert(sound != NULL, "invalid S2D Sound played!");
    s2d_assert(channel >= 0 && channel <= _s2d_data.nr_parallel_sound_effects, "S2D Sound played on invalid channel!");
    Mix_PlayChannel(channel, (Mix_Chunk*) sound, 0);
}
void s2d_sound_free(s2d_sound_t sound) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    s2d_assert(sound != NULL, "invalid S2D Sound freed!");
    Mix_FreeChunk((Mix_Chunk*) sound);
}
void s2d_sound_volume(s2d_sound_t sound, float volume_percentage) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    s2d_assert(sound != NULL, "invalid S2D Sound volume set!");
    s2d_assert(volume_percentage >= 0.0 && volume_percentage <= 1.0, "invalid volume set");
    Mix_VolumeChunk((Mix_Chunk*) sound, s2d_floor(((float)MIX_MAX_VOLUME)*volume_percentage));
}
void s2d_sound_channel_volume(int channel, float volume_percentage) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    s2d_assert(channel >= 0 && channel <= _s2d_data.nr_parallel_sound_effects, "S2D volume set on invalid channel!");
    s2d_assert(volume_percentage >= 0.0 && volume_percentage <= 1.0, "invalid volume set");
    Mix_Volume(channel, s2d_floor(((float)MIX_MAX_VOLUME)*volume_percentage));
}

s2d_music_t s2d_music_load(char* path) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    s2d_assert(path != NULL, "S2D Sound loaded without file name!");
    return Mix_LoadMUS(path);
}
void s2d_music_play(s2d_music_t music, int loop) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    s2d_assert(music != NULL, "invalid S2D Music played!");
    Mix_PlayMusic((Mix_Music*) music, (loop) ? -1 : 0);
}
void s2d_music_stop(void) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    Mix_HaltMusic();
}
void s2d_music_free(s2d_music_t music) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    s2d_assert(music != NULL, "invalid S2D Music freed!");
    Mix_FreeMusic((Mix_Music*) music);
}
void s2d_music_volume(float volume_percentage) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    s2d_assert(volume_percentage >= 0.0 && volume_percentage <= 1.0, "invalid volume set");
    Mix_VolumeMusic(s2d_floor(((float)MIX_MAX_VOLUME)*volume_percentage));
}

void s2d_clear(void) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    SDL_RenderClear(_s2d_data.renderer);
}
void s2d_render(void) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    SDL_RenderPresent(_s2d_data.renderer);
}

void s2d_wait_ms(uint32_t ms) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    SDL_Delay(ms);
}
int64_t s2d_time_ms_since_init(void) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    return (int64_t) SDL_GetTicks64();
}

int s2d_file_read(char* path, void** out_data, size_t* out_len) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    void* ptr; size_t len;
    ptr = SDL_LoadFile(path, &len);
    if(ptr == NULL) return -1;
    out_data[0] = ptr;
    out_len[0] = len;
    return 0;
}

int s2d_file_write(char* path, void* data, size_t len) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    SDL_RWops* rw; size_t check_size;
    
    /* check if file exists! we need to do it the ugly way since SDL2 has no stat: */
    rw = SDL_RWFromFile(path, "rb");
    if(rw != NULL) {
        SDL_FreeRW(rw);
        return -1;
    }
    
    rw = SDL_RWFromFile(path, "wb");
    s2d_assert(rw != NULL, "Error creating file %s!", path);
    
    check_size = SDL_RWwrite(rw, data, 1, len);
    s2d_assert(check_size == len, "Error writing file %s!\nExpected to write %zu bytes, but only wrote %zu bytes.", path, len, check_size);
    
    SDL_FreeRW(rw);
    
    return 0;
}

char* s2d_str_new(const char* str_const) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    return SDL_strdup(str_const);
}
char* s2d_str_concat(char* str, const char* str_const) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    size_t old_len = s2d_str_len(str);
    size_t const_len = s2d_str_len(str_const);
    char* new_ptr = s2d_mem_realloc(str, old_len + const_len + 1);
    s2d_mem_copy(&new_ptr[old_len], str_const, const_len);
    new_ptr[old_len + const_len] = '\0';
    return new_ptr;
}
size_t s2d_str_len(const char* str) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    return SDL_strlen(str);
}
int s2d_str_cmp(const char* a, const char* b) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    return SDL_strcmp(a,b);
}

void* s2d_mem_alloc(size_t size, int zero) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    void* ptr;
    if(zero) ptr = SDL_calloc(size, 1);
    else ptr = SDL_malloc(size);
    s2d_assert(ptr != NULL, "S2D ran out of memory!");
    return ptr;
}
void* s2d_mem_realloc(void* old_ptr, size_t new_size) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    void* ptr = SDL_realloc(old_ptr, new_size);
    s2d_assert(ptr != NULL, "S2D ran out of memory!");
    return ptr;
}
void s2d_mem_free(void* ptr) {
    SDL_free(ptr);
}

void s2d_mem_zero(void* ptr, size_t size) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    SDL_memset(ptr, 0, size);
}
void s2d_mem_copy(void* dst, const void* src, size_t size) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    SDL_memmove(dst, src, size);
}
int s2d_mem_cmp(const void* a, const void* b, size_t size) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    return SDL_memcmp(a, b, size);
}

s2d_texture_t s2d_texture_load(char* path) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    s2d_assert(path != NULL, "S2D Texture loaded without file name!");
    return IMG_LoadTexture(_s2d_data.renderer, path);
}
void s2d_texture_free(s2d_texture_t tex) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    SDL_DestroyTexture((SDL_Texture*) tex);
}

void s2d_texture_dimensions(s2d_texture_t tex, int* out_width, int* out_height)  {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    SDL_QueryTexture((SDL_Texture*) tex, NULL, NULL, out_width, out_height);
}

void s2d_texture_blit_rects(s2d_texture_t tex, int* src_dst_rects, int nr) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    SDL_Rect sdl_src, sdl_dst;
    for(int i = 0; i < nr; i++) {
        sdl_src.x = src_dst_rects[8*i];
        sdl_src.y = src_dst_rects[8*i];
        sdl_src.w = src_dst_rects[8*i];
        sdl_src.h = src_dst_rects[8*i];
        sdl_dst.x = src_dst_rects[8*i];
        sdl_dst.y = src_dst_rects[8*i];
        sdl_dst.w = src_dst_rects[8*i];
        sdl_dst.h = src_dst_rects[8*i];
        SDL_RenderCopy(_s2d_data.renderer, (SDL_Texture*) tex, &sdl_src, &sdl_dst);
    }
}
void s2d_texture_blit_mesh(s2d_texture_t tex, int* grid_nrs, int dst_width, int dst_height, int tile_width, int tile_height, int src_tiles_per_row, int origin_x, int origin_y, float scale) {
    s2d_assert(_s2d_data.is_init, "S2D Library function called before initialization!");
    int nrs_index, nr, src_x, src_y;
    SDL_Rect sdl_src, sdl_dst;
    sdl_src.w = sdl_dst.w = tile_width;
    sdl_src.h = sdl_dst.h = tile_height;
    for(int y = 0; y < dst_height; y++) {
        for(int x = 0; x < dst_width; x++) {
            nrs_index = x + y*dst_width;
            nr = grid_nrs[nr];
            src_x = nr % src_tiles_per_row;
            src_y = (nr - src_x) / src_tiles_per_row;
            sdl_src.x = src_x * tile_width;
            sdl_src.y = src_y * tile_height;
            sdl_dst.x = (int) ((scale*((float)sdl_src.x)) + origin_x);
            sdl_dst.y = (int) ((scale*((float)sdl_src.y)) + origin_y);
            SDL_RenderCopy(_s2d_data.renderer, (SDL_Texture*) tex, &sdl_src, &sdl_dst);
        }
    }
}











