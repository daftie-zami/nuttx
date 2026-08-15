/****************************************************************************
 * boards/arm/rm57/rm57l843-launchxl2/src/rm57_bringup.c
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

#include <nuttx/board.h>
#include <nuttx/irq.h>
#include <arch/board/board.h>

#include "rm57_gio.h"
#include "rm57l843-launchxl2.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#undef HAVE_BUTTON_LED_DEMO
#if defined(CONFIG_ARCH_BUTTONS) && defined(CONFIG_ARCH_IRQBUTTONS) && \
    defined(CONFIG_RM57_GIO_IRQ)
#  define HAVE_BUTTON_LED_DEMO 1
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: rm57_button_b4_isr and rm57_button_b5_isr
 *
 * Description:
 *   Demo button IRQ handlers: light LED B6 while button B4 is held down and
 *   LED B7 while button B5 is held down.  These run directly in interrupt
 *   context - board_userled()/rm57_giowrite() boil down to a single atomic
 *   putreg32() to a hardware set/clear register, so no work queue or
 *   deferred processing is needed here.
 *   The interrupt fires on both the press and release edge, so the
 *   handler re-reads the pin level to tell which edge occurred (this is
 *   also why board_userled_initialize() must run first - the LED GIOs
 *   otherwise stay in their reset-default input mode and this write would
 *   be silently ineffective).
 *
 ****************************************************************************/

#ifdef HAVE_BUTTON_LED_DEMO
static int rm57_button_b4_isr(int irq, FAR void *context, FAR void *arg)
{
  bool pressed = !rm57_gioread(GIO_BUTTON_B4);

  board_userled(BOARD_LED_B6, pressed);
  return OK;
}

static int rm57_button_b5_isr(int irq, FAR void *context, FAR void *arg)
{
  bool pressed = !rm57_gioread(GIO_BUTTON_B5);

  board_userled(BOARD_LED_B7, pressed);
  return OK;
}
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: rm57_bringup
 *
 * Description:
 *   Bring up board features.
 *
 ****************************************************************************/

int rm57_bringup(void)
{
#ifdef CONFIG_RM57_DCAN
  int ret = rm57_can_setup();
  if (ret < 0)
    {
      return ret;
    }
#endif

#if defined(CONFIG_RM57_EMAC) && defined(CONFIG_NETDEV_LATEINIT)
  int eth_ret = rm57_eth_setup();
  if (eth_ret < 0)
    {
      return eth_ret;
    }
#endif

#ifdef HAVE_BUTTON_LED_DEMO
  board_button_initialize();
  board_userled_initialize();

  /* GIO_LED_B6/GIO_LED_B7 both encode GIO_OUTPUT_SET (initial value = on)
   * in their pinset, so board_userled_initialize() just lit both LEDs.
   * Force a known off baseline so the only thing that lights either LED
   * afterward is a button handler below - otherwise both would look
   * "stuck on" until the first button edge.
   */

  board_userled_all(0);

  board_button_irq(BOARD_BUTTON_B4, rm57_button_b4_isr, NULL);
  board_button_irq(BOARD_BUTTON_B5, rm57_button_b5_isr, NULL);
#endif

  return OK;
}
