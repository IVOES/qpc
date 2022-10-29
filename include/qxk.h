/*$file${include::qxk.h} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: qpc.qm
* File:  ${include::qxk.h}
*
* This code has been generated by QM 5.2.2 <www.state-machine.com/qm>.
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
/*$endhead${include::qxk.h} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*! @file
* @brief QXK/C (preemptive dual-mode kernel) platform-independent
* public interface.
*/
#ifndef QP_INC_QXK_H_
#define QP_INC_QXK_H_

/*==========================================================================*/
/* QF configuration for QXK -- data members of the QActive class... */

/* QXK event-queue used for AOs */
#define QF_EQUEUE_TYPE     QEQueue

/* QXK OS-object used to store the private stack pointer for extended threads.
* (The private stack pointer is NULL for basic-threads).
*/
#define QF_OS_OBJECT_TYPE  void*

/* QXK thread type used to store the private Thread-Local Storage pointer */
#define QF_THREAD_TYPE     void*

/*! Access Thread-Local Storage (TLS) and cast it on the given `type_` */
#define QXK_TLS(type_) ((type_)QXK_current()->thread)

/*==========================================================================*/
#include "qequeue.h"  /* QXK kernel uses the native QP event queue  */
#include "qmpool.h"   /* QXK kernel uses the native QP memory pool  */
#include "qf.h"       /* QF framework integrates directly with QXK  */

/*==========================================================================*/
/*$declare${QXK::QXK-base} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QXK::QXK-base::onIdle} .................................................*/
/*! QXK idle callback (customized in BSPs for QXK)
* @static @public @memberof QXK
*
* @details
* QXK_onIdle() is called continuously by the QXK idle thread. This callback
* gives the application an opportunity to enter a power-saving CPU mode,
* or perform some other idle processing.
*
* @note
* QXK_onIdle() is invoked with interrupts enabled and must also return with
* interrupts enabled.
*/
void QXK_onIdle(void);

/*${QXK::QXK-base::schedLock} ..............................................*/
/*! QXK Scheduler lock
* @static @public @memberof QXK
*
* @details
* This function locks the QXK scheduler to the specified ceiling.
*
* @param[in] ceiling  preemption ceiling to which the QXK scheduler
*                     needs to be locked
*
* @returns
* The previous QXK Scheduler lock status, which is to be used to unlock
* the scheduler by restoring its previous lock status in
* QXK_schedUnlock().
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
QSchedStatus QXK_schedLock(uint_fast8_t const ceiling);

/*${QXK::QXK-base::schedUnlock} ............................................*/
/*! QXK Scheduler unlock
* @static @public @memberof QXK
*
* @details
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
void QXK_schedUnlock(QSchedStatus const stat);

/*${QXK::QXK-base::Timeouts} ...............................................*/
/*! timeout signals for extended threads */
enum QXK_Timeouts {
    QXK_DELAY_SIG = 1,
    QXK_TIMEOUT_SIG
};
/*$enddecl${QXK::QXK-base} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${QXK::QXK-extern-C} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QXK::QXK-extern-C::Attr} ...............................................*/
/*! @brief The QXK kernel class
* @class QXK
*
* @note
* The order and alignment of the data members in this struct might
* be important in QXK ports, where the members might be accessed
* in assembly.
*/
typedef struct QXK_Attr {
    struct QActive * volatile curr; /*!< current thread (NULL=basic) */
    struct QActive * volatile next; /*!< next thread to run */
    struct QActive * volatile prev; /*!< previous thread */
    uint8_t volatile actPrio;       /*!< QF-prio of the active AO */
    uint8_t volatile lockCeil;      /*!< lock-ceiling (0==no-lock) */
    uint8_t volatile lockHolder;    /*!< prio of the lock holder */
} QXK;

/*${QXK::QXK-extern-C::attr_} ..............................................*/
/*! attributes of the QXK kernel */
extern QXK QXK_attr_;

