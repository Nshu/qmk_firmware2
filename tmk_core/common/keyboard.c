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
#include <string.h>
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

#define EVENT_QUE_HOLD_TIME 20
#define QUE_SIZE 8
typedef keyevent_t data_t;

void swap_element_in_array(data_t *array, uint8_t index1, uint8_t index2){
    data_t tmp = array[index1];
    array[index1] = array[index2];
    array[index2] = tmp;
}

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

data_t read_que_from_last(data_t que_data[QUE_SIZE], uint8_t *que_head, uint8_t *que_num, uint8_t from_last) {
    if (*que_num > 0) {
        uint8_t virtual_que_last_index = *que_head + *que_num - 1;
        uint8_t virtual_que_index = virtual_que_last_index - from_last;
        if(virtual_que_index < 0) return TICK;
        uint8_t real_que_index = virtual_que_index % QUE_SIZE;
        return que_data[real_que_index];
    } else {
        return TICK;
    }
}

void que_clear(uint8_t *que_head, uint8_t *que_num) {
    *que_head = 0;
    *que_num = 0;
}

void swap_element_in_que(data_t que_data[QUE_SIZE], uint8_t v_index1, uint8_t v_index2){
    uint8_t r_index1 = v_index1 % QUE_SIZE;
    uint8_t r_index2 = v_index2 % QUE_SIZE;
    swap_element_in_array(que_data,r_index1,r_index2);
}

uint16_t ktk(keypos_t key) {
    return keymap_key_to_keycode(layer_switch_get_layer(key), key);
}

void keycode_val_to_name(uint8_t keycode, char *keycode_name){
    static const char keycode_val_to_name[][16] = {
            "KC_NO",
            "KC_ROLL_OVER",
            "KC_POST_FAIL",
            "KC_UNDEFINED",
            "KC_A",
            "KC_B",
            "KC_C",
            "KC_D",
            "KC_E",
            "KC_F",
            "KC_G",
            "KC_H",
            "KC_I",
            "KC_J",
            "KC_K",
            "KC_L",
            "KC_M",
            "KC_N",
            "KC_O",
            "KC_P",
            "KC_Q",
            "KC_R",
            "KC_S",
            "KC_T",
            "KC_U",
            "KC_V",
            "KC_W",
            "KC_X",
            "KC_Y",
            "KC_Z",
            "KC_1",
            "KC_2",
            "KC_3",
            "KC_4",
            "KC_5",
            "KC_6",
            "KC_7",
            "KC_8",
            "KC_9",
            "KC_0",
            "KC_ENTER",
            "KC_ESCAPE",
            "KC_BSPACE",
            "KC_TAB",
            "KC_SPACE",
            "KC_MINUS",
            "KC_EQUAL",
            "KC_LBRACKET",
            "KC_RBRACKET",
            "KC_BSLASH",
            "KC_NONUS_HASH",
            "KC_SCOLON",
            "KC_QUOTE",
            "KC_GRAVE",
            "KC_COMMA",
            "KC_DOT",
            "KC_SLASH",
            "KC_CAPSLOCK",
            "KC_F1",
            "KC_F2",
            "KC_F3",
            "KC_F4",
            "KC_F5",
            "KC_F6",
            "KC_F7",
            "KC_F8",
            "KC_F9",
            "KC_F10",
            "KC_F11",
            "KC_F12",
            "KC_PSCREEN",
            "KC_SCROLLLOCK",
            "KC_PAUSE",
            "KC_INSERT",
            "KC_HOME",
            "KC_PGUP",
            "KC_DELETE",
            "KC_END",
            "KC_PGDOWN",
            "KC_RIGHT",
            "KC_LEFT",
            "KC_DOWN",
            "KC_UP",
            "KC_NUMLOCK",
    };
    if(keycode <= 83){
        strcpy(keycode_name,keycode_val_to_name[keycode]);
    }
    else{
        strcpy(keycode_name,"____");
    }
}

