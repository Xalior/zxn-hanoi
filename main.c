//
// Created by D Rimron-Soutter (Xalior) on 20/10/2018.
//


#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <input.h>
#include <z80.h>
#include <intrinsic.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "zxnext_layer2.h"
#include "zxnext_sprite.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/
#define printInk(k)                 printf("\x10%c", '0'+k)
#define printPaper(k)               printf("\x11%c", '0'+k)
#define printBright(k)              printf("\x13%d", k);
#define printAt(row, col)           printf("\x16%c%c", (col)+1, (row)+1)
#define printStrAt(row, col, str)   printf("\x16%c%c%s", (col)+1, (row)+1, str)

/*******************************************************************************
 * Personalisation Options
 ******************************************************************************/
#define MOVE_SPEED                  8
#define RIGHT_COLUMN                164
#define HIGHEST_POINT               167

/*******************************************************************************
 * Variables
 ******************************************************************************/
unsigned char *readBuffer[256];     // As data is read from the SDCard it
                                    // arrives in here, in 1/4k chunks...
uint8_t high_score = 60;            // Default high-score - best playtime,
                                    // in seconds, overwritten in memory
/*******************************************************************************
 * Functions
 ******************************************************************************/
void init_hardware(void)
{
    // Set up IM2 interrupt service routine, we don't really use this but
    // it is a cheap and cheaty way to count frames - don't worry if you
    // don't understand all of this, we're not going to use, we just want
    // the side effects of it running...
    intrinsic_di();
    im2_init((void *) 0x6000);
    memset((void *) 0x6000, 0x61, 257);
    z80_bpoke(0x6161, 0xFB);
    z80_bpoke(0x6162, 0xED);
    z80_bpoke(0x6163, 0x4D);
    intrinsic_ei();

    // Put Z80 in 14 MHz turbo mode.
    ZXN_NEXTREGA(REG_PERIPHERAL_2, ZXN_READ_REG(REG_PERIPHERAL_2) | RP2_ENABLE_TURBO);
    ZXN_NEXTREG(REG_TURBO_MODE, RTM_14MHZ);

    // And setup the memory banks for screens - mostly this is for emulators...
    layer2_set_main_screen_ram_bank(8);
    layer2_set_shadow_screen_ram_bank(11);

    // Now we need to make sure we're in the right folder. Saves us typing
    // "/HANOI" every time we want a file later
    esxdos_f_chdir("/HANOI");

}

void intro_screen(void) {
    // Default colourscheme for the game
    zx_border(INK_BLACK);               // A black border,
    zx_cls(INK_BLACK | PAPER_MAGENTA | BRIGHT); // Black INK, Bright Magenta Paper
                    // The reason we use Bright Magenta Paper is that it makes the
                    // ULA go see-through, which means we can do fancy text FX on it


    // Throw up a pretty quick bitmap as a background for our later loader
    layer2_load_screen("Loader.nxi", MAIN_SCREEN, 6, false);


    layer2_configure(true, false, false, 0);

    layer2_set_layer_priorities(LAYER_PRIORITIES_U_S_L);

    layer2_set_global_transparency_color(231);

    // Wait for 50 frames, count the
    for(int i=0; i<50;i++) {
        intrinsic_halt();
    }

}

static void init_assets(void)
{
    set_sprite_layers_system(true, false, LAYER_PRIORITIES_U_L_S, false);
    load_sprite_patterns("Sprites.spr", readBuffer, 15, 0);

    layer2_configure(true, false, false, 0);
    layer2_load_screen("MainMenu.nxi", MAIN_SCREEN, 6, false);
    layer2_set_layer_priorities(LAYER_PRIORITIES_U_S_L);

    if (errno)
    {
        printAt(0,0);
        printf("FAILED TO LOAD HANOI: ERROR: %d", errno);
        exit(errno);
    }
}

void draw_menu(void) {
    printPaper(INK_MAGENTA | BRIGHT);
    printInk(INK_YELLOW);
    printAt(5, 10); printf("%02d seconds", high_score);
}

void wait_for_key(void) {
    while(!in_inkey()) {
        intrinsic_halt();
    }
}

