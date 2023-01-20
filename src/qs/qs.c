/*$file${src::qs::qs.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: qpc.qm
* File:  ${src::qs::qs.c}
*
* This code has been generated by QM 5.2.5 <www.state-machine.com/qm>.
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
/*$endhead${src::qs::qs.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*! @file
* @brief QS software tracing services
*/
#define QP_IMPL           /* this is QP implementation */
#include "qs_port.h"      /* QS port */
#include "qs_pkg.h"       /* QS package-scope interface */
#include "qstamp.h"       /* QP time-stamp */
#include "qassert.h"      /* QP Functional Safety (FuSa) Subsystem */

Q_DEFINE_THIS_MODULE("qs")

/* ensure that the predefined records don't overlap the
* user records (application-specific).
*/
Q_ASSERT_STATIC((enum_t)QS_PRE_MAX <= (enum_t)QS_USER);

/*$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/* Check for the minimum required QP version */
#if (QP_VERSION < 700U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 7.0.0 or higher required
#endif
/*$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$define${QS::QS-tx} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QS::QS-tx::priv_} ......................................................*/
QS_tx QS_priv_;

/*${QS::QS-tx::initBuf} ....................................................*/
/*! @static @public @memberof QS_tx */
void QS_initBuf(
    uint8_t * const sto,
    uint_fast32_t const stoSize)
{
    QS_priv_.buf      = &sto[0];
    QS_priv_.end      = (QSCtr)stoSize;
    QS_priv_.head     = 0U;
    QS_priv_.tail     = 0U;
    QS_priv_.used     = 0U;
    QS_priv_.seq      = 0U;
    QS_priv_.chksum   = 0U;
    QS_priv_.critNest = 0U;

    QS_glbFilter_(-(int_fast16_t)QS_ALL_RECORDS); /* all global filters OFF */
    QS_locFilter_((int_fast16_t)QS_ALL_IDS);      /* all local filters ON */
    QS_priv_.locFilter_AP = (void *)0;            /* deprecated "AP-filter" */

    /* produce an empty record to "flush" the QS trace buffer */
    QS_beginRec_((uint_fast8_t)QS_EMPTY);
    QS_endRec_();

    /* produce the reset record to inform QSPY of a new session */
    QS_target_info_pre_(0xFFU); /* send Reset and Target info */

    /* hold off flushing after successfull initialization (see QS_INIT()) */
}

/*${QS::QS-tx::getByte} ....................................................*/
/*! @static @public @memberof QS_tx */
uint16_t QS_getByte(void) {
    uint16_t ret;
    if (QS_priv_.used == 0U) {
        ret = QS_EOD; /* set End-Of-Data */
    }
    else {
        uint8_t const * const buf = QS_priv_.buf;  /* put in a temporary */
        QSCtr tail = QS_priv_.tail; /* put in a temporary (register) */
        ret = (uint16_t)buf[tail];    /* set the byte to return */
        ++tail; /* advance the tail */
        if (tail == QS_priv_.end) { /* tail wrap around? */
            tail = 0U;
        }
        QS_priv_.tail = tail; /* update the tail */
        --QS_priv_.used;      /* one less byte used */
    }
    return ret; /* return the byte or EOD */
}

/*${QS::QS-tx::getBlock} ...................................................*/
/*! @static @public @memberof QS_tx */
uint8_t const * QS_getBlock(uint16_t * const pNbytes) {
    QSCtr const used = QS_priv_.used; /* put in a temporary (register) */
    uint8_t const *buf;

    /* any bytes used in the ring buffer? */
    if (used != 0U) {
        QSCtr tail       = QS_priv_.tail; /* put in a temporary (register) */
        QSCtr const end  = QS_priv_.end;  /* put in a temporary (register) */
        QSCtr n = (QSCtr)(end - tail);
        if (n > used) {
            n = used;
        }
        if (n > (QSCtr)(*pNbytes)) {
            n = (QSCtr)(*pNbytes);
        }
        *pNbytes = (uint16_t)n;    /* n-bytes available */
        buf = &QS_priv_.buf[tail]; /* the bytes are at the tail */

        QS_priv_.used = (QSCtr)(used - n);
        tail += n;
        if (tail == end) {
            tail = 0U;
        }
        QS_priv_.tail = tail;
    }

    else { /* no bytes available */
        *pNbytes = 0U;      /* no bytes available right now */
        buf = (uint8_t *)0; /* no bytes available right now */
    }
    return buf;
}

/*${QS::QS-tx::glbFilter_} .................................................*/
/*! @static @public @memberof QS_tx */
void QS_glbFilter_(int_fast16_t const filter) {
    bool const isRemove = (filter < 0);
    uint8_t const rec = isRemove ? (uint8_t)(-filter) : (uint8_t)filter;
    switch (rec) {
        case (uint8_t)QS_ALL_RECORDS: {
            uint8_t const tmp = (isRemove ? 0x00U : 0xFFU);

            /* set all global filters (partially unrolled loop) */
            for (uint_fast8_t i = 0U;
                 i < Q_DIM(QS_filter_.glb);
                 i += 4U)
            {
                QS_filter_.glb[i     ] = tmp;
                QS_filter_.glb[i + 1U] = tmp;
                QS_filter_.glb[i + 2U] = tmp;
                QS_filter_.glb[i + 3U] = tmp;
            }
            if (isRemove) {
                /* leave the "not maskable" filters enabled,
                * see qs.h, Miscellaneous QS records (not maskable)
                */
                QS_filter_.glb[0] = 0x01U;
                QS_filter_.glb[6] = 0x40U;
                QS_filter_.glb[7] = 0xFCU;
                QS_filter_.glb[8] = 0x7FU;
            }
            else {
                /* never turn the last 3 records on (0x7D, 0x7E, 0x7F) */
                QS_filter_.glb[15] = 0x1FU;
            }
            break;
        }
        case (uint8_t)QS_SM_RECORDS:
            if (isRemove) {
                QS_filter_.glb[0] &= (uint8_t)(~0xFEU & 0xFFU);
                QS_filter_.glb[1] &= (uint8_t)(~0x03U & 0xFFU);
                QS_filter_.glb[6] &= (uint8_t)(~0x80U & 0xFFU);
                QS_filter_.glb[7] &= (uint8_t)(~0x03U & 0xFFU);
            }
            else {
                QS_filter_.glb[0] |= 0xFEU;
                QS_filter_.glb[1] |= 0x03U;
                QS_filter_.glb[6] |= 0x80U;
                QS_filter_.glb[7] |= 0x03U;
            }
            break;
        case (uint8_t)QS_AO_RECORDS:
            if (isRemove) {
                QS_filter_.glb[1] &= (uint8_t)(~0xFCU & 0xFFU);
                QS_filter_.glb[2] &= (uint8_t)(~0x07U & 0xFFU);
                QS_filter_.glb[5] &= (uint8_t)(~0x20U & 0xFFU);
            }
            else {
                QS_filter_.glb[1] |= 0xFCU;
                QS_filter_.glb[2] |= 0x07U;
                QS_filter_.glb[5] |= 0x20U;
            }
            break;
        case (uint8_t)QS_EQ_RECORDS:
            if (isRemove) {
                QS_filter_.glb[2] &= (uint8_t)(~0x78U & 0xFFU);
                QS_filter_.glb[5] &= (uint8_t)(~0x40U & 0xFFU);
            }
            else {
                QS_filter_.glb[2] |= 0x78U;
                QS_filter_.glb[5] |= 0x40U;
            }
            break;
        case (uint8_t)QS_MP_RECORDS:
            if (isRemove) {
                QS_filter_.glb[3] &= (uint8_t)(~0x03U & 0xFFU);
                QS_filter_.glb[5] &= (uint8_t)(~0x80U & 0xFFU);
            }
            else {
                QS_filter_.glb[3] |= 0x03U;
                QS_filter_.glb[5] |= 0x80U;
            }
            break;
        case (uint8_t)QS_QF_RECORDS:
            if (isRemove) {
                QS_filter_.glb[2] &= (uint8_t)(~0x80U & 0xFFU);
                QS_filter_.glb[3] &= (uint8_t)(~0xFCU & 0xFFU);
                QS_filter_.glb[4] &= (uint8_t)(~0xC0U & 0xFFU);
                QS_filter_.glb[5] &= (uint8_t)(~0x1FU & 0xFFU);
            }
            else {
                QS_filter_.glb[2] |= 0x80U;
                QS_filter_.glb[3] |= 0xFCU;
                QS_filter_.glb[4] |= 0xC0U;
                QS_filter_.glb[5] |= 0x1FU;
            }
            break;
        case (uint8_t)QS_TE_RECORDS:
            if (isRemove) {
                QS_filter_.glb[4] &= (uint8_t)(~0x3FU & 0xFFU);
            }
            else {
                QS_filter_.glb[4] |= 0x3FU;
            }
            break;
        case (uint8_t)QS_SC_RECORDS:
            if (isRemove) {
                QS_filter_.glb[6] &= (uint8_t)(~0x3FU & 0xFFU);
            }
            else {
                QS_filter_.glb[6] |= 0x3FU;
            }
            break;
        case (uint8_t)QS_SEM_RECORDS:
            if (isRemove) {
                QS_filter_.glb[8] &= (uint8_t)(~0x80U & 0xFFU);
                QS_filter_.glb[9] &= (uint8_t)(~0x07U & 0xFFU);
            }
            else {
                QS_filter_.glb[8] |= 0x80U;
                QS_filter_.glb[9] |= 0x07U;
            }
            break;
        case (uint8_t)QS_MTX_RECORDS:
            if (isRemove) {
                QS_filter_.glb[9]  &= (uint8_t)(~0xF8U & 0xFFU);
                QS_filter_.glb[10] &= (uint8_t)(~0x01U & 0xFFU);
            }
            else {
                QS_filter_.glb[9]  |= 0xF8U;
                QS_filter_.glb[10] |= 0x01U;
            }
            break;
        case (uint8_t)QS_U0_RECORDS:
            if (isRemove) {
                QS_filter_.glb[12] &= (uint8_t)(~0xF0U & 0xFFU);
                QS_filter_.glb[13] &= (uint8_t)(~0x01U & 0xFFU);
            }
            else {
                QS_filter_.glb[12] |= 0xF0U;
                QS_filter_.glb[13] |= 0x01U;
            }
            break;
        case (uint8_t)QS_U1_RECORDS:
            if (isRemove) {
                QS_filter_.glb[13] &= (uint8_t)(~0x3EU & 0xFFU);
            }
            else {
                QS_filter_.glb[13] |= 0x3EU;
            }
            break;
        case (uint8_t)QS_U2_RECORDS:
            if (isRemove) {
                QS_filter_.glb[13] &= (uint8_t)(~0xC0U & 0xFFU);
                QS_filter_.glb[14] &= (uint8_t)(~0x07U & 0xFFU);
            }
            else {
                QS_filter_.glb[13] |= 0xC0U;
                QS_filter_.glb[14] |= 0x07U;
            }
            break;
        case (uint8_t)QS_U3_RECORDS:
            if (isRemove) {
                QS_filter_.glb[14] &= (uint8_t)(~0xF8U & 0xFFU);
            }
            else {
                QS_filter_.glb[14] |= 0xF8U;
            }
            break;
        case (uint8_t)QS_U4_RECORDS:
            if (isRemove) {
                QS_filter_.glb[15] &= 0x1FU;
            }
            else {
                QS_filter_.glb[15] |= 0x1FU;
            }
            break;
        case (uint8_t)QS_UA_RECORDS:
            if (isRemove) {
                QS_filter_.glb[12] &= (uint8_t)(~0xF0U & 0xFFU);
                QS_filter_.glb[13] = 0U;
                QS_filter_.glb[14] = 0U;
                QS_filter_.glb[15] &= (uint8_t)(~0x1FU & 0xFFU);
            }
            else {
                QS_filter_.glb[12] |= 0xF0U;
                QS_filter_.glb[13] |= 0xFFU;
                QS_filter_.glb[14] |= 0xFFU;
                QS_filter_.glb[15] |= 0x1FU;
            }
            break;
        default: {
            QS_CRIT_STAT_
            QS_CRIT_E_();
            /* QS rec number must be below 0x7D, so no need for escaping */
            Q_ASSERT_NOCRIT_(210, rec < 0x7DU);
            QS_CRIT_X_();

            if (isRemove) {
                QS_filter_.glb[rec >> 3U]
                    &= (uint8_t)(~(1U << (rec & 7U)) & 0xFFU);
            }
            else {
                QS_filter_.glb[rec >> 3U]
                    |= (1U << (rec & 7U));
                /* never turn the last 3 records on (0x7D, 0x7E, 0x7F) */
                QS_filter_.glb[15] &= 0x1FU;
            }
            break;
        }
    }
}

/*${QS::QS-tx::locFilter_} .................................................*/
/*! @static @public @memberof QS_tx */
void QS_locFilter_(int_fast16_t const filter) {
    bool const isRemove = (filter < 0);
    uint8_t const qs_id = isRemove ? (uint8_t)(-filter) : (uint8_t)filter;
    uint8_t const tmp = (isRemove ? 0x00U : 0xFFU);
    uint_fast8_t i;
    switch (qs_id) {
        case (uint8_t)QS_ALL_IDS:
            /* set all local filters (partially unrolled loop) */
            for (i = 0U; i < Q_DIM(QS_filter_.loc); i += 4U) {
                QS_filter_.loc[i     ] = tmp;
                QS_filter_.loc[i + 1U] = tmp;
                QS_filter_.loc[i + 2U] = tmp;
                QS_filter_.loc[i + 3U] = tmp;
            }
            break;
        case (uint8_t)QS_AO_IDS:
            for (i = 0U; i < 8U; i += 4U) {
                QS_filter_.loc[i     ] = tmp;
                QS_filter_.loc[i + 1U] = tmp;
                QS_filter_.loc[i + 2U] = tmp;
                QS_filter_.loc[i + 3U] = tmp;
            }
            break;
        case (uint8_t)QS_EP_IDS:
            i = 8U;
            QS_filter_.loc[i     ] = tmp;
            QS_filter_.loc[i + 1U] = tmp;
            break;
        case (uint8_t)QS_AP_IDS:
            i = 12U;
            QS_filter_.loc[i     ] = tmp;
            QS_filter_.loc[i + 1U] = tmp;
            QS_filter_.loc[i + 2U] = tmp;
            QS_filter_.loc[i + 3U] = tmp;
            break;
        default: {
            QS_CRIT_STAT_
            QS_CRIT_E_();
            /* qs_id must be in range */
            Q_ASSERT_NOCRIT_(310, qs_id < 0x7FU);
            QS_CRIT_X_();
            if (isRemove) {
                QS_filter_.loc[qs_id >> 3U]
                    &= (uint8_t)(~(1U << (qs_id & 7U)) & 0xFFU);
            }
            else {
                QS_filter_.loc[qs_id >> 3U]
                    |= (1U << (qs_id & 7U));
            }
            break;
        }
    }
    QS_filter_.loc[0] |= 0x01U; /* leave QS_ID == 0 always on */
}

/*${QS::QS-tx::beginRec_} ..................................................*/
/*! @static @private @memberof QS_tx */
void QS_beginRec_(uint_fast8_t const rec) {
    uint8_t const b = (uint8_t)(QS_priv_.seq + 1U);
    uint8_t chksum  = 0U;                /* reset the checksum */
    uint8_t * const buf = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr head          = QS_priv_.head; /* put in a temporary (register) */
    QSCtr const end     = QS_priv_.end;  /* put in a temporary (register) */

    QS_priv_.seq = b; /* store the incremented sequence num */
    QS_priv_.used += 2U; /* 2 bytes about to be added */

    QS_INSERT_ESC_BYTE_(b)

    chksum = (uint8_t)(chksum + rec); /* update checksum */
    QS_INSERT_BYTE_((uint8_t)rec) /* rec byte does not need escaping */

    QS_priv_.head   = head;   /* save the head */
    QS_priv_.chksum = chksum; /* save the checksum */
}

/*${QS::QS-tx::endRec_} ....................................................*/
/*! @static @private @memberof QS_tx */
void QS_endRec_(void) {
    uint8_t * const buf = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr   head        = QS_priv_.head;
    QSCtr const end     = QS_priv_.end;
    uint8_t b = QS_priv_.chksum;
    b ^= 0xFFU;   /* invert the bits in the checksum */

    QS_priv_.used += 2U; /* 2 bytes about to be added */

    if ((b != QS_FRAME) && (b != QS_ESC)) {
        QS_INSERT_BYTE_(b)
    }
    else {
        QS_INSERT_BYTE_(QS_ESC)
        QS_INSERT_BYTE_(b ^ QS_ESC_XOR)
        ++QS_priv_.used; /* account for the ESC byte */
    }

    QS_INSERT_BYTE_(QS_FRAME) /* do not escape this QS_FRAME */

    QS_priv_.head = head; /* save the head */

    /* overrun over the old data? */
    if (QS_priv_.used > end) {
        QS_priv_.used = end;   /* the whole buffer is used */
        QS_priv_.tail = head;  /* shift the tail to the old data */
    }
}

/*${QS::QS-tx::u8_raw_} ....................................................*/
/*! @static @private @memberof QS_tx */
void QS_u8_raw_(uint8_t const d) {
    uint8_t chksum = QS_priv_.chksum;    /* put in a temporary (register) */
    uint8_t * const buf = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr head          = QS_priv_.head; /* put in a temporary (register) */
    QSCtr const end     = QS_priv_.end;  /* put in a temporary (register) */

    QS_priv_.used += 1U; /* 1 byte about to be added */
    QS_INSERT_ESC_BYTE_(d)

    QS_priv_.head   = head;    /* save the head */
    QS_priv_.chksum = chksum;  /* save the checksum */
}

/*${QS::QS-tx::2u8_raw_} ...................................................*/
/*! @static @private @memberof QS_tx */
void QS_2u8_raw_(
    uint8_t const d1,
    uint8_t const d2)
{
    uint8_t chksum = QS_priv_.chksum;    /* put in a temporary (register) */
    uint8_t * const buf = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr head          = QS_priv_.head; /* put in a temporary (register) */
    QSCtr const end     = QS_priv_.end;  /* put in a temporary (register) */

    QS_priv_.used += 2U; /* 2 bytes are about to be added */
    QS_INSERT_ESC_BYTE_(d1)
    QS_INSERT_ESC_BYTE_(d2)

    QS_priv_.head   = head;    /* save the head */
    QS_priv_.chksum = chksum;  /* save the checksum */
}

/*${QS::QS-tx::u16_raw_} ...................................................*/
/*! @static @private @memberof QS_tx */
void QS_u16_raw_(uint16_t const d) {
    uint8_t chksum = QS_priv_.chksum;    /* put in a temporary (register) */
    uint8_t * const buf = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr head          = QS_priv_.head; /* put in a temporary (register) */
    QSCtr const end     = QS_priv_.end;  /* put in a temporary (register) */
    uint16_t x   = d;

    QS_priv_.used += 2U; /* 2 bytes are about to be added */

    QS_INSERT_ESC_BYTE_((uint8_t)x)
    x >>= 8U;
    QS_INSERT_ESC_BYTE_((uint8_t)x)

    QS_priv_.head   = head;    /* save the head */
    QS_priv_.chksum = chksum;  /* save the checksum */
}

/*${QS::QS-tx::u32_raw_} ...................................................*/
/*! @static @private @memberof QS_tx */
void QS_u32_raw_(uint32_t const d) {
    uint8_t chksum = QS_priv_.chksum;    /* put in a temporary (register) */
    uint8_t * const buf = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr head          = QS_priv_.head; /* put in a temporary (register) */
    QSCtr const end     = QS_priv_.end;  /* put in a temporary (register) */
    uint32_t x = d;

    QS_priv_.used += 4U; /* 4 bytes are about to be added */
    for (uint_fast8_t i = 4U; i != 0U; --i) {
        QS_INSERT_ESC_BYTE_((uint8_t)x)
        x >>= 8U;
    }

    QS_priv_.head   = head;    /* save the head */
    QS_priv_.chksum = chksum;  /* save the checksum */
}

/*${QS::QS-tx::obj_raw_} ...................................................*/
/*! @static @private @memberof QS_tx */
void QS_obj_raw_(void const * const obj) {
    #if (QS_OBJ_PTR_SIZE == 1U)
        QS_u8_raw_((uint8_t)obj);
    #elif (QS_OBJ_PTR_SIZE == 2U)
        QS_u16_raw_((uint16_t)obj);
    #elif (QS_OBJ_PTR_SIZE == 4U)
        QS_u32_raw_((uint32_t)obj);
    #elif (QS_OBJ_PTR_SIZE == 8U)
        QS_u64_raw_((uint64_t)obj);
    #else
        QS_u32_raw_((uint32_t)obj);
    #endif
}

/*${QS::QS-tx::str_raw_} ...................................................*/
/*! @static @private @memberof QS_tx */
void QS_str_raw_(char const * const str) {
    uint8_t chksum = QS_priv_.chksum;    /* put in a temporary (register) */
    uint8_t * const buf = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr head          = QS_priv_.head; /* put in a temporary (register) */
    QSCtr const end     = QS_priv_.end;  /* put in a temporary (register) */
    QSCtr used          = QS_priv_.used; /* put in a temporary (register) */

    for (char const *s = str; *s != '\0'; ++s) {
        chksum += (uint8_t)*s; /* update checksum */
        QS_INSERT_BYTE_((uint8_t)*s)  /* ASCII char doesn't need escaping */
        ++used;
    }
    QS_INSERT_BYTE_((uint8_t)'\0')  /* zero-terminate the string */
    ++used;

    QS_priv_.head   = head;   /* save the head */
    QS_priv_.chksum = chksum; /* save the checksum */
    QS_priv_.used   = used;   /* save # of used buffer space */
}

/*${QS::QS-tx::u8_fmt_} ....................................................*/
/*! @static @private @memberof QS_tx */
void QS_u8_fmt_(
    uint8_t const format,
    uint8_t const d)
{
    uint8_t chksum = QS_priv_.chksum;    /* put in a temporary (register) */
    uint8_t * const buf = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr   head        = QS_priv_.head; /* put in a temporary (register) */
    QSCtr const end     = QS_priv_.end;  /* put in a temporary (register) */

    QS_priv_.used += 2U; /* 2 bytes about to be added */

    QS_INSERT_ESC_BYTE_(format)
    QS_INSERT_ESC_BYTE_(d)

    QS_priv_.head   = head;   /* save the head */
    QS_priv_.chksum = chksum; /* save the checksum */
}

/*${QS::QS-tx::u16_fmt_} ...................................................*/
/*! @static @private @memberof QS_tx */
void QS_u16_fmt_(
    uint8_t const format,
    uint16_t const d)
{
    uint8_t chksum = QS_priv_.chksum;    /* put in a temporary (register) */
    uint8_t * const buf = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr head          = QS_priv_.head; /* put in a temporary (register) */
    QSCtr const end     = QS_priv_.end;  /* put in a temporary (register) */
    uint8_t b = (uint8_t)d;

    QS_priv_.used += 3U; /* 3 bytes about to be added */

    QS_INSERT_ESC_BYTE_(format)
    QS_INSERT_ESC_BYTE_(b)
    b = (uint8_t)(d >> 8U);
    QS_INSERT_ESC_BYTE_(b)

    QS_priv_.head   = head;   /* save the head */
    QS_priv_.chksum = chksum; /* save the checksum */
}

/*${QS::QS-tx::u32_fmt_} ...................................................*/
/*! @static @private @memberof QS_tx */
void QS_u32_fmt_(
    uint8_t const format,
    uint32_t const d)
{
    uint8_t chksum = QS_priv_.chksum;    /* put in a temporary (register) */
    uint8_t * const buf = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr head          = QS_priv_.head; /* put in a temporary (register) */
    QSCtr const end     = QS_priv_.end;  /* put in a temporary (register) */
    uint32_t x = d;

    QS_priv_.used += 5U; /* 5 bytes about to be added */
    QS_INSERT_ESC_BYTE_(format) /* insert the format byte */

    /* insert 4 bytes... */
    for (uint_fast8_t i = 4U; i != 0U; --i) {
        QS_INSERT_ESC_BYTE_((uint8_t)x)
        x >>= 8U;
    }

    QS_priv_.head   = head;   /* save the head */
    QS_priv_.chksum = chksum; /* save the checksum */
}

/*${QS::QS-tx::str_fmt_} ...................................................*/
/*! @static @private @memberof QS_tx */
void QS_str_fmt_(char const * const str) {
    uint8_t chksum = QS_priv_.chksum;
    uint8_t * const buf = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr head          = QS_priv_.head; /* put in a temporary (register) */
    QSCtr const end     = QS_priv_.end;  /* put in a temporary (register) */
    QSCtr used          = QS_priv_.used; /* put in a temporary (register) */

    used += 2U; /* account for the format byte and the terminating-0 */
    QS_INSERT_BYTE_((uint8_t)QS_STR_T)
    chksum += (uint8_t)QS_STR_T;

    for (char const *s = str; *s != '\0'; ++s) {
        QS_INSERT_BYTE_((uint8_t)*s) /* ASCII char doesn't need escaping */
        chksum += (uint8_t)*s; /* update checksum */
        ++used;
    }
    QS_INSERT_BYTE_(0U) /* zero-terminate the string */

    QS_priv_.head   = head;    /* save the head */
    QS_priv_.chksum = chksum;  /* save the checksum */
    QS_priv_.used   = used;    /* save # of used buffer space */
}

/*${QS::QS-tx::mem_fmt_} ...................................................*/
/*! @static @private @memberof QS_tx */
void QS_mem_fmt_(
    uint8_t const * const blk,
    uint8_t const size)
{
    uint8_t chksum = QS_priv_.chksum;
    uint8_t * const buf = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr head          = QS_priv_.head; /* put in a temporary (register) */
    QSCtr const end     = QS_priv_.end;  /* put in a temporary (register) */
    uint8_t const *pb   = blk;

    QS_priv_.used += ((QSCtr)size + 2U); /* size+2 bytes to be added */

    QS_INSERT_BYTE_((uint8_t)QS_MEM_T)
    chksum += (uint8_t)QS_MEM_T;

    QS_INSERT_ESC_BYTE_(size)
    /* output the 'size' # bytes */
    for (uint8_t len = size; len > 0U; --len) {
        QS_INSERT_ESC_BYTE_(*pb)
        ++pb;
    }

    QS_priv_.head   = head;   /* save the head */
    QS_priv_.chksum = chksum; /* save the checksum */
}

/*${QS::QS-tx::sig_dict_pre_} ..............................................*/
/*! @static @private @memberof QS_tx */
void QS_sig_dict_pre_(
    QSignal const sig,
    void const * const obj,
    char const * const name)
{
    QS_CRIT_STAT_
    QS_CRIT_E_();
    QS_MEM_SYS_();

    QS_beginRec_((uint_fast8_t)QS_SIG_DICT);
    QS_SIG_PRE_(sig);
    QS_OBJ_PRE_(obj);
    QS_str_raw_((*name == '&') ? &name[1] : name);
    QS_endRec_();

    QS_MEM_APP_();
    QS_CRIT_X_();
    QS_onFlush();
}

/*${QS::QS-tx::obj_dict_pre_} ..............................................*/
/*! @static @private @memberof QS_tx */
void QS_obj_dict_pre_(
    void const * const obj,
    char const * const name)
{
    QS_CRIT_STAT_
    QS_CRIT_E_();
    QS_MEM_SYS_();

    QS_beginRec_((uint_fast8_t)QS_OBJ_DICT);
    QS_OBJ_PRE_(obj);
    QS_str_raw_((*name == '&') ? &name[1] : name);
    QS_endRec_();

    QS_MEM_APP_();
    QS_CRIT_X_();
    QS_onFlush();
}

/*${QS::QS-tx::obj_arr_dict_pre_} ..........................................*/
/*! @static @private @memberof QS_tx */
void QS_obj_arr_dict_pre_(
    void const * const obj,
    uint_fast16_t const idx,
    char const * const name)
{
    QS_CRIT_STAT_
    QS_CRIT_E_();
    Q_REQUIRE_NOCRIT_(400, idx < 1000U);
    QS_CRIT_X_();

    /* format idx into a char buffer as "xxx\0" */
    uint8_t idx_str[4];
    uint_fast16_t tmp = idx;
    uint8_t i;
    idx_str[3] = 0U; /* zero-terminate */
    idx_str[2] = (uint8_t)((uint8_t)'0' + (tmp % 10U));
    tmp /= 10U;
    idx_str[1] =  (uint8_t)((uint8_t)'0' + (tmp % 10U));
    if (idx_str[1] == (uint8_t)'0') {
       i = 2U;
    }
    else {
       tmp /= 10U;
       idx_str[0] =  (uint8_t)((uint8_t)'0' + (tmp % 10U));
        if (idx_str[0] == (uint8_t)'0') {
           i = 1U;
        }
        else {
           i = 0U;
        }
    }

    uint8_t j = ((*name == '&') ? 1U : 0U);

    QS_CRIT_E_();
    QS_MEM_SYS_();

    QS_beginRec_((uint_fast8_t)QS_OBJ_DICT);
    QS_OBJ_PRE_(obj);
    for (; name[j] != '\0'; ++j) {
        QS_U8_PRE_(name[j]);
        if (name[j] == '[') {
            ++j;
            break;
        }
    }
    for (; idx_str[i] != 0U; ++i) {
        QS_U8_PRE_(idx_str[i]);
    }
    /* skip chars until ']' */
    for (; name[j] != '\0'; ++j) {
        if (name[j] == ']') {
            break;
        }
    }
    for (; name[j] != '\0'; ++j) {
        QS_U8_PRE_(name[j]);
    }
    QS_U8_PRE_(0U); /* zero-terminate */
    QS_endRec_();

    QS_MEM_APP_();
    QS_CRIT_X_();
    QS_onFlush();
}

/*${QS::QS-tx::fun_dict_pre_} ..............................................*/
/*! @static @private @memberof QS_tx */
void QS_fun_dict_pre_(
    QSpyFunPtr const fun,
    char const * const name)
{
    QS_CRIT_STAT_
    QS_CRIT_E_();
    QS_MEM_SYS_();

    QS_beginRec_((uint_fast8_t)QS_FUN_DICT);
    QS_FUN_PRE_(fun);
    QS_str_raw_((*name == '&') ? &name[1] : name);
    QS_endRec_();

    QS_MEM_APP_();
    QS_CRIT_X_();
    QS_onFlush();
}

/*${QS::QS-tx::usr_dict_pre_} ..............................................*/
/*! @static @private @memberof QS_tx */
void QS_usr_dict_pre_(
    enum_t const rec,
    char const * const name)
{
    QS_CRIT_STAT_
    QS_CRIT_E_();
    QS_MEM_SYS_();

    QS_beginRec_((uint_fast8_t)QS_USR_DICT);
    QS_u8_raw_((uint8_t)rec);
    QS_str_raw_(name);
    QS_endRec_();

    QS_MEM_APP_();
    QS_CRIT_X_();
    QS_onFlush();
}

/*${QS::QS-tx::enum_dict_pre_} .............................................*/
/*! @static @private @memberof QS_tx */
void QS_enum_dict_pre_(
    enum_t const value,
    uint8_t const group,
    char const * const name)
{
    QS_CRIT_STAT_
    QS_CRIT_E_();
    QS_MEM_SYS_();

    QS_beginRec_((uint_fast8_t)QS_ENUM_DICT);
    QS_2U8_PRE_((uint8_t)value, group);
    QS_str_raw_(name);
    QS_endRec_();

    QS_MEM_APP_();
    QS_CRIT_X_();
    QS_onFlush();
}

/*${QS::QS-tx::ASSERTION} ..................................................*/
/*! @static @public @memberof QS_tx */
void QS_ASSERTION(
    char const * const module,
    int_t const id,
    uint32_t const delay)
{
    QS_BEGIN_NOCRIT_PRE_(QS_ASSERT_FAIL, 0U)
        QS_TIME_PRE_();
        QS_U16_PRE_(id);
        QS_STR_PRE_((module != (char *)0) ? module : "?");
    QS_END_NOCRIT_PRE_()
    QS_onFlush();

    for (uint32_t volatile delay_ctr = delay; delay_ctr > 0U; --delay_ctr) {
    }
    QS_onCleanup();
}

/*${QS::QS-tx::target_info_pre_} ...........................................*/
/*! @static @private @memberof QS_tx */
void QS_target_info_pre_(uint8_t const isReset) {
    static uint8_t const ZERO = (uint8_t)'0';
    static uint8_t const * const TIME = (uint8_t const *)&Q_BUILD_TIME[0];
    static uint8_t const * const DATE = (uint8_t const *)&Q_BUILD_DATE[0];
    static union {
        uint16_t u16;
        uint8_t  u8[2];
    } endian_test;

    endian_test.u16 = 0x0102U;
    QS_beginRec_((uint_fast8_t)QS_TARGET_INFO);
        QS_U8_PRE_(isReset);
        QS_U16_PRE_(((endian_test.u8[0] == 0x01U) /* big endian? */
                    ? (0x8000U | QP_VERSION)
                    : QP_VERSION)); /* target endianness + version number */

        /* send the object sizes... */
        QS_U8_PRE_(Q_SIGNAL_SIZE | (QF_EVENT_SIZ_SIZE << 4U));

    #ifdef QF_EQUEUE_CTR_SIZE
        QS_U8_PRE_(QF_EQUEUE_CTR_SIZE | (QF_TIMEEVT_CTR_SIZE << 4U));
    #else
        QS_U8_PRE_(QF_TIMEEVT_CTR_SIZE << 4U);
    #endif /* QF_EQUEUE_CTR_SIZE */

    #ifdef QF_MPOOL_CTR_SIZE
        QS_U8_PRE_(QF_MPOOL_SIZ_SIZE | (QF_MPOOL_CTR_SIZE << 4U));
    #else
        QS_U8_PRE_(0U);
    #endif /* QF_MPOOL_CTR_SIZE */

        QS_U8_PRE_(QS_OBJ_PTR_SIZE | (QS_FUN_PTR_SIZE << 4U));
        QS_U8_PRE_(QS_TIME_SIZE);

        /* send the limits... */
        QS_U8_PRE_(QF_MAX_ACTIVE);
        QS_U8_PRE_(QF_MAX_EPOOL | (QF_MAX_TICK_RATE << 4U));

        /* send the build time in three bytes (sec, min, hour)... */
        QS_U8_PRE_((10U * (uint8_t)(TIME[6] - ZERO))
                   + (uint8_t)(TIME[7] - ZERO));
        QS_U8_PRE_((10U * (uint8_t)(TIME[3] - ZERO))
                   + (uint8_t)(TIME[4] - ZERO));
        if (Q_BUILD_TIME[0] == ' ') {
            QS_U8_PRE_(TIME[1] - ZERO);
        }
        else {
            QS_U8_PRE_((10U * (uint8_t)(TIME[0] - ZERO))
                      + (uint8_t)(TIME[1] - ZERO));
        }

        /* send the build date in three bytes (day, month, year) ... */
        if (Q_BUILD_DATE[4] == ' ') {
            QS_U8_PRE_(DATE[5] - ZERO);
        }
        else {
            QS_U8_PRE_((10U * (uint8_t)(DATE[4] - ZERO))
                       + (uint8_t)(DATE[5] - ZERO));
        }
        /* convert the 3-letter month to a number 1-12 ... */
        uint8_t b;
        switch ((int_t)DATE[0] + (int_t)DATE[1] + (int_t)DATE[2]) {
            case (int_t)'J' + (int_t)'a' + (int_t)'n':
                b = 1U;
                break;
            case (int_t)'F' + (int_t)'e' + (int_t)'b':
                b = 2U;
                break;
            case (int_t)'M' + (int_t)'a' + (int_t)'r':
                b = 3U;
                break;
            case (int_t)'A' + (int_t)'p' + (int_t)'r':
                b = 4U;
                break;
            case (int_t)'M' + (int_t)'a' + (int_t)'y':
                b = 5U;
                break;
            case (int_t)'J' + (int_t)'u' + (int_t)'n':
                b = 6U;
                break;
            case (int_t)'J' + (int_t)'u' + (int_t)'l':
                b = 7U;
                break;
            case (int_t)'A' + (int_t)'u' + (int_t)'g':
                b = 8U;
                break;
            case (int_t)'S' + (int_t)'e' + (int_t)'p':
                b = 9U;
                break;
            case (int_t)'O' + (int_t)'c' + (int_t)'t':
                b = 10U;
                break;
            case (int_t)'N' + (int_t)'o' + (int_t)'v':
                b = 11U;
                break;
            case (int_t)'D' + (int_t)'e' + (int_t)'c':
                b = 12U;
                break;
            default:
                b = 0U;
                break;
        }
        QS_U8_PRE_(b); /* store the month */
        QS_U8_PRE_((10U * (uint8_t)(DATE[9] - ZERO))
                   + (uint8_t)(DATE[10] - ZERO));
    QS_endRec_();
}
/*$enddef${QS::QS-tx} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#ifndef QF_MEM_ISOLATE
/*$define${QS::QS-filter} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${QS::QS-filter::filter_} ................................................*/
QSpyFilter QS_filter_;
/*$enddef${QS::QS-filter} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#endif
