=====================
LAUNCHXL2-RM57L
=====================

.. tags:: chip:rm57, arch:armv7-r, vendor:ti

The `LAUNCHXL2-RM57L <https://www.ti.com/tool/LAUNCHXL2-RM57L>`__ is a
LaunchPad-format evaluation board from Texas Instruments built around the
Hercules RM57L843 safety microcontroller (dual Cortex-R5F cores in
lockstep). It's aimed at evaluating TI's Hercules safety MCU family rather
than any particular application.

.. warning::

   This board port is new and experimental. Only the serial console
   (SCI1), the two user LEDs, and the DMA controller are currently
   supported; pin-mux configuration, button support, and most on-chip
   peripherals are not yet implemented. Some of the values documented
   below (PLL/clock configuration, LED polarity, JTAG IDCODE) were taken
   from TI's HALCoGen-generated reference project or from the RM57L843
   datasheet rather than confirmed against this specific board's
   schematic — see the comments in
   ``boards/arm/rm57/rm57l843-launchxl2/include/board.h`` for details.

Features
========

* TI Hercules RM57L843 microcontroller
* Dual-core lockstep ARM Cortex-R5F, running at 150 MHz HCLK (300 MHz
  PLL/GCLK) in the current clock configuration
* 4 MB of on-chip program flash, 512 KB of on-chip SRAM
* Onboard XDS110 debug probe (JTAG)
* 2 user LEDs
* SCI1 wired up as the serial console; SCI2-SCI4 are supported by the
  driver but not enabled by this board's default configuration (and
  SCI2-SCI4 pin-mux is not programmed by this board port - see the
  `Pin Mapping`_ note below)

.. note::

   Peripherals that are part of the RM57L843 chip itself (N2HET, MibSPI,
   CAN, ADC, the ESM diagnostic module, PBIST/STC self-test, etc.) but are
   not yet wired up by this board port are tracked on the
   :doc:`RM57 chip documentation page </platforms/arm/rm57/index>`.

Buttons and LEDs
================

LEDs
----

The LAUNCHXL2-RM57L has two user LEDs, labeled **B6** and **B7** on the
board silkscreen, driven by GIOB[6] and GIOB[7] respectively.

Buttons
-------

Button support is not yet implemented by this board port.

Pin Mapping
===========

Only the pins used by the currently supported peripherals are listed.

.. list-table::
   :widths: auto
   :header-rows: 1

   * - Pin
     - Signal
     - Notes
   * - (n/a)
     - LIN1RX / LIN1TX
     - SCI1, used as the serial console
   * - P4 / T5
     - LIN2RX / LIN2TX
     - SCI2, multiplexed away from N2HET2[19]/N2HET2[20]
   * - W3 / N2
     - SCI3RX / SCI3TX
     - SCI3, multiplexed away from N2HET1[6]/N2HET1[13]
   * - A13 / B13
     - SCI4RX / SCI4TX
     - SCI4, multiplexed away from N2HET1[17]/N2HET1[19]
   * - J2
     - GIOB[6]
     - User LED B6
   * - F1
     - GIOB[7]
     - User LED B7

.. note::

   The RM57L843 SCI1/LIN1 pins are used at their reset-default (primary)
   function, so no pin-mux configuration is required for the console.
   Header/connector pin numbers for LIN1RX/LIN1TX have not been confirmed
   against the LAUNCHXL2-RM57L schematic. SCI2/LIN2, SCI3 and SCI4 share
   their balls with N2HET1/N2HET2 and are re-multiplexed at boot by
   ``BOARD_PINMUX_INITIALIZER`` in ``include/board.h``, but only when the
   corresponding ``CONFIG_RM57_SCIn`` is selected.

Serial Console
==============

SCI1 is used as the serial console. The default configuration is:

* 9600 baud, 8 data bits, no parity, 2 stop bits

The baud rate and stop bits are configurable via ``CONFIG_SCI1_BAUD`` and
``CONFIG_SCI1_2STOP``.

Additional SCI ports (SCI2-SCI4) can be enabled via
``CONFIG_RM57_SCI2``/``RM57_SCI3``/``RM57_SCI4``, which also pulls their
RX/TX balls into the pin-mux table applied at boot; whichever one is *not*
selected as the console (``CONFIG_SCIn_SERIAL_CONSOLE``) is registered as
``/dev/ttyS0``, ``/dev/ttyS1``, etc. in SCI1-SCI4 order. SCI1/LIN1 and
SCI2/LIN2 are dual-role SCI/LIN modules; the serial driver always runs
them in SCI-compatibility mode. See
:doc:`the RM57 chip documentation </platforms/arm/rm57/index>` for the
serial RX/TX DMA options and the LIN configuration scaffold.

Power Supply
============

The LAUNCHXL2-RM57L can be powered over USB through the onboard XDS110
debug probe. Consult the
`LAUNCHXL2-RM57L user's guide <https://www.ti.com/tool/LAUNCHXL2-RM57L>`_
for the full range of supported input voltages.

Debugging
=========

The board's onboard XDS110 probe exposes a JTAG interface (Hercules'
ICEpick-C JTAG router is JTAG-only; XDS110's default SWD mode is not
usable here). OpenOCD can be used with a Cortex-R5 (``cortex_r4`` driver)
target configuration that routes through the ICEpick-C, followed by GDB
to load and debug the image.

Configurations
==============

nsh
---

Basic NuttShell configuration (console enabled on SCI1, at 9600 baud).

scitest
-------

NuttShell plus all four SCI ports, for exercising the serial driver and
the SCI pin multiplexing. SCI1 remains the console (9600 baud, 8N2, on
the XDS110 backchannel); SCI2, SCI3 and SCI4 come up at 115200 baud, 8N1
and are registered as ``/dev/ttyS0``, ``/dev/ttyS1`` and ``/dev/ttyS2``
respectively. Their RX/TX balls are multiplexed at boot (see the Pin
Mapping table above), so they must be wired to a USB-serial adapter, or
looped back to each other, to be useful.

``CONFIG_SERIAL_TERMIOS`` is enabled, and the ``serialblaster``,
``serialrx`` and ``termios`` examples are built in::

    nsh> serialrx /dev/ttyS1 1000     # count 1000 received bytes
    nsh> serialblaster /dev/ttyS1     # transmit continuously
    nsh> echo hello > /dev/ttyS2      # one-shot write

Both ``serialrx`` and ``serialblaster`` take the device path as their
first argument and default to ``/dev/ttyS0``.

scidma
------

Identical to ``scitest``, except that all four SCI ports - the console
included - run with both RX and TX DMA
(``CONFIG_RM57_SCIn_RXDMA``/``TXDMA``, eight DMA channels in total).
The two configurations are an A/B pair for the same serial tests.

RX DMA hands received bytes to the upper half when the DMA half/full
interrupt fires, i.e. only after ``CONFIG_RM57_SERIAL_RXDMA_BUFFER_SIZE
/ 2`` bytes (32 by default). To keep short messages and interactive
console input from stalling behind that threshold, RTI compare 1 calls
``rm57_serial_dma_poll()`` at ``CONFIG_RM57_SERIAL_RXDMA_POLL_HZ``
(1 kHz here), bounding the extra latency by one poll period. Both the
console and the test ports therefore behave the same as in ``scitest``,
only with the transfers themselves done by DMA.