void que_print(data_t que_data[QUE_SIZE], char *str, uint8_t *que_head, uint8_t *que_num) {
    udprintf("=== que_print === %32s\n", str);
//    udprintf("*que_head: %d\t", *que_head);
//    udprintf("*que_num: %d\n", *que_num);

//    udprint("real_que keycode:[");
//    for (uint8_t i = 0; i < QUE_SIZE; i++) {
//        if (i != 0) udprint(",");
//        char keycode_name[17];
//        uint8_t current_keycode = ktk(que_data[i].key);
//        keycode_val_to_name(current_keycode,keycode_name);
//        udprintf(" %s", keycode_name);
//    }
//    udprint("]\n");
//    udprint("real_que time:[");
//    for (uint8_t i = 0; i < QUE_SIZE; i++) {
//        if (i != 0) udprint(",");
//        udprintf(" %u", que_data[i].time);
//    }
//    udprint("]\n");

    udprint("virtual_que keycode:[");
    for(uint8_t i = *que_head; i < *que_head + *que_num; i++){
        if (i != *que_head) udprint(",");
        char keycode_name[17];
        uint8_t current_keycode = ktk(que_data[i % QUE_SIZE].key);
        keycode_val_to_name(current_keycode,keycode_name);
        udprintf(" %s", keycode_name);
    }
    udprint("]\n");
    udprint("virtual_que time:[");
    for(uint8_t i = *que_head; i < *que_head + *que_num; i++){
        if (i != *que_head) udprint(",");
        udprintf(" %u", que_data[i % QUE_SIZE].time);
    }
    udprint("]\n");

    udprintln("================");
}

#define atk(keycode)    alphabet_to_keypos[keycode - 0x04]
#define bspc_keypos     alphabet_to_keypos[26]