void hanoi_draw_row(int row, int thisX, int thisY) {
    switch(row) {
        case 1:
            // Row 1
            set_sprite_slot(0);
            set_sprite_attributes_ext(0, thisX, thisY, 0, 0, true);
            break;
        case 2:
            // Row 2
            set_sprite_slot(1);
            set_sprite_attributes_ext(1, thisX - 8, thisY, 0, 0, true);
            set_sprite_slot(2);
            set_sprite_attributes_ext(2, thisX + 8, thisY, 0, 0, true);
            break;
        case 3:
            // Row 3
            set_sprite_slot(3);
            set_sprite_attributes_ext(3, thisX - 16, thisY, 0, 0, true);
            set_sprite_slot(4);
            set_sprite_attributes_ext(4, thisX, thisY, 0, 0, true);
            set_sprite_slot(5);
            set_sprite_attributes_ext(5, thisX + 16, thisY, 0, 0, true);
            break;
        case 4:
            // Row 4
            set_sprite_slot(6);
            set_sprite_attributes_ext(6, thisX - 24, thisY, 0, 0, true);
            set_sprite_slot(7);
            set_sprite_attributes_ext(7, thisX - 8, thisY, 0, 0, true);
            set_sprite_slot(8);
            set_sprite_attributes_ext(8, thisX + 8, thisY, 0, 0, true);
            set_sprite_slot(9);
            set_sprite_attributes_ext(9, thisX + 24, thisY, 0, 0, true);
            break;
        case 5:
            // Row 5
            set_sprite_slot(10);
            set_sprite_attributes_ext(10, thisX - 32, thisY, 0, 0, true);
            set_sprite_slot(11);
            set_sprite_attributes_ext(11, thisX - 16, thisY, 0, 0, true);
            set_sprite_slot(12);
            set_sprite_attributes_ext(12, thisX, thisY, 0, 0, true);
            set_sprite_slot(13);
            set_sprite_attributes_ext(13, thisX + 16, thisY, 0, 0, true);
            set_sprite_slot(14);
            set_sprite_attributes_ext(14, thisX + 32, thisY, 0, 0, true);
            break;
    }
}

