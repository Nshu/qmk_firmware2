#include QMK_KEYBOARD_H
#include "debug.h"
#include "action_layer.h"
#include "version.h"
#include "keymap_JIS.h"
#include "keymap_jp.h"
#include "macro_keycode.h"

//Assin keys for using as a US keyboard here, and convert US key to JIS key by keymap_JIS.h, 
const uint16_t PROGMEM
keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[0] =
LAYOUT_ergodox(

        DF(0), KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, DF(4),
        KC_F1, KC_LEFT, KC_BSPC, KC_DEL, KC_P, KC_Y, LCTL(KC_C),
        KC_SPC, KC_A, KC_O, KC_E, KC_U, KC_I,
        KC_ENT, KC_Z, KC_Q, KC_J, KC_K, KC_X, LCTL(KC_V),
        KC_LGUI, KC_DOWN, KC_UP, KC_LSFT, KC_LCTL, //KC_MS_L, KC_MS_D,
        IME_FILTER_OFF, KC_NO,
        KC_NO,
        MO(1), IME_OFF, KC_LALT,

        KC_NO, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_NO,
        KC_NO, KC_F, KC_G, KC_C, KC_R, KC_RGHT, KC_F12,
        KC_D, KC_H, KC_T, KC_N, KC_S, KC_ESC,
        KC_NO, KC_B, KC_M, KC_W, KC_V, KC_L, KC_TAB,
        KC_RCTL, KC_RSFT, JP_MHEN, JP_HENK, KC_RGUI,
        KC_NO, KC_NO,
        KC_NO,
        KC_RALT, IME_ON, MO(2)
),
[1] =
LAYOUT_ergodox(

        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, JA_HASH, JA_GRV, LCTL(KC_INS),
        KC_TRNS, JA_BSLS, JA_DOT , JA_COLN, BACKL  , KC_5   ,
KC_TRNS, KC_1   , KC_2   , KC_3   , KC_4   , KC_LCTL, S(KC_INS),
KC_TRNS, KC_TRNS, KC_TRNS, S(KC_CAPS), KC_WH_D,
        KC_TRNS, KC_TRNS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS,

        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, JA_AT, JA_PLUS, JA_ASTR, JA_PERC, KC_TRNS, KC_TRNS,
        KC_6, BACKL, JA_EQL, JA_MINS, JA_SLSH, KC_TRNS,
        KC_TRNS, KC_RCTL, KC_7, KC_8, KC_9, KC_0, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_NO, KC_NO, KC_TRNS,
        KC_TRNS, KC_TRNS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS
),
[2] =
LAYOUT_ergodox(

        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, JA_EXLM, KC_LALT, KC_TRNS,
        KC_TRNS, JA_LCBR, JA_LBRC, JA_LPRN, BACKL, JA_LT,
        KC_TRNS, JA_CIRC, JA_DLR, JA_TILD, JA_PIPE, KC_LCTL, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS,

        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_RALT, JA_QUES, JA_DQT, JA_QUOT, KC_NO, KC_TRNS,
        JA_GT, BACKL, JA_SCLN, JA_COMM, JA_UNDS, KC_TRNS,
        KC_TRNS, KC_RCTL, KC_AMPR, JA_RPRN, JA_RBRC, JA_RCBR, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS
),
[3] =
LAYOUT_ergodox(

        KC_NO, LCTL(KC_B), LCTL(LALT(KC_B)), LCTL(S(KC_B)), KC_NO, KC_NO, KC_NO,
        KC_NO, LCTL(KC_F), LCTL(S(KC_F)), KC_ESC, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_BTN3, KC_RSFT,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO,
        KC_NO,
        KC_NO, KC_NO, KC_NO,

        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO,
        KC_NO,
        KC_NO, KC_NO, KC_NO
),
//for US
[4] =
LAYOUT_ergodox(

        DF(0), KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, DF(4),
        KC_F1, KC_LEFT, KC_BSPC, KC_DEL, KC_P, KC_Y, LCTL(KC_C),
        KC_SPC, KC_A, KC_O, KC_E, KC_U, KC_I,
        KC_ENT, KC_Z, KC_Q, KC_J, KC_K, KC_X, LCTL(KC_V),
        KC_LGUI, KC_DOWN, KC_UP, KC_LSFT, KC_LCTL, //KC_MS_L, KC_MS_D,
        IME_FILTER_OFF, KC_NO,
        KC_NO,
        MO(5), IME_OFF, KC_LALT,

        KC_NO, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_NO,
        KC_NO, KC_F, KC_G, KC_C, KC_R, KC_RGHT, KC_F12,
        KC_D, KC_H, KC_T, KC_N, KC_S, KC_ESC,
        KC_NO, KC_B, KC_M, KC_W, KC_V, KC_L, KC_TAB,
        KC_RCTL, KC_RSFT, JP_MHEN, JP_HENK, KC_RGUI,
        KC_NO, KC_NO,
        KC_NO,
        KC_RALT, IME_ON, MO(6)
),
[5] =
LAYOUT_ergodox(

        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_HASH, KC_GRV, LCTL(KC_INS),
        KC_TRNS, KC_BSLS, KC_DOT , KC_COLN, BACKL  , KC_5   ,
KC_TRNS, KC_1   , KC_2   , KC_3   , KC_4   , KC_LCTL, S(KC_INS),
        KC_TRNS, KC_TRNS, KC_TRNS,
        S(KC_CAPS), KC_WH_D,
        KC_TRNS, KC_TRNS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS,

        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_AT, KC_PLUS, KC_ASTR, KC_PERC, KC_TRNS, KC_TRNS,
        KC_6, BACKL, KC_EQL, KC_MINS, KC_SLSH, KC_TRNS,
        KC_TRNS, KC_RCTL, KC_7, KC_8, KC_9, KC_0, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_NO, KC_NO, KC_TRNS,
        KC_TRNS, KC_TRNS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS
),
[6] =
LAYOUT_ergodox(

        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_EXLM, KC_LALT, KC_TRNS,
        KC_TRNS, KC_LCBR, KC_LBRC, KC_LPRN, BACKL, KC_LT,
        KC_TRNS, KC_CIRC, KC_DLR, KC_TILD, KC_PIPE, KC_LCTL, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS,

        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_RALT, KC_QUES, KC_DQT, KC_QUOT, KC_NO, KC_TRNS,
        KC_GT, BACKL, KC_SCLN, KC_COMM, KC_UNDS, KC_TRNS,
        KC_TRNS, KC_RCTL, KC_AMPR, KC_RPRN, KC_RBRC, KC_RCBR, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS,
        KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS
),


};

