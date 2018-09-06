/*
Copyright 2011, 2012, 2013 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include "keyboard.h"
#include "matrix.h"
#include "keymap.h"
#include "host.h"
#include "led.h"
#include "keycode.h"
#include "timer.h"
#include "print.h"
#include "debug.h"
#include "command.h"
#include "util.h"
#include "sendchar.h"
#include "eeconfig.h"
#include "backlight.h"
#include "keymap_jp.h"
#include "action_layer.h"

#ifdef BOOTMAGIC_ENABLE
#   include "bootmagic.h"
#else

#   include "magic.h"

#endif
#ifdef MOUSEKEY_ENABLE
#   include "mousekey.h"
#endif
#ifdef PS2_MOUSE_ENABLE
#   include "ps2_mouse.h"
#endif
#ifdef SERIAL_MOUSE_ENABLE
#   include "serial_mouse.h"
#endif
#ifdef ADB_MOUSE_ENABLE
#   include "adb.h"
#endif
#ifdef RGBLIGHT_ENABLE
#   include "rgblight.h"
#endif
#ifdef STENO_ENABLE
#   include "process_steno.h"
#endif
#ifdef FAUXCLICKY_ENABLE
#   include "fauxclicky.h"
#endif
#ifdef SERIAL_LINK_ENABLE
#   include "serial_link/system/serial_link.h"
#endif
#ifdef VISUALIZER_ENABLE
#   include "visualizer/visualizer.h"
#endif
#ifdef POINTING_DEVICE_ENABLE
#   include "pointing_device.h"
#endif
#ifdef MIDI_ENABLE
#   include "process_midi.h"
#endif

#ifdef MATRIX_HAS_GHOST
extern const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS];
static matrix_row_t get_real_keys(uint8_t row, matrix_row_t rowdata){
    matrix_row_t out = 0;
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        //read each key in the row data and check if the keymap defines it as a real key
        if (pgm_read_byte(&keymaps[0][row][col]) && (rowdata & (1<<col))){
            //this creates new row data, if a key is defined in the keymap, it will be set here
            out |= 1<<col;
        }
    }
    return out;
}

static inline bool popcount_more_than_one(matrix_row_t rowdata)
{
    rowdata &= rowdata-1; //if there are less than two bits (keys) set, rowdata will become zero
    return rowdata;
}

static inline bool has_ghost_in_row(uint8_t row, matrix_row_t rowdata)
{
    /* No ghost exists when less than 2 keys are down on the row.
    If there are "active" blanks in the matrix, the key can't be pressed by the user,
    there is no doubt as to which keys are really being pressed.
    The ghosts will be ignored, they are KC_NO.   */
    rowdata = get_real_keys(row, rowdata);
    if ((popcount_more_than_one(rowdata)) == 0){
        return false;
    }
    /* Ghost occurs when the row shares a column line with other row,
    and two columns are read on each row. Blanks in the matrix don't matter,
    so they are filtered out.
    If there are two or more real keys pressed and they match columns with
    at least two of another row's real keys, the row will be ignored. Keep in mind,
    we are checking one row at a time, not all of them at once.
    */
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        if (i != row && popcount_more_than_one(get_real_keys(i, matrix_get_row(i)) & rowdata)){
            return true;
        }
    }
    return false;
}

#endif

/** \brief matrix_setup
 *
 * FIXME: needs doc
 */
__attribute__ ((weak))
void matrix_setup(void) {
}

/** \brief keyboard_setup
 *
 * FIXME: needs doc
 */
void keyboard_setup(void) {
    matrix_setup();
}

/** \brief is_keyboard_master
 *
 * FIXME: needs doc
 */
__attribute__((weak))
bool is_keyboard_master(void) {
    return true;
}

/** \brief keyboard_init
 *
 * FIXME: needs doc
 */