/*${QXK::QXK-extern-C::sched_} .............................................*/
/*! QXK scheduler finds the highest-priority thread ready to run
* @static @private @memberof QXK
*
* @details
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
uint_fast8_t QXK_sched_(void);

/*${QXK::QXK-extern-C::activate_} ..........................................*/
/*! QXK activator activates the next active object. The activated AO
* preempts the currently executing AOs.
* @static @private @memberof QXK
*
* @details
* QXK_activate_() activates ready-to run AOs that are above the initial
* active priority (QXK_attr_.actPrio).
*
* @attention
* QXK_activate_() must be always called with interrupts **disabled** and
* returns with interrupts **disabled**.
*/
void QXK_activate_(void);

/*${QXK::QXK-extern-C::current} ............................................*/
/*! obtain the currently executing active-object/thread
* @static @public @memberof QXK
*
* @returns
* pointer to the currently executing active-object/thread
*/
QActive * QXK_current(void);

/*${QXK::QXK-extern-C::stackInit_} .........................................*/
/*! initialize the private stack of a given AO (defined in QXK port) */
void QXK_stackInit_(
    void * thr,
     QXThreadHandler const handler,
    void * const stkSto,
    uint_fast16_t const stkSize);

/*${QXK::QXK-extern-C::contextSw} ..........................................*/
#if defined(Q_SPY) || defined(QXK_ON_CONTEXT_SW)
/*! QXK context switch management
* @static @public @memberof QXK
*
* @details
* This function performs software tracing (if #Q_SPY is defined)
* and calls QXK_onContextSw() (if #QXK_ON_CONTEXT_SW is defined)
*
* @param[in] next  pointer to the next thread (NULL for basic-thread)
*
* @attention
* QXK_contextSw() is invoked with interrupts **disabled** and must also
* return with interrupts **disabled**.
*/
void QXK_contextSw(QActive * const next);
#endif /*  defined(Q_SPY) || defined(QXK_ON_CONTEXT_SW) */

/*${QXK::QXK-extern-C::onContextSw} ........................................*/
#ifdef QXK_ON_CONTEXT_SW
/*! QXK context switch callback (customized in BSPs for QXK)
* @static @public @memberof QXK
*
* @details
* This callback function provides a mechanism to perform additional
* custom operations when QXK switches context from one thread to
* another.
*
* @param[in] prev   pointer to the previous thread (NULL for idle thead)
* @param[in] next   pointer to the next thread (NULL for idle thead)
*
* @attention
* QXK_onContextSw() is invoked with interrupts **disabled** and must also
* return with interrupts **disabled**.
*
* @note
* This callback is enabled by defining the macro #QXK_ON_CONTEXT_SW.
*
* @include qxk_oncontextsw.c
*/
void QXK_onContextSw(
    QActive * prev,
    QActive * next);
#endif /* def QXK_ON_CONTEXT_SW */
/*$enddecl${QXK::QXK-extern-C} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${QXK::QXThread} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QXK::QXThread} .........................................................*/
/*! @brief eXtended (blocking) thread of the QXK preemptive kernel
* @class QXThread
* @extends QActive
*
* @details
* ::QXThread represents the eXtended (blocking) thread of the QXK
* kernel. Each extended thread in the application must be represented
* by the corresponding ::QXThread instance
*
* @note
* Typically, ::QXThread is instantiated directly in the application code.
* The customization of the thread occurs in the QXThread_ctor(), where you
* provide the thread-handler function as the parameter.
*
* @usage
* The following example illustrates how to instantiate and use an extended
* thread in your application.
* @include qxk_thread.c
*/
typedef struct QXThread {
/* protected: */
    QActive super;

/* private: */

    /*! time event to handle blocking timeouts */
    QTimeEvt timeEvt;
} QXThread;

/* public: */

/*! constructor of an extended-thread
* @public @memberof QXThread
*
* @details
* Performs the first step of QXThread initialization by assigning the
* thread-handler function and the tick rate at which it will handle
* the timeouts.
*
* @param[in,out] me       pointer (see @ref oop)
* @param[in]     handler  the thread-handler function
* @param[in]     tickRate the ticking rate for timeouts in this thread
*                (see QXThread_delay() and QTIMEEVT_TICK_X())
*
* @note
* Must be called only ONCE before QXTHREAD_START().
*
* @usage
* The following example illustrates how to invoke QXThread_ctor() in the
* main() function
*
* @include
* qxk_thread_ctor.c
*/
void QXThread_ctor(QXThread * const me,
    QXThreadHandler const handler,
    uint_fast8_t const tickRate);

