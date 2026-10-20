#include "s2d.h"


int main(void) {
    s2d_event_t e; bool keep_going = 1;

    s2d_init(800, 600, false, false, "S2D test", 0);
    
    while(keep_going) {
        while(s2d_event_poll(&e)) {
            s2d_log_line("Event recieved: %i, key/button/x: %i, y: %i", e.type, e.value.pos.x, e.value.pos.y);
            if((e.type == S2D_EVENT_TYPE_KEY_PRESS && e.value.key == S2D_KEY_Q)
                || e.type == S2D_EVENT_TYPE_QUIT) keep_going = 0;
                
                
            s2d_assert(!(e.type == S2D_EVENT_TYPE_KEY_PRESS && e.value.key == S2D_KEY_T),   
                "This is test assert #%i", 1);
        }
    
        s2d_clear();
        s2d_render();
    }
    
    s2d_quit();
}