void play_game(void) {

    zx_cls(INK_BLACK | PAPER_MAGENTA | BRIGHT); // Black INK, Bright Magenta Paper

    uint8_t X = 38;
    uint8_t Y = 167;

    // These are our "pillars of Hanoi". Three of them, called

    // Left - this is our first pillar, and fully loaded to begin
    uint8_t left_pillar[5] = {5, 4, 3, 2, 1};
    uint8_t left_pillar_height = 5;

    // Middle - empty
    uint8_t middle_pillar[5] = {0, 0, 0, 0, 0};
    uint8_t middle_pillar_height = 0;

    // Right - and our target pillar
    uint8_t right_pillar[5] = {0, 0, 0, 0, 0};
    uint8_t right_pillar_height = 0;

    // The status of the current game...

    bool    animating = false;  // Is there already a piece moving?
    bool    playing = true;     // Is the game in play? FALSE == won

    uint8_t inkey = 0;
    uint8_t lastkey = 0;
    uint8_t moving = 0;
    uint8_t moving_phase = 0;
    uint8_t moving_x, moving_y, destination_x, destination_y;

    uint8_t timer = 0;
    uint8_t frames = 0;

    layer2_load_screen("MainGame.nxi", MAIN_SCREEN, 6, false);


    while(playing) {
        frames++;
        if(frames==50) {
            frames=0;
            timer++;
            printAt(23, 4); printf("Time Elapsed: %d second", timer);
            if(timer>1) printf("s");
        }

        set_sprite_layers_system(true, false, LAYER_PRIORITIES_U_S_L, false);
        layer2_set_layer_priorities(LAYER_PRIORITIES_U_S_L);

        for(int pillar_render=left_pillar_height; pillar_render; pillar_render--) {
            hanoi_draw_row(left_pillar[pillar_render-1], X, HIGHEST_POINT - (16 * (pillar_render)));
        }
        zx_border(INK_MAGENTA);


        // Is there a piece currently in motion?
        if (animating) {
            // If so, what direction is it moving?
            switch(moving_phase) {
                // Up?
                case 1:
                    if(moving_y>=destination_y) {
                        moving_y = moving_y - MOVE_SPEED;
                    } else {
                        moving_y = destination_y;
                        animating = false;
                    }
                    break;
                // Sideyways
                case 2:
                    if(moving_x>destination_x) {
                        moving_x=moving_x-MOVE_SPEED;
                        // Did I go too far?
                        if(moving_x<destination_x){
                            moving_x=destination_x;
                        }
                    } else if (moving_x<destination_x) {
                        moving_x=moving_x+MOVE_SPEED;
                        // Did I go too far?
                        if(moving_x>destination_x){
                            moving_x=destination_x;
                        }
                    } else {
                        moving_phase = 3;
                    }
                    break;
                // Down
                case 3:
                    if(moving_y<=destination_y) {
                        moving_y = moving_y + MOVE_SPEED;
                    } else {
                        moving_y = destination_y;
                        moving_phase = 0;
                        animating = false;
                    }
                    break;
            }

            hanoi_draw_row(moving, moving_x, moving_y);
        } else if(right_pillar[4]==1) { // WINNER!
//            score = (timer*50)+frame;
//            timer = 0; frame = 0;
            playing = false;
        } else {
            inkey = in_inkey();
            if(inkey!=lastkey) {
                switch (inkey) {
                    case 0:
                        break;
                    case 49:  // 1
                        if (moving_phase == 0) {
                            if (left_pillar_height) {
                                moving = left_pillar[left_pillar_height - 1];
                                left_pillar[left_pillar_height - 1] = 0;
                                moving_y = Y - ((left_pillar_height + 1) * 16);
                                moving_x = X;
                                left_pillar_height--;
                                animating = true;
                                moving_phase = 1;
                                destination_y = 21;
                                destination_x = X;
                            }
                        } else {
                            if ((left_pillar_height == 0) || (left_pillar[left_pillar_height - 1] > moving)) {
                                if (moving_x != X) {
                                    destination_x = X;
                                    moving_phase = 2;
                                } else {
                                    moving_phase = 3;
                                }
                                destination_y = Y - ((left_pillar_height + 1) * 16);
                                left_pillar_height++;
                                left_pillar[left_pillar_height - 1] = moving;
                                animating = true;
                            } else {
                                // invalid move
                            }
                        }
                        break;
                    case 50:  // 2
                        if (moving_phase == 0) {
                            if (middle_pillar_height) {
                                moving = middle_pillar[middle_pillar_height - 1];
                                middle_pillar[middle_pillar_height - 1] = 0;
                                moving_y = HIGHEST_POINT - ((middle_pillar_height + 1) * 16);
                                moving_x = X + 84;
                                middle_pillar_height--;
                                animating = true;
                                moving_phase = 1;
                                destination_y = 21;
                                destination_x = X;
                            }
                        } else {
                            if ((middle_pillar_height == 0) || (middle_pillar[middle_pillar_height - 1] > moving)) {
                                if (moving_x != X + 84) {
                                    destination_x = X + 84;
                                    moving_phase = 2;
                                } else {
                                    moving_phase = 3;
                                }
                                destination_y = HIGHEST_POINT - ((middle_pillar_height + 1) * 16);
                                middle_pillar_height++;
                                middle_pillar[middle_pillar_height - 1] = moving;
                                animating = true;
                            } else {
                                // invalid move
                            }
                        }
                        break;
                    case 51:  // 3
                        if (moving_phase == 0) {
                            if (right_pillar_height) {
                                moving = right_pillar[right_pillar_height - 1];
                                right_pillar[right_pillar_height - 1] = 0;
                                moving_y = HIGHEST_POINT - ((right_pillar_height + 1) * 16);
                                moving_x = X + RIGHT_COLUMN;
                                right_pillar_height--;
                                animating = true;
                                moving_phase = 1;
                                destination_y = 21;
                                destination_x = X;
                            }
                        } else {
                            if ((right_pillar_height == 0) || (right_pillar[right_pillar_height - 1] > moving)) {
                                if (moving_x != X + RIGHT_COLUMN) {
                                    destination_x = X + RIGHT_COLUMN;
                                    moving_phase = 2;
                                } else {
                                    moving_phase = 3;
                                }
                                destination_y = HIGHEST_POINT - ((right_pillar_height + 1) * 16);
                                right_pillar_height++;
                                right_pillar[right_pillar_height - 1] = moving;
                                animating = true;
                            } else {
                                // invalid move
                            }
                        }
                        break;
                }
                lastkey = inkey;
            }
        }
        intrinsic_halt();

    }

    set_sprite_layers_system(false, false, LAYER_PRIORITIES_U_L_S, false);
}


/*******************************************************************************
 * Main Loop
 ******************************************************************************/

void main(void)
{
    // Make sure the next is ready for us
    init_hardware();
    // First impressions matter
    intro_screen();
    // And load all our graphics; this is where we do our loading screen, etc.
    init_assets();

    // When should this stop... - https://xal.io/nostop
    while(true) {
        // Main Menu - not much here, but you can extend it...
        draw_menu();
        // Pause
        wait_for_key();
        // The reason we came
        play_game();
        // How did you do.... - https://xal.io/dowell
//        show_score();
        // Reuse that wait logic - because all devs are lazy
        wait_for_key();
        // Rinse and repeat - the loop now goes on forever
    }
}
