/*$file${.::table.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: dpp.qm
* File:  ${.::table.c}
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
/*$endhead${.::table.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#include "qpc.h"
#include "dpp.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE

/* Active object class -----------------------------------------------------*/
/*$declare${AOs::Table} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Table} ............................................................*/
typedef struct {
/* protected: */
    QMActive super;

/* private: */
    uint8_t fork[N_PHILO];
    uint8_t isHungry[N_PHILO];
} Table;

/* protected: */
static QState Table_initial(Table * const me, void const * const par);
static QState Table_active  (Table * const me, QEvt const * const e);
static QMState const Table_active_s = {
    QM_STATE_NULL, /* superstate (top) */
    Q_STATE_CAST(&Table_active),
    Q_ACTION_NULL, /* no entry action */
    Q_ACTION_NULL, /* no exit action */
    Q_ACTION_NULL  /* no initial tran. */
};
static QState Table_serving  (Table * const me, QEvt const * const e);
static QState Table_serving_e(Table * const me);
static QMState const Table_serving_s = {
    &Table_active_s, /* superstate */
    Q_STATE_CAST(&Table_serving),
    Q_ACTION_CAST(&Table_serving_e),
    Q_ACTION_NULL, /* no exit action */
    Q_ACTION_NULL  /* no initial tran. */
};
static QState Table_paused  (Table * const me, QEvt const * const e);
static QState Table_paused_e(Table * const me);
static QState Table_paused_x(Table * const me);
static QMState const Table_paused_s = {
    &Table_active_s, /* superstate */
    Q_STATE_CAST(&Table_paused),
    Q_ACTION_CAST(&Table_paused_e),
    Q_ACTION_CAST(&Table_paused_x),
    Q_ACTION_NULL  /* no initial tran. */
};
/*$enddecl${AOs::Table} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
Q_ASSERT_STATIC(sizeof(Table) <= (1U << TABLE_SIZE_POW2));

#define RIGHT(n_) ((uint8_t)(((n_) + (N_PHILO - 1U)) % N_PHILO))
#define LEFT(n_)  ((uint8_t)(((n_) + 1U) % N_PHILO))
#define FREE      ((uint8_t)0)
#define USED      ((uint8_t)1)

/*$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/* Check for the minimum required QP version */
#if (QP_VERSION < 700U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 7.0.0 or higher required
#endif
/*$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$define${AOs::Table_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Table_ctor} .......................................................*/
void Table_ctor(
    QActive * const act,
    void const * const mpu)
{
    Table * const me = (Table *)act; /* downcast */
    QMActive_ctor(&me->super, Q_STATE_CAST(&Table_initial));

    for (uint8_t n = 0U; n < N_PHILO; ++n) {
        me->fork[n] = FREE;
        me->isHungry[n] = 0U;
    }

    /* initialize the MPU settings for this AO */
    ((QActive *)me)->thread = (void const *)mpu;
}
/*$enddef${AOs::Table_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$define${AOs::Table} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Table} ............................................................*/

/*${AOs::Table::SM} ........................................................*/
static QState Table_initial(Table * const me, void const * const par) {
    static struct {
        QMState const *target;
        QActionHandler act[2];
    } const tatbl_ = { /* tran-action table */
        &Table_serving_s, /* target state */
        {
            Q_ACTION_CAST(&Table_serving_e), /* entry */
            Q_ACTION_NULL /* zero terminator */
        }
    };
    /*${AOs::Table::SM::initial} */
    Q_UNUSED_PAR(par);

    QS_OBJ_DICTIONARY(AO_Table);

    QActive_subscribe((QActive *)me, DONE_SIG);
    QActive_subscribe((QActive *)me, PAUSE_SIG);
    QActive_subscribe((QActive *)me, SERVE_SIG);
    QActive_subscribe((QActive *)me, TEST_SIG);

    for (uint8_t n = 0U; n < N_PHILO; ++n) {
        me->fork[n] = FREE;
        me->isHungry[n] = 0U;
        BSP_displayPhilStat(n, "thinking");
    }

    QS_FUN_DICTIONARY(&Table_active);
    QS_FUN_DICTIONARY(&Table_serving);
    QS_FUN_DICTIONARY(&Table_paused);

    return QM_TRAN_INIT(&tatbl_);
}

/*${AOs::Table::SM::active} ................................................*/
/*${AOs::Table::SM::active} */
static QState Table_active(Table * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Table::SM::active::TEST} */
        case TEST_SIG: {
            status_ = QM_HANDLED();
            break;
        }
        /*${AOs::Table::SM::active::EAT} */
        case EAT_SIG: {
            Q_ERROR();
            status_ = QM_HANDLED();
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    (void)me; /* unused parameter */
    return status_;
}

