/* bsp.h ----------------- */
extern QActive * const the_Ticker0; /* "ticker" pointer for clock rate 0 */
extern QActive * const the Ticker1; /* "ticker" pointer for clock rate 1 */


/* bsp.c ----------------- */
static QTicker l_ticker0; /* "ticker" instance for clock rate 0 */
QActive * const the_Ticker0 = &l_ticker0.super;

static QTicker l_ticker1; /* "ticker" instance for clock rate 1 */
QActive * const the_Ticker1 = &l_ticker1.super;

/* system clock tick ISR for tick rate 0 */
void SysTick_Handler(void) {
    . . .
    QTICKER_TICK(the_Ticker0, &qs_tick0_id);
    . . .
}

/* system clock tick ISR for tick rate 1 */
void Timer0A_IRQHandler(void) {
    . . .
    QTICKER_TICK(the_Ticker1, &qs_tick1_id);
    . . .
}


/* main.c --------------- */
main () {
    . . .
    QACTIVE_START(the_Ticker0,
                  1U, /* priority */
                  0, 0, 0, 0, 0); /* not used */

    QACTIVE_START(the_Ticker1,
                  2U, /* priority */
                  0, 0, 0, 0, 0); /* not used */
    . . .
}