void keyboard_init(void) {
    timer_init();
    matrix_init();
#ifdef PS2_MOUSE_ENABLE
    ps2_mouse_init();
#endif
#ifdef SERIAL_MOUSE_ENABLE
    serial_mouse_init();
#endif
#ifdef ADB_MOUSE_ENABLE
    adb_mouse_init();
#endif
#ifdef BOOTMAGIC_ENABLE
    bootmagic();
#else
    magic();
#endif
#ifdef BACKLIGHT_ENABLE
    backlight_init();
#endif
#ifdef RGBLIGHT_ENABLE
    rgblight_init();
#endif
#ifdef STENO_ENABLE
    steno_init();
#endif
#ifdef FAUXCLICKY_ENABLE
    fauxclicky_init();
#endif
#ifdef POINTING_DEVICE_ENABLE
    pointing_device_init();
#endif
#if defined(NKRO_ENABLE) && defined(FORCE_NKRO)
    keymap_config.nkro = 1;
#endif
}

/** \brief Keyboard task: Do keyboard routine jobs
 *
 * Do routine keyboard jobs: 
 *
 * * scan matrix
 * * handle mouse movements
 * * run visualizer code
 * * handle midi commands
 * * light LEDs
 *
 * This is repeatedly called as fast as possible.
 */

#define QUE_SIZE 8
typedef keyevent_t data_t;

bool enque(data_t que_data[QUE_SIZE], data_t enq_data, uint8_t *que_head, uint8_t *que_num) {
    if (*que_num < QUE_SIZE) {
        que_data[(*que_head + *que_num) % QUE_SIZE] = enq_data;
        (*que_num)++;
        return true;
    } else {
        return false;
    }
}

data_t unenque(data_t que_data[QUE_SIZE], uint8_t *que_head, uint8_t *que_num) {
    data_t unenq_data;
    if (*que_num > 0) {
        unenq_data = que_data[(*que_head + *que_num - 1) % QUE_SIZE];
        (*que_num)--;
        return unenq_data;
    } else {
        return TICK;
    }
}

data_t deque(data_t que_data[QUE_SIZE], uint8_t *que_head, uint8_t *que_num) {
    data_t deq_data;
//    udprintf("*que_num in deque: %u\n",*que_num);
    if (*que_num > 0) {
//        udprint("*que_num > 0\n");
        deq_data = que_data[*que_head];
        *que_head = (*que_head + 1) % QUE_SIZE;
        (*que_num)--;
        return deq_data;
    } else {
        return TICK;
    }
}

data_t read_que_head(data_t que_data[QUE_SIZE], uint8_t *que_head, uint8_t *que_num) {
    if (*que_num > 0) {
        return que_data[*que_head];
    } else {
        return TICK;
    }
}

void que_clear(uint8_t *que_head, uint8_t *que_num) {
    *que_head = 0;
    *que_num = 0;
}

void que_print(data_t que_data[QUE_SIZE], char *str, uint8_t *que_head, uint8_t *que_num) {
    udprintf("=== que_print === %32s\n", str);
    udprintf("*que_head: %d\t", *que_head);
    udprintf("*que_num: %d\n", *que_num);
    udprint("que:[");
//    for(uint8_t i=*que_head; i < *que_head + QUE_SIZE; i++){
//        if(i != *que_head) udprint(",");
//        udprintf(" %d",que_data[i % QUE_SIZE].time);
//    }
    for (uint8_t i = 0; i < QUE_SIZE; i++) {
        if (i != 0) udprint(",");
        udprintf(" %u", que_data[i].time);
    }
    udprint("]\n");
    udprintln("================");
}

uint16_t ktk(keypos_t key) {
    return keymap_key_to_keycode(layer_switch_get_layer(key), key);
}

#define atk(keycode)    alphabet_to_keypos[keycode - 0x04]
#define bspc_keypos     alphabet_to_keypos[24]