/*! delay (block) the current extended thread for a specified # ticks
* @static @public @memberof QXThread
*
* @details
* Blocking delay for the number of clock tick at the associated tick rate.
*
* @param[in]  nTicks    number of clock ticks (at the associated rate)
*                       to wait for the event to arrive.
* @note
* For the delay to work, the QTIMEEVT_TICK_X() macro needs to be called
* periodically at the associated clock tick rate.
*
* @sa QXThread_ctor()
* @sa QTIMEEVT_TICK_X()
*/
bool QXThread_delay(uint_fast16_t const nTicks);

/*! cancel the delay
* @public @memberof QXThread
*
* @details
* Cancel the blocking delay and cause return from the QXThread_delay()
* function.
*
* @returns
* "true" if the thread was actually blocked on QXThread_delay() and
* "false" otherwise.
*/
bool QXThread_delayCancel(QXThread * const me);

/*! obtain a message from the private message queue (block if no messages)
* @static @public @memberof QXThread
*
* @details
* The QXThread_queueGet() operation allows the calling extended thread to
* receive QP events directly into its own built-in event queue from an ISR,
* basic thread (AO), or another extended thread.
*
* If QXThread_queueGet() is called when no events are present in the
* thread's private event queue, the operation blocks the current extended
* thread until either an event is received, or a user-specified timeout
* expires.
*
* @param[in]  nTicks number of clock ticks (at the associated rate)
*                    to wait for the event to arrive. The value of
*                    ::QXTHREAD_NO_TIMEOUT indicates that no timeout will
*                    occur and the queue will block indefinitely.
* @returns
* A pointer to the event. If the pointer is not NULL, the event was delivered.
* Otherwise the event pointer of NULL indicates that the queue has timed out.
*/
QEvt const * QXThread_queueGet(uint_fast16_t const nTicks);

/* protected: */

/*! Overrides QHsm_init_() */
void QXThread_init_(
    QHsm * const me,
    void const * const par,
    uint_fast8_t const qs_id);

/*! Overrides QHsm_dispatch_() */
void QXThread_dispatch_(
    QHsm * const me,
    QEvt const * const e,
    uint_fast8_t const qs_id);

/*! start QXThread private implementation
* @private @memberof QXThread
*
* @details
* Starts execution of an extended thread and registers it with the framework.
* The extended thread becomes ready-to-run immediately and is scheduled
* if the QXK is already running.
*
* @param[in,out] me      pointer (see @ref oop)
* @param[in]     prio    QF-priority of the thread, but no preemption-
*                        threshold. See also ::QPrioSpec.
* @param[in]     qSto    pointer to the storage for the ring buffer of the
*                        event queue. This cold be NULL, if this extended
*                        thread does not use the built-in event queue.
* @param[in]     qLen    length of the event queue [in events],
*                        or zero if queue not used
* @param[in]     stkSto  pointer to the stack storage (must be provided)
* @param[in]     stkSize stack size [in bytes] (must not be zero)
* @param[in]     par     pointer to an extra parameter (might be NULL).
*
* @note
* Currently, extended trheads in QXK do NOT support preemption-threshold.
* The `prio` must NOT provide preemption-threshold and this function
* will assert it in the precondition.
*
* @usage
* QXThread_start_() should NOT be called directly, only via the macro
* QXTHREAD_START(). The following example shows starting an extended
* thread:
* @include qxk_start.c
*/
void QXThread_start_(
    QActive * const me,
    QPrioSpec const prioSpec,
    QEvt const * * const qSto,
    uint_fast16_t const qLen,
    void * const stkSto,
    uint_fast16_t const stkSize,
    void const * const par);

