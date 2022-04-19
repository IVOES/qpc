/*============================================================================
* QP/C Real-Time Embedded Framework (RTEF)
* Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
*
* SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
*
* This software is dual-licensed under the terms of the open source GNU
* General Public License version 3 (or any later version), or alternatively,
* under the terms of one of the closed source Quantum Leaps commercial
* licenses.
*
* The terms of the open source GNU General Public License version 3
* can be found at: <www.gnu.org/licenses/gpl-3.0>
*
* The terms of the closed source Quantum Leaps commercial licenses
* can be found at: <www.state-machine.com/licensing>
*
* Redistributions in source code must retain this top-level comment block.
* Plagiarizing this software to sidestep the license obligations is illegal.
*
* Contact information:
* <www.state-machine.com>
* <info@state-machine.com>
============================================================================*/
/*!
* @date Last updated on: 2021-12-23
* @version Last updated for: @ref qpc_7_0_0
*
* @file
* @brief QXK preemptive dual-mode kernel core functions
* @ingroup qxk
*/
#define QP_IMPL           /* this is QP implementation */
#include "qf_port.h"      /* QF port */
#include "qxk_pkg.h"      /* QXK package-scope internal interface */
#include "qassert.h"      /* QP embedded systems-friendly assertions */
#ifdef Q_SPY              /* QS software tracing enabled? */
    #include "qs_port.h"  /* QS port */
    #include "qs_pkg.h"   /* QS facilities for pre-defined trace records */
#else
    #include "qs_dummy.h" /* disable the QS software tracing */
#endif /* Q_SPY */

/* protection against including this source file in a wrong project */
#ifndef QXK_H
    #error "Source file included in a project NOT based on the QXK kernel"
#endif /* QXK_H */

Q_DEFINE_THIS_MODULE("qxk")

/* Global-scope objects *****************************************************/
QXK_PrivAttr QXK_attr_; /* global private attributes of the QXK kernel */

/* Local-scope objects ******************************************************/
static QActive l_idleThread;

/*==========================================================================*/
/*!
* @description
* Initializes QF and must be called exactly once before any other QF
* function. Typically, QF_init() is called from main() even before
* initializing the Board Support Package (BSP).
*
* @note QF_init() clears the internal QF variables, so that the framework
* can start correctly even if the startup code fails to clear the
* uninitialized data (as is required by the C Standard).
*/
void QF_init(void) {
    QF_maxPool_      = 0U;
    QF_subscrList_   = (QSubscrList *)0;
    QF_maxPubSignal_ = 0;

    QF_bzero(&QF_timeEvtHead_[0], sizeof(QF_timeEvtHead_));
    QF_bzero(&QF_active_[0],      sizeof(QF_active_));
    QF_bzero(&QXK_attr_,          sizeof(QXK_attr_));
    QF_bzero(&l_idleThread,       sizeof(l_idleThread));

    /* setup the QXK scheduler as initially locked and not running */
    QXK_attr_.lockPrio = (QF_MAX_ACTIVE + 1U);

    /* setup the QXK idle loop... */
    QF_active_[0] = &l_idleThread; /* register idle thread with QF */
    QXK_attr_.idleThread = &l_idleThread; /* save the idle thread ptr */
    QXK_attr_.actPrio = l_idleThread.prio; /* set the base priority */

#ifdef QXK_INIT
    QXK_INIT(); /* port-specific initialization of the QXK kernel */
#endif
}

/*==========================================================================*/
/*!
* @description
* This function stops the QF application. After calling this function,
* QF attempts to gracefully stop the application. This graceful shutdown
* might take some time to complete. The typical use of this function is
* for terminating the QF application to return back to the operating
* system or for handling fatal errors that require shutting down
* (and possibly re-setting) the system.
*
* @attention
* After calling QF_stop() the application must terminate and cannot
* continue. In particular, QF_stop() is **not** intended to be followed
* by a call to QF_init() to "resurrect" the application.
*
* @sa QF_onCleanup()
*/
void QF_stop(void) {
    QF_onCleanup(); /* application-specific cleanup callback */
    /* nothing else to do for the preemptive QXK kernel */
}

