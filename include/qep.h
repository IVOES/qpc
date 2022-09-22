/*$file${include::qep.h} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: qpc.qm
* File:  ${include::qep.h}
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
/*$endhead${include::qep.h} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*! @file
* @brief QEP/C platform-independent public interface.
*
* @tr{RQP001} @tr{RQP101}
*/
#ifndef QP_INC_QEP_H_
#define QP_INC_QEP_H_

/*==========================================================================*/
/*! The current QP version as an unsigned number
*
* @details
* ::QP_VERSION is a decimal constant, where XX is a 1-digit or 2-digit
* major version number, Y is a 1-digit minor version number, and Z is
* a 1-digit release number.
*/
#define QP_VERSION 711U

/*! The current QP version as a zero terminated string literal.
*
* @details
* ::QP_VERSION_STR is of the form "XX.Y.Z", where XX is a 1-or 2-digit
* major version number, Y is a 1-digit minor version number, and Z is
* a 1-digit release number.
*/
#define QP_VERSION_STR "7.1.1"

/*! Encrypted  current QP release (7.1.1) and date (2022-09-30) */
#define QP_RELEASE 0x7C50BF18U

/*==========================================================================*/
/*$declare${glob-types} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${glob-types::int_t} .....................................................*/
/*! alias for line numbers in assertions and return from QF_run() */
typedef int int_t;

/*${glob-types::enum_t} ....................................................*/
/*! alias for enumerations used for event signals */
typedef int enum_t;

/*${glob-types::float32_t} .................................................*/
/*! alias for IEEE 754 32-bit floating point number,
* MISRA-C 2012 Dir 4.6(A)
*
* @note
* QP does not use floating-point types anywhere in the internal
* implementation, except in QS software tracing, where utilities for
* output of floating-point numbers are provided for application-specific
* trace records.
*/
typedef float float32_t;

/*${glob-types::float64_t} .................................................*/
/*! alias for IEEE 754 64-bit floating point number,
* MISRA-C 2012 Dir 4.6(A)
*
* @note
* QP does not use floating-point types anywhere in the internal
* implementation, except in QS software tracing, where utilities for
* output of floating-point numbers are provided for application-specific
* trace records.
*/
typedef double float64_t;
/*$enddecl${glob-types} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${QEP-config} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QEP-config::Q_SIGNAL_SIZE} .............................................*/
/*! The size (in bytes) of the signal of an event. Valid values:
* 1U, 2U, or 4U; default 2U
*
* @details
* This macro can be defined in the QEP port file (qep_port.h) to
* configure the ::QSignal type. When the macro is not defined, the
* default of 2 bytes is applied.
*/
#ifndef Q_SIGNAL_SIZE
#define Q_SIGNAL_SIZE 2U
#endif /* ndef Q_SIGNAL_SIZE */
/*$enddecl${QEP-config} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*==========================================================================*/
/*$declare${QEP} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QEP::QP_versionStr[8]} .................................................*/
/*! the current QP version number string in ROM, based on #QP_VERSION_STR */
extern char const QP_versionStr[8];

/*${QEP::QSignal} ..........................................................*/
/*! ::QSignal represents the signal of an event
*
* @details
* The relationship between an event and a signal is as follows. A signal
* in UML is the specification of an asynchronous stimulus that triggers
* reactions, and as such is an essential part of an event. (The signal
* conveys the type of the occurrence--what happened?) However, an event
* can also contain additional quantitative information about the
* occurrence in form of event parameters.
*/
#if (Q_SIGNAL_SIZE == 2U)
typedef uint16_t QSignal;
#endif /*  (Q_SIGNAL_SIZE == 2U) */

/*${QEP::QSignal} ..........................................................*/
#if (Q_SIGNAL_SIZE == 1U)
typedef uint8_t QSignal;
#endif /*  (Q_SIGNAL_SIZE == 1U) */

/*${QEP::QSignal} ..........................................................*/
#if (Q_SIGNAL_SIZE == 4U)
typedef uint16_t QSignal;
#endif /*  (Q_SIGNAL_SIZE == 4U) */

/*${QEP::QEvt} .............................................................*/
/*! @brief Event class
* @class QEvt
*
* @details
* ::QEvt represents events without parameters and serves as the base class
* for derivation of events with parameters.
*
* @tr{RQP001} @tr{RQP004}
* @tr{AQP210}
*
* @usage
* The following example illustrates how to add an event parameter by
* derivation of the ::QEvt class. Please note that the ::QEvt member
* super is defined as the FIRST member of the derived struct.
* @include qep_qevt.c
*/
typedef struct {
/* public: */

    /*! Signal of the event.
    * @public @memberof QEvt
    *
    * @tr{RQP002}
    */
    QSignal sig;

/* private: */

    /*! Pool ID (==0 for immutable event)
    * @private @memberof QEvt
    *
    * @tr{RQP003}
    */
    uint8_t poolId_;

    /*! Reference counter (for mutable events)
    * @private @memberof QEvt
    *
    * @tr{RQP003}
    */
    uint8_t volatile refCtr_;
} QEvt;

