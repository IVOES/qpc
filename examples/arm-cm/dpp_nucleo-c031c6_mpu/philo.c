/*$file${.::philo.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: dpp.qm
* File:  ${.::philo.c}
*
* This code has been generated by QM 5.2.5 <www.state-machine.com/qm>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This generated code is open source software: you can redistribute it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation.
*
* This code is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* NOTE:
* Alternatively, this generated code may be distributed under the terms
* of Quantum Leaps commercial licenses, which expressly supersede the GNU
* General Public License and are specifically designed for licensees
* interested in retaining the proprietary status of their code.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*/
/*$endhead${.::philo.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#include "qpc.h"
#include "dpp.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE

/* Active object class -----------------------------------------------------*/
/*$declare${AOs::Philo} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Philo} ............................................................*/
typedef struct {
/* protected: */
    QActive super;

/* private: */
    QTimeEvt timeEvt;
    uint8_t philoNum;
} Philo;

/* protected: */
static QState Philo_initial(Philo * const me, void const * const par);
static QState Philo_thinking(Philo * const me, QEvt const * const e);
static QState Philo_hungry(Philo * const me, QEvt const * const e);
static QState Philo_eating(Philo * const me, QEvt const * const e);
/*$enddecl${AOs::Philo} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
Q_ASSERT_STATIC(sizeof(Philo) <= (1U << PHILO_SIZE_POW2));

#define THINK_TIME  \
    (QTimeEvtCtr)((BSP_random() % BSP_TICKS_PER_SEC) + (BSP_TICKS_PER_SEC/2U))
#define EAT_TIME    \
    (QTimeEvtCtr)((BSP_random() % BSP_TICKS_PER_SEC) + BSP_TICKS_PER_SEC)

/*$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/* Check for the minimum required QP version */
#if (QP_VERSION < 700U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 7.0.0 or higher required
#endif
/*$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$define${AOs::Philo_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Philo_ctor} .......................................................*/
void Philo_ctor(
    QActive * const act,
    uint8_t const nPhilo,
    void const * const mpu)
{
    Philo * const me = (Philo *)act;

    QActive_ctor(&me->super, Q_STATE_CAST(&Philo_initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, TIMEOUT_SIG, 0U);
    me->philoNum = nPhilo;

    /* initialize the MPU settings for this AO */
    ((QActive *)me)->thread = (void const *)mpu;
}
/*$enddef${AOs::Philo_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$define${AOs::Philo} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Philo} ............................................................*/

/*${AOs::Philo::SM} ........................................................*/
static QState Philo_initial(Philo * const me, void const * const par) {
    /*${AOs::Philo::SM::initial} */
    Q_UNUSED_PAR(par);

    static uint8_t registered; /* starts off with 0, per C-standard */
    if (registered == 0U) {
        registered = 1U;

        QS_FUN_DICTIONARY(&Philo_initial);
        QS_FUN_DICTIONARY(&Philo_thinking);
        QS_FUN_DICTIONARY(&Philo_hungry);
        QS_FUN_DICTIONARY(&Philo_eating);
    }
    #ifdef Q_SPY
    Philo * const Philo = me; /* 'Philo' name for the dictionary */
    #endif
    QS_OBJ_ARR_DICTIONARY(Philo, me->philoNum);
    QS_OBJ_ARR_DICTIONARY(&Philo->timeEvt, me->philoNum);

    /* subscriptions */
    QActive_subscribe(&me->super, EAT_SIG);
    QActive_subscribe(&me->super, TEST_SIG);
    return Q_TRAN(&Philo_thinking);
}

/*${AOs::Philo::SM::thinking} ..............................................*/
static QState Philo_thinking(Philo * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Philo::SM::thinking} */
        case Q_ENTRY_SIG: {
            QTimeEvt_armX(&me->timeEvt, THINK_TIME, 0U);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Philo::SM::thinking} */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->timeEvt);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Philo::SM::thinking::TIMEOUT} */
        case TIMEOUT_SIG: {
            status_ = Q_TRAN(&Philo_hungry);
            break;
        }
        /*${AOs::Philo::SM::thinking::EAT, DONE} */
        case EAT_SIG: /* intentionally fall through */
        case DONE_SIG: {
            /* EAT or DONE must be for other Philos than this one */
            Q_ASSERT(Q_EVT_CAST(TableEvt)->philoNum != me->philoNum);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Philo::SM::thinking::TEST} */
        case TEST_SIG: {
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

/*${AOs::Philo::SM::hungry} ................................................*/
static QState Philo_hungry(Philo * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Philo::SM::hungry} */
        case Q_ENTRY_SIG: {
            TableEvt *pe = Q_NEW(TableEvt, HUNGRY_SIG);
            pe->philoNum = me->philoNum;
            QACTIVE_POST(AO_Table, &pe->super, me);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Philo::SM::hungry::EAT} */
        case EAT_SIG: {
            /*${AOs::Philo::SM::hungry::EAT::[Q_EVT_CAST(TableEvt)->philoNum=~} */
            if (Q_EVT_CAST(TableEvt)->philoNum == me->philoNum) {
                status_ = Q_TRAN(&Philo_eating);
            }
            else {
                status_ = Q_UNHANDLED();
            }
            break;
        }
        /*${AOs::Philo::SM::hungry::DONE} */
        case DONE_SIG: {
            /* DONE must be for other Philos than this one */
            Q_ASSERT(Q_EVT_CAST(TableEvt)->philoNum != me->philoNum);
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

/*${AOs::Philo::SM::eating} ................................................*/
static QState Philo_eating(Philo * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Philo::SM::eating} */
        case Q_ENTRY_SIG: {
            QTimeEvt_armX(&me->timeEvt, EAT_TIME, 0U);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Philo::SM::eating} */
        case Q_EXIT_SIG: {
            TableEvt *pe = Q_NEW(TableEvt, DONE_SIG);
            pe->philoNum = me->philoNum;
            QACTIVE_PUBLISH(&pe->super, &me->super);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Philo::SM::eating::TIMEOUT} */
        case TIMEOUT_SIG: {
            status_ = Q_TRAN(&Philo_thinking);
            break;
        }
        /*${AOs::Philo::SM::eating::EAT, DONE} */
        case EAT_SIG: /* intentionally fall through */
        case DONE_SIG: {
            /* EAT or DONE must be for other Philos than this one */
            Q_ASSERT(Q_EVT_CAST(TableEvt)->philoNum != me->philoNum);
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
/*$enddef${AOs::Philo} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