/*==========================================================================*/
/*! process all events posted during initialization */
static void initial_events(void); /* prototype */
static void initial_events(void) {
    QXK_attr_.lockPrio = 0U; /* unlock the scheduler */

    /* any active objects need to be scheduled before starting event loop? */
    if (QXK_sched_() != 0U) {
        QXK_activate_(); /* activate AOs to process all events posted so far*/
    }
}

/*==========================================================================*/
/*!
* @description
* QF_run() is typically called from main() after you initialize
* the QF and start at least one active object with QACTIVE_START().
*
* @returns In QXK, the QF_run() function does not return.
*/
int_t QF_run(void) {
    QF_INT_DISABLE();
    initial_events(); /* process all events posted during initialization */
    QF_onStartup(); /* application-specific startup callback */

    /* produce the QS_QF_RUN trace record */
    QS_BEGIN_NOCRIT_PRE_(QS_QF_RUN, 0U)
    QS_END_NOCRIT_PRE_()

    QF_INT_ENABLE();

    /* the QXK idle loop... */
    for (;;) {
        QXK_onIdle(); /* application-specific QXK idle callback */
    }

#ifdef __GNUC__
    return 0;
#endif
}

/*==========================================================================*/
/*!
* @description
* Starts execution of the AO and registers the AO with the framework.
* Also takes the top-most initial transition in the AO's state machine.
* This initial transition is taken in the callee's thread of execution.
*
* @param[in,out] me      pointer (see @ref oop)
* @param[in]     prio    priority at which to start the active object
* @param[in]     qSto    pointer to the storage for the ring buffer of the
*                        event queue (used only with the built-in ::QEQueue)
* @param[in]     qLen    length of the event queue [events]
* @param[in]     stkSto  pointer to the stack storage (used only when
*                        per-AO stack is needed)
* @param[in]     stkSize stack size [bytes]
* @param[in]     par     pointer to an extra parameter (might be NULL).
*
* @note This function should be called via the macro QACTIVE_START().
*
* @usage
* The following example shows starting an AO when a per-task stack is needed:
* @include qf_start.c
*/
void QActive_start_(QActive * const me, uint_fast8_t prio,
                    QEvt const * * const qSto, uint_fast16_t const qLen,
                    void * const stkSto, uint_fast16_t const stkSize,
                    void const * const par)
{
    /*! @pre AO cannot be started:
    * - from an ISR;
    * - the priority must be in range;
    * - the stack storage must NOT be provided (because the QK kernel does
    * not need per-AO stacks).
    */
    Q_REQUIRE_ID(200, (!QXK_ISR_CONTEXT_())
        && (0U < prio) && (prio <= QF_MAX_ACTIVE)
        && (stkSto == (void *)0)
        && (stkSize == 0U));

    QEQueue_init(&me->eQueue, qSto, qLen); /* initialize the built-in queue */
    me->osObject = (void *)0; /* no private stack for AO */
    me->prio     = (uint8_t)prio;  /* set the current priority of the AO */
    me->dynPrio  = (uint8_t)prio;  /* set the dynamic priority of the AO */
    QF_add_(me); /* make QF aware of this active object */

    QHSM_INIT(&me->super, par, me->prio); /* top-most initial tran. */
    QS_FLUSH(); /* flush the trace buffer to the host */

    /* see if this AO needs to be scheduled in case QXK is already running */
    QF_CRIT_STAT_
    QF_CRIT_E_();
    if (QXK_sched_() != 0U) { /* activation needed? */
        QXK_activate_();
    }
    QF_CRIT_X_();
}

/*==========================================================================*/
/*!
* @description
* This function locks the QXK scheduler to the specified ceiling.
*
* @param[in]   ceiling    priority ceiling to which the QXK scheduler
*                         needs to be locked
*
* @returns
* The previous QXK Scheduler lock status, which is to be used to unlock
* the scheduler by restoring its previous lock status in QXK_schedUnlock().
*
* @note
* A QXK scheduler can be locked from both basic threads (AOs) and
* extended threads and the scheduler locks can nest.
*
* @note
* QXK_schedLock() must be always followed by the corresponding
* QXK_schedUnlock().
*
* @attention
* QXK will fire an assertion if a thread holding the lock attempts
* to block.
*
* @sa QXK_schedUnlock()
*
* @usage
* The following example shows how to lock and unlock the QXK scheduler:
* @include qxk_lock.c
*/
QSchedStatus QXK_schedLock(uint_fast8_t ceiling) {
    QF_CRIT_STAT_
    QF_CRIT_E_();

    /*! @pre The QXK scheduler lock:
    * - cannot be called from an ISR;
    */
    Q_REQUIRE_ID(400, !QXK_ISR_CONTEXT_());

    /* first store the previous lock prio if below the ceiling */
    QSchedStatus stat;
    if ((uint_fast8_t)QXK_attr_.lockPrio < ceiling) {
        stat = ((QSchedStatus)QXK_attr_.lockPrio << 8U);
        QXK_attr_.lockPrio = (uint8_t)ceiling;

        QS_BEGIN_NOCRIT_PRE_(QS_SCHED_LOCK, 0U)
            QS_TIME_PRE_(); /* timestamp */
            /* the previous lock prio & new lock prio */
            QS_2U8_PRE_(stat, QXK_attr_.lockPrio);
        QS_END_NOCRIT_PRE_()

        /* add the previous lock holder priority */
        stat |= (QSchedStatus)QXK_attr_.lockHolder;
        QXK_attr_.lockHolder = (QXK_attr_.curr != (QActive *)0)
                               ? QXK_attr_.curr->prio
                               : 0U;
    }
    else {
       stat = 0xFFU;
    }
    QF_CRIT_X_();

    return stat; /* return the status to be saved in a stack variable */
}

/*==========================================================================*/
/*!
* @description
* This function unlocks the QXK scheduler to the previous status.
*
* @param[in]   stat       previous QXK Scheduler lock status returned from
*                         QXK_schedLock()
*
* @note
* A QXK scheduler can be locked from both basic threads (AOs) and
* extended threads and the scheduler locks can nest.
*
* @note
* QXK_schedUnlock() must always follow the corresponding QXK_schedLock().
*
* @sa QXK_schedLock()
*
* @usage
* The following example shows how to lock and unlock the QXK scheduler:
* @include qxk_lock.c
*/
void QXK_schedUnlock(QSchedStatus stat) {
    /* has the scheduler been actually locked by the last QXK_schedLock()? */
    if (stat != 0xFFU) {
        uint_fast8_t const lockPrio = (uint_fast8_t)QXK_attr_.lockPrio;
        uint_fast8_t const prevPrio = (uint_fast8_t)(stat >> 8U);
        QF_CRIT_STAT_
        QF_CRIT_E_();

        /*! @pre The scheduler cannot be unlocked:
        * - from the ISR context; and
        * - the current lock priority must be greater than the previous
        */
        Q_REQUIRE_ID(500, (!QXK_ISR_CONTEXT_())
                          && (lockPrio > prevPrio));

        QS_BEGIN_NOCRIT_PRE_(QS_SCHED_UNLOCK, 0U)
            QS_TIME_PRE_(); /* timestamp */
            /* prio before unlocking & prio after unlocking */
            QS_2U8_PRE_(lockPrio, prevPrio);
        QS_END_NOCRIT_PRE_()

        /* restore the previous lock priority and lock holder */
        QXK_attr_.lockPrio   = (uint8_t)prevPrio;
        QXK_attr_.lockHolder = (uint8_t)(stat & 0xFFU);

        /* find the highest-prio thread ready to run */
        if (QXK_sched_() != 0U) { /* priority found? */
            QXK_activate_(); /* activate any unlocked basic threads */
        }

        QF_CRIT_X_();
    }
}

/*==========================================================================*/
/*!
* @description
* The QXK scheduler finds the priority of the highest-priority thread
* that is ready to run.
*
* @returns
* the 1-based priority of the the thread (basic or extended) run next,
* or zero if no eligible thread is found.
*
* @attention
* QXK_sched_() must be always called with interrupts **disabled** and
* returns with interrupts **disabled**.
*/
uint_fast8_t QXK_sched_(void) {
    /* find the highest-prio thread ready to run */
    uint_fast8_t p = QPSet_findMax(&QXK_attr_.readySet);

    if (p <= (uint_fast8_t)QXK_attr_.lockPrio) { /* below the lock prio? */
        /* dynamic priority of the thread holding the lock */
        p = (uint_fast8_t)QF_active_[QXK_attr_.lockHolder]->dynPrio;
        if (p != 0U) {
            Q_ASSERT_ID(610, QPSet_hasElement(&QXK_attr_.readySet, p));
        }
    }

    QActive * const next = QF_active_[p];

    /* the next thread found must be registered in QF */
    Q_ASSERT_ID(620, next != (QActive *)0);

    /* is the current thread a basic-thread? */
    if (QXK_attr_.curr == (QActive *)0) {

        /* is next a basic-thread? */
        if (next->osObject == (void *)0) {
            if (p > (uint_fast8_t)QXK_attr_.actPrio) {
                QXK_attr_.next = next; /* set the next AO to activate */
            }
            else {
                QXK_attr_.next = (QActive *)0;
                p = 0U; /* no activation needed */
            }
        }
        else {  /* this is an extened-thread */

            QS_BEGIN_NOCRIT_PRE_(QS_SCHED_NEXT, next->prio)
                QS_TIME_PRE_();  /* timestamp */
                /* prio of the next AO & prio of the curr AO */
                QS_2U8_PRE_(p, QXK_attr_.actPrio);
            QS_END_NOCRIT_PRE_()

            QXK_attr_.next = next;
            p = 0U; /* no activation needed */
            QXK_CONTEXT_SWITCH_();
        }
    }
    else { /* currently executing an extended-thread */

        /* is the next thread different from the current? */
        if (next != QXK_attr_.curr) {

            QS_BEGIN_NOCRIT_PRE_(QS_SCHED_NEXT, next->prio)
                QS_TIME_PRE_();  /* timestamp */
                /* next prio & current prio */
                QS_2U8_PRE_(p, QXK_attr_.curr->prio);
            QS_END_NOCRIT_PRE_()

            QXK_attr_.next = next;
            p = 0U; /* no activation needed */
            QXK_CONTEXT_SWITCH_();
        }
        else { /* next is the same as the current */
            QXK_attr_.next = (QActive *)0; /* no need to context-switch */
            p = 0U; /* no activation needed */
        }
    }
    return p;
}

/*==========================================================================*/
/*!
* @attention
* QXK_activate_() must be always called with interrupts **disabled** and
* returns with interrupts **disabled**.
*
* @note
* The activate function might enable interrupts internally, but it always
* returns with interrupts **disabled**.
*/
void QXK_activate_(void) {
    uint_fast8_t const pin = (uint_fast8_t)QXK_attr_.actPrio;
    QActive *a = QXK_attr_.next; /* the next AO (basic-thread) to run */

    /* QXK Context switch callback defined or QS tracing enabled? */
#if (defined QXK_ON_CONTEXT_SW) || (defined Q_SPY)
    uint_fast8_t pprev = pin;
#endif /* QXK_ON_CONTEXT_SW || Q_SPY */

    /*! @pre QXK_attr_.next must be valid */
    Q_REQUIRE_ID(700, (a != (QActive *)0) && (pin < QF_MAX_ACTIVE));

    /* dynamic priority of the next AO */
    uint_fast8_t p = (uint_fast8_t)a->dynPrio;

    /* loop until no more ready-to-run AOs of higher prio than the initial */
    do  {
        a = QF_active_[p]; /* obtain the pointer to the AO */
        QXK_attr_.actPrio = (uint8_t)p; /* this becomes the base prio */
        QXK_attr_.next = (QActive *)0; /* clear the next AO */

        QS_BEGIN_NOCRIT_PRE_(QS_SCHED_NEXT, a->prio)
            QS_TIME_PRE_();     /* timestamp */
            /* next prio & prev prio */
            QS_2U8_PRE_(p, pprev);
        QS_END_NOCRIT_PRE_()

#if (defined QXK_ON_CONTEXT_SW) || (defined Q_SPY)
        if (p != pprev) {  /* changing threads? */

#ifdef QXK_ON_CONTEXT_SW
            Q_ASSERT_ID(710, pprev < QF_MAX_ACTIVE);

            /* context-switch callback */
            QXK_onContextSw(((pprev != 0U) ? QF_active_[pprev] : (QActive*)0),
                            a);
#endif /* QXK_ON_CONTEXT_SW */

            pprev = p; /* update previous priority */
        }
#endif /* QXK_ON_CONTEXT_SW || Q_SPY */

        QF_INT_ENABLE(); /* unconditionally enable interrupts */

        /* perform the run-to-completion (RTC) step...
        * 1. retrieve the event from the AO's event queue, which by this
        *    time must be non-empty and QActive_get_() asserts it.
        * 2. dispatch the event to the AO's state machine.
        * 3. determine if event is garbage and collect it if so
        */
        QEvt const * const e = QActive_get_(a);
        QHSM_DISPATCH(&a->super, e, a->prio);
        QF_gc(e);

        QF_INT_DISABLE(); /* unconditionally disable interrupts */

        if (a->eQueue.frontEvt == (QEvt *)0) { /* empty queue? */
            QPSet_remove(&QXK_attr_.readySet, p);
        }

        /* find new highest-prio AO ready to run...
        * NOTE: this part must match the QXK_sched_(),
        * current is a basic-thread path.
        */
        p = QPSet_findMax(&QXK_attr_.readySet);

        if (p <= (uint_fast8_t)QXK_attr_.lockPrio) { /* below lock prio? */
            p = (uint_fast8_t)QXK_attr_.lockHolder; /* thread holding lock */
            if (p != 0U) {
                Q_ASSERT_ID(710, QPSet_hasElement(&QXK_attr_.readySet, p));
            }
        }
        a = QF_active_[p];

        /* the AO must be registered in QF */
        Q_ASSERT_ID(720, a != (QActive *)0);

        /* is the next a basic thread? */
        if (a->osObject == (void *)0) {
            if (p > pin) {
                QXK_attr_.next = a;
            }
            else {
                QXK_attr_.next = (QActive *)0;
                p = 0U; /* no activation needed */
            }
        }
        else {  /* next is the extened thread */

            QS_BEGIN_NOCRIT_PRE_(QS_SCHED_NEXT, a->prio)
                QS_TIME_PRE_(); /* timestamp */
                /* next prio & curr prio */
                QS_2U8_PRE_(p, QXK_attr_.actPrio);
            QS_END_NOCRIT_PRE_()

            QXK_attr_.next = a;
            p = 0U; /* no activation needed */
            QXK_CONTEXT_SWITCH_();
        }
    } while (p != 0U); /* while activation needed */

    QXK_attr_.actPrio = (uint8_t)pin; /* restore the base prio */

#if (defined QXK_ON_CONTEXT_SW) || (defined Q_SPY)
    if (pin != 0U) { /* resuming an active object? */
        a = QF_active_[pin]; /* the pointer to the preempted AO */

        QS_BEGIN_NOCRIT_PRE_(QS_SCHED_RESUME, a->prio)
            QS_TIME_PRE_();  /* timestamp */
            QS_2U8_PRE_(pin, pprev); /* resumed prio & previous prio */
        QS_END_NOCRIT_PRE_()
    }
    else {  /* resuming priority==0 --> idle */
        a = (QActive *)0;

        QS_BEGIN_NOCRIT_PRE_(QS_SCHED_IDLE, 0U)
            QS_TIME_PRE_();    /* timestamp */
            QS_U8_PRE_(pprev); /* previous priority */
        QS_END_NOCRIT_PRE_()
    }

#ifdef QXK_ON_CONTEXT_SW
    /* context-switch callback */
    QXK_onContextSw(QF_active_[pprev], a);
#endif /* QXK_ON_CONTEXT_SW */

#endif /* QXK_ON_CONTEXT_SW || Q_SPY */
}

/*==========================================================================*/
struct QActive *QXK_current(void) {
    /*! @pre the QXK kernel must be running */
    Q_REQUIRE_ID(800, QXK_attr_.lockPrio <= QF_MAX_ACTIVE);

    QF_CRIT_STAT_
    QF_CRIT_E_();

    struct QActive *curr = QXK_attr_.curr;
    if (curr == (QActive *)0) { /* basic thread? */
        curr = QF_active_[QXK_attr_.actPrio];
    }
    QF_CRIT_X_();

    /*! @post the current thread must be valid */
    Q_ENSURE_ID(890, curr != (QActive *)0);

    return curr;
}