/*! post to the QXThread event queue private implementation
* @private @memberof QXThread
*
* @details
* Direct event posting is the simplest asynchronous communication method
* available in QF. The following example illustrates how the Philo active
* object posts directly the HUNGRY event to the Table active object.
* <br>
* The parameter `margin` specifies the minimum number of free slots in
* the queue that must be available for posting to succeed. The function
* returns 1 (success) if the posting succeeded (with the provided margin)
* and 0 (failure) when the posting fails.
*
* @param[in,out] me     pointer (see @ref oop)
* @param[in]     e      pointer to the event to be posted
* @param[in]     margin number of required free slots in the queue after
*                       posting the event. The special value #QF_NO_MARGIN
*                       means that this function will assert if posting fails.
* @param[in]     sender pointer to a sender object (used only for QS tracing).
*
* @returns
* 'true' (success) if the posting succeeded (with the provided margin) and
* 'false' (failure) when the posting fails.
*
* @note
* Should be called only via the macro QXTHREAD_POST_X().
*
* @note
* The #QF_NO_MARGIN value of the `margin` parameter is special and
* denotes situation when the post() operation is assumed to succeed
* (event delivery guarantee). An assertion fires, when the event cannot
* be delivered in this case.
*
* @note
* For compatibility with the V-table from the superclass ::QActive, the
* me-pointer is typed as pointing to QActive. However, the `me` pointer
* here actually points to the QXThread subclass. Therefore the downcast
* (QXThread *)me is always correct.
*/
bool QXThread_post_(
    QActive * const me,
    QEvt const * const e,
    uint_fast16_t const margin,
    void const * const sender);

/*! post to the QXThread event queue (LIFO) private implementation
* @private @memberof QXThread
*
* @details
* Last-In-First-Out (LIFO) policy is not supported for extened threads.
*
* @param[in] me pointer (see @ref oop)
* @param[in] e  pointer to the event to post to the queue
*
* @sa
* QActive_postLIFO_()
*/
void QXThread_postLIFO_(
    QActive * const me,
    QEvt const * const e);

/* private: */

/*! block QXThread private implementation
* @private @memberof QXThread
*
* @details
* Internal implementation of blocking the given extended thread.
*
* @note
* Must be called from within a critical section
*/
void QXThread_block_(QXThread const * const me);

/*! unblock QXThread private implementation
* @private @memberof QXThread
*
* @details
* Internal implementation of un-blocking the given extended thread.
*
* @note
* must be called from within a critical section
*/
void QXThread_unblock_(QXThread const * const me);

/*! arm internal time event private implementation
* @private @memberof QXThread
*
* @details
* Internal implementation of arming the private time event for a given
* timeout at a given system tick rate.
*
* @note
* Must be called from within a critical section
*/
void QXThread_teArm_(QXThread * const me,
    enum_t const sig,
    uint_fast16_t const nTicks);

/*! disarm internal time event private implementation
* @private @memberof QXThread
*
* @details
* Internal implementation of disarming the private time event.
*
* @note
* Must be called from within a critical section
*/
bool QXThread_teDisarm_(QXThread * const me);

/*! dummy static to force generation of "struct QXThread" */
extern QXThread const * QXThread_dummy;
/*$enddecl${QXK::QXThread} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${QXK::QXThreadVtable} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QXK::QXThreadVtable} ...................................................*/
/*! @brief Virtual Table for the ::QXThread class
* (inherited from ::QActiveVtable)
*
* @note
* ::QXThread inherits ::QActive without adding any new virtual
* functions and therefore, ::QXThreadVtable is typedef'ed as ::QActiveVtable.
*/
typedef QActiveVtable QXThreadVtable;
/*$enddecl${QXK::QXThreadVtable} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${QXK::QXSemaphore} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QXK::QXSemaphore} ......................................................*/
/*! @brief Counting Semaphore of the QXK preemptive kernel
* @class QXSemaphore
*
* @details
* ::QXSemaphore is a blocking mechanism intended primarily for signaling
* @ref ::QXThread "extended threads". The semaphore is initialized with
* the maximum count (see QXSemaphore_init()), which allows you to create
* a binary semaphore (when the maximum count is 1) and
* counting semaphore when the maximum count is > 1.
*
* @usage
* The following example illustrates how to instantiate and use the semaphore
* in your application.
* @include qxk_sema.c
*/
typedef struct {
/* private: */

    /*! set of extended threads waiting on this semaphore */
    QPSet waitSet;

    /*! semaphore up-down counter */
    uint8_t volatile count;

    /*! maximum value of the semaphore counter */
    uint8_t max_count;
} QXSemaphore;

/* public: */

