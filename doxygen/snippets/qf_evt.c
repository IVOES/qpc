typedef struct {
    QEvt super;  /* inherit QEvt */
    uint8_t id;  /* id of the pressed button */
} ButtonPressEvt;
. . .
/* immutable button-press event */
static ButtonWorkEvt const pressEvt = {
    QEVT_INITIALIZER(BUTTON_PRESSED_SIG),
    .id = 123U
};
. . .
QACTIVE_POST(AO_Button, &pressEvt.super, &l_SysTick_Handler);