const uint16_t PROGMEM
fn_actions[] = {
[1] = ACTION_LAYER_TAP_TOGGLE(1)                // FN1 - Momentary Layer 1 (Symbols)
};

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt) {
    // MACRODOWN only works in this function
    switch (id) {
        case 0:
            if (record->event.pressed) {
                SEND_STRING(QMK_KEYBOARD
                "/"
                QMK_KEYMAP
                " @ "
                QMK_VERSION);
            }
            break;
        case 1:
            if (record->event.pressed) { // For resetting EEPROM
                eeconfig_init();
            }
            break;
    }
    return MACRO_NONE;
};

extern bool is_ime_on;

void ime_off(){
    // X_INT2 = X_KANA  IME on
    SEND_STRING(SS_TAP(X_INT2));
    // X_GRAVE = X_ZHTG IME toggle = off
    SEND_STRING(SS_TAP(X_GRAVE));
    is_ime_on = false;
}
void ime_on(){
    // X_INT2 = X_KANA  IME on
    SEND_STRING(SS_TAP(X_INT2));
    is_ime_on = true;
}
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static bool is_IME_OFF_key_pressing;
    static bool is_IME_ON_key_pressing;

    switch (keycode) {
        // dynamically generate these.
        case EPRM:
            if (record->event.pressed) {
                eeconfig_init();
            }
            return false;
        case VRSN:
            if (record->event.pressed) {
                SEND_STRING(QMK_KEYBOARD
                "/"
                QMK_KEYMAP
                " @ "
                QMK_VERSION);
            }
            return false;
        case RGB_SLD:
            if (record->event.pressed) {
#ifdef RGBLIGHT_ENABLE
                rgblight_mode(1);
#endif
            }
            return false;
        case IME_OFF:
            if (record->event.pressed) {
                is_IME_OFF_key_pressing = true;
                ime_off();

                if(is_IME_ON_key_pressing) { //IME_ON -> IME_OFF
                    SEND_STRING(SS_DOWN(X_LCTRL));
                }
            }
            else{
                is_IME_OFF_key_pressing = false;

                //release ctrl and shift
                SEND_STRING(SS_UP(X_LCTRL));
                SEND_STRING(SS_UP(X_LSHIFT));
            }
            udprintvln(is_ime_on,%d);
            udprintvln(is_IME_ON_key_pressing,%d);
            udprintvln(is_IME_OFF_key_pressing,%d);
            return false;
        case IME_ON:
            if (record->event.pressed) {
                is_IME_ON_key_pressing = true;
                ime_on();

                if(is_IME_OFF_key_pressing){ //IME_ON -> IME_ON
                    ime_off();
                    SEND_STRING(SS_DOWN(X_LSHIFT));
                }
            }
            else{
                is_IME_ON_key_pressing = false;

                //release ctrl and shift
                SEND_STRING(SS_UP(X_LCTRL));
                SEND_STRING(SS_UP(X_LSHIFT));
            }
            udprintvln(is_ime_on,%d);
            udprintvln(is_IME_ON_key_pressing,%d);
            udprintvln(is_IME_OFF_key_pressing,%d);
            return false;
        case IME_FILTER_OFF:
            if (record->event.pressed) {
                is_ime_on = false;
            }
            return false;
        case LAYER1:
            if (record->event.pressed) {
            }
            return false;
        case LAYER2:
            if (record->event.pressed) {
            }
            return false;
        case BACKL:
            if (record->event.pressed) {
            }
            return false;
    }
    return true;
}

// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {
#ifdef RGBLIGHT_COLOR_LAYER_0
    rgblight_setrgb(RGBLIGHT_COLOR_LAYER_0);
#endif
};

// Runs constantly in the background, in a loop.
void matrix_scan_user(void) {

};

// Runs whenever there is a layer state change.
uint32_t layer_state_set_user(uint32_t state) {
    ergodox_board_led_off();
    ergodox_right_led_1_off();
    ergodox_right_led_2_off();
    ergodox_right_led_3_off();

    uint8_t layer = biton32(state);
    switch (layer) {
        case 0:
#ifdef RGBLIGHT_COLOR_LAYER_0
            rgblight_setrgb(RGBLIGHT_COLOR_LAYER_0);
#else
#ifdef RGBLIGHT_ENABLE
            rgblight_init();
#endif
#endif
            break;
        case 1:
            ergodox_right_led_1_on();
#ifdef RGBLIGHT_COLOR_LAYER_1
            rgblight_setrgb(RGBLIGHT_COLOR_LAYER_1);
#endif
            break;
        case 2:
            ergodox_right_led_2_on();
#ifdef RGBLIGHT_COLOR_LAYER_2
            rgblight_setrgb(RGBLIGHT_COLOR_LAYER_2);
#endif
            break;
        case 3:
            ergodox_right_led_3_on();
#ifdef RGBLIGHT_COLOR_LAYER_3
            rgblight_setrgb(RGBLIGHT_COLOR_LAYER_3);
#endif
            break;
        case 4:
            ergodox_right_led_1_on();
            ergodox_right_led_2_on();
#ifdef RGBLIGHT_COLOR_LAYER_4
            rgblight_setrgb(RGBLIGHT_COLOR_LAYER_4);
#endif
            break;
        case 5:
            ergodox_right_led_1_on();
            ergodox_right_led_3_on();
#ifdef RGBLIGHT_COLOR_LAYER_5
            rgblight_setrgb(RGBLIGHT_COLOR_LAYER_5);
#endif
            break;
        case 6:
            ergodox_right_led_2_on();
            ergodox_right_led_3_on();
#ifdef RGBLIGHT_COLOR_LAYER_6
            rgblight_setrgb(RGBLIGHT_COLOR_LAYER_6);
#endif
            break;
        case 7:
            ergodox_right_led_1_on();
            ergodox_right_led_2_on();
            ergodox_right_led_3_on();
#ifdef RGBLIGHT_COLOR_LAYER_7
            rgblight_setrgb(RGBLIGHT_COLOR_LAYER_6);
#endif
            break;
        default:
            break;
    }

    return state;
};
