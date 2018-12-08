//
// Created by abata on 2018/12/08.
//

#ifndef QMK_FIRMWARE2_QUE_MODULE_QMK_ADAPTER_H
#define QMK_FIRMWARE2_QUE_MODULE_QMK_ADAPTER_H

#ifndef QMK_FIRMWARE2_QUE_MODULE_H
#define QMK_FIRMWARE2_QUE_MODULE_H

#include "keyboard.h"
#include "timer.h"

#define EVENT_QUE_HOLD_TIME 20
#define QUE_SIZE 8
#define NULL_QUE (TICK)
typedef keyevent_t data_t;

#endif //QMK_FIRMWARE2_QUE_MODULE_H

#endif //QMK_FIRMWARE2_QUE_MODULE_QMK_ADAPTER_H
