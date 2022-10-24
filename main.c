#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

typedef struct{
    int x;
    int y;
    int r;
} creature;

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize instance: %s\n", description);
    exit(1);
}

void move(creature *creature, char d, int r){
    if(d == 'w') creature->y-=2;
    if(d == 'a') creature->x-=2;
    if(d == 's') creature->y+=2;
    if(d == 'd') creature->x+=2;
    creature->r = r;
}

int main(){
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    ALLEGRO_DISPLAY* disp = al_create_display(640, 480);
    must_init(disp, "display");

    must_init(al_install_audio(), "audio");
    must_init(al_init_acodec_addon(), "audio codecs");
    must_init(al_reserve_samples(16), "reserve samples");

    must_init(al_init_image_addon(), "image addon");
    ALLEGRO_BITMAP* player_sprite = al_load_bitmap("assets/images/player.bmp");
    must_init(player_sprite, "player.png");
    ALLEGRO_BITMAP* sword = al_load_bitmap("assets/images/sword.png");
    must_init(sword, "sword.png");
    ALLEGRO_BITMAP* background = al_load_bitmap("assets/images/grass.png");
    must_init(background, "grass.png");

    ALLEGRO_SAMPLE* background_music = al_load_sample("assets/audio/overworld.ogg");
    must_init(background_music, "overworld.ogg");
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    creature player = {320, 240, 0};

    #define KEY_SEEN 1
    #define KEY_RELEASED 2

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    al_start_timer(timer);

    al_play_sample(background_music, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);
    for(;;){
        al_wait_for_event(queue, &event);

        switch(event.type){
            case ALLEGRO_EVENT_TIMER:
                if(key[ALLEGRO_KEY_W]){
                    move(&player, 'w', 0);
                }
                if(key[ALLEGRO_KEY_S]){
                    move(&player, 's', 600);
                }
                if(key[ALLEGRO_KEY_A]){
                    move(&player, 'a', 300);
                }
                if(key[ALLEGRO_KEY_D]){
                    move(&player, 'd', 900);
                }

                if(key[ALLEGRO_KEY_ESCAPE])
                    done = true;

                for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                    key[i] &= KEY_SEEN;

                redraw = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                break;

            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;
                
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if(done) break;

        if(redraw && al_is_event_queue_empty(queue)){
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_bitmap(background, 0, 0, 0);
            al_draw_rotated_bitmap(sword, 16, 16, player.x, player.y, player.r, 0);
            al_draw_rotated_bitmap(player_sprite, 16, 16, player.x, player.y, player.r, 0);
            al_flip_display();
            redraw = false;
        }
    }

    al_destroy_bitmap(player_sprite);
    al_destroy_sample(background_music);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
