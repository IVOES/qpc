/*.$file${.::calc1_sub.h} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: calc1_sub.qm
* File:  ${.::calc1_sub.h}
*
* This code has been generated by QM 5.1.4 <www.state-machine.com/qm/>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This code is covered by the following QP license:
* License #   : QPC-EVAL
* Issued to   : Institution or an individual evaluating the QP/C framework
* Framework(s): qpc
* Support ends: 2022-12-31
* Product(s)  :
* This license is available only for evaluation purposes and
* the generated code is still licensed under the terms of GPL.
* Please submit request for extension of the evaluation period at:
* <www.state-machine.com/licensing/#RequestForm>
*/
/*.$endhead${.::calc1_sub.h} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#ifndef CALC1_SUB_H
#define CALC1_SUB_H

enum CalcSignals {
    C_SIG = Q_USER_SIG,
    CE_SIG,
    DIGIT_0_SIG,
    DIGIT_1_9_SIG,
    POINT_SIG,
    OPER_SIG,
    EQUALS_SIG,
    OFF_SIG
};

/*.$declare${Events::CalcEvt} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*.${Events::CalcEvt} ......................................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    uint8_t key_code;
} CalcEvt;
/*.$enddecl${Events::CalcEvt} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*.$declare${SMs::Calc_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*.${SMs::Calc_ctor} .......................................................*/
void Calc_ctor(void);
/*.$enddecl${SMs::Calc_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

extern QHsm * const the_calc;  /* "opaque" pointer to calculator HSM */

#endif /* CALC1_SUB_H */
