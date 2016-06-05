/*
 *  NodeGarden NodEdoN support
 *
 *  Copyright (C) 2015 by David Thornley <david@thornleys.info>
 *  Based on mach-zsun-reader.c
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>
#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#include <linux/gpio.h>

#define FAKE_TPLINK

#ifdef FAKE_TPLINK
#define TPLINK_GPIO_PIN_USBPOWER 				8
#endif

#define NODEGARDEN_NODEDON_GPIO_TPLINK_PIN_USBPOWER 	8
#define NODEGARDEN_NODEDON_GPIO_TPLINK_BTN_RESET		11
#define NODEGARDEN_NODEDON_GPIO_TPLINK_LED_SYSTEM		27

#define NODEGARDEN_NODEDON_GPIO_LED_SYSTEM		0
#define NODEGARDEN_NODEDON_GPIO_SW_SD			22

#define NODEGARDEN_NODEDON_ART_DATA_ADDR		0x1fff0000

#define NODEGARDEN_NODEDON_MAC0_OFFSET			0x0000
#define NODEGARDEN_NODEDON_MAC1_OFFSET			0x0006

#define NODEGARDEN_NODEDON_CALDATA_OFFSET		0x1000
#define NODEGARDEN_NODEDON_WMAC_MAC_OFFSET		0x1002

#define NODEGARDEN_NODEDON_KEYS_POLL_INTERVAL	20	/* msecs */
#define NODEGARDEN_NODEDON_KEYS_DEBOUNCE_INTERVAL	(3 * NODEGARDEN_NODEDON_KEYS_POLL_INTERVAL)

static struct gpio_led NODEGARDEN_NODEDON_leds_gpio[] __initdata = {
#ifdef FAKE_TPLINK
	{
		.name		= "nodedon:blue:system",
		.gpio		= NODEGARDEN_NODEDON_GPIO_TPLINK_LED_SYSTEM,
		.active_low	= 1,
	},
#else
	{
		.name		= "nodedon:green:system",
		.gpio		= NODEGARDEN_NODEDON_GPIO_LED_SYSTEM,
		.active_low	= 0,
	}
#endif
};

static struct gpio_keys_button NODEGARDEN_NODEDON_gpio_keys[] __initdata = {
#ifdef FAKE_TPLINK
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = NODEGARDEN_NODEDON_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= NODEGARDEN_NODEDON_GPIO_TPLINK_BTN_RESET,
		.active_low	= 0,
	},
#else
	{
		.desc           = "SD Card insert switch",
		.type           = EV_KEY,
		.code           = BTN_1,
		.debounce_interval = NODEGARDEN_NODEDON_KEYS_DEBOUNCE_INTERVAL,
		.gpio           = NODEGARDEN_NODEDON_GPIO_SW_SD,
		.active_low	= 1,
	}
#endif
};

static void __init nodegarden_nodedon_common_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(NODEGARDEN_NODEDON_ART_DATA_ADDR);

	ath79_register_m25p80(NULL);

	ath79_setup_ar933x_phy4_switch(true, true);

	ath79_register_wmac(art + NODEGARDEN_NODEDON_CALDATA_OFFSET,
			    		art + NODEGARDEN_NODEDON_WMAC_MAC_OFFSET);

	//ath79_init_mac(ath79_eth0_data.mac_addr, art + NODEGARDEN_NODEDON_MAC0_OFFSET, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, art + NODEGARDEN_NODEDON_MAC1_OFFSET, 0);


	ath79_register_mdio(0, 0x0);

	//ath79_register_eth(0);
	ath79_register_eth(1);
}

static void __init nodegarden_nodedon_setup(void)
{
	nodegarden_nodedon_common_setup();

	ath79_gpio_function_disable(AR724X_GPIO_FUNC_ETH_SWITCH_LED0_EN |
								AR724X_GPIO_FUNC_ETH_SWITCH_LED1_EN |
								AR724X_GPIO_FUNC_ETH_SWITCH_LED2_EN |
								AR724X_GPIO_FUNC_ETH_SWITCH_LED3_EN |
								AR724X_GPIO_FUNC_ETH_SWITCH_LED4_EN);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(NODEGARDEN_NODEDON_leds_gpio),
				 				 NODEGARDEN_NODEDON_leds_gpio);

	ath79_register_gpio_keys_polled(-1, NODEGARDEN_NODEDON_KEYS_POLL_INTERVAL,
										ARRAY_SIZE(NODEGARDEN_NODEDON_gpio_keys),
										NODEGARDEN_NODEDON_gpio_keys);

#ifdef FAKE_TPLINK
	/* enable when running image on TPLink boards*/
	gpio_request_one(NODEGARDEN_NODEDON_GPIO_TPLINK_PIN_USBPOWER,
				 		GPIOF_OUT_INIT_HIGH | GPIOF_EXPORT_DIR_FIXED,
	 			 		"USB power");
#endif

	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_NG_NODEDON, "NG-NODEDON", "NodeGarden NODEDON", nodegarden_nodedon_setup);

