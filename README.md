# S2D - Simple 2D Games library

This library is a simple windowing/input, graphics, sound and utility library for 2D games. It is currently implemented in SDL2.

## windowing and input

The library creates a single window of fixed dimensions either in windowed or full-screen mode, optionally the default mouse cursor can be disabled. For input, Key press/release and Mouse click/release/move/scroll events are reported through the `s2d_event_poll` function, as well as the press on the window close button and the internal notice that the music has stopped playing.

    void s2d_init(int width, int height, int fullscreen, int hide_cursor, char* title, int nr_parallel_sound_effect);
    void s2d_quit(void);
    int s2d_event_poll(s2d_event_t* e);  /* returns 1 while events are pending, 0 otherwise */

## graphics

The library gives facilities to load textures and blit them on screen, by using it either as a free-form texture map to select UV rectangles out of to map onto XY rectangles - as 8 ints in order u,v,delta_u,delta_v,x,y,delta_x,delta_y - or to use it as a tile-map to create a mesh from with each tile of the mesh being equivalent to an index of the tile map. Before these functions, the screen needs to get cleared, and after it, the render needs to be completed with the `s2d_render` function.

    s2d_texture_t s2d_texture_load(char* path);
    void s2d_texture_free(s2d_texture_t tex);
    void s2d_texture_dimensions(s2d_texture_t tex, int* out_width, int* out_height);
    void s2d_texture_blit_rects(s2d_texture_t tex, int* src_dst_rect_pairs, int nr);
    void s2d_texture_blit_mesh(s2d_texture_t tex, int* grid_nrs, int dst_width, int dst_height, int tile_width, int tile_height, int src_tiles_per_row, int origin_x, int origin_y, float scale);
    void s2d_clear(void);
    void s2d_render(void);

## sound

There are two kinds of sound that can be used analogously: a) sound effects, that need to be placed on a channel. The maximum number of such sound effects is set at initialization; b) music, which only has one channel, but is otherwise very similar. It has the extra feature of having a loop option and giving an event notice when it's done playing, so another track can be lined up.

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

## utilities