void action_exec_by_keycode(uint8_t keycode, uint16_t pressed_time, uint16_t release_time) {
    // [0]=kc_a's keypos ... [25]= kc_z's keypos [26] = kc_backspace's keypos
    static keypos_t alphabet_to_keypos[27] = {};
    if (alphabet_to_keypos[0].row == 0) {
        for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
            for (uint8_t c = 0; c < MATRIX_COLS; c++) {
                keypos_t current_keypos = {
                        .row = r,
                        .col = c
                };
                uint8_t current_keycode = pgm_read_word(&keymaps[0][r][c]);
//                udprintf("r = %2u. c = %2u, keycode = %3u\n",r,c,current_keycode);
                switch (current_keycode) {
                    case KC_A ... KC_Z:
                        // kc_a = 0x04 ... kc_z = 0x17
                        alphabet_to_keypos[current_keycode - 0x04] = current_keypos;
                        break;
                    case KC_BSPC:
                        alphabet_to_keypos[26] = current_keypos;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    keypos_t keypos = TICK.key;
    switch (keycode) {
        case KC_A ... KC_Z:
            keypos = atk(keycode);
            break;
        case KC_BSPC:
            keypos = alphabet_to_keypos[26];
            break;
        default:
            break;
    }
//    udprintln("----------------------------");
//    for(uint8_t i=0; i<25; i++){
//        udprintf("alphabet_to_keypos[%2d] : r = %u, c = %u\n",i,alphabet_to_keypos[i].row,alphabet_to_keypos[i].col);
//    }
//    udprintln("----------------------------");
//    udprintv(keypos.col,%u);
//    udprintv(keypos.row,%u);
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

void action_exec_by_series_keycode(uint8_t *keycode, uint8_t num_of_keycode, uint16_t last_event_time,
                                   uint16_t current_event_time) {
    udprintln("=== enter action_exec_by_series_keycode ===");
    uint16_t pressed_times[num_of_keycode];
    uint16_t release_times[num_of_keycode];

    uint16_t time_range_divider = num_of_keycode * 2 + 1;
    for (uint8_t i = 0; i < num_of_keycode * 2; i++) {
        if (i % 2 == 1) { // odd number
            uint8_t index_for_odd = (i - 1) / 2;
            pressed_times[index_for_odd] = TIMER_RATE_16(current_event_time, last_event_time, time_range_divider, i);
        } else { // even number
            uint8_t index_for_even = i / 2 - 1;
            release_times[index_for_even] = TIMER_RATE_16(current_event_time, last_event_time, time_range_divider, i);
        }
    }

    udprintvln(num_of_keycode,%u);
    for (uint8_t i = 0; i < num_of_keycode; i++) {
        udprintvln(keycode[i],%u);
        udprintvln(pressed_times[i],%u);
        udprintvln(release_times[i],%u);
        action_exec_by_keycode(keycode[i], pressed_times[i], release_times[i]);
    }
    udprintln("=== exit action_exec_by_series_keycode ===");
}

// converted > return true
bool is_convert_action_event(keyevent_t action_event, bool is_ime_on, keyevent_t *hist_que, uint8_t *hist_que_head,
                             uint8_t *hist_que_num) {
    udprintf("ime: %d\n", is_ime_on);
    udprintf("action_event.pressed: %d\n", action_event.pressed);
    if (is_ime_on) {
        if (action_event.pressed) {
            uint8_t current_keycode = ktk(action_event.key);
            udprintvln(current_keycode, %u);

            keyevent_t last_event = read_que_from_last(hist_que, hist_que_head, hist_que_num, 0);

            uint16_t l_t = last_event.time;
            uint16_t a_t = action_event.time;

            switch (current_keycode) {
                case KC_A:
                case KC_I:
                case KC_U:
                case KC_E:
                case KC_O: {
                    switch (ktk(last_event.key)) {
                        case KC_C: {
                            keyevent_t last2_event = read_que_from_last(hist_que, hist_que_head, hist_que_num, 1);
                            switch (ktk(last2_event.key)) {
                                case KC_C: // ex) cca > kka
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_BSPC,
                                            KC_K,
                                            KC_K,
                                    };
                                    action_exec_by_series_keycode(keycode_series, 4, l_t, a_t);
                                    action_exec(action_event);
                                    return true;
                                }
                                default: // ex) ca > ka
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_K,
                                    };
                                    action_exec_by_series_keycode(keycode_series, 2, l_t, a_t);
                                    action_exec(action_event);
                                    return true;
                                }
                            }
                        }
                        case KC_L: {
                            keyevent_t last2_event = read_que_from_last(hist_que, hist_que_head, hist_que_num, 1);
                            switch (ktk(last2_event.key)) {
                                case KC_T: {
                                    switch (current_keycode) {
                                        case KC_I: // tli > texi
                                        {
                                            uint8_t keycode_series[] = {
                                                    KC_BSPC,
                                                    KC_E,
                                                    KC_X
                                            };
                                            action_exec_by_series_keycode(keycode_series, 3, l_t, a_t);
                                            action_exec(action_event);
                                            return true;
                                        }
                                        case KC_U: // tlu > toxu
                                        {
                                            uint8_t keycode_series[] = {
                                                    KC_BSPC,
                                                    KC_O,
                                                    KC_X
                                            };
                                            action_exec_by_series_keycode(keycode_series, 3, l_t, a_t);
                                            action_exec(action_event);
                                            return true;
                                        }
                                        default:
                                            return false;
                                    }
                                }
                                case KC_D: {
                                    switch (current_keycode) {
                                        case KC_I: { // dli > dexi
                                            uint8_t keycode_series[] = {
                                                    KC_BSPC,
                                                    KC_E,
                                                    KC_X
                                            };
                                            action_exec_by_series_keycode(keycode_series, 3, l_t, a_t);
                                            action_exec(action_event);
                                            return true;
                                        }
                                        default:
                                            return false;
                                    }
                                }
                                case KC_L: // ex) lla > zza
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_BSPC,
                                            KC_Z,
                                            KC_Z,
                                    };
                                    action_exec_by_series_keycode(keycode_series, 4, l_t, a_t);
                                    action_exec(action_event);
                                    return true;
                                }

                                default: // ex) la > za
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_Z
                                    };
                                    action_exec_by_series_keycode(keycode_series, 2, l_t, a_t);
                                    action_exec(action_event);
                                    return true;
                                }
                            }
                        }
                        default:
                            return false;
                    }
                }
                case KC_Z: // z > ya
                {
                    switch (ktk(last_event.key)) {
                        case KC_C: {
                            keyevent_t last2_event = read_que_from_last(hist_que, hist_que_head, hist_que_num, 1);
                            switch (ktk(last2_event.key)) {
                                case KC_C: // ex) ccz > kkya
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_BSPC,
                                            KC_K,
                                            KC_K,
                                            KC_Y,
                                            KC_A
                                    };
                                    action_exec_by_series_keycode(keycode_series, 6, l_t, a_t);
                                    return true;
                                }
                                default: // ex) cz > kya
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_K,
                                            KC_Y,
                                            KC_A
                                    };
                                    action_exec_by_series_keycode(keycode_series, 4, l_t, a_t);
                                    return true;
                                }
                            }
                        }
                        case KC_L: {
                            keyevent_t last2_event = read_que_from_last(hist_que, hist_que_head, hist_que_num, 1);
                            switch (ktk(last2_event.key)) {
                                case KC_L: // ex) llz > zzya
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_BSPC,
                                            KC_Z,
                                            KC_Z,
                                            KC_Y,
                                            KC_A
                                    };
                                    action_exec_by_series_keycode(keycode_series, 6, l_t, a_t);
                                    return true;
                                }
                                default: // ex) lz > zya
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_Z,
                                            KC_Y,
                                            KC_A
                                    };
                                    action_exec_by_series_keycode(keycode_series, 4, l_t, a_t);
                                    return true;
                                }
                            }
                        }
                        default: {
                            uint8_t keycode_series[] = {
                                    KC_Y,
                                    KC_A
                            };
                            action_exec_by_series_keycode(keycode_series, 2, l_t, a_t);
                            return true;
                        }
                    }

                }
                case KC_K: // k > yu
                {
                    switch (ktk(last_event.key)) {
                        case KC_C: {
                            keyevent_t last2_event = read_que_from_last(hist_que, hist_que_head, hist_que_num, 1);
                            switch (ktk(last2_event.key)) {
                                case KC_C: // ex) cck > kkyu
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_BSPC,
                                            KC_K,
                                            KC_K,
                                            KC_Y,
                                            KC_U
                                    };
                                    action_exec_by_series_keycode(keycode_series, 6, l_t, a_t);
                                    return true;
                                }
                                default: // ex) ck > kyu
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_K,
                                            KC_Y,
                                            KC_U
                                    };
                                    action_exec_by_series_keycode(keycode_series, 4, l_t, a_t);
                                    return true;
                                }
                            }
                        }
                        case KC_L: {
                            keyevent_t last2_event = read_que_from_last(hist_que, hist_que_head, hist_que_num, 1);
                            switch (ktk(last2_event.key)) {
                                case KC_L: // ex) llk > zzyu
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_BSPC,
                                            KC_Z,
                                            KC_Z,
                                            KC_Y,
                                            KC_U
                                    };
                                    action_exec_by_series_keycode(keycode_series, 6, l_t, a_t);
                                    return true;
                                }
                                default: // ex) lk > zyu
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_Z,
                                            KC_Y,
                                            KC_U
                                    };
                                    action_exec_by_series_keycode(keycode_series, 2, l_t, a_t);
                                    return true;
                                }
                            }
                        }
                        default: {
                            uint8_t keycode_series[] = {
                                    KC_Y,
                                    KC_U
                            };
                            action_exec_by_series_keycode(keycode_series, 2, l_t, a_t);
                            return true;
                        }
                    }

                }
                case KC_Q: // q > yo
                {
                    switch (ktk(last_event.key)) {
                        case KC_C: {
                            keyevent_t last2_event = read_que_from_last(hist_que, hist_que_head, hist_que_num, 1);
                            switch (ktk(last2_event.key)) {
                                case KC_C: // ex) ccq > kkyo
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_BSPC,
                                            KC_K,
                                            KC_K,
                                            KC_Y,
                                            KC_O
                                    };
                                    action_exec_by_series_keycode(keycode_series, 6, l_t, a_t);
                                    return true;
                                }
                                default: // ex) cq > kyo
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_K,
                                            KC_Y,
                                            KC_O
                                    };
                                    action_exec_by_series_keycode(keycode_series, 4, l_t, a_t);
                                    return true;
                                }
                            }
                        }
                        case KC_L: {
                            keyevent_t last2_event = read_que_from_last(hist_que, hist_que_head, hist_que_num, 1);
                            switch (ktk(last2_event.key)) {
                                case KC_L: // ex) llq > zzyo
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_BSPC,
                                            KC_Z,
                                            KC_Z,
                                            KC_Y,
                                            KC_O
                                    };
                                    action_exec_by_series_keycode(keycode_series, 6, l_t, a_t);
                                    return true;
                                }
                                default: // ex) lq > zyo
                                {
                                    uint8_t keycode_series[] = {
                                            KC_BSPC,
                                            KC_Z,
                                            KC_Y,
                                            KC_O
                                    };
                                    action_exec_by_series_keycode(keycode_series, 2, l_t, a_t);
                                    return true;
                                }
                            }
                        }
                        default: {
                            uint8_t keycode_series[] = {
                                    KC_Y,
                                    KC_O
                            };
                            action_exec_by_series_keycode(keycode_series, 2, l_t, a_t);
                            return true;
                        }
                    }

                }
                default:
                    return false;

            }
        }
    }
    return false;
}

