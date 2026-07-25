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
   SCI/LIN (serial), DMA, and GIO (LED) peripherals have been brought up
   so far; the PLL/clock configuration values, JTAG IDCODE, and LED
   polarity have not all been independently confirmed against
   hardware/schematics. See the
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
               DMA options; no other peripheral driver uses it yet.
GIO            General purpose I/O, used for LEDs
============== =====

.. todo::

   Many peripherals available on the RM57L843 (N2HET, MibSPI, CAN, ADC,
   the ESM diagnostic module, PBIST/STC self-test, etc.) are not yet
   implemented. Contributions are welcome.

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
Cortex-R5's 32-byte D-cache line size). New bytes are picked up either
when the DMA half/full ("HBC"/"BTC") interrupt fires, or by calling
``rm57_serial_dma_poll()`` from a periodic context (e.g. a timer) -
useful for boards that need low-latency reception of short, infrequent
messages that would otherwise sit in the ring buffer until it is half
full.

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

Supported Boards
=================

.. toctree::
   :glob:
   :maxdepth: 1

   boards/*/*
