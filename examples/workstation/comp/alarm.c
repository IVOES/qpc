/*.$file${.::alarm.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: comp.qm
* File:  ${.::alarm.c}
*
* This code has been generated by QM 5.0.4 <www.state-machine.com/qm/>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*/
/*.$endhead${.::alarm.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#include "qpc.h"
#include "bsp.h"
#include "alarm.h"
#include "clock.h"

//Q_DEFINE_THIS_FILE

/* Alarm component --------------------*/
/*.$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*. Check for the minimum required QP version */
#if (QP_VERSION < 680U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 6.8.0 or higher required
#endif
/*.$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*.$define${Components::Alarm} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*.${Components::Alarm} ....................................................*/
/*.${Components::Alarm::ctor} ..............................................*/
void Alarm_ctor(Alarm * const me) {
    QHsm_ctor(&me->super, Q_STATE_CAST(&Alarm_initial));
}

/*.${Components::Alarm::SM} ................................................*/
QState Alarm_initial(Alarm * const me, QEvt const * const e) {
    /*.${Components::Alarm::SM::initial} */
    me->alarm_time = 12U*60U;
    (void)e; /* avoid compiler warning about unused parameter */
    return Q_TRAN(&Alarm_off);
}
/*.${Components::Alarm::SM::off} ...........................................*/
QState Alarm_off(Alarm * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*.${Components::Alarm::SM::off} */
        case Q_ENTRY_SIG: {
            /* while in the off state, the alarm is kept in decimal format */
            me->alarm_time = (me->alarm_time/60)*100 + me->alarm_time%60;
            BSP_showTime24H("*** Alarm OFF ", me->alarm_time, 100U);
            status_ = Q_HANDLED();
            break;
        }
        /*.${Components::Alarm::SM::off} */
        case Q_EXIT_SIG: {
            /* upon exit, the alarm is converted to binary format */
            me->alarm_time = (me->alarm_time/100U)*60U + me->alarm_time%100U;
            status_ = Q_HANDLED();
            break;
        }
        /*.${Components::Alarm::SM::off::ALARM_ON} */
        case ALARM_ON_SIG: {
            /*.${Components::Alarm::SM::off::ALARM_ON::[alarminrange?]} */
            if ((me->alarm_time / 100U < 24U)
                && (me->alarm_time % 100U < 60U))
            {
                status_ = Q_TRAN(&Alarm_on);
            }
            /*.${Components::Alarm::SM::off::ALARM_ON::[else]} */
            else {
                me->alarm_time = 0U;
                BSP_showTime24H("*** Alarm reset", me->alarm_time, 100U);
                status_ = Q_HANDLED();
            }
            break;
        }
        /*.${Components::Alarm::SM::off::ALARM_SET} */
        case ALARM_SET_SIG: {
            /* while setting, the alarm is kept in decimal format */
            me->alarm_time = (10U * me->alarm_time
                              + ((SetEvt const *)e)->digit) % 10000U;
            BSP_showTime24H("*** Alarm reset ",  me->alarm_time, 100U);
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*.${Components::Alarm::SM::on} ............................................*/
QState Alarm_on(Alarm * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*.${Components::Alarm::SM::on} */
        case Q_ENTRY_SIG: {
            BSP_showTime24H("*** Alarm ON ",  me->alarm_time, 60U);
            status_ = Q_HANDLED();
            break;
        }
        /*.${Components::Alarm::SM::on::ALARM_OFF} */
        case ALARM_OFF_SIG: {
            status_ = Q_TRAN(&Alarm_off);
            break;
        }
        /*.${Components::Alarm::SM::on::ALARM_SET} */
        case ALARM_SET_SIG: {
            BSP_showMsg("*** Cannot set Alarm when it is ON");
            status_ = Q_HANDLED();
            break;
        }
        /*.${Components::Alarm::SM::on::TIME} */
        case TIME_SIG: {
            /*.${Components::Alarm::SM::on::TIME::[Q_EVT_CAST(TimeEvt)->current_ti~} */
            if (Q_EVT_CAST(TimeEvt)->current_time == me->alarm_time) {
                BSP_showMsg("ALARM!!!");

                /* asynchronously post the event to the container AO */
                QACTIVE_POST(APP_alarmClock, Q_NEW(QEvt, ALARM_SIG), me);
                status_ = Q_HANDLED();
            }
            else {
                status_ = Q_UNHANDLED();
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*.$enddef${Components::Alarm} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