/*! initialize the counting semaphore
* @public @memberof QXSemaphore
*
* @details
* Initializes a semaphore with the specified count and maximum count.
* If the semaphore is used for resource sharing, both the initial count
* and maximum count should be set to the number of identical resources
* guarded by the semaphore. If the semaphore is used as a signaling
* mechanism, the initial count should set to 0 and maximum count to 1
* (binary semaphore).
*
* @param[in,out] me     pointer (see @ref oop)
* @param[in]     count  initial value of the semaphore counter
* @param[in]     max_count  maximum value of the semaphore counter.
*                The purpose of the max_count is to limit the counter
*                so that the semaphore cannot unblock more times than
*                the maximum.
*
* @note
* QXSemaphore_init() must be called **before** the semaphore can be used
* (signaled or waited on).
*/
void QXSemaphore_init(QXSemaphore * const me,
    uint_fast8_t const count,
    uint_fast8_t const max_count);

/*! wait (block) on the semaphore
* @public @memberof QXSemaphore
*
* @details
* When an extended thread calls QXSemaphore_wait() and the value of the
* semaphore counter is greater than 0, QXSemaphore_wait() decrements the
* semaphore counter and returns (true) to its caller. However, if the value
* of the semaphore counter is 0, the function places the calling thread in
* the waiting list for the semaphore. The thread waits until the semaphore
* is signaled by calling QXSemaphore_signal(), or the specified timeout
* expires. If the semaphore is signaled before the timeout expires, QXK
* resumes the highest-priority extended thread waiting for the semaphore.
*
* @param[in,out] me     pointer (see @ref oop)
* @param[in]     nTicks number of clock ticks (at the associated rate)
*                       to wait for the semaphore. The value of
*                       ::QXTHREAD_NO_TIMEOUT indicates that no timeout will
*                       occur and the semaphore will wait indefinitely.
*
* @returns
* 'true' if the semaphore has been signaled and 'false' if a timeout
* occurred.
*
* @note
* Multiple extended threads can wait for a given semaphore.
*/
bool QXSemaphore_wait(QXSemaphore * const me,
    uint_fast16_t const nTicks);

/*! try wait on the semaphore (non-blocking)
* @public @memberof QXSemaphore
*
* @details
* This function checks if the semaphore counter is greater than 0,
* in which case the counter is decremented.
*
* @param[in,out] me     pointer (see @ref oop)
*
* @returns
* 'true' if the semaphore has count available and 'false' NOT available.
*
* @note
* This function can be called from any context, including ISRs and basic
* threads (active objects).
*/
bool QXSemaphore_tryWait(QXSemaphore * const me);

/*! signal (unblock) the semaphore
* @public @memberof QXSemaphore
*
* @details
* If the semaphore counter value is 0 or more, it is incremented, and
* this function returns to its caller. If the extended threads are waiting
* for the semaphore to be signaled, QXSemaphore_signal() removes the highest-
* priority thread waiting for the semaphore from the waiting list and makes
* this thread ready-to-run. The QXK scheduler is then called to determine if
* the awakened thread is now the highest-priority thread that is ready-to-run.
*
* @param[in,out] me     pointer (see @ref oop)
*
* @returns
* 'true' when the semaphore signaled and 'false' when the semaphore count
* exceeded the maximum.
*
* @note
* A semaphore can be signaled from many places, including from ISRs, basic
* threads (AOs), and extended threads.
*/
bool QXSemaphore_signal(QXSemaphore * const me);
/*$enddecl${QXK::QXSemaphore} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${QXK::QXMutex} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QXK::QXMutex} ..........................................................*/
/*! @brief Blocking Mutex the QXK preemptive kernel
* @class QXMutex
* @extends QActive
*
* @details
* ::QXMutex is a blocking mutual exclusion mechanism that can also apply
* the **priority-ceiling protocol** to avoid unbounded priority inversion
* (if initialized with a non-zero ceiling priority, see QXMutex_init()).
* In that case, ::QXMutex requires its own uinque QP priority level, which
* cannot be used by any thread or any other ::QXMutex.
* If initialized with preemption-ceiling of zero, ::QXMutex does **not**
* use the priority-ceiling protocol and does not require a unique QP
* priority (see QXMutex_init()).
* ::QXMutex is **recursive** (re-entrant), which means that it can be locked
* multiple times (up to 255 levels) by the *same* thread without causing
* deadlock.
* ::QXMutex is primarily intended for the @ref ::QXThread
* "extened (blocking) threads", but can also be used by the @ref ::QActive
* "basic threads" through the non-blocking QXMutex_tryLock() API.
*
* @note
* ::QXMutex should be used in situations when at least one of the extended
* threads contending for the mutex blocks while holding the mutex (between
* the QXMutex_lock() and QXMutex_unlock() operations). If no blocking is
* needed while holding the mutex, the more efficient non-blocking mechanism
* of @ref srs_qxk_schedLock() "selective QXK scheduler locking" should be used
* instead. @ref srs_qxk_schedLock() "Selective scheduler locking" is available
* for both @ref ::QActive "basic threads" and @ref ::QXThread "extended
* threads", so it is applicable to situations where resources are shared
* among all these threads.
*
* @usage
* The following example illustrates how to instantiate and use the mutex
* in your application.
* @include qxk_mutex.c
*/
typedef struct {
/* protected: */
    QActive super;

/* private: */

    /*! set of extended-threads waiting on this mutex */
    QPSet waitSet;
} QXMutex;

/* public: */

/*! initialize the QXK priority-ceiling mutex ::QXMutex
* @public @memberof QXMutex
*
* @details
* Initialize the QXK priority ceiling mutex.
*
* @param[in,out] me    pointer (see @ref oop)
* @param[in] prioSpec  the priority specification for the mutex
*                      (See also ::QPrioSpec). This value might
*                      also be zero.
* @note
* `prioSpec == 0` means that the priority-ceiling protocol shall **not**
* be used by this mutex. Such mutex will **not** change (boost) the
* priority of the holding threads.<br>
*
* Conversely, `prioSpec != 0` means that the priority-ceiling protocol
* shall be used by this mutex. Such mutex **will** temporarily boost
* the priority and priority-threshold of the holding thread to the
* priority specification in `prioSpec` (see ::QPrioSpec).
*
* @usage
* @include qxk_mutex.c
*/
void QXMutex_init(QXMutex * const me,
    QPrioSpec const prioSpec);

/*! lock the QXK priority-ceiling mutex ::QXMutex
* @public @memberof QXMutex
*
* @param[in,out] me   pointer (see @ref oop)
* @param[in]  nTicks  number of clock ticks (at the associated rate)
*                     to wait for the mutex. The value of
*                     ::QXTHREAD_NO_TIMEOUT indicates that no timeout will
*                     occur and the mutex could block indefinitely.
* @returns
* 'true' if the mutex has been acquired and 'false' if a timeout occurred.
*
* @note
* The mutex locks are allowed to nest, meaning that the same extended thread
* can lock the same mutex multiple times (< 255). However, each call to
* QXMutex_lock() must be balanced by the matching call to QXMutex_unlock().
*
* @usage
* @include qxk_mutex.c
*/
bool QXMutex_lock(QXMutex * const me,
    uint_fast16_t const nTicks);

/*! try to lock the QXK priority-ceiling mutex ::QXMutex
* @public @memberof QXMutex
*
* @param[in,out] me      pointer (see @ref oop)
*
* @returns
* 'true' if the mutex was successfully locked and 'false' if the mutex was
* unavailable and was NOT locked.
*
* @note
* This function **can** be called from both basic threads (active objects)
* and extended threads.
*
* @note
* The mutex locks are allowed to nest, meaning that the same extended thread
* can lock the same mutex multiple times (<= 255). However, each successful
* call to QXMutex_tryLock() must be balanced by the matching call to
* QXMutex_unlock().
*/
bool QXMutex_tryLock(QXMutex * const me);

/*! unlock the QXK priority-ceiling mutex ::QXMutex
* @public @memberof QXMutex
*!
* @param[in,out] me      pointer (see @ref oop)
*
* @note
* This function **can** be called from both basic threads (active objects)
* and extended threads.
*
* @note
* The mutex locks are allowed to nest, meaning that the same extended thread
* can lock the same mutex multiple times (<= 225). However, each call to
* QXMutex_lock() or a *successful* call to QXMutex_tryLock() must be
* balanced by the matching call to QXMutex_unlock().
*
* @usage
* @include qxk_mutex.c
*/
void QXMutex_unlock(QXMutex * const me);
/*$enddecl${QXK::QXMutex} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${QXK-macros} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QXK-macros::QXTHREAD_START} ............................................*/
/*! Virtual call to start an extened thread
*
* @details
* Starts execution of the thread and registers the AO with the framework.
*
* @param[in,out] me_   pointer (see @ref oop)
* @param[in] prioSpec_ priority specification at which to start the
*                      extended thread (see ::QPrioSpec)
* @param[in] qSto_     pointer to the storage for the ring buffer of the
*                      event queue (used only with the built-in ::QEQueue)
* @param[in] qLen_     length of the event queue (in events)
* @param[in] stkSto_   pointer to the stack storage (used only when
*                      per-AO stack is needed)
* @param[in] stkSize_  stack size (in bytes)
* @param[in] par_      pointer to the additional port-specific parameter(s)
*                      (might be NULL).
* @usage
* @include qxk_start.c
*/
#define QXTHREAD_START(me_, prioSpec_, qSto_, qLen_, stkSto_, stkSize_, par_) \
do { \
    Q_ASSERT((me_)->super.super.vptr); \
    ((*((QActiveVtable const *)((me_)->super.super.vptr))->start)( \
        &(me_)->super, (prioSpec_), (QEvt const **)(qSto_), (qLen_), \
        (stkSto_), (stkSize_), (par_))); \
} while (false)

/*${QXK-macros::QXTHREAD_NO_TIMEOUT} .......................................*/
/*! No-timeout when blocking on semaphores, mutextes, and queues */
#define QXTHREAD_NO_TIMEOUT ((uint_fast16_t)0)

/*${QXK-macros::QXTHREAD_POST_X} ...........................................*/
/*! Asynchronous posting events to the event queue of an eXtended thread
* @details
* This macro does not assert if the queue overflows and cannot accept
* the event with the specified margin of free slots remaining.
*
* @param[in,out] me_   pointer (see @ref oop)
* @param[in]     e_    pointer to the event to post
* @param[in]     margin_ the minimum free slots in the queue, which
*                      must still be available after posting the event.
*                      The special value #QF_NO_MARGIN causes asserting
*                      failure in case event allocation fails.
* @param[in]     sender_ pointer to the sender object (used in QS tracing)
*
* @returns
* 'true' if the posting succeeded, and 'false' if the posting failed due
* to insufficient margin of free slots available in the queue.
*
* @note
* The `sender_` parameter is actually only used when QS tracing is enabled
* (macro #Q_SPY is defined). When QS software tracing is disabled, the
* QXTHREAD_POST_X() macro does not pass the `sender_` parameter, so the
* overhead of passing this extra argument is entirely avoided.
*
* @note
* The pointer to the sender object is not necessarily a pointer to an
* active object. In fact, if QXTHREAD_POST_X() is called from an interrupt
* or other context, you can create a unique object just to unambiguously
* identify the sender of the event.
*
* @usage
* @include qf_postx.c
*/
#define QXTHREAD_POST_X(me_, e_, margin_, sender_) \
    QACTIVE_POST_X(&(me_)->super, (e_), (margin_), (sender_))
/*$enddecl${QXK-macros} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*==========================================================================*/
/* interface used only inside QP implementation, but not in applications */
#ifdef QP_IMPL
/* QXK implementation... */
/*$declare${QXK-impl} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QXK-impl::QXK_ISR_CONTEXT_} ............................................*/
#ifndef QXK_ISR_CONTEXT_
/*! Internal macro that reports the execution context (ISR vs. thread)
*
* @returns true if the code executes in the ISR context and false
* otherwise
*/
#define QXK_ISR_CONTEXT_() (QF_intNest_ != 0U)
#endif /* ndef QXK_ISR_CONTEXT_ */

/*${QXK-impl::QF_SCHED_STAT_} ..............................................*/
/*! QXK scheduler lock status */
#define QF_SCHED_STAT_ QSchedStatus lockStat_;

/*${QXK-impl::QF_SCHED_LOCK_} ..............................................*/
/*! QXK selective scheduler locking */
#define QF_SCHED_LOCK_(ceil_) do { \
    if (QXK_ISR_CONTEXT_()) { \
        lockStat_ = 0xFFU; \
    } else { \
        lockStat_ = QXK_schedLock((ceil_)); \
    } \
} while (false)