void unenque_zenkaku(data_t que_data[QUE_SIZE], uint8_t *que_head, uint8_t *que_num) {
    if (*que_num > 0) {
        keyevent_t last_event = read_que_from_last(que_data, que_head, que_num, 0);
        switch (ktk(last_event.key)) {
            case KC_A:
            case KC_I:
            case KC_U:
            case KC_E:
            case KC_O:
            case KC_Z:
            case KC_K:
            case KC_X:
                que_clear(que_head, que_num);
            default:
                unenque(que_data, que_head, que_num);
        }
    }
}

bool is_in_prefix_key(int keycode){
    switch(keycode){
        case MO(0) ... MO(2):
        case KC_LCTRL ... KC_RGUI:
            return true;

        default:
            return false;
    }
}

bool is_ime_on = false;

void keyboard_task(void) {
//    udprint("=== keyboard_task start === ");
//    udprintf("time: %u\n",timer_read() | 1);
    static keyevent_t event_que[QUE_SIZE];
    static keyevent_t hist_que[QUE_SIZE];
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
                        enque(event_que, current_event, &event_que_head, &event_que_num) ? udprintf(
                                "\nenque ok. t: %u, pressed: %d\n",
                                (timer_read() | 1),current_event.pressed)
                                                                                         : udprintln(
                                "enque ng");
                        que_print(event_que, "event_que after enque", &event_que_head, &event_que_num);

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
    if (current_que_head.key.col != TICK.key.col){ //deque_event != TICK
        if (TIMER_DIFF_16((timer_read() | 1), current_que_head.time) > EVENT_QUE_HOLD_TIME) {
            keyevent_t action_event = deque(event_que, &event_que_head, &event_que_num);

//            udprint("\n=== enter is_convert_action_event ===\n");
            if (!is_convert_action_event(action_event, is_ime_on, hist_que, &hist_que_head, &hist_que_num)) {
//                que_print(hist_que, "hist_que after convert", &hist_que_head, &hist_que_num);
                action_exec(action_event);
            }
//            udprint("=====================================\n\n");

            if (action_event.pressed) {
                switch (ktk(action_event.key)) {
                    case KC_A ... KC_Z:
                        if (!enque(hist_que, action_event, &hist_que_head, &hist_que_num)) {
                            deque(hist_que, &hist_que_head, &hist_que_num);
                            enque(hist_que, action_event, &hist_que_head, &hist_que_num);
                        }
                        break;
                    case KC_BSPC:
                        if (is_ime_on) {
                            unenque_zenkaku(hist_que, &hist_que_head, &hist_que_num);
                            break;
                        }
                        unenque(hist_que, &hist_que_head, &hist_que_num);
                        break;
                    case KC_CAPSLOCK:
                    case JP_KANA:
                    case JP_HENK:
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