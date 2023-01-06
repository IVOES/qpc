/*$file${src::qxk::qxk_mutex.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: qpc.qm
* File:  ${src::qxk::qxk_mutex.c}
*
* This code has been generated by QM 5.2.4 <www.state-machine.com/qm>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This code is covered by the following QP license:
* License #    : LicenseRef-QL-dual
* Issued to    : Any user of the QP/C real-time embedded framework
* Framework(s) : qpc
* Support ends : 2023-12-31
* License scope:
*
* Copyright (C) 2005 Quantum Leaps, LLC <state-machine.com>.
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
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*/
/*$endhead${src::qxk::qxk_mutex.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*! @file
* @brief QXMutex_init(), QXMutex_lock(), QXMutex_tryLock() and
* QXMutex_unlock() definitions.
*/
#define QP_IMPL           /* this is QP implementation */
#include "qf_port.h"      /* QF port */
#include "qf_pkg.h"       /* QF package-scope interface */
#include "qassert.h"      /* QP embedded systems-friendly assertions */
#ifdef Q_SPY              /* QS software tracing enabled? */
    #include "qs_port.h"  /* QS port */
    #include "qs_pkg.h"   /* QS facilities for pre-defined trace records */
#else
    #include "qs_dummy.h" /* disable the QS software tracing */
#endif /* Q_SPY */

/* protection against including this source file in a wrong project */
#ifndef QP_INC_QXK_H_
    #error "Source file included in a project NOT based on the QXK kernel"
#endif /* QP_INC_QXK_H_ */

Q_DEFINE_THIS_MODULE("qxk_mutex")

/*==========================================================================*/
/*$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/* Check for the minimum required QP version */
#if (QP_VERSION < 700U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 7.0.0 or higher required
#endif
/*$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$define${QXK::QXMutex} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QXK::QXMutex} ..........................................................*/

/*${QXK::QXMutex::init} ....................................................*/
void QXMutex_init(QXMutex * const me,
    QPrioSpec const prioSpec)
{
    /*! @pre preemption-threshold must not be used */
    Q_REQUIRE_ID(100, (prioSpec & 0xFF00U) == 0U);

    me->super.prio  = (uint8_t)(prioSpec & 0xFFU); /* QF-priority */
    me->super.pthre = 0U;   /* preemption-threshold (not used) */

    if (prioSpec != 0U) {  /* priority-ceiling protocol used? */
        QActive_register_(&me->super); /* register this mutex as AO */
    }
}