/*${QXK-impl::QF_SCHED_UNLOCK_} ............................................*/
/*! QXK selective scheduler unlocking */
#define QF_SCHED_UNLOCK_() do { \
    if (lockStat_ != 0xFFU) { \
        QXK_schedUnlock(lockStat_); \
    } \
} while (false)

/*${QXK-impl::QACTIVE_EQUEUE_WAIT_} ........................................*/
/*! QXK native event queue waiting */
#define QACTIVE_EQUEUE_WAIT_(me_) \
    (Q_ASSERT_ID(110, (me_)->eQueue.frontEvt != (QEvt *)0))

/*${QXK-impl::QACTIVE_EQUEUE_SIGNAL_} ......................................*/
/*! QXK native event queue signaling */
#define QACTIVE_EQUEUE_SIGNAL_(me_) do { \
    QPSet_insert(&QF_readySet_, (uint_fast8_t)(me_)->prio); \
    if (!QXK_ISR_CONTEXT_()) { \
        if (QXK_sched_() != 0U) { \
            QXK_activate_(); \
        } \
    } \
} while (false)

/*${QXK-impl::QXK_PTR_CAST_} ...............................................*/
/*! internal macro to encapsulate casting of pointers for MISRA deviations
*
* @details
* This macro is specifically and exclusively used for casting pointers
* that are never de-referenced, but only used for internal bookkeeping and
* checking (via assertions) the correct operation of the QXK kernel.
* Such pointer casting is not compliant with MISRA-2012-Rule 11.3(req)
* as well as other messages (e.g., PC-Lint-Plus warning 826).
* Defining this specific macro for this purpose allows to selectively
* disable the warnings for this particular case.
*/
#define QXK_PTR_CAST_(type_, ptr_) ((type_)(ptr_))

/*${QXK-impl::QXTHREAD_CAST_} ..............................................*/
/*! internal macro to encapsulate casting of pointers for MISRA deviations
*
* @details
* This macro is specifically and exclusively used for downcasting pointers
* to QActive to pointers to QXThread in situations when it is known
* that such downcasting is correct.However, such pointer casting is not
* compliant with MISRA-2012-Rule 11.3(req) as well as other messages (e.g.,
* PC-Lint-Plus warning 826). Defining this specific macro for this purpose
* allows to selectively disable the warnings for this particular case.
*/
#define QXTHREAD_CAST_(ptr_) ((QXThread *)(ptr_))

/*${QXK-impl::QXK_threadExit_} .............................................*/
/*! called when QXThread exits
* @private @memberof QXThread
*
* @details
* Called when the extended-thread handler function returns.
*
* @note
* Most thread handler functions are structured as endless loops that never
* return. But it is also possible to structure threads as one-shot functions
* that perform their job and return. In that case this function peforms
* cleanup after the thread.
*/
void QXK_threadExit_(void);
/*$enddecl${QXK-impl} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/* Native QF event pool operations... */
/*$declare${QF-QMPool-impl} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QF-QMPool-impl::QF_EPOOL_TYPE_} ........................................*/
#define QF_EPOOL_TYPE_ QMPool

/*${QF-QMPool-impl::QF_EPOOL_INIT_} ........................................*/
#define QF_EPOOL_INIT_(p_, poolSto_, poolSize_, evtSize_) \
    (QMPool_init(&(p_), (poolSto_), (poolSize_), (evtSize_)))

/*${QF-QMPool-impl::QF_EPOOL_EVENT_SIZE_} ..................................*/
#define QF_EPOOL_EVENT_SIZE_(p_) ((uint_fast16_t)(p_).blockSize)

/*${QF-QMPool-impl::QF_EPOOL_GET_} .........................................*/
#define QF_EPOOL_GET_(p_, e_, m_, qs_id_) \
    ((e_) = (QEvt *)QMPool_get(&(p_), (m_), (qs_id_)))

/*${QF-QMPool-impl::QF_EPOOL_PUT_} .........................................*/
#define QF_EPOOL_PUT_(p_, e_, qs_id_) \
    (QMPool_put(&(p_), (e_), (qs_id_)))
/*$enddecl${QF-QMPool-impl} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#endif /* QP_IMPL */

#endif /* QP_INC_QXK_H_ */