void keycode_event_action(uint8_t keycode, uint16_t pressed_time, uint16_t release_time) {
    // [0]=kc_a's keypos ... [23]= kc_z's keypos [24] = kc_backspace's keypos
    static keypos_t alphabet_to_keypos[25] = {};
    if (alphabet_to_keypos[0].row == 0) {
        for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
            for (uint8_t c = 0; c < MATRIX_COLS; c++) {
                keypos_t current_keypos = {
                        .row = r,
                        .col = c
                };
                switch (/* keycode = */pgm_read_word(&keymaps[0][r][c])) {
                    case KC_A ... KC_Z:
                        // kc_a = 0x04 ... kc_z = 0x17
                        alphabet_to_keypos[keycode - 0x04] = current_keypos;
                        break;
                    case KC_BSPC:
                        alphabet_to_keypos[24] = current_keypos;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    keypos_t keypos = TICK.key;
    switch(keycode){
        case KC_A ... KC_Z:
            keypos = atk(keycode);
            break;
        case KC_BSPC:
            keypos = alphabet_to_keypos[24];
            break;
        default:
            break;
    }
    udprintln("----------------------------");
    for(uint8_t i=0; i<25; i++){
        udprintf("alphabet_to_keypos[%d] : r = %u, c = %u\n",i,alphabet_to_keypos[i].row,alphabet_to_keypos[i].col);
    }
    udprintln("----------------------------");
    udprintv(keypos.col,%u);
    udprintv(keypos.row,%u);
    action_exec((keyevent_t) {
            .time = pressed_time,
            .pressed = true,
            .key = keypos
    });
    action_exec((keyevent_t) {
            .time = release_time,
            .pressed = false,
            .key = keypos
    });
}

bool is_convert_action_event(keyevent_t action_event, bool is_ime_on, keyevent_t *hist_que, uint8_t *hist_que_head,
                             uint8_t *hist_que_num) {
    udprintf("ime: %d\n",is_ime_on);
    udprintf("action_event.pressed: %d\n",action_event.pressed);
    if (is_ime_on) {
        if (action_event.pressed) {
            uint8_t current_keycode = ktk(action_event.key);
            udprintv(current_keycode,%u);
            if ((current_keycode == KC_A) || \
                (current_keycode == KC_I) || \
                (current_keycode == KC_U) || \
                (current_keycode == KC_E) || \
                (current_keycode == KC_O)) {

                uint8_t hist_que_last = *hist_que_head + *hist_que_num - 1;
                keyevent_t last_event = hist_que[hist_que_last];

                uint16_t l_t = last_event.time;
                uint16_t a_t = action_event.time;

                switch (ktk(last_event.key)) {
                    case KC_C: // ex) ca > ka
                    {
                        uint16_t bs_t_pressed = TIMER_RATE_16(a_t, l_t, 5, 1);
                        uint16_t bs_t_release = TIMER_RATE_16(a_t, l_t, 5, 2);
                        uint16_t k_t_pressed = TIMER_RATE_16(a_t, l_t, 5, 3);
                        uint16_t k_t_release = TIMER_RATE_16(a_t, l_t, 5, 4);
                        udprintln("nnnnnn");

                        keycode_event_action(KC_BSPC, bs_t_pressed, bs_t_release);
                        keycode_event_action(KC_K, k_t_pressed, k_t_release);
                        action_exec(action_event);
                        return true;
                    }

                    case KC_L: {
                        keyevent_t last2_event = hist_que[hist_que_last - 1];
                        switch (ktk(last2_event.key)) {
                            case KC_T: {
                                switch (current_keycode) {
                                    case KC_I: // tli > texi
                                    {
                                        uint16_t bs_t_pressed = TIMER_RATE_16(a_t, l_t, 7, 1);
                                        uint16_t bs_t_release = TIMER_RATE_16(a_t, l_t, 7, 2);
                                        uint16_t e_t_pressed = TIMER_RATE_16(a_t, l_t, 7, 3);
                                        uint16_t e_t_release = TIMER_RATE_16(a_t, l_t, 7, 4);
                                        uint16_t x_t_pressed = TIMER_RATE_16(a_t, l_t, 7, 5);
                                        uint16_t x_t_release = TIMER_RATE_16(a_t, l_t, 7, 6);

                                        keycode_event_action(KC_BSPC, bs_t_pressed, bs_t_release);
                                        keycode_event_action(KC_E, e_t_pressed, e_t_release);
                                        keycode_event_action(KC_X, x_t_pressed, x_t_release);
                                        action_exec(action_event);
                                        return true;
                                    }
                                    case KC_U: // tlu > toxu
                                    {
                                        uint16_t bs_t_pressed = TIMER_RATE_16(a_t, l_t, 7, 1);
                                        uint16_t bs_t_release = TIMER_RATE_16(a_t, l_t, 7, 2);
                                        uint16_t o_t_pressed = TIMER_RATE_16(a_t, l_t, 7, 3);
                                        uint16_t o_t_release = TIMER_RATE_16(a_t, l_t, 7, 4);
                                        uint16_t x_t_pressed = TIMER_RATE_16(a_t, l_t, 7, 5);
                                        uint16_t x_t_release = TIMER_RATE_16(a_t, l_t, 7, 6);

                                        keycode_event_action(KC_BSPC, bs_t_pressed, bs_t_release);
                                        keycode_event_action(KC_O, o_t_pressed, o_t_release);
                                        keycode_event_action(KC_X, x_t_pressed, x_t_release);
                                        action_exec(action_event);
                                        return true;
                                    }
                                    default:
                                        return false;
                                }
                            }
                            default: // ex) la > za
                            {
                                uint16_t bs_t_pressed = TIMER_RATE_16(a_t, l_t, 5, 1);
                                uint16_t bs_t_release = TIMER_RATE_16(a_t, l_t, 5, 2);
                                uint16_t z_t_pressed = TIMER_RATE_16(a_t, l_t, 5, 3);
                                uint16_t z_t_release = TIMER_RATE_16(a_t, l_t, 5, 4);

                                keycode_event_action(KC_BSPC, bs_t_pressed, bs_t_release);
                                keycode_event_action(KC_Z, z_t_pressed, z_t_release);
                                action_exec(action_event);
                                return true;
                            }
                        }
                    }
                    default:
                        return false;
                }
            }
        }
    }
    return false;
}

void keyboard_task(void) {
//    udprint("=== keyboard_task start === ");
//    udprintf("time: %u\n",timer_read() | 1);
    static keyevent_t event_que[QUE_SIZE];
    static keyevent_t hist_que[QUE_SIZE];
    static bool is_ime_on;
    static uint8_t event_que_head = 0;
    static uint8_t event_que_num = 0;
    static uint8_t hist_que_head = 0;
    static uint8_t hist_que_num = 0;
//    udprintf("event_que_head: %u\n",event_que_head);

//    que_print(event_que,"declaration",&event_que_head,&event_que_num);


    static matrix_row_t matrix_prev[MATRIX_ROWS];
#ifdef MATRIX_HAS_GHOST
    //  static matrix_row_t matrix_ghost[MATRIX_ROWS];
#endif
    static uint8_t led_status = 0;
    matrix_row_t matrix_row = 0;
    matrix_row_t matrix_change = 0;
#ifdef QMK_KEYS_PER_SCAN
    uint8_t keys_processed = 0;
#endif

    matrix_scan();
    if (is_keyboard_master()) {
        for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
            matrix_row = matrix_get_row(r);
            matrix_change = matrix_row ^ matrix_prev[r];
            if (matrix_change) {
#ifdef MATRIX_HAS_GHOST
                if (has_ghost_in_row(r, matrix_row)) {
                    /* Keep track of whether ghosted status has changed for
                    * debugging. But don't update matrix_prev until un-ghosted, or
                    * the last key would be lost.
                    */
                    //if (debug_matrix && matrix_ghost[r] != matrix_row) {
                    //    matrix_print();
                    //}
                    //matrix_ghost[r] = matrix_row;
                    continue;
                }
                //matrix_ghost[r] = matrix_row;
#endif
                if (debug_matrix) matrix_print();
                for (uint8_t c = 0; c < MATRIX_COLS; c++) {
                    if (matrix_change & ((matrix_row_t) 1 << c)) {

//                        udprintln("enter action_exec");
//                        keypos_t key = (keypos_t){ .row = r, .col = c };
//                        uint16_t keycode = keymap_key_to_keycode(layer_switch_get_layer(key), key);
//                        udprintf("keycode fantom: %d\n",keycode);

                        keyevent_t current_event = (keyevent_t) {
                                .key = (keypos_t) {.row = r, .col = c},
                                .pressed = (matrix_row & ((matrix_row_t) 1 << c)),
                                .time = (timer_read() | 1) /* time should not be 0 */
                        };
                        enque(event_que, current_event, &event_que_head, &event_que_num) ? udprintf("enque ok. t: %u\n",
                                                                                                  (timer_read() | 1))
                                                                                       : udprintln(
                                "enque ng");
//                        que_print(event_que, "after enque", &event_que_head, &event_que_num);

                        // record a processed key
                        matrix_prev[r] ^= ((matrix_row_t) 1 << c);
#ifdef QMK_KEYS_PER_SCAN
                        // only jump out if we have processed "enough" keys.
                        if (++keys_processed >= QMK_KEYS_PER_SCAN)
#endif
                        // process a key per task call
//                        goto MATRIX_LOOP_END;
                    }
                }
            }
        }
    }
    // call with pseudo tick event when no real key event.
#ifdef QMK_KEYS_PER_SCAN
    // we can get here with some keys processed now.
    if (!keys_processed)
#endif
//    MATRIX_LOOP_END:


    keyevent_t current_que_head = read_que_head(event_que, &event_que_head, &event_que_num);
//    static uint8_t prev_que_head_key_col = 0;
//    if(current_que_head.key.col != prev_que_head_key_col)
//            udprintf("current_que_head.key.col: %u\n",current_que_head.key.col);
//    prev_que_head_key_col = current_que_head.key.col;
    if (current_que_head.key.col != TICK.key.col){ //deque_event != TICK
        if (TIMER_DIFF_16((timer_read() | 1), current_que_head.time) > 10) {
//            que_print(event_que,"after 10msec check.",&event_que_head,&event_que_num);
//            udprintf("current_que_head.time: %u\n", current_que_head.time);
//            udprintf("current time.   : %u\n", (timer_read() | 1));

            keyevent_t action_event = deque(event_que, &event_que_head, &event_que_num);

            //convert action_event
            udprint("\n=== enter is_convert_action_event ===\n");
            if (!is_convert_action_event(action_event, is_ime_on, hist_que, &hist_que_head, &hist_que_num))
                action_exec(action_event);
            udprint("=====================================\n\n");

            if (action_event.pressed) {
                switch (ktk(action_event.key)) {
                    case KC_A ... KC_Z:
                        enque(hist_que, action_event, &hist_que_head, &hist_que_num);
                        break;

                    case KC_BSPC:
                        unenque(hist_que, &hist_que_head, &hist_que_num);
                        break;

                    case KC_ZH:
                        if (is_ime_on) is_ime_on = false;
                        else is_ime_on = true;
                        que_clear(&hist_que_head, &hist_que_num);
                        break;

                    case KC_CAPSLOCK:
                    case JP_KANA:
                        is_ime_on = true;
                        que_clear(&hist_que_head, &hist_que_num);
                        break;

                    default:
                        que_clear(&hist_que_head, &hist_que_num);
                        break;
                }
            }
        }
    }
    else {
        action_exec(TICK);
    }



#ifdef MOUSEKEY_ENABLE
    // mousekey repeat & acceleration
    mousekey_task();
#endif

#ifdef PS2_MOUSE_ENABLE
    ps2_mouse_task();
#endif

#ifdef SERIAL_MOUSE_ENABLE
    serial_mouse_task();
#endif

#ifdef ADB_MOUSE_ENABLE
    adb_mouse_task();
#endif

#ifdef SERIAL_LINK_ENABLE
    serial_link_update();
#endif

#ifdef VISUALIZER_ENABLE
    visualizer_update(default_layer_state, layer_state, visualizer_get_mods(), host_keyboard_leds());
#endif

#ifdef POINTING_DEVICE_ENABLE
    pointing_device_task();
#endif

#ifdef MIDI_ENABLE
    midi_task();
#endif

    // update LED
    if (led_status != host_keyboard_leds()) {
        led_status = host_keyboard_leds();
        keyboard_set_leds(led_status);
    }

//    udprintln("=== keyboard_task end ===\n");
}

/** \brief keyboard set leds
 *
 * FIXME: needs doc
 */
void keyboard_set_leds(uint8_t leds) {
    if (debug_keyboard) {
        debug("keyboard_set_led: ");
        debug_hex8(leds);
        debug("\n");
    }
    led_set(leds);
}