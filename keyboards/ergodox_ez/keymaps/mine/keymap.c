#include QMK_KEYBOARD_H
#include "debug.h"
#include "action_layer.h"
#include "version.h"
#include "keymap_JIS.h"

#define BASE 0 // default layer
#define SHFT 1 // symbols
#define SYMB 2 // symbols
#define SYM2 3 // media keys
#define SHCT 4 // short cut

#define KC_ZH 0x35

enum custom_keycodes {
  PLACEHOLDER = SAFE_RANGE, // can always be here
  EPRM,
  VRSN,
  RGB_SLD
};

//Assin keys for using as a US keyboard here, and convert US key to JIS key by keymap_JIS.h, 
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[BASE] = LAYOUT_ergodox(

        KC_NO  , KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  , KC_F6  ,
        KC_NO  , KC_INS , KC_BSPC, KC_DEL , KC_P   , KC_Y   , LCTL(KC_C),
        KC_HOME, KC_A   , KC_O   , KC_E   , KC_U   , KC_I   ,
LT(SYM2,KC_END), KC_Z   , KC_Q   , KC_J   , KC_K   , KC_X   , LCTL(KC_V),
        KC_LGUI, KC_LEFT, KC_DOWN, KC_UP  , KC_RGHT,   
                                                     KC_NO  , KC_NO  ,
                                                              RSFT(KC_D),
                                               LT(SYMB,KC_TAB),CTL_T(KC_ENT),SFT_T(KC_ZH),

                 KC_F7  , KC_F8  , KC_F9  , KC_F10 , KC_F11 , KC_F12 , KC_NO  , 
                 KC_NO  , KC_F   , KC_G   , KC_C   , KC_R   , KC_PSCR, KC_NO  , 
                          KC_D   , KC_H   , KC_T   , KC_N   , KC_S   , KC_PGUP,
                 KC_NO  , KC_B   , KC_M   , KC_W   , KC_V   , KC_L   , LT(SYM2,KC_PGDN),
                                   KC_MS_L, KC_MS_D, KC_MS_U, KC_MS_R, KC_RALT,
                 KC_NO  , KC_NO  ,
                 KC_NO  ,
                 KC_APP ,CTL_T(KC_SPC),LT(SHFT,KC_ESC)
),
[SHFT] = LAYOUT_ergodox(
        
        S(KC_NO  ), S(KC_F1  ), S(KC_F2  ), S(KC_F3  ), S(KC_F4  ), S(KC_F5  ), S(KC_F6  ),
        S(KC_NO  ), S(KC_INS ),   KC_TRNS ,   KC_TRNS , S(KC_P   ), S(KC_Y   ),   KC_TRNS ,
        S(KC_HOME), S(KC_A   ), S(KC_O   ), S(KC_E   ), S(KC_U   ), S(KC_I   ),
LT(SYM2,S(KC_END )),S(KC_Z   ), S(KC_Q   ), S(KC_J   ), S(KC_K   ), S(KC_X   ),   KC_TRNS ,
        S(KC_LGUI), S(KC_LEFT), S(KC_DOWN), S(KC_UP  ), S(KC_RGHT),
                                                        S(KC_NO  ), S(KC_NO  ),
                                                                    S(KC_NO  ),
                                           KC_TRNS,C_S_T(S(KC_ENT)),SFT_T(KC_ZH), 
                    S(KC_F7  ), S(KC_F8  ), S(KC_F9  ), S(KC_F10 ), S(KC_F11 ), S(KC_F12 ), S(KC_NO  ), 
                    S(KC_NO  ), S(KC_F   ), S(KC_G   ), S(KC_C   ), S(KC_R   ), S(KC_PSCR), S(KC_NO  ), 
                                S(KC_D   ), S(KC_H   ), S(KC_T   ), S(KC_N   ), S(KC_S   ), S(KC_PGUP),
                    S(KC_NO  ), S(KC_B   ), S(KC_M   ), S(KC_W   ), S(KC_V   ), S(KC_L   ), LT(SYM2,S(KC_PGDN)),
                                            S(KC_MS_L), S(KC_MS_D), S(KC_MS_U), S(KC_MS_R), S(KC_RALT),
                      KC_NO  ,   KC_NO  ,
                      KC_NO  ,
                      KC_NO  ,   KC_NO  ,   KC_TRNS
),
[SYMB] = LAYOUT_ergodox(
       
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
       KC_TRNS, KC_TRNS, KC_BSPC, KC_DEL , JA_DQT , JA_QUOT, LCTL(KC_T), 
       KC_TRNS, KC_1   , KC_2   , KC_3   , KC_4   , KC_5   , 
       KC_TRNS, JA_BSLS, JA_SLSH, JA_COLN, JA_DOT , KC_NO  , LCTL(KC_W), 
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
                                                    KC_NO  , KC_NO  , 
                                                             KC_NO  , 
                                           KC_TRNS, KC_NO  , KC_NO  ,
       
                KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
                KC_TRNS, JA_CIRC, JA_DLR , JA_LPRN, JA_RPRN, KC_TRNS, KC_TRNS,   
                         KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , KC_TRNS, 
                KC_TRNS, KC_NO  , JA_EQL , JA_TILD, JA_MINS, JA_UNDS, KC_TRNS, 
                                  KC_BTN1, KC_WH_D, KC_WH_U, KC_BTN2, KC_TRNS, 
                KC_TRNS, KC_TRNS,  
                KC_TRNS, 
                KC_TRNS, KC_TRNS, KC_ESC 
),
[SYM2] = LAYOUT_ergodox(
    
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
       KC_TRNS, LCTL(S(KC_O)), KC_TRNS, KC_TRNS, JA_EXLM, KC_NO  , KC_TRNS, 
       KC_TRNS, KC_NO  , JA_PERC, JA_ASTR, JA_PLUS, JA_AT  , 
       KC_TRNS, KC_NO  , JA_HASH, JA_SCLN, JA_COMM, KC_NO  , KC_TRNS, 
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
                                                    KC_TRNS, KC_TRNS, 
                                                             KC_TRNS,  
                                           KC_TRNS, KC_TRNS, KC_ZH  ,  
    
                KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
                KC_TRNS, KC_NO  , JA_QUES, JA_LCBR, JA_RCBR, KC_NO  , KC_TRNS, 
                         JA_GRV , JA_AMPR, JA_LBRC, JA_RBRC, KC_NO  , KC_TRNS,
                KC_TRNS, KC_NO  , JA_PIPE, JA_LT  , JA_GT  , KC_NO  , KC_TRNS, 
                                  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
                KC_TRNS, KC_TRNS, 
                KC_TRNS, 
                KC_TRNS, KC_TRNS, KC_ESC 
),
[SHCT] = LAYOUT_ergodox(
		
       KC_NO  , LCTL(KC_B),LCTL(LALT(KC_B)), LCTL(S(KC_B)), KC_NO  , KC_NO  , KC_NO  , 
       KC_NO  , LCTL(KC_F), LCTL(S(KC_F)), KC_NO  , KC_NO  , KC_NO  , KC_NO  , 
       KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  ,
       KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , 
       KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , 
			                                              KC_NO  , KC_NO  , 
																										         KC_NO  , 
																			     KC_NO  , KC_NO  , KC_NO  , 

                KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , 
                KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , 
                         KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , 
                KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , 
                                  KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , 
								KC_NO  , KC_NO  , 
                KC_NO  , 
                KC_NO  , KC_NO  , KC_NO   
),


};

const uint16_t PROGMEM fn_actions[] = {
    [1] = ACTION_LAYER_TAP_TOGGLE(SYMB)                // FN1 - Momentary Layer 1 (Symbols)
};

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
  // MACRODOWN only works in this function
  switch(id) {
    case 0:
      if (record->event.pressed) {
        SEND_STRING (QMK_KEYBOARD "/" QMK_KEYMAP " @ " QMK_VERSION);
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

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    // dynamically generate these.
    case EPRM:
      if (record->event.pressed) {
        eeconfig_init();
      }
      return false;
      break;
    case VRSN:
      if (record->event.pressed) {
        SEND_STRING (QMK_KEYBOARD "/" QMK_KEYMAP " @ " QMK_VERSION);
      }
      return false;
      break;
    case RGB_SLD:
      if (record->event.pressed) {
        #ifdef RGBLIGHT_ENABLE
          rgblight_mode(1);
        #endif
      }
      return false;
      break;
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