/*${AOs::Table::SM::active::serving} .......................................*/
/*${AOs::Table::SM::active::serving} */
static QState Table_serving_e(Table * const me) {
    uint8_t n;
    for (n = 0U; n < N_PHILO; ++n) { /* give permissions to eat... */
        if ((me->isHungry[n] != 0U)
            && (me->fork[LEFT(n)] == FREE)
            && (me->fork[n] == FREE))
        {
            TableEvt *te;

            me->fork[LEFT(n)] = USED;
            me->fork[n] = USED;
            te = Q_NEW(TableEvt, EAT_SIG);
            te->philoNum = n;
            QACTIVE_PUBLISH(&te->super, (QActive *)me);
            me->isHungry[n] = 0U;
            BSP_displayPhilStat(n, "eating  ");
        }
    }
    return QM_ENTRY(&Table_serving_s);
}
/*${AOs::Table::SM::active::serving} */
static QState Table_serving(Table * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Table::SM::active::serving::HUNGRY} */
        case HUNGRY_SIG: {
            uint8_t n, m;

            n = Q_EVT_CAST(TableEvt)->philoNum;
            /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (me->isHungry[n] == 0U));

            BSP_displayPhilStat(n, "hungry  ");
            m = LEFT(n);
            /*${AOs::Table::SM::active::serving::HUNGRY::[bothfree]} */
            if ((me->fork[m] == FREE) && (me->fork[n] == FREE)) {
                TableEvt *pe;
                me->fork[m] = USED;
                me->fork[n] = USED;
                pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoNum = n;
                QACTIVE_PUBLISH(&pe->super, (QActive *)me);
                BSP_displayPhilStat(n, "eating  ");
                status_ = QM_HANDLED();
            }
            /*${AOs::Table::SM::active::serving::HUNGRY::[else]} */
            else {
                me->isHungry[n] = 1U;
                status_ = QM_HANDLED();
            }
            break;
        }
        /*${AOs::Table::SM::active::serving::DONE} */
        case DONE_SIG: {
            uint8_t n, m;
            TableEvt *pe;

            n = Q_EVT_CAST(TableEvt)->philoNum;
            /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (me->isHungry[n] == 0U));

            BSP_displayPhilStat(n, "thinking");
            m = LEFT(n);
            /* both forks of Phil[n] must be used */
            Q_ASSERT((me->fork[n] == USED) && (me->fork[m] == USED));

            me->fork[m] = FREE;
            me->fork[n] = FREE;
            m = RIGHT(n); /* check the right neighbor */

            if ((me->isHungry[m] != 0U) && (me->fork[m] == FREE)) {
                me->fork[n] = USED;
                me->fork[m] = USED;
                me->isHungry[m] = 0U;
                pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoNum = m;
                QACTIVE_PUBLISH(&pe->super, (QActive *)me);
                BSP_displayPhilStat(m, "eating  ");
            }
            m = LEFT(n); /* check the left neighbor */
            n = LEFT(m); /* left fork of the left neighbor */
            if ((me->isHungry[m] != 0U) && (me->fork[n] == FREE)) {
                me->fork[m] = USED;
                me->fork[n] = USED;
                me->isHungry[m] = 0U;
                pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoNum = m;
                QACTIVE_PUBLISH(&pe->super, (QActive *)me);
                BSP_displayPhilStat(m, "eating  ");
            }
            status_ = QM_HANDLED();
            break;
        }
        /*${AOs::Table::SM::active::serving::EAT} */
        case EAT_SIG: {
            Q_ERROR();
            status_ = QM_HANDLED();
            break;
        }
        /*${AOs::Table::SM::active::serving::PAUSE} */
        case PAUSE_SIG: {
            static struct {
                QMState const *target;
                QActionHandler act[2];
            } const tatbl_ = { /* tran-action table */
                &Table_paused_s, /* target state */
                {
                    Q_ACTION_CAST(&Table_paused_e), /* entry */
                    Q_ACTION_NULL /* zero terminator */
                }
            };
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}

/*${AOs::Table::SM::active::paused} ........................................*/
/*${AOs::Table::SM::active::paused} */
static QState Table_paused_e(Table * const me) {
    BSP_displayPaused(1U);
    (void)me; /* unused parameter */
    return QM_ENTRY(&Table_paused_s);
}
/*${AOs::Table::SM::active::paused} */
static QState Table_paused_x(Table * const me) {
    BSP_displayPaused(0U);
    (void)me; /* unused parameter */
    return QM_EXIT(&Table_paused_s);
}
/*${AOs::Table::SM::active::paused} */
static QState Table_paused(Table * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Table::SM::active::paused::SERVE} */
        case SERVE_SIG: {
            static struct {
                QMState const *target;
                QActionHandler act[3];
            } const tatbl_ = { /* tran-action table */
                &Table_serving_s, /* target state */
                {
                    Q_ACTION_CAST(&Table_paused_x), /* exit */
                    Q_ACTION_CAST(&Table_serving_e), /* entry */
                    Q_ACTION_NULL /* zero terminator */
                }
            };
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        /*${AOs::Table::SM::active::paused::HUNGRY} */
        case HUNGRY_SIG: {
            uint8_t n = Q_EVT_CAST(TableEvt)->philoNum;
            /* philo ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (me->isHungry[n] == 0U));
            me->isHungry[n] = 1U;
            BSP_displayPhilStat(n, "hungry  ");
            status_ = QM_HANDLED();
            break;
        }
        /*${AOs::Table::SM::active::paused::DONE} */
        case DONE_SIG: {
            uint8_t n, m;

            n = Q_EVT_CAST(TableEvt)->philoNum;
            /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (me->isHungry[n] == 0U));

            BSP_displayPhilStat(n, "thinking");
            m = LEFT(n);
            /* both forks of Phil[n] must be used */
            Q_ASSERT((me->fork[n] == USED) && (me->fork[m] == USED));

            me->fork[m] = FREE;
            me->fork[n] = FREE;
            status_ = QM_HANDLED();
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
/*$enddef${AOs::Table} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
