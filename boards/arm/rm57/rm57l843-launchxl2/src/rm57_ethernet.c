/****************************************************************************
 * boards/arm/rm57/rm57l843-launchxl2/src/rm57_ethernet.c
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>

#include "arm_internal.h"
#include "hardware/rm57l843_memorymap.h"
#include "rm57_ethernet.h"

#include "rm57l843-launchxl2.h"

/* Only meaningful under CONFIG_NETDEV_LATEINIT: otherwise
 * arm_netinitialize() (rm57_netinitialize.c) already registers the EMAC
 * with a fixed placeholder MAC address before rm57_bringup() ever runs,
 * and calling rm57_ethinitialize() a second time here would attempt to
 * register the same netdev twice.
 */

#if defined(CONFIG_RM57_EMAC) && defined(CONFIG_NETDEV_LATEINIT)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Die ID registers (SPNU562A system module) - no CONFIG_RM57_EMAC-
 * independent consumer exists yet in the shared hardware/rm57_sys.h
 * header, so the addresses are computed inline here rather than adding
 * board-specific-use macros to a shared file.
 */

#define RM57_SYS_DIEIDL (RM57_SYS1_BASE + 0x007c)
#define RM57_SYS_DIEIDH (RM57_SYS1_BASE + 0x0080)

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: rm57_eth_setup
 *
 * Description:
 *   Derive a locally-administered MAC address from the device's unique
 *   die ID and register the EMAC driver.
 *
 ****************************************************************************/

int rm57_eth_setup(void)
{
  uint8_t mac[6];
  uint32_t dieidl;
  uint32_t dieidh;

  dieidl = getreg32(RM57_SYS_DIEIDL);
  dieidh = getreg32(RM57_SYS_DIEIDH);

  /* Locally-administered, unicast (bit 1 set, bit 0 clear of the first
   * octet) - avoids colliding with any IEEE-assigned OUI.
   */

  mac[0] = 0x02;
  mac[1] = (uint8_t)(dieidh >> 24);
  mac[2] = (uint8_t)(dieidh >> 16);
  mac[3] = (uint8_t)(dieidh >> 8);
  mac[4] = (uint8_t)(dieidl >> 8);
  mac[5] = (uint8_t)(dieidl >> 0);

  return rm57_ethinitialize(0, mac);
}

#endif /* CONFIG_RM57_EMAC && CONFIG_NETDEV_LATEINIT */
