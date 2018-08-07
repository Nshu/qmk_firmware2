//S:\qmk_firmware2\tmk_core\common\keycode.h
#define MOD_BIT(code)   (1<<((code) & 0x07))
//////////////

//S:\qmk_firmware2\keyboards\ergodox_ez\keymaps\vim\keymap.c
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  bool SHIFTED = (keyboard_report->mods & MOD_BIT(KC_LSFT)) |
                 (keyboard_report->mods & MOD_BIT(KC_RSFT));

  switch (keycode) {

    case VIM_A:
      if (record->event.pressed) { SHIFTED ? VIM_APPEND_LINE() : VIM_APPEND(); }
      return false;
  }
}
/////////////////