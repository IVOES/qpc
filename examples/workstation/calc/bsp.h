/*****************************************************************************
* Product: Board Support Package (BSP) for the Calculator example
* Last updated for version 6.5.0
* Last updated on  2019-03-20
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <www.gnu.org/licenses/>.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*****************************************************************************/
#ifndef BSP_H_
#define BSP_H_

void BSP_clear(void);
void BSP_negate(void);
void BSP_insert(int keyId);
double BSP_get_value(void);
void BSP_display(double value);
void BSP_display_error(char const *err);
void BSP_exit(void);

void BSP_show_display(void);
void BSP_message(char const *msg);

#endif /* BSP_H_ */
