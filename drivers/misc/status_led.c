// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2000-2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#include <common.h>
#include <status_led.h>

/*
 * The purpose of this code is to signal the operational status of a
 * target which usually boots over the network; while running in
 * U-Boot, a status LED is blinking. As soon as a valid BOOTP reply
 * message has been received, the LED is turned off. The Linux
 * kernel, once it is running, will start blinking the LED again,
 * with another frequency.
 */

/* ------------------------------------------------------------------------- */

typedef struct {
	led_id_t mask;
	int state;
	int period;
	int cnt;
} led_dev_t;

led_dev_t led_dev[] = {
	{	CONFIG_LED_STATUS_BIT,
		CONFIG_LED_STATUS_STATE,
		LED_STATUS_PERIOD,
		0,
	},
#if defined(CONFIG_LED_STATUS1)
	{	CONFIG_LED_STATUS_BIT1,
		CONFIG_LED_STATUS_STATE1,
		LED_STATUS_PERIOD1,
		0,
	},
#endif
#if defined(CONFIG_LED_STATUS2)
	{	CONFIG_LED_STATUS_BIT2,
		CONFIG_LED_STATUS_STATE2,
		LED_STATUS_PERIOD2,
		0,
	},
#endif
#if defined(CONFIG_LED_STATUS3)
	{	CONFIG_LED_STATUS_BIT3,
		CONFIG_LED_STATUS_STATE3,
		LED_STATUS_PERIOD3,
		0,
	},
#endif
#if defined(CONFIG_LED_STATUS4)
	{	CONFIG_LED_STATUS_BIT4,
		CONFIG_LED_STATUS_STATE4,
		LED_STATUS_PERIOD4,
		0,
	},
#endif
#if defined(CONFIG_LED_STATUS5)
	{	CONFIG_LED_STATUS_BIT5,
		CONFIG_LED_STATUS_STATE5,
		LED_STATUS_PERIOD5,
		0,
	},
#endif
};

#define MAX_LED_DEV	(sizeof(led_dev)/sizeof(led_dev_t))
#define LED_INITIALIZED_BIT			0x8
#define LED_STATE_MASK				0x3

#define LED_IS_INITIALIZED(x)		(!!((x) & LED_INITIALIZED_BIT))
#define LED_INITIALIZE(x)			((x) |= (LED_INITIALIZED_BIT))
#define LED_GET_STATE(x)			((x) & LED_STATE_MASK)

void status_led_init(void)
{
	led_dev_t *ld;
	int i;

	for (i = 0, ld = led_dev; i < MAX_LED_DEV; i++, ld++) {
		__led_init (ld->mask, ld->state);
		LED_INITIALIZE(ld->state);
	}
}

void status_led_tick(ulong timestamp)
{
	led_dev_t *ld;
	int i;

	for (i = 0, ld = led_dev; i < MAX_LED_DEV; i++, ld++) {

		if (!LED_IS_INITIALIZED(ld->state))
			status_led_init();

		if (ld->state != CONFIG_LED_STATUS_BLINKING)
			continue;

		if (++ld->cnt >= ld->period) {
			__led_toggle (ld->mask);
			ld->cnt -= ld->period;
		}
	}
}

void status_led_set(int led, int state)
{
	led_dev_t *ld;

	if (led < 0 || led >= MAX_LED_DEV)
		return;

	ld = &led_dev[led];

	if (!LED_IS_INITIALIZED(ld->state))
		status_led_init();

	ld->state = LED_GET_STATE(ld->state);
	if (state == CONFIG_LED_STATUS_BLINKING) {
		ld->cnt = 0;		/* always start with full period    */
		state = CONFIG_LED_STATUS_ON;	/* always start with LED _ON_ */
	}
	__led_set (ld->mask, state);
}
