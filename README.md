# Grevir Pulse IO

PWE transmitter and receiver modules adapted from Ardoinus
`pwm_serial_comms.h`. Grevir Pulse Codec owns bit framing and waveform timing;
this package connects that codec to a caller supplied GPIO pin and clock.
`GrevirPulseIO.h` includes both modules. The package depends on Grevir Base,
Time, Core, Peripherals and Pulse Codec.

```cpp
using Receive = ardo::PweReceiverModule<
  ardo::InputPin<BoardGPIO, 4>, setl::PweBitCollector<8>, BoardClock,
  std::uint32_t, 10, 200, 100>;
using Transmit = ardo::PweTransmitterModule<
  ardo::OutputPin<BoardGPIO, 5>, setl::PweBitCollector<8>, BoardClock,
  std::uint32_t, 10, 200, 100>;
using App = ardo::Application<Receive, Transmit>;
```

Periods and `BoardClock::now()` use the same selected time unit (microseconds by
default). `App::runSetup()` configures the pins and drives the transmitter's idle
level. Call it once, then call `App::runLoop()` frequently enough to
observe each pulse edge and meet encoder deadlines. `Transmit::instance.send()`
returns false while a prior frame is active. `Receive::instance.isDataReady()`
checks whether a complete frame is waiting; `readValue()` consumes it. Inversion
and bit polarity are template options. The pin types supply Core resource claims,
so an application assigning the same GPIO twice is rejected at compile time.

The original Gammil repeater prototype is retained under `extras/legacy`; it is
an archive, not a supported example. Native tests connect transmitter and
receiver through mock GPIO for normal and inverted frames. They check pin setup,
backpressure, readout and conflict-free module composition. The codec's own tests
cover detailed waveform and boundary behavior. Target timing and silicon
behavior are not established by these host tests.

`examples/AvrLoopback` is a minimal ATmega328P Arduino sketch. It assigns an
externally driven input on pin 4 and an output on pin 5 and uses the Arduino
microsecond clock. The input has no pull-up because the output drives it.
That sketch compiles and links for Uno on weftpi with Debian `avr-g++` 14.2 and
Arduino CLI. A simavr 1.6 probe connects simulated D5 to D4 and decodes a frame;
see [the target evidence](https://github.com/owebeeone/grevir-wz/blob/main/dev-docs/GrevirPulseIoAvrEvidence.md). A physical
connection and waveform measurement remain hardware work.

For installed consumers:

```cmake
find_package(grevir-pulse-io CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE grevir::pulse_io)
```
