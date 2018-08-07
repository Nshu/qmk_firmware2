/* key matrix position */
typedef struct {
    uint8_t col;
    uint8_t row;
} keypos_t; //S:\qmk_firmware2\tmk_core\common\keyboard.h

/* key event */
typedef struct {
    keypos_t key;
    bool     pressed;
    uint16_t time;
} keyevent_t; //S:\qmk_firmware2\tmk_core\common\keyboard.h

/* tapping count and state */
typedef struct {
    bool    interrupted :1;
    bool    reserved2   :1;
    bool    reserved1   :1;
    bool    reserved0   :1;
    uint8_t count       :4;
} tap_t; //S:\qmk_firmware2\tmk_core\common\action.h

/* Key event container for recording */
typedef struct {
    keyevent_t  event;
    #ifndef NO_ACTION_TAPPING
        tap_t tap;
    #endif
} keyrecord_t; //S:\qmk_firmware2\tmk_core\common\action.h

#if (MATRIX_COLS <= 8)
    typedef  uint8_t    matrix_row_t;
    #elif (MATRIX_COLS <= 16)
    typedef  uint16_t   matrix_row_t;
    #elif (MATRIX_COLS <= 32)
    typedef  uint32_t   matrix_row_t;
    #else
    #error "MATRIX_COLS: invalid value"
#endif //S:\qmk_firmware2\tmk_core\common\matrix.h

typedef union {
    uint8_t raw[KEYBOARD_REPORT_SIZE];
    struct {
        uint8_t mods;
        uint8_t reserved;
        uint8_t keys[KEYBOARD_REPORT_KEYS];
    };
    #ifdef NKRO_ENABLE
        struct {
            uint8_t mods;
            uint8_t bits[KEYBOARD_REPORT_BITS];
        } nkro;
    #endif
} __attribute__ ((packed)) report_keyboard_t; //S:\qmk_firmware2\tmk_core\common\report.h