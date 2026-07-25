/****************************************************************************
 * arch/arm/src/rm57/hardware/rm57_dma.h
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

/* Register offsets and bit fields below are transcribed from TI's TRM
 * (SPNU562A, chapter 20 "Direct Memory Access Controller (DMA) Module"),
 * not assumed from another Hercules part - the RM57L843 DMA controller
 * (32 channels, 48 hardware requests, 2 ports) differs materially from
 * TMS570LS04x's smaller DMA instance.
 */

#ifndef __ARCH_ARM_SRC_RM57_HARDWARE_RM57_DMA_H
#define __ARCH_ARM_SRC_RM57_HARDWARE_RM57_DMA_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "hardware/rm57l843_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* DMA Control Register Offsets (base = RM57_DMA_BASE, TRM Table 20-7) *****/

#define RM57_DMA_GCTRL_OFFSET        0x0000 /* Global Control Register */
#define RM57_DMA_PEND_OFFSET         0x0004 /* Channel Pending Register */
#define RM57_DMA_DMASTAT_OFFSET      0x000c /* DMA Status Register */
#define RM57_DMA_DMAREVID_OFFSET     0x0010 /* DMA Revision ID Register */
#define RM57_DMA_HWCHENAS_OFFSET     0x0014 /* HW Channel Enable Set/Status */
#define RM57_DMA_HWCHENAR_OFFSET     0x001c /* HW Channel Enable Reset/Status */
#define RM57_DMA_SWCHENAS_OFFSET     0x0024 /* SW Channel Enable Set/Status */
#define RM57_DMA_SWCHENAR_OFFSET     0x002c /* SW Channel Enable Reset/Status */
#define RM57_DMA_CHPRIOS_OFFSET      0x0034 /* Channel Priority Set */
#define RM57_DMA_CHPRIOR_OFFSET      0x003c /* Channel Priority Reset */
#define RM57_DMA_GCHIENAS_OFFSET     0x0044 /* Global Channel Interrupt Enable Set */
#define RM57_DMA_GCHIENAR_OFFSET     0x004c /* Global Channel Interrupt Enable Reset */

/* DMA Request Assignment Registers: DREQASI(n) covers channels
 * 4n..4n+3 (n = 0..7), see RM57_DMA_DREQASI_CHSHIFT() below.
 */

#define RM57_DMA_DREQASI_OFFSET(n)   (0x0054 + ((n) << 2))

#define RM57_DMA_FTCMAP_OFFSET       0x00b4 /* FTC Interrupt Mapping (Group A/B) */
#define RM57_DMA_LFSMAP_OFFSET       0x00bc /* LFS Interrupt Mapping (Group A/B) */
#define RM57_DMA_HBCMAP_OFFSET       0x00c4 /* HBC Interrupt Mapping (Group A/B) */
#define RM57_DMA_BTCMAP_OFFSET       0x00cc /* BTC Interrupt Mapping (Group A/B) */
#define RM57_DMA_FTCINTENAS_OFFSET   0x00dc /* FTC Interrupt Enable Set */
#define RM57_DMA_FTCINTENAR_OFFSET   0x00e4 /* FTC Interrupt Enable Reset */
#define RM57_DMA_LFSINTENAS_OFFSET   0x00ec /* LFS Interrupt Enable Set */
#define RM57_DMA_LFSINTENAR_OFFSET   0x00f4 /* LFS Interrupt Enable Reset */
#define RM57_DMA_HBCINTENAS_OFFSET   0x00fc /* HBC Interrupt Enable Set */
#define RM57_DMA_HBCINTENAR_OFFSET   0x0104 /* HBC Interrupt Enable Reset */
#define RM57_DMA_BTCINTENAS_OFFSET   0x010c /* BTC Interrupt Enable Set */
#define RM57_DMA_BTCINTENAR_OFFSET   0x0114 /* BTC Interrupt Enable Reset */
#define RM57_DMA_GINTFLAG_OFFSET     0x011c /* Global Interrupt Flag (OR of FTC/LFS/HBC/BTC) */
#define RM57_DMA_FTCFLAG_OFFSET      0x0124 /* FTC Interrupt Flag (W1C) */
#define RM57_DMA_LFSFLAG_OFFSET      0x012c /* LFS Interrupt Flag (W1C) */
#define RM57_DMA_HBCFLAG_OFFSET      0x0134 /* HBC Interrupt Flag (W1C) */
#define RM57_DMA_BTCFLAG_OFFSET      0x013c /* BTC Interrupt Flag (W1C) */

/* BERFLAG (offset 0x144) never sets on this device - bus errors are
 * reported to the ESM instead (TRM 20.3.1.42); no register bits defined.
 */

#define RM57_DMA_BERFLAG_OFFSET      0x0144

#define RM57_DMA_FTCAOFFSET_OFFSET   0x014c /* FTCA channel offset (Group A) */
#define RM57_DMA_LFSAOFFSET_OFFSET   0x0150 /* LFSA channel offset (Group A) */
#define RM57_DMA_HBCAOFFSET_OFFSET   0x0154 /* HBCA channel offset (Group A) */
#define RM57_DMA_BTCAOFFSET_OFFSET   0x0158 /* BTCA channel offset (Group A) */

/* Port Assignment Registers: PAR(n) covers channels 8n..8n+7 (n = 0..3),
 * see RM57_DMA_PAR_CHSHIFT() below.
 */

#define RM57_DMA_PAR_OFFSET(n)       (0x0094 + ((n) << 2))

#define RM57_DMA_PTCRL_OFFSET        0x0178 /* Port Control Register */
#define RM57_DMA_RTCTRL_OFFSET       0x017c /* RAM Test Control Register */
#define RM57_DMA_DCTRL_OFFSET        0x0180 /* Debug Control Register */

/* Global Control Register (GCTRL) bits */

#define DMA_GCTRL_DMARES             (1 << 0)  /* Bit 0:  Software reset */
#define DMA_GCTRL_DEBUGMODE_SHIFT    (8)
#define DMA_GCTRL_DEBUGMODE_MASK     (3 << DMA_GCTRL_DEBUGMODE_SHIFT)
#define DMA_GCTRL_BUS_BUSY           (1 << 14) /* Bit 14: External bus busy (read-only) */
#define DMA_GCTRL_DMA_EN             (1 << 16) /* Bit 16: DMA enable */

/* DMA Request Assignment Registers (DREQASI0-7): each register packs 4
 * channels; within the register, channel (4n+k)'s 6-bit ASI field sits at
 * bit offset (24 - 8*k) for k = 0..3 (TRM Figure 20-31/20-32).
 */

#define DMA_DREQASI_FIELD_MASK       (0x3f)
#  define RM57_DMA_DREQASI_CHSHIFT(ch) (24 - 8 * ((ch) & 3))

/* Port Assignment Registers (PAR0-3): each register packs 8 channels; the
 * 3-bit CHnPA field for channel (8n+k) sits at bit offset (30 - 4*k) for
 * k = 0..7 (TRM Figure 20-39..20-42). Field value encoding is transcribed
 * directly from the TRM's value/description table, which lists "Others"
 * (i.e. reset value 0, and 4h-7h) as the last entry rather than pairing
 * it with 0h explicitly - the mapping below should be re-checked against
 * HALCoGen's dma_PortAsgn enum or hardware before being relied upon for
 * anything other than the two combined-port values used by this driver.
 */

#  define RM57_DMA_PAR_CHSHIFT(ch)   (30 - 4 * ((ch) & 7))
#define DMA_PAR_FIELD_MASK           (0x7)

#define DMA_PAR_AB_A_RD_B_WR         (0x1) /* Port A+B combined: A=read, B=write (used for TX: SRAM->peripheral) */
#define DMA_PAR_A_ONLY               (0x2) /* Port A only (memory-to-memory) */
#define DMA_PAR_B_ONLY                (0x3) /* Port B only */
#define DMA_PAR_AB_B_RD_A_WR         (0x0) /* Port A+B combined: B=read, A=write (used for RX: peripheral->SRAM; also the hardware reset default) */

/* RAM Test Control Register (RTCTRL) */

#define DMA_RTCTRL_RTC                (1 << 0)  /* Bit 0: open write access to reserved control-packet RAM */

/* DMA Control Packet Layout (control-packet RAM base = RM57_DMARAM_BASE)
 *
 * Primary control packet for channel n is at RM57_DMARAM_BASE + n*0x20;
 * the read-only working control packet for channel n is at
 * RM57_DMARAM_BASE + 0x800 + n*0x20 (TRM Figure 20-5, sections 20.3.2.*).
 */

#define RM57_DMA_CHANNEL_STRIDE       0x0020
#define RM57_DMA_WORKING_CP_OFFSET    0x0800

#define RM57_DMA_PCP_ISADDR_OFFSET    0x00 /* Initial Source Address */
#define RM57_DMA_PCP_IDADDR_OFFSET    0x04 /* Initial Destination Address */
#define RM57_DMA_PCP_ITCOUNT_OFFSET   0x08 /* Initial Transfer Count (frames|elements) */
#define RM57_DMA_PCP_CHCTRL_OFFSET    0x10 /* Channel Control */
#define RM57_DMA_PCP_EIOFF_OFFSET     0x14 /* Element Index Offset */
#define RM57_DMA_PCP_FIOFF_OFFSET     0x18 /* Frame Index Offset */

#define RM57_DMA_WCP_CSADDR_OFFSET    0x00 /* Current Source Address (read-only) */
#define RM57_DMA_WCP_CDADDR_OFFSET    0x04 /* Current Destination Address (read-only) */
#define RM57_DMA_WCP_CTCOUNT_OFFSET   0x08 /* Current Transfer Count = residual (read-only) */

/* Initial/Current Transfer Count Register (ITCOUNT/CTCOUNT): 13-bit frame
 * count and 13-bit element count (TRM 20.3.2.3/20.3.2.9).
 */

#define DMA_TCOUNT_ETCOUNT_SHIFT      (0)
#define DMA_TCOUNT_ETCOUNT_MASK       (0x1fff << DMA_TCOUNT_ETCOUNT_SHIFT)
#  define DMA_TCOUNT_ETCOUNT(n)       ((uint32_t)(n) << DMA_TCOUNT_ETCOUNT_SHIFT)
#define DMA_TCOUNT_FTCOUNT_SHIFT      (16)
#define DMA_TCOUNT_FTCOUNT_MASK       (0x1fff << DMA_TCOUNT_FTCOUNT_SHIFT)
#  define DMA_TCOUNT_FTCOUNT(n)       ((uint32_t)(n) << DMA_TCOUNT_FTCOUNT_SHIFT)

/* Channel Control Register (CHCTRL, TRM Figure 20-113/Table 20-103) */

#define DMA_CHCTRL_AIM                (1 << 0)  /* Bit 0:     Auto-initiation mode */
#define DMA_CHCTRL_ADDMW_SHIFT        (1)       /* Bits 2-1:  Addressing mode write */
#define DMA_CHCTRL_ADDMW_MASK         (3 << DMA_CHCTRL_ADDMW_SHIFT)
#  define DMA_CHCTRL_ADDMW_CONST      (0 << DMA_CHCTRL_ADDMW_SHIFT)
#  define DMA_CHCTRL_ADDMW_INCR       (1 << DMA_CHCTRL_ADDMW_SHIFT)
#  define DMA_CHCTRL_ADDMW_INDEXED    (3 << DMA_CHCTRL_ADDMW_SHIFT)
#define DMA_CHCTRL_ADDMR_SHIFT        (3)       /* Bits 4-3:  Addressing mode read */
#define DMA_CHCTRL_ADDMR_MASK         (3 << DMA_CHCTRL_ADDMR_SHIFT)
#  define DMA_CHCTRL_ADDMR_CONST      (0 << DMA_CHCTRL_ADDMR_SHIFT)
#  define DMA_CHCTRL_ADDMR_INCR       (1 << DMA_CHCTRL_ADDMR_SHIFT)
#  define DMA_CHCTRL_ADDMR_INDEXED    (3 << DMA_CHCTRL_ADDMR_SHIFT)
#define DMA_CHCTRL_TTYPE              (1 << 8)  /* Bit 8:     0=frame trigger, 1=block trigger */
#define DMA_CHCTRL_WES_SHIFT          (12)      /* Bits 13-12: Write element size */
#define DMA_CHCTRL_WES_MASK           (3 << DMA_CHCTRL_WES_SHIFT)
#  define DMA_CHCTRL_WES_8BIT         (0 << DMA_CHCTRL_WES_SHIFT)
#  define DMA_CHCTRL_WES_16BIT        (1 << DMA_CHCTRL_WES_SHIFT)
#  define DMA_CHCTRL_WES_32BIT        (2 << DMA_CHCTRL_WES_SHIFT)
#  define DMA_CHCTRL_WES_64BIT        (3 << DMA_CHCTRL_WES_SHIFT)
#define DMA_CHCTRL_RES_SHIFT          (14)      /* Bits 15-14: Read element size */
#define DMA_CHCTRL_RES_MASK           (3 << DMA_CHCTRL_RES_SHIFT)
#  define DMA_CHCTRL_RES_8BIT         (0 << DMA_CHCTRL_RES_SHIFT)
#  define DMA_CHCTRL_RES_16BIT        (1 << DMA_CHCTRL_RES_SHIFT)
#  define DMA_CHCTRL_RES_32BIT        (2 << DMA_CHCTRL_RES_SHIFT)
#  define DMA_CHCTRL_RES_64BIT        (3 << DMA_CHCTRL_RES_SHIFT)
#define DMA_CHCTRL_CHAIN_SHIFT        (16)      /* Bits 21-16: Chained channel (0=none, n+1=channel n) */
#define DMA_CHCTRL_CHAIN_MASK         (0x3f << DMA_CHCTRL_CHAIN_SHIFT)
#  define DMA_CHCTRL_CHAIN(n)         (((uint32_t)(n) + 1) << DMA_CHCTRL_CHAIN_SHIFT)
#  define DMA_CHCTRL_CHAIN_NONE       (0 << DMA_CHCTRL_CHAIN_SHIFT)

/* Element/Frame Index Offset Registers (EIOFF/FIOFF): 13-bit source and
 * destination byte offsets (TRM 20.3.2.5/20.3.2.6). Unused by the serial
 * RX/TX DMA scheme in this port (element size is fixed at 1 byte with
 * post-increment addressing), provided for completeness.
 */

#define DMA_IOFF_SRC_SHIFT             (0)
#define DMA_IOFF_SRC_MASK              (0x1fff << DMA_IOFF_SRC_SHIFT)
#define DMA_IOFF_DST_SHIFT             (16)
#define DMA_IOFF_DST_MASK              (0x1fff << DMA_IOFF_DST_SHIFT)

/* DMA hardware request line numbers (SPNS215C Table 6-41 "DMA Request
 * Line Connection", default 1:1 DREQASI mapping). These are *request*
 * numbers, passed to rm57_dmachannel() - they are not channel numbers.
 */

#define DMA_REQ_LIN1RX                28 /* SCI1/LIN1 receive */
#define DMA_REQ_LIN1TX                29 /* SCI1/LIN1 transmit */
#define DMA_REQ_SCI3RX                30 /* SCI3 receive */
#define DMA_REQ_SCI3TX                31 /* SCI3 transmit */
#define DMA_REQ_LIN2RX                40 /* SCI2/LIN2 receive */
#define DMA_REQ_LIN2TX                41 /* SCI2/LIN2 transmit */
#define DMA_REQ_SCI4RX                42 /* SCI4 receive */
#define DMA_REQ_SCI4TX                43 /* SCI4 transmit */

#define RM57_DMA_NCHANNELS             32
#define RM57_DMA_NREQUESTS             48

#endif /* __ARCH_ARM_SRC_RM57_HARDWARE_RM57_DMA_H */