/*${QXK::QXMutex::lock} ....................................................*/
bool QXMutex_lock(QXMutex * const me,
    uint_fast16_t const nTicks)
{
    QF_CRIT_STAT_
    QF_CRIT_E_();

    QXThread * const curr = QXK_PTR_CAST_(QXThread*, QXK_attr_.curr);

    /*! @pre this function must:
    * - NOT be called from an ISR;
    * - be called from an extended thread;
    * - the mutex-priority must be in range
    * - the thread must NOT be already blocked on any object.
    */
    Q_REQUIRE_ID(200, (!QXK_ISR_CONTEXT_()) /* don't call from an ISR! */
        && (curr != (QXThread *)0) /* current thread must be extended */
        && (me->super.prio <= QF_MAX_ACTIVE)
        && (curr->super.super.temp.obj == (QMState *)0)); /* not blocked */
    /*! @pre also: the thread must NOT be holding a scheduler lock. */
    Q_REQUIRE_ID(201, QXK_attr_.lockHolder != curr->super.prio);

    /* is the mutex available? */
    bool locked = true; /* assume that the mutex will be locked */
    if (me->super.eQueue.nFree == 0U) {
        me->super.eQueue.nFree = 1U; /* mutex lock nesting */

        /*! @pre also: the newly locked mutex must have no holder yet */
        Q_REQUIRE_ID(202, me->super.thread == (void *)0);

        /* set the new mutex holder to the curr thread and
        * save the thread's prio/pthre in the mutex
        * NOTE: reuse the otherwise unused eQueue data member.
        */
        me->super.thread = curr;
        me->super.eQueue.head = (QEQueueCtr)curr->super.prio;
        me->super.eQueue.tail = (QEQueueCtr)curr->super.pthre;

        QS_BEGIN_NOCRIT_PRE_(QS_MTX_LOCK, curr->super.prio)
            QS_TIME_PRE_();  /* timestamp */
            QS_OBJ_PRE_(me); /* this mutex */
            QS_2U8_PRE_((uint8_t)me->super.eQueue.head, /* holder prio */
                        (uint8_t)me->super.eQueue.nFree); /* nesting */
        QS_END_NOCRIT_PRE_()

        if (me->super.prio != 0U) { /* priority-ceiling protocol used? */
            /* the holder priority must be lower than that of the mutex
            * and the priority slot must be occupied by this mutex
            */
            Q_ASSERT_ID(210, (curr->super.prio < me->super.prio)
                && (QActive_registry_[me->super.prio] == &me->super));

            /* remove the thread's original prio from the ready set
            * and insert the mutex's prio into the ready set
            */
            QPSet_remove(&QF_readySet_, (uint_fast8_t)me->super.eQueue.head);
            QPSet_insert(&QF_readySet_, (uint_fast8_t)me->super.prio);

            /* put the thread into the AO registry in place of the mutex */
            QActive_registry_[me->super.prio] = &curr->super;

            /* set thread's prio/pthre to that of the mutex */
            curr->super.prio  = me->super.prio;
            curr->super.pthre = me->super.pthre;
        }
    }
    /* is the mutex locked by this thread already (nested locking)? */
    else if (me->super.thread == &curr->super) {

        /* the nesting level beyond the arbitrary but high limit
        * most likely means cyclic or recursive locking of a mutex.
        */
        Q_ASSERT_ID(220, me->super.eQueue.nFree < 0xFFU);

        ++me->super.eQueue.nFree; /* lock one more level */

        QS_BEGIN_NOCRIT_PRE_(QS_MTX_LOCK, curr->super.prio)
            QS_TIME_PRE_();  /* timestamp */
            QS_OBJ_PRE_(me); /* this mutex */
            QS_2U8_PRE_((uint8_t)me->super.eQueue.head, /* holder prio */
                        (uint8_t)me->super.eQueue.nFree); /* nesting */
        QS_END_NOCRIT_PRE_()
    }
    else { /* the mutex is already locked by a different thread */
        /* the mutex holder must be valid */
        Q_ASSERT_ID(230, me->super.thread != (void *)0);

        if (me->super.prio != 0U) { /* priority-ceiling protocol used? */
            /* the prio slot must be occupied by the thr. holding the mutex */
            Q_ASSERT_ID(240, QActive_registry_[me->super.prio]
                             == QXK_PTR_CAST_(QActive *, me->super.thread));
        }

        /* remove the curr thread's prio from the ready set (will block)
        * and insert it to the waiting set on this mutex
        */
        uint_fast8_t const p = (uint_fast8_t)curr->super.prio;
        QPSet_remove(&QF_readySet_, p);
        QPSet_insert(&me->waitSet,  p);

        /* set the blocking object (this mutex) */
        curr->super.super.temp.obj = QXK_PTR_CAST_(QMState*, me);
        QXThread_teArm_(curr, (enum_t)QXK_TIMEOUT_SIG, nTicks);

        QS_BEGIN_NOCRIT_PRE_(QS_MTX_BLOCK, curr->super.prio)
            QS_TIME_PRE_();  /* timestamp */
            QS_OBJ_PRE_(me); /* this mutex */
            QS_2U8_PRE_((uint8_t)me->super.eQueue.head, /* holder prio */
                        curr->super.prio); /* blocked thread prio */
        QS_END_NOCRIT_PRE_()

        /* schedule the next thread if multitasking started */
        (void)QXK_sched_(); /* schedule other threads */
        QF_CRIT_X_();
        QF_CRIT_EXIT_NOP(); /* BLOCK here !!! */

        /* AFTER unblocking... */
        QF_CRIT_E_();
        /* the blocking object must be this mutex */
        Q_ASSERT_ID(240, curr->super.super.temp.obj
                         == QXK_PTR_CAST_(QMState*, me));

        /* did the blocking time-out? (signal of zero means that it did) */
        if (curr->timeEvt.super.sig == 0U) {
            if (QPSet_hasElement(&me->waitSet, p)) { /* still waiting? */
                QPSet_remove(&me->waitSet, p); /* remove unblocked thread */
                locked = false; /* the mutex was NOT locked */
            }
        }
        else { /* blocking did NOT time out */
            /* the thread must NOT be waiting on this mutex */
            Q_ASSERT_ID(250, !QPSet_hasElement(&me->waitSet, p));
        }
        curr->super.super.temp.obj = (QMState *)0; /* clear blocking obj. */
    }
    QF_CRIT_X_();

    return locked;
}

