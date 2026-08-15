/****************************************************************************
 * boards/arm/rm57/rm57l843-launchxl2/src/rm57l843-launchxl2.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __BOARDS_ARM_RM57_RM57L843_LAUNCHXL2_SRC_RM57L843_LAUNCHXL2_H
#define __BOARDS_ARM_RM57_RM57L843_LAUNCHXL2_SRC_RM57L843_LAUNCHXL2_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include "rm57_gio.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* LEDs */
#define GIO_LED_B6    (GIO_OUTPUT | GIO_CFG_DEFAULT | GIO_OUTPUT_SET | \
                       GIO_PORT_GIOB | GIO_PIN6)
#define GIO_LED_B7    (GIO_OUTPUT | GIO_CFG_DEFAULT | GIO_OUTPUT_SET | \
                       GIO_PORT_GIOB | GIO_PIN7)

/* Buttons */
#define GIO_BUTTON_B4 (GIO_INPUT | GIO_CFG_PULLUP | GIO_INT_BOTHEDGES | \
                       GIO_PORT_GIOB | GIO_PIN4)
#define GIO_BUTTON_B5 (GIO_INPUT | GIO_CFG_PULLUP | GIO_INT_BOTHEDGES | \
                       GIO_PORT_GIOB | GIO_PIN5)

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: rm57_bringup
 *
 * Description:
 *   Bring up board features.
 *
 ****************************************************************************/

int rm57_bringup(void);

/****************************************************************************
 * Name: rm57_can_setup
 *
 * Description:
 *   Register the enabled DCAN instances (character device and/or
 *   SocketCAN, per the RM57_DCANn Kconfig choices).
 *
 ****************************************************************************/

#ifdef CONFIG_RM57_DCAN
int rm57_can_setup(void);
#endif

/****************************************************************************
 * Name: rm57_eth_setup
 *
 * Description:
 *   Derive a locally-administered MAC address from the device's unique
 *   die ID and register the EMAC driver.  Only built/called under
 *   CONFIG_NETDEV_LATEINIT - see src/rm57_ethernet.c.
 *
 ****************************************************************************/

#if defined(CONFIG_RM57_EMAC) && defined(CONFIG_NETDEV_LATEINIT)
int rm57_eth_setup(void);
#endif

#endif /* __BOARDS_ARM_RM57_RM57L843_LAUNCHXL2_SRC_RM57L843_LAUNCHXL2_H */
