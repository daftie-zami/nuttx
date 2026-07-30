================
TI/Hercules RM57
================

.. tags:: chip:rm57, arch:armv7-r

The RM57 is part of TI's Hercules family of ARM Cortex-R5F safety
microcontrollers, aimed at applications that need lockstep execution and
built-in self-test (industrial, medical, and automotive safety systems).
NuttX currently supports the **RM57L843** part.

.. warning::

   Support for this chip family is new and experimental. Only the
   SCI/LIN (serial), DMA, GIO (LED), DCAN, and EMAC/MDIO (Ethernet)
   peripherals have been brought up so far; the PLL/clock configuration
   values, JTAG IDCODE, LED polarity, and (for Ethernet) the exact PHY
   pin assignments and RMII clock source have not all been independently
   confirmed against hardware/schematics. See the
   :doc:`board documentation <boards/rm57l843-launchxl2/index>` for
   details.

Peripheral Support
==================

The following list indicates RM57 peripherals currently supported in NuttX:

============== =====
Peripheral     Notes
============== =====
SCI            SCI1-SCI4. SCI1/LIN1 and SCI2/LIN2 are dual-role SCI/LIN
               modules; the serial driver always runs them in SCI
               compatibility mode (GCR1 LIN_MODE=0). Interrupt-driven by
               default; optional RX/TX DMA (see `Serial DMA`_ below).
               Used for the console.
LIN            Configuration and polled-transfer scaffold only
               (``CONFIG_RM57_LIN``, requires ``CONFIG_EXPERIMENTAL``) -
               not a complete LIN protocol stack. See
               ``arch/arm/src/rm57/rm57_lin.h`` for what is and is not
               implemented.
DMA            32-channel, 48-request-line DMA controller
               (``CONFIG_RM57_DMA``). Used by the serial driver's RX/TX
               DMA options and by the DCAN IF3 DMA receive path.
DCAN           All four DCAN (Bosch D_CAN, CAN 2.0A/B) controllers
               (``CONFIG_RM57_DCANn``). Per instance either a character
               device (``/dev/canN``) or a SocketCAN network interface
               (``canN``); both frontends can coexist in one image on
               different instances. See `DCAN`_ below.
GIO            General purpose I/O, used for LEDs
EMAC           10/100 Ethernet MAC + EMAC Control module + MDIO
               (``CONFIG_RM57_EMAC``). Single netdev interface (``eth0``),
               MII or RMII (``RM57_EMAC_MII`` / ``RM57_EMAC_RMII``). See
               `EMAC/MDIO (Ethernet)`_ below.
============== =====

.. todo::

   Many peripherals available on the RM57L843 (N2HET, MibSPI, ADC,
   the ESM diagnostic module, PBIST/STC self-test, etc.) are not yet
   implemented. Contributions are welcome.

DCAN
====

Each of the four DCAN controllers has 64 ECC-protected message objects.
The driver partitions them into a transmit FIFO at the top of the range
(``CONFIG_RM57_DCAN_TXFIFO_DEPTH``, default 8) and receive filter FIFOs
below it (``CONFIG_RM57_DCAN_RXFIFO_DEPTH`` objects per acceptance
filter, up to ``CONFIG_RM57_DCAN_NFILTERS_MAX`` filters, with an
accept-everything filter installed while no user filter is active).
Message-object interrupts for reception are routed to the DCAN's level 1
interrupt line, transmission and status/error to level 0.

Per instance, ``RM57_DCANn_CHARDRIVER`` selects the classic character
driver (bit timing, acceptance filter, loopback/silent mode, bus-off
recovery, NART and ABOM ioctls; error reporting with
``CONFIG_CAN_ERRORS``) and ``RM57_DCANn_SOCKET`` selects SocketCAN
(``candump``/``cansend`` work out of the box; error frames with
``CONFIG_NET_CAN_ERRORS``, bit timing via ``SIOCGCANBITRATE`` /
``SIOCSCANBITRATE``).

Three receive paths are available per instance:

- ``RM57_DCANn_RX_PIO`` (default): interrupt per message, read through
  interface register set 2.
- ``RM57_DCANn_RX_IF3IRQ``: the message handler auto-copies received
  objects into interface register set 3; the dedicated IF3 interrupt
  reads them without any interface-register busy-wait.
- ``RM57_DCANn_RX_IF3DMA`` (``CONFIG_EXPERIMENTAL``): as above, but a
  DMA channel in auto-initiation mode moves the IF3 image to memory;
  a watchdog guards against lost request pulses.

The CAN clock (VCLKA1) is mapped to VCLK (75 MHz with the default board
clocking); the bit-timing search targets a 75% sample point, matching
TI's HALCoGen defaults (500 kbit/s -> BRP=10, TSEG1=10, TSEG2=4).
DCAN1-DCAN3 use dedicated balls; DCAN4 is pin-multiplexed with GIOB[2]
and GIOB[3] and the board claims those balls when ``RM57_DCAN4`` is
enabled.  On the LAUNCHXL2-RM57L board, DCAN1 and DCAN2 have on-board
transceivers.  The board configurations ``can`` (character driver,
loopback self-test), ``cansock`` (SocketCAN with candump/cansend) and
``candma`` (IF3 DMA receive path) are provided.

Serial DMA
==========

Each SCI port can optionally use the DMA controller for RX and/or TX,
independently, via ``CONFIG_RM57_SCIn_RXDMA`` / ``CONFIG_RM57_SCIn_TXDMA``
(n = 1-4; both require ``CONFIG_RM57_DMA``). Because SCI1/LIN1 and
SCI2/LIN2 are run in SCI-compatibility mode (single-buffer, MBUF_MODE=0),
the SCI module generates one DMA request per byte in each direction (TRM
SPNU562A section 28.2.3) - there is no multi-byte burst transfer for a
single UART frame.

RX DMA writes into a per-port circular ring buffer sized by
``CONFIG_RM57_SERIAL_RXDMA_BUFFER_SIZE`` (rounded up to a multiple of the
Cortex-R5's 32-byte D-cache line size). New bytes are picked up when the
DMA half/full ("HBC"/"BTC") interrupt fires, which alone would leave
short, infrequent messages sitting in the ring buffer until it is half
full. Unlike, say, the STM32 USART, the SCI has no idle-line interrupt
to signal the end of a shorter burst, so ``rm57_serial_dma_poll()``
flushes the rings instead.

``CONFIG_RM57_SERIAL_RXDMA_POLL`` (enabled by default whenever a port
uses RX DMA) dedicates **RTI compare 1** to calling it at
``CONFIG_RM57_SERIAL_RXDMA_POLL_HZ`` (1 kHz by default), which bounds
the added receive latency by one poll period and is what makes an RX
DMA console usable interactively. Compare 1 shares free-running counter
0 with the system tick on compare 0, so no additional timer hardware is
consumed. Disable the option if every RX DMA port is used purely for
bulk transfers and the interrupt is not wanted.

The underlying hardware DMA request lines used are fixed per SCI
instance (datasheet SPNS215C Table 6-41):

================ ========== ==========
SCI instance     RX request TX request
================ ========== ==========
SCI1/LIN1        28         29
SCI3             30         31
SCI2/LIN2        40         41
SCI4             42         43
================ ========== ==========

EMAC/MDIO (Ethernet)
=====================

The RM57L843 has a single 10/100 Ethernet MAC built from three sub-blocks
(SPNU562A chapter 31): the EMAC module itself (TX/RX CPPI DMA engines,
statistics, MAC address/hash filtering), the EMAC Control module (VIM
interrupt pacing/routing), and the MDIO module (a shared PHY management
bus, independent of the MAC). ``CONFIG_RM57_EMAC`` enables all three
together and registers one ``netdev_register()`` interface
(``NET_LL_ETHERNET``) - there is no separate character-device frontend,
unlike DCAN.

Descriptor rings (TX: ``CONFIG_RM57_EMAC_NTXDESC``, default 8; RX:
``CONFIG_RM57_EMAC_NRXDESC``, default 16) live in the EMAC's internal 8KB
CPPI RAM, which this port's MPU maps as non-cacheable device memory, so
descriptor fields need no cache maintenance. Packet buffers are ordinary
cacheable SRAM; TX uses a small zero-copy free-buffer pool (buffers are
swapped into ``dev->d_buf``, not copied) while RX buffers are fixed 1:1
with RX descriptors and processed synchronously in place. Both rings are
managed as CPPI "append to a running queue" chains (SPNU562A Section
31.2.6.2), including the documented EOQ-race recovery for restarting a
channel that halted between the driver reading its tail and appending a
new descriptor.

``CONFIG_RM57_EMAC_MII`` / ``CONFIG_RM57_EMAC_RMII`` select the PHY
interface mode; the LAUNCHXL2-RM57L's DP83640 PHY is wired for MII, while
the IOMM reset default is RMII, so MII must be explicitly selected to
match that board. The EMAC's internal logic clock (``VCLKA4_DIVR_EMAC``)
must be exactly 25MHz for MII or 50MHz for RMII (SPNS215 Section 6.6.3);
see the ``BOARD_VCLKA4_*`` comment in the board's ``board.h`` for how
each mode's clock is derived and the caveat on the RMII (PLL2-sourced)
path, which this port has not independently verified against real
hardware.

The MDIO module addresses up to 32 PHYs (``CONFIG_RM57_EMAC_PHYADDR``,
default 1) over a single-transaction register interface
(``USERACCESS0``), and generates a link-change interrupt
(``LINKINT0``, routed through the ``C0_MISC_PULSE`` VIM channel) that
this driver always keeps enabled for its own ``netdev_carrier_on/off``
tracking; ``CONFIG_RM57_EMAC_PHY_INTERRUPT`` additionally exposes that
event to userspace via ``SIOCMIINOTIFY``/``phy_notify``.
``CONFIG_RM57_EMAC_AUTONEG`` (default) negotiates speed/duplex; disabling
it forces the mode selected by ``CONFIG_RM57_EMAC_ETHFD`` /
``CONFIG_RM57_EMAC_ETH100MBPS``. ``CONFIG_RM57_EMAC_LOOPBACK`` sets
``MACCONTROL.LOOPBACK`` and skips the PHY link wait entirely, so the
descriptor ring and interrupt path can be exercised standalone without a
PHY or cable - the Ethernet equivalent of ``RM57_DCAN_LOOPBACK``.

Multicast group membership (``CONFIG_NET_MCASTGROUP``) is implemented via
the EMAC's 64-bit hash address table (SPNU562A Section 31.5.37, 6-bit
XOR-fold of the destination address), with a per-bit reference count
since two joined addresses can hash to the same bit.

The four EMAC Control module interrupt pulses this device routes to the
VIM (SPNS215 Table 6-39) are, in channel order: ``C0_MISC_PULSE`` (76,
statistics/host-error/MDIO events), ``C0_TX_PULSE`` (77, TX completion),
``C0_THRESH_PULSE`` (78, RX flow-control threshold - not used by this
driver), ``C0_RX_PULSE`` (79, RX completion). Each ISR follows the
disable-service-acknowledge-reenable discipline SPNU562A Section
31.2.17.3 documents: the top-half clears the relevant EMAC Control
enable bit and defers to the low-priority work queue, which does the
real work, writes the ``MACEOIVECTOR`` acknowledge key, then re-enables
the source last.

The ``eth`` board configuration (``CONFIG_NETDEV_LATEINIT``) derives a
locally-administered MAC address from the device's unique die ID at
board bring-up; without ``CONFIG_NETDEV_LATEINIT`` the interface
self-registers via ``arm_netinitialize()`` with a fixed placeholder
address (``02:00:00:00:00:01``) instead.

Supported Boards
=================

.. toctree::
   :glob:
   :maxdepth: 1

   boards/*/*