/*${QXK::QXMutex::tryLock} .................................................*/
bool QXMutex_tryLock(QXMutex * const me) {
    QF_CRIT_STAT_
    QF_CRIT_E_();

    QActive *curr = QXK_attr_.curr;
    if (curr == (QActive *)0) { /* called from a basic thread? */
        curr = QActive_registry_[QXK_attr_.actPrio];
    }

    /*! @pre this function must:
    * - NOT be called from an ISR;
    * - the calling thread must be valid;
    * - the mutex-priority must be in range
    */
    Q_REQUIRE_ID(300, (!QXK_ISR_CONTEXT_()) /* don't call from an ISR! */
        && (curr != (QActive *)0) /* current thread must be valid */
        && (me->super.prio <= QF_MAX_ACTIVE));
    /*! @pre also: the thread must NOT be holding a scheduler lock. */
    Q_REQUIRE_ID(301, QXK_attr_.lockHolder != curr->prio);

    /* is the mutex available? */
    if (me->super.eQueue.nFree == 0U) {
        me->super.eQueue.nFree = 1U;  /* mutex lock nesting */

        /*! @pre also: the newly locked mutex must have no holder yet */
        Q_REQUIRE_ID(302, me->super.thread == (void *)0);

        /* set the new mutex holder to the curr thread and
        * save the thread's prio/pthre in the mutex
        * NOTE: reuse the otherwise unused eQueue data member.
        */
        me->super.thread = curr;
        me->super.eQueue.head = (QEQueueCtr)curr->prio;
        me->super.eQueue.tail = (QEQueueCtr)curr->pthre;

        QS_BEGIN_NOCRIT_PRE_(QS_MTX_LOCK, curr->prio)
            QS_TIME_PRE_();  /* timestamp */
            QS_OBJ_PRE_(me); /* this mutex */
            QS_2U8_PRE_((uint8_t)me->super.eQueue.head, /* holder prio */
                        (uint8_t)me->super.eQueue.nFree); /* nesting */
        QS_END_NOCRIT_PRE_()

        if (me->super.prio != 0U) { /* priority-ceiling protocol used? */
            /* the holder priority must be lower than that of the mutex
            * and the priority slot must be occupied by this mutex
            */
            Q_ASSERT_ID(210, (curr->prio < me->super.prio)
                && (QActive_registry_[me->super.prio] == &me->super));

            /* remove the thread's original prio from the ready set
            * and insert the mutex's prio into the ready set
            */
            QPSet_remove(&QF_readySet_, (uint_fast8_t)me->super.eQueue.head);
            QPSet_insert(&QF_readySet_, (uint_fast8_t)me->super.prio);

            /* put the thread into the AO registry in place of the mutex */
            QActive_registry_[me->super.prio] = curr;

            /* set thread's prio/pthre to that of the mutex */
            curr->prio  = me->super.prio;
            curr->pthre = me->super.pthre;
        }
    }
    /* is the mutex locked by this thread already (nested locking)? */
    else if (me->super.thread == curr) {
        /* the nesting level must not exceed the specified limit */
        Q_ASSERT_ID(320, me->super.eQueue.nFree < 0xFFU);

        ++me->super.eQueue.nFree; /* lock one more level */

        QS_BEGIN_NOCRIT_PRE_(QS_MTX_LOCK, curr->prio)
            QS_TIME_PRE_();  /* timestamp */
            QS_OBJ_PRE_(me); /* this mutex */
            QS_2U8_PRE_((uint8_t)me->super.eQueue.head, /* holder prio */
                        (uint8_t)me->super.eQueue.nFree); /* nesting */
        QS_END_NOCRIT_PRE_()
    }
    else { /* the mutex is already locked by a different thread */
        if (me->super.prio != 0U) {  /* priority-ceiling protocol used? */
            /* the prio slot must be occupied by the thr. holding the mutex */
            Q_ASSERT_ID(340, QActive_registry_[me->super.prio]
                             == QXK_PTR_CAST_(QActive *, me->super.thread));
        }

        QS_BEGIN_NOCRIT_PRE_(QS_MTX_BLOCK_ATTEMPT, curr->prio)
            QS_TIME_PRE_();  /* timestamp */
            QS_OBJ_PRE_(me); /* this mutex */
            QS_2U8_PRE_((uint8_t)me->super.eQueue.head, /* holder prio */
                        curr->prio); /* trying thread prio */
        QS_END_NOCRIT_PRE_()

        curr = (QActive *)0; /* means that mutex is NOT available */
    }
    QF_CRIT_X_();

    return curr != (QActive *)0;
}

/*${QXK::QXMutex::unlock} ..................................................*/
void QXMutex_unlock(QXMutex * const me) {
    QF_CRIT_STAT_
    QF_CRIT_E_();

    QActive *curr = QXK_attr_.curr;
    if (curr == (QActive *)0) { /* called from a basic thread? */
        curr = QActive_registry_[QXK_attr_.actPrio];
    }

    /*! @pre this function must:
    * - NOT be called from an ISR;
    * - the calling thread must be valid;
    */
    Q_REQUIRE_ID(400, (!QXK_ISR_CONTEXT_()) /* don't call from an ISR! */
        && (curr != (QActive *)0)); /* current thread must be valid */

    /*! @pre also: the mutex must be already locked at least once. */
    Q_REQUIRE_ID(401, me->super.eQueue.nFree > 0U);
    /*! @pre also: the mutex must be held by this thread. */
    Q_REQUIRE_ID(402, me->super.thread == curr);

    /* is this the last nesting level? */
    if (me->super.eQueue.nFree == 1U) {

        if (me->super.prio != 0U) { /* priority-ceiling protocol used? */

            /* restore the holding thread's prio/pthre from the mutex */
            curr->prio  = (uint8_t)me->super.eQueue.head;
            curr->pthre = (uint8_t)me->super.eQueue.tail;

            /* put the mutex back into the AO registry */
            QActive_registry_[me->super.prio] = &me->super;

            /* remove the mutex' prio from the ready set
            * and insert the original thread's priority
            */
            QPSet_remove(&QF_readySet_, (uint_fast8_t)me->super.prio);
            QPSet_insert(&QF_readySet_, (uint_fast8_t)me->super.eQueue.head);
        }

        QS_BEGIN_NOCRIT_PRE_(QS_MTX_UNLOCK, curr->prio)
            QS_TIME_PRE_();  /* timestamp */
            QS_OBJ_PRE_(me); /* this mutex */
            QS_2U8_PRE_((uint8_t)me->super.eQueue.head, /* holder prio */
                        0U); /* nesting */
        QS_END_NOCRIT_PRE_()

        /* are any other threads waiting on this mutex? */
        if (QPSet_notEmpty(&me->waitSet)) {
            /* find the highest-priority thread waiting on this mutex */
            uint_fast8_t const p = QPSet_findMax(&me->waitSet);

            /* remove this thread from waiting on the mutex
            * and insert it into the ready set.
            */
            QPSet_remove(&me->waitSet,  p);
            QPSet_insert(&QF_readySet_, p);

            QXThread * const thr =
                QXK_PTR_CAST_(QXThread*, QActive_registry_[p]);

            /* the waiting thread must:
            * - be registered in QF
            * - have the priority corresponding to the registration
            * - be an extended thread
            * - be blocked on this mutex
            */
            Q_ASSERT_ID(410, (thr != (QXThread *)0)
                && (thr->super.prio == (uint8_t)p)
                && (thr->super.super.state.act == Q_ACTION_CAST(0))
                && (thr->super.super.temp.obj
                    == QXK_PTR_CAST_(QMState*, me)));

            /* disarm the internal time event */
            (void)QXThread_teDisarm_(thr);

            /* set the new mutex holder to the curr thread and
            * save the thread's prio/pthre in the mutex
            * NOTE: reuse the otherwise unused eQueue data member.
            */
            me->super.thread = thr;
            me->super.eQueue.head = (QEQueueCtr)thr->super.prio;
            me->super.eQueue.tail = (QEQueueCtr)thr->super.pthre;

            QS_BEGIN_NOCRIT_PRE_(QS_MTX_LOCK, thr->super.prio)
                QS_TIME_PRE_();  /* timestamp */
                QS_OBJ_PRE_(me); /* this mutex */
                QS_2U8_PRE_((uint8_t)me->super.eQueue.head, /* holder prio */
                            (uint8_t)me->super.eQueue.nFree); /* nesting */
            QS_END_NOCRIT_PRE_()

            if (me->super.prio != 0U) { /* priority-ceiling protocol used? */
                /* the holder priority must be lower than that of the mutex */
                Q_ASSERT_ID(410, thr->super.prio < me->super.prio);

                /* set thread's preemption-threshold to that of the mutex */
                thr->super.pthre = me->super.pthre;

                /* put the thread into AO registry in place of the mutex */
                QActive_registry_[me->super.prio] = &thr->super;
            }
        }
        else { /* no threads are waiting for this mutex */
            me->super.eQueue.nFree = 0U; /* free up the nesting count */

            /* the mutex no longer held by any thread */
            me->super.thread = (void *)0;
            me->super.eQueue.head = 0U;
            me->super.eQueue.tail = 0U;

            if (me->super.prio != 0U) { /* priority-ceiling protocol used? */
                /* put the mutex back at the original mutex slot */
                QActive_registry_[me->super.prio] =
                    QXK_PTR_CAST_(QActive*, me);
            }
        }

        /* schedule the next thread if multitasking started */
        if (QXK_sched_() != 0U) { /* activation needed? */
            QXK_activate_(); /* synchronously activate basic-thred(s) */
        }
    }
    else { /* releasing one level of nested mutex lock */
        Q_ASSERT_ID(420, me->super.eQueue.nFree > 0U);
        --me->super.eQueue.nFree; /* unlock one level */

        QS_BEGIN_NOCRIT_PRE_(QS_MTX_UNLOCK_ATTEMPT, curr->prio)
            QS_TIME_PRE_();  /* timestamp */
            QS_OBJ_PRE_(me); /* this mutex */
            QS_2U8_PRE_((uint8_t)me->super.eQueue.head, /* holder prio */
                        (uint8_t)me->super.eQueue.nFree); /* nesting */
        QS_END_NOCRIT_PRE_()
    }
    QF_CRIT_X_();
}
/*$enddef${QXK::QXMutex} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