/* public: */

/*! Custom event constructor
* @public @memberof QEvt
*
* @tr{RQP005}
*/
#ifdef Q_EVT_CTOR
void QEvt_ctor(QEvt * const me,
    enum_t const sig);
#endif /* def Q_EVT_CTOR */

/*${QEP::QStateRet} ........................................................*/
/*! All possible values returned from state/action handlers
*
* @note
* The order matters for algorithmic correctness.
*/
enum QStateRet {
    /* unhandled and need to "bubble up" */
    Q_RET_SUPER,     /*!< event passed to superstate to handle */
    Q_RET_SUPER_SUB, /*!< event passed to submachine superstate */
    Q_RET_UNHANDLED, /*!< event unhandled due to a guard */

    /* handled and do not need to "bubble up" */
    Q_RET_HANDLED,   /*!< event handled (internal transition) */
    Q_RET_IGNORED,   /*!< event silently ignored (bubbled up to top) */

    /* entry/exit */
    Q_RET_ENTRY,     /*!< state entry action executed */
    Q_RET_EXIT,      /*!< state exit  action executed */

    /* no side effects */
    Q_RET_NULL,      /*!< return value without any effect */

    /* transitions need to execute transition-action table in ::QMsm */
    Q_RET_TRAN,      /*!< regular transition */
    Q_RET_TRAN_INIT, /*!< initial transition in a state or submachine */
    Q_RET_TRAN_EP,   /*!< entry-point transition into a submachine */

    /* transitions that additionally clobber me->state */
    Q_RET_TRAN_HIST, /*!< transition to history of a given state */
    Q_RET_TRAN_XP    /*!< exit-point transition out of a submachine */
};

/*${QEP::QState} ...........................................................*/
/*! Type returned from state-handler functions */
typedef enum QStateRet QState;

/*${QEP::QStateHandler} ....................................................*/
/*! Pointer to a state-handler function. */
typedef QState (* QStateHandler )(void * const me, QEvt const * const e);

/*${QEP::QActionHandler} ...................................................*/
/*! Pointer to an action-handler function. */
typedef QState (* QActionHandler )(void * const me);

/*${QEP::QXThread} .........................................................*/
/* forward declaration */
struct QXThread;

/*${QEP::QXThreadHandler} ..................................................*/
/*! Pointer to an eXthended thread handler function */
typedef void (* QXThreadHandler )(struct QXThread * const me);

/*${QEP::QMState} ..........................................................*/
/*! @brief State object for the ::QMsm class (QM State Machine).
*
* @details
* This class groups together the attributes of a ::QMsm state, such as the
* parent state (state nesting), the associated state handler function and
* the exit action handler function. These attributes are used inside the
* QMsm_dispatch() and QMsm_init() functions.
*
* @tr{RQP104}
*
* @attention
* The ::QMState class is only intended for the QM code generator and should
* not be used in hand-crafted code.
*/
typedef struct QMState {
    struct QMState const *superstate; /*!< superstate of this state */
    QStateHandler const stateHandler; /*!< state handler function */
    QActionHandler const entryAction; /*!< entry action handler function */
    QActionHandler const exitAction;  /*!< exit action handler function */
    QActionHandler const initAction;  /*!< init action handler function */
} QMState;

/*${QEP::QMTranActTable} ...................................................*/
/*! @brief Transition-Action Table for the ::QMsm State Machine. */
typedef struct QMTranActTable {
    QMState const *target;       /*!< target of the transition */
    QActionHandler const act[1]; /*!< array of actions */
} QMTranActTable;

/*${QEP::QHsmAttr} .........................................................*/
/*! @brief Attribute of for the ::QHsm class (Hierarchical State Machine).
*
* @details
* This union represents possible values stored in the 'state' and 'temp'
* attributes of the ::QHsm class.
*/
union QHsmAttr {
    QStateHandler   fun;         /*!< @private pointer to a state-handler */
    QActionHandler  act;         /*!< @private pointer to an action-handler */
    QXThreadHandler thr;         /*!< @private pointer to an thread-handler */
    QMTranActTable const *tatbl; /*!< @private transition-action table */
    struct QMState const *obj;   /*!< @private pointer to QMState object */
};

/*${QEP::QHSM_MAX_NEST_DEPTH_} .............................................*/
/*! maximum depth of state nesting in a HSM (including the top level),
* must be >= 3
*/
enum { QHSM_MAX_NEST_DEPTH_ = 6};

/*${QEP::QReservedSig} .....................................................*/
/*! Reserved signals by the HSM-style state machine
* implementation strategy.
*/
enum QReservedSig {
    Q_ENTRY_SIG = 1, /*!< signal for coding entry actions */
    Q_EXIT_SIG,      /*!< signal for coding exit actions */
    Q_INIT_SIG,      /*!< signal for coding initial transitions */
    Q_USER_SIG       /*!< offset for the user signals (QP Application) */
};

/*${QEP::QHsm} .............................................................*/
/*! @brief Hierarchical State Machine class
* @class QHsm
*
* @details
* QHsm represents a Hierarchical State Machine (HSM) with full support for
* hierarchical nesting of states, entry/exit actions, initial transitions,
* and transitions to history in any composite state. This class is designed
* for ease of manual coding of HSMs in C, but it is also supported by the
* QM modeling tool.<br>
*
* QHsm is also the base class for the QMsm state machine, which provides
* a superior efficiency, but requires the use of the QM modeling tool to
* generate code.
*
* @note
* QHsm is not intended to be instantiated directly, but rather serves as the
* abstract base class for derivation of state machines in the QP application.
*
* @tr{RQP103}
* @tr{AQP211}
*
* @usage
* The following example illustrates how to derive a state machine class
* from QHsm. Please note that the QHsm member `super` is defined as the
* FIRST member of the derived class.
* @include qep_qhsm.c
*/
typedef struct {
/* private: */

    /*! Virtual pointer
    * @private @memberof QHsm
    *
    * @tr{RQP102}
    */
    struct QHsmVtable const * vptr;

/* protected: */

    /*! Current active state (state-variable).
    * @private @memberof QHsm
    */
    union QHsmAttr state;

    /*! Temporary: target/act-table, etc.
    * @private @memberof QHsm
    */
    union QHsmAttr temp;
} QHsm;

/* public: */

/*! Tests if a given state is part of the current active state
* configuration in ::QHsm subclasses.
* @public @memberof QHsm
*
* @details
* Tests if a state machine derived from QHsm is-in a given state.
*
* @note For a HSM, to "be in a state" means also to be in a superstate of
* of the state.
*
* @param[in] me    pointer (see @ref oop)
* @param[in] state pointer to the state-handler function to be tested
*
* @returns
*'true' if the HSM "is in" the @p state and 'false' otherwise
*
* @tr{RQP103}
* @tr{RQP120S}
*/
bool QHsm_isIn(QHsm * const me,
    QStateHandler const state);

/*! Obtain the current active state from a HSM (read only).
* @public @memberof QHsm
*
* @param[in] me pointer (see @ref oop)
*
* @returns the current active state of the QHsm class
*
* @note
* This function is used in QM for auto-generating code for state history.
*/
QStateHandler QHsm_state(QHsm * const me);

/*! Obtain the current active child state of a given parent in ::QHsm
* @public @memberof QHsm
*
* @details
* Finds the child state of the given `parent`, such that this child state
* is an ancestor of the currently active state. The main purpose of this
* function is to support **shallow history** transitions in state machines
* derived from QHsm.
*
* @param[in] me     pointer (see @ref oop)
* @param[in] parent pointer to the state-handler function
*
* @returns
* the child of a given `parent` state, which is an ancestor of the current
* active state. For the corner case when the currently active state is the
* given `parent` state, function returns the `parent` state.
*
* @note
* this function is designed to be called during state transitions, so it
* does not necessarily start in a stable state configuration.
* However, the function establishes stable state configuration upon exit.
*
* @tr{RQP103}
* @tr{RQP120H}
*/
QStateHandler QHsm_childState(QHsm * const me,
    QStateHandler const parent);

/* protected: */

/*! Protected "constructor" of ::QHsm
* @protected @memberof QHsm
*
* @details
* Performs the first step of HSM initialization by assigning the initial
* pseudostate to the currently active state of the state machine.
*
* @param[in,out] me      pointer (see @ref oop)
* @param[in]     initial pointer to the top-most initial state-handler
*                        function in the derived state machine
*
* @note Must be called only by the constructors of the derived state
* machines.
*
* @note Must be called only ONCE before QHSM_INIT().
*
* @usage
* The following example illustrates how to invoke QHsm_ctor() in the
* "constructor" of a derived state machine:
* @include qep_qhsm_ctor.c
*
* @tr{RQP103}
*/
void QHsm_ctor(QHsm * const me,
    QStateHandler initial);

/*! The top-state of QHsm.
* @protected @memberof QHsm
*
* @details
* QHsm_top() is the ultimate root of state hierarchy in all HSMs derived
* from ::QHsm.
*
* @param[in] me pointer (see @ref oop)
* @param[in] e  pointer to the event to be dispatched to the FSM
*
* @returns
* Always returns ::Q_RET_IGNORED, which means that the top state ignores
* all events.
*
* @note The parameters to this state handler are not used. They are provided
* for conformance with the state-handler function signature ::QStateHandler.
*
* @tr{RQP103} @tr{RQP120T}
*/
QState QHsm_top(QHsm const * const me,
    QEvt const * const e);

/*! Implementation of the top-most initial tran. in ::QHsm.
* @protected @memberof QHsm
*
* @details
* Executes the top-most initial transition in a HSM.
*
* @param[in,out] me  pointer (see @ref oop)
* @param[in]     e   pointer to an extra parameter (might be NULL)
* @param[in]     qs_id QS-id of this state machine (for QS local filter)
*
* @note Must be called only ONCE after the QHsm_ctor().
*
* @tr{RQP103} @tr{RQP120I} @tr{RQP120D}
*/
void QHsm_init_(QHsm * const me,
    void const * const e,
    uint_fast8_t const qs_id);

/*! Implementation of dispatching events to a ::QHsm
* @protected @memberof QHsm
*
* @details
* Dispatches an event for processing to a hierarchical state machine (HSM).
* The processing of an event represents one run-to-completion (RTC) step.
*
* @param[in,out] me pointer (see @ref oop)
* @param[in]     e  pointer to the event to be dispatched to the HSM
* @param[in]     qs_id QS-id of this state machine (for QS local filter)
*
* @note
* This function should be called only via the virtual table (see
* QHSM_DISPATCH()) and should NOT be called directly in the applications.
*
* @tr{RQP103}
* @tr{RQP120A} @tr{RQP120B} @tr{RQP120C} @tr{RQP120D} @tr{RQP120E}
*/
void QHsm_dispatch_(QHsm * const me,
    QEvt const * const e,
    uint_fast8_t const qs_id);

/* private: */

/*! Implementation of getting the state handler in a ::QHsm subclass
* @private @memberof QHsm
*/
#ifdef Q_SPY
QStateHandler QHsm_getStateHandler_(QHsm * const me);
#endif /* def Q_SPY */

/*! Helper function to execute transition sequence in a hierarchical state
* machine (HSM).
* @private @memberof QHsm
*
* @param[in,out] path array of pointers to state-handler functions
*                     to execute the entry actions
* @param[in]     qs_id QS-id of this state machine (for QS local filter)
*
* @returns
* the depth of the entry path stored in the @p path parameter.
*
* @tr{RQP103}
* @tr{RQP120E} @tr{RQP120F}
*/
int_fast8_t QHsm_tran_(QHsm * const me,
    QStateHandler path[QHSM_MAX_NEST_DEPTH_],
    uint_fast8_t const qs_id);

/*${QEP::QHsmVtable} .......................................................*/
/*! @brief Virtual table for the ::QHsm class.
*
* @tr{RQP102}
*/
struct QHsmVtable {
    /*! Triggers the top-most initial transition in the HSM. */
    void (*init)(QHsm * const me, void const * const e,
                 uint_fast8_t const qs_id);

    /*! Dispatches an event to the HSM. */
    void (*dispatch)(QHsm * const me, QEvt const * const e,
                     uint_fast8_t const qs_id);

#ifdef Q_SPY
    /*! Get the current state handler of the HSM. */
    QStateHandler (*getStateHandler)(QHsm * const me);
#endif /* Q_SPY */
};

/*${QEP::QMsm} .............................................................*/
/*! @brief QM state machine implementation strategy
* @class QMsm
* @extends QHsm
*
* @details
* QMsm (QM State Machine) provides a more efficient state machine
* implementation strategy than ::QHsm, but requires the use of the QM
* modeling tool, but are the fastest and need the least run-time
* support (the smallest event-processor taking up the least code space).
*
* @note
* QMsm is not intended to be instantiated directly, but rather serves
* as the abstrace base class for derivation of state machines in the
* application code.
*
* @tr{RQP104}
*
* @usage
* The following example illustrates how to derive a state machine class
* from QMsm. Please note that the QMsm member `super` is defined
* as the *first* member of the derived struct.
* @include qep_qmsm.c
*/
typedef struct {
/* protected: */
    QHsm super;
} QMsm;

/* public: */

/*! Tests if a given state is part of the current active state
* configuration in a MSM.
* @public @memberof QMsm
*
* @details
* Tests if a state machine derived from QMsm is-in a given state.
*
* @note
* For a MSM, to "be-in" a state means also to "be-in" a superstate of
* of the state.
*
* @param[in] me    pointer (see @ref oop)
* @param[in] state pointer to the QMState object that corresponds to the
*                  tested state.
* @returns
* 'true' if the MSM "is in" the @p state and 'false' otherwise
*/
bool QMsm_isInState(QMsm const * const me,
    QMState const * const state);

/*! Obtain the current active state from a MSM (read only)
* @public @memberof QMsm
*
* @param[in] me     pointer (see @ref oop)
*
* @returns the current active state-object
*
* @note
* This function is used in QM for auto-generating code for state history
*/
QMState const * QMsm_stateObj(QHsm const * const me);

/*! Obtain the current active child state of a given parent in ::QMsm
* @public @memberof QMsm
*
* @details
* Finds the child state of the given @c parent, such that this child state
* is an ancestor of the currently active state. The main purpose of this
* function is to support **shallow history** transitions in state machines
* derived from QMsm.
*
* @param[in] me     pointer (see @ref oop)
* @param[in] parent pointer to the state-handler object
*
* @returns
* the child of a given @c parent state, which is an ancestor of
* the currently active state. For the corner case when the currently active
* state is the given @c parent state, function returns the @c parent state.
*
* @sa QMsm_childStateObj()
*/
QMState const * QMsm_childStateObj(
    QHsm const * const me,
    QMState const * const parent);

/* protected: */

/*! Constructor of ::QMsm
* @protected @memberof QMsm
*
* @details
* Performs the first step of QMsm initialization by assigning the initial
* pseudostate to the currently active state of the state machine.
*
* @param[in,out] me       pointer (see @ref oop)
* @param[in]     initial  the top-most initial transition for the MSM.
*
* @note
* Must be called only ONCE before QHSM_INIT().
*
* @note
* QMsm inherits QHsm, so by the @ref oop convention it should call the
* constructor of the superclass, i.e., QHsm_ctor(). However, this would pull
* in the QHsmVtable, which in turn will pull in the code for QHsm_init_() and
* QHsm_dispatch_() implemetations. To avoid this code size penalty, in case
* ::QHsm is not used in a given project, the QMsm_ctor() performs direct
* intitialization of the Vtable, which avoids pulling in the code for QMsm.
*
* @usage
* The following example illustrates how to invoke QMsm_ctor() in the
* "constructor" of a derived state machine:
* @include qep_qmsm_ctor.c
*/
void QMsm_ctor(QMsm * const me,
    QStateHandler initial);

/* public: */

/*! Implementation of the top-most initial tran. in ::QMsm.
* @private @memberof QMsm
*
* @details
* Executes the top-most initial transition in a MSM.
*
* @param[in,out] me  pointer (see @ref oop)
* @param[in]     e   pointer to an extra parameter (might be NULL)
* @param[in]     qs_id QS-id of this state machine (for QS local filter)
*
* @note
* This function should be called only via the virtual table (see
* QHSM_INIT()) and should NOT be called directly in the applications.
*/
void QMsm_init_(
    QHsm * const me,
    void const * const e,
    uint_fast8_t const qs_id);

/* private: */

/*! Implementation of dispatching events to a ::QMsm
* @private @memberof QMsm
*
* @details
* Dispatches an event for processing to a meta state machine (MSM).
* The processing of an event represents one run-to-completion (RTC) step.
*
* @param[in,out] me pointer (see @ref oop)
* @param[in]     e  pointer to the event to be dispatched to the MSM
* @param[in]     qs_id QS-id of this state machine (for QS local filter)
*
* @note
* This function should be called only via the virtual table (see
* QHSM_DISPATCH()) and should NOT be called directly in the applications.
*/
void QMsm_dispatch_(
    QHsm * const me,
    QEvt const * const e,
    uint_fast8_t const qs_id);

/* public: */

/*! Implementation of getting the state handler in a ::QMsm subclass
* @public @memberof QMsm
*/
#ifdef Q_SPY
QStateHandler QMsm_getStateHandler_(QHsm * const me);
#endif /* def Q_SPY */

/* private: */

/*! Execute transition-action table
* @private @memberof QMsm
*
* @details
* Helper function to execute transition sequence in a transition-action table.
*
* @param[in,out] me    pointer (see @ref oop)
* @param[in]     tatbl pointer to the transition-action table
* @param[in]     qs_id QS-id of this state machine (for QS local filter)
*
* @returns
* status of the last action from the transition-action table.
*
* @note
* This function is for internal use inside the QEP event processor and
* should **not** be called directly from the applications.
*/
QState QMsm_execTatbl_(
    QHsm * const me,
    QMTranActTable const * tatbl,
    uint_fast8_t const qs_id);

/*! Exit the current state up to the explicit transition source
* @private @memberof QMsm
*
* @details
* Static helper function to exit the current state configuration to the
* transition source, which in a hierarchical state machine might be a
* superstate of the current state.
*
* @param[in,out] me   pointer (see @ref oop)
* @param[in]     cs   pointer to the current state
* @param[in]     ts   pointer to the transition source state
* @param[in]     qs_id QS-id of this state machine (for QS local filter)
*/
void QMsm_exitToTranSource_(
    QHsm * const me,
    QMState const * cs,
    QMState const * ts,
    uint_fast8_t const qs_id);

/*! Enter history of a composite state
* @private @memberof QMsm
*
* @details
* Static helper function to execute the segment of transition to history
* after entering the composite state and
*
* @param[in,out] me   pointer (see @ref oop)
* @param[in]     hist pointer to the history substate
* @param[in]     qs_id QS-id of this state machine (for QS local filter)
*
* @returns
* #Q_RET_TRAN_INIT, if an initial transition has been executed in the last
* entered state or #Q_RET_NULL if no such transition was taken.
*/
QState QMsm_enterHistory_(
    QHsm * const me,
    QMState const *const hist,
    uint_fast8_t const qs_id);
/*$enddecl${QEP} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*==========================================================================*/
/*$declare${QEP-macros} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QEP-macros::QHSM_INIT} .................................................*/
#ifdef Q_SPY
/*! Virtual call to the top-most initial transition in a HSM.
*
* @param[in,out] me_ pointer (see @ref oop)
* @param[in]     e_  constant pointer the ::QEvt or a class derived from
*                    ::QEvt (see @ref oop)
* @param[in]     qs_id_ QS local filter ID (used only when Q_SPY is defined)
*
* @note Must be called only ONCE after the SM "constructor".
*
* @tr{RQP102}
*
* @usage
* The following example illustrates how to initialize a SM, and dispatch
* events to it:
* @include qep_qhsm_use.c
*/
#define QHSM_INIT(me_, par_, qs_id_) do { \
    Q_ASSERT((me_)->vptr); \
    (*(me_)->vptr->init)((me_), (par_), (qs_id_)); \
} while (false)
#endif /* def Q_SPY */

/*${QEP-macros::QHSM_INIT} .................................................*/
#ifndef Q_SPY
#define QHSM_INIT(me_, par_, dummy) do { \
    Q_ASSERT((me_)->vptr); \
    (*(me_)->vptr->init)((me_), (par_), 0U); \
} while (false)
#endif /* ndef Q_SPY */

/*${QEP-macros::QHSM_DISPATCH} .............................................*/
#ifdef Q_SPY
/*! Virtual call to dispatch an event to a HSM
*
* @details
* Processes one event at a time in Run-to-Completion fashion.
*
* @param[in,out] me_ pointer (see @ref oop)
* @param[in]     e_  constant pointer the ::QEvt or a class
*                    derived from ::QEvt (see @ref oop)
* @param[in]     qs_id_ QS local filter ID (used only when Q_SPY is defined)
*
* @note Must be called after the "constructor" and after QHSM_INIT().
*
* @tr{RQP102}
*/
#define QHSM_DISPATCH(me_, e_, qs_id_) \
    ((*(me_)->vptr->dispatch)((me_), (e_), (qs_id_)))
#endif /* def Q_SPY */

/*${QEP-macros::QHSM_DISPATCH} .............................................*/
#ifndef Q_SPY
#define QHSM_DISPATCH(me_, e_, dummy) \
    ((*(me_)->vptr->dispatch)((me_), (e_), 0U))
#endif /* ndef Q_SPY */

/*${QEP-macros::Q_HSM_UPCAST} ..............................................*/
/*! Perform upcast from a subclass of ::QHsm to the base class ::QHsm
*
* @details
* Upcasting from a subclass to superclass is a very frequent and **safe**
* operation in object-oriented programming and object-oriented languages
* (such as C++) perform such upcasting automatically. However, OOP is
* implemented in C just as a set of coding conventions (see @ref oop),
* and the C compiler does not "know" that certain types are related by
* inheritance. Therefore for C, the upcast must be performed explicitly.
* Unfortunately, pointer casting violates the advisory MISRA-C 2012
* Rule 11.3(req) "A cast shall not be performed between a pointer to object
* type and a pointer to a different object type". This macro encapsulates
* this deviation and provides a descriptive name for the reason of this cast.
*/
#define Q_HSM_UPCAST(ptr_) ((QHsm *)(ptr_))

/*${QEP-macros::Q_TRAN} ....................................................*/
/*! Perform upcast from a subclass of ::QHsm to the base class ::QHsm
*
* @details
* Upcasting from a subclass to superclass is a very frequent and **safe**
* operation in object-oriented programming and object-oriented languages
* (such as C++) perform such upcasting automatically. However, OOP is
* implemented in C just as a set of coding conventions (see @ref oop),
* and the C compiler does not "know" that certain types are related by
* inheritance. Therefore for C, the upcast must be performed explicitly.
* Unfortunately, pointer casting violates the advisory MISRA-C 2012
* Rule 11.3(req) "A cast shall not be performed between a pointer to object
* type and a pointer to a different object type". This macro encapsulates
* this deviation and provides a descriptive name for the reason of this cast.
*/
#define Q_TRAN(target_) \
    ((Q_HSM_UPCAST(me))->temp.fun = Q_STATE_CAST(target_), Q_RET_TRAN)

/*${QEP-macros::Q_TRAN_HIST} ...............................................*/
/*! Macro to call in a state-handler when it executes a transition
* to history. Applicable only to HSMs.
*
* @tr{RQP103} @tr{RQP120H}
*
* @usage
* @include qep_qhist.c
*/
#define Q_TRAN_HIST(hist_) \
    ((Q_HSM_UPCAST(me))->temp.fun = (hist_), Q_RET_TRAN_HIST)

/*${QEP-macros::Q_SUPER} ...................................................*/
/*! Macro to call in a state-handler when it designates the superstate
* of a given state. Applicable only to ::QHsm subclasses.
*
* @tr{RQP103}
*
* @usage
* @include qep_qsuper.c
*/
#define Q_SUPER(super_) \
    ((Q_HSM_UPCAST(me))->temp.fun = Q_STATE_CAST(super_), Q_RET_SUPER)

/*${QEP-macros::Q_HANDLED} .................................................*/
/*! Macro to call in a state-handler when it handles an event.
* @tr{RQP103} @tr{RQP120B} @tr{RQP120C}
*/
#define Q_HANDLED() (Q_RET_HANDLED)

/*${QEP-macros::Q_UNHANDLED} ...............................................*/
/*! Macro to call in a state-handler when it attempts to handle
* an event but a guard condition evaluates to 'false' and there is no other
* explicit way of handling the event. Applicable only to ::QHsm subclasses.
*/
#define Q_UNHANDLED() (Q_RET_UNHANDLED)

/*${QEP-macros::Q_ACTION_NULL} .............................................*/
/*! Macro to provide strictly-typed zero-action to terminate action lists
*! in the transition-action-tables
*/
#define Q_ACTION_NULL ((QActionHandler)0)

/*${QEP-macros::Q_EVT_CAST} ................................................*/
/*! Perform downcast of an event onto a subclass of ::QEvt `class_`
*
* @details
* This macro encapsulates the downcast of ::QEvt pointers, which violates
* MISRA-C 2012 Rule 11.3(R) "A cast shall not be performed between a
* pointer to object type and a pointer to a different object type". This
* macro helps to localize this deviation.
*
* @param class_  a subclass of ::QEvt
*
* @tr{RQP003}
* @tr{PQA11_3}
*/
#define Q_EVT_CAST(class_) ((class_ const *)(e))

/*${QEP-macros::Q_STATE_CAST} ..............................................*/
/*! Perform cast to ::QStateHandler.
* @details
* This macro encapsulates the cast of a specific state handler function
* pointer to ::QStateHandler, which violates MISRA:C-2012 Rule 11.1(req)
* "Conversions shall not be performed between a pointer to function and
* any other type". This macro helps to localize this deviation.
*
* @tr{PQP11_1} @tr{PQA11_1}
*
* @usage
* @include qep_qhsm_ctor.c
*/
#define Q_STATE_CAST(handler_) ((QStateHandler)(handler_))

/*${QEP-macros::Q_ACTION_CAST} .............................................*/
/*! Perform cast to ::QActionHandler.
* @details
* This macro encapsulates the cast of a specific action handler function
* pointer to ::QActionHandler, which violates MISRA:C-2012 Rule 11.1(R)
* "Conversions shall not be performed between a pointer to function and
* any other type". This macro helps to localize this deviation.
*
* @tr{PQP11_1} @tr{PQA11_1}
*/
#define Q_ACTION_CAST(action_) ((QActionHandler)(action_))

/*${QEP-macros::Q_UNUSED_PAR} ..............................................*/
/*! Helper macro to clearly mark unused parameters of functions. */
#define Q_UNUSED_PAR(par_) ((void)(par_))

/*${QEP-macros::Q_DIM} .....................................................*/
/*! Helper macro to calculate static dimension of a 1-dim `array_`
*
* @param array_ 1-dimensional array
* @returns the length of the array (number of elements it can hold)
*/
#define Q_DIM(array_) (sizeof(array_) / sizeof((array_)[0U]))

/*${QEP-macros::Q_UINT2PTR_CAST} ...........................................*/
/*! Perform cast from unsigned integer `uint_` to pointer of type `type_`
*
* @details
* This macro encapsulates the cast to (type_ *), which QP ports or
* application might use to access embedded hardware registers.
* Such uses can trigger PC-Lint "Note 923: cast from int to pointer"
* and this macro helps to encapsulate this deviation.
*/
#define Q_UINT2PTR_CAST(type_, uint_) ((type_ *)(uint_))

/*${QEP-macros::QEVT_INITIALIZER} ..........................................*/
/*! Initializer of static constant QEvt instances
*
* @details
* This macro encapsulates the ugly casting of enumerated signals
* to QSignal and constants for QEvt.poolID and QEvt.refCtr_.
*/
#define QEVT_INITIALIZER(sig_) { (QSignal)(sig_), 0U, 0U }

/*${QEP-macros::QM_ENTRY} ..................................................*/
#ifdef Q_SPY
/*! Macro to call in a QM action-handler when it executes
* an entry action. Applicable only to ::QMsm subclasses.
*/
#define QM_ENTRY(state_) \
    ((Q_HSM_UPCAST(me))->temp.obj = (state_), Q_RET_ENTRY)
#endif /* def Q_SPY */

/*${QEP-macros::QM_ENTRY} ..................................................*/
#ifndef Q_SPY
#define QM_ENTRY(dummy) (Q_RET_ENTRY)
#endif /* ndef Q_SPY */

/*${QEP-macros::QM_EXIT} ...................................................*/
#ifdef Q_SPY
/*! Macro to call in a QM action-handler when it executes
* an exit action. Applicable only to ::QMsm subclasses.
*/
#define QM_EXIT(state_) \
    ((Q_HSM_UPCAST(me))->temp.obj = (state_), Q_RET_EXIT)
#endif /* def Q_SPY */

/*${QEP-macros::QM_EXIT} ...................................................*/
#ifndef Q_SPY
#define QM_EXIT(dummy) (Q_RET_EXIT)
#endif /* ndef Q_SPY */

/*${QEP-macros::QM_SM_EXIT} ................................................*/
/*! Macro to call in a QM submachine exit-handler.
* Applicable only to subclasses of ::QMsm.
*/
#define QM_SM_EXIT(state_) \
    ((Q_HSM_UPCAST(me))->temp.obj = (state_), Q_RET_EXIT)

/*${QEP-macros::QM_TRAN} ...................................................*/
/*! Macro to call in a QM state-handler when it executes a regular
* transition. Applicable only to ::QMsm subclasses.
*/
#define QM_TRAN(tatbl_) ((Q_HSM_UPCAST(me))->temp.tatbl \
    = (struct QMTranActTable const *)(tatbl_), Q_RET_TRAN)

/*${QEP-macros::QM_TRAN_INIT} ..............................................*/
/*! Macro to call in a QM state-handler when it executes an initial
* transition. Applicable only to ::QMsm subclasses.
*/
#define QM_TRAN_INIT(tatbl_) ((Q_HSM_UPCAST(me))->temp.tatbl \
    = (struct QMTranActTable const *)(tatbl_), Q_RET_TRAN_INIT)

/*${QEP-macros::QM_TRAN_HIST} ..............................................*/
/*! Macro to call in a QM state-handler when it executes a transition
* to history. Applicable only to ::QMsm subclasses.
*/
#define QM_TRAN_HIST(history_, tatbl_) \
    ((((Q_HSM_UPCAST(me))->state.obj  = (history_)), \
      ((Q_HSM_UPCAST(me))->temp.tatbl = \
          (struct QMTranActTable const *)(tatbl_))), \
     Q_RET_TRAN_HIST)

/*${QEP-macros::QM_TRAN_EP} ................................................*/
/*! Macro to call in a QM state-handler when it executes a transition
* to the submachine via an entry point.
*/
#define QM_TRAN_EP(tatbl_) ((Q_HSM_UPCAST(me))->temp.tatbl \
    = (struct QMTranActTable const *)(tatbl_), Q_RET_TRAN_EP)

/*${QEP-macros::QM_TRAN_XP} ................................................*/
/*! Macro to call in a QM state-handler when it executes a transition
* to exit point. Applicable only to ::QMsm subclasses.
*/
#define QM_TRAN_XP(xp_, tatbl_) \
    ((((Q_HSM_UPCAST(me))->state.act  = (xp_)), \
      ((Q_HSM_UPCAST(me))->temp.tatbl = \
          (struct QMTranActTable const *)(tatbl_))), \
     Q_RET_TRAN_XP)

/*${QEP-macros::QM_HANDLED} ................................................*/
/*! Macro to call in a QM state-handler when it handled an event.
* Applicable only to ::QMsm subclasses.
*/
#define QM_HANDLED() (Q_RET_HANDLED)

/*${QEP-macros::QM_UNHANDLED} ..............................................*/
/*! Macro to call in a QM state-handler when when it attempts to
* handle an event but a guard condition evaluates to 'false' and there is
* no other explicit way of handling the event. Applicable only to
* ::QMsm subclasses.
*/
#define QM_UNHANDLED() (Q_RET_UNHANDLED)

/*${QEP-macros::QM_SUPER} ..................................................*/
/*! Macro to call in a QM state-handler when it designates the
* superstate to handle an event. Applicable only to QMSMs.
*/
#define QM_SUPER(super_) (Q_RET_SUPER)

/*${QEP-macros::QM_SUPER_SUB} ..............................................*/
/*! Macro to call in a QM submachine-handler when it designates the
* host state to handle an event. Applicable only to subclasses of ::QMsm.
*/
#define QM_SUPER_SUB(host_) \
    ((Q_HSM_UPCAST(me))->temp.obj = (host_), Q_RET_SUPER_SUB)

/*${QEP-macros::QM_STATE_NULL} .............................................*/
/*! Macro to provide strictly-typed zero-state to use for submachines.
*! Applicable to subclasses of ::QMsm.
*/
#define QM_STATE_NULL ((QMState *)0)
/*$enddecl${QEP-macros} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#endif /* QP_INC_QEP_H_ */
