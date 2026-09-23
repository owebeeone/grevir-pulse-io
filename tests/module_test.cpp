// Adapted from Ardoinus devel/code/pwe_test_module.cxx; MIT license.
#include <GrevirPulseIO.h>
#include <grevir/core/application.hpp>
#include <grevir/peripherals/gpio/input.hpp>
#include <grevir/peripherals/gpio/output.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>

namespace {
struct Clock {
  using TimeType = setl::Time<std::uint32_t, setl::TimeUnit::MICROS>;
  inline static TimeType current{};
  static TimeType now() {
    return current;
  }
};

struct GPIO {
  inline static bool line = false;
  inline static bool input_configured = false;
  inline static bool output_configured = false;
  inline static unsigned transitions = 0;

  static void pinMode(unsigned pin, ardo::gpio::InputPinMode) {
    if (pin == 4) {
      input_configured = true;
    }
  }
  static void pinMode(unsigned pin, ardo::gpio::OutputPinMode) {
    if (pin == 5) {
      output_configured = true;
    }
  }
  static bool digitalRead(unsigned) {
    return line;
  }
  static void digitalWrite(unsigned, bool level) {
    if (line != level) {
      ++transitions;
    }
    line = level;
  }
};

using Input = ardo::InputPin<GPIO, 4>;
using Output = ardo::OutputPin<GPIO, 5>;
using Bits = setl::PweBitCollector<8>;

template <bool inverted>
void transfer_frame() {
  using Receiver = ardo::PweReceiverModule<Input, Bits, Clock, std::uint32_t,
                                           10, 200, 100, inverted>;
  using Transmitter = ardo::PweTransmitterModule<Output, Bits, Clock, std::uint32_t,
                                                 10, 200, 100, inverted>;
  using App = ardo::Application<Receiver, Transmitter>;
  Clock::current = Clock::TimeType(0);
  GPIO::line = !inverted;
  GPIO::input_configured = false;
  GPIO::output_configured = false;
  GPIO::transitions = 0;
  Receiver::instance.decoder.reset();
  Transmitter::instance.encoder.reset();
  App::runSetup();
  REQUIRE(GPIO::input_configured);
  REQUIRE(GPIO::output_configured);
  REQUIRE(GPIO::line == inverted);

  for (std::uint32_t tick = 0; tick < 250; ++tick) {
    Clock::current = Clock::TimeType(tick);
    App::runLoop();
  }
  REQUIRE_FALSE(Receiver::instance.isDataReady());
  REQUIRE(Transmitter::instance.send(0x5a));
  REQUIRE_FALSE(Transmitter::instance.send(0x33));

  for (std::uint32_t tick = 250; tick < 1400; ++tick) {
    Clock::current = Clock::TimeType(tick);
    App::runLoop();
  }
  REQUIRE(GPIO::transitions >= 16);
  REQUIRE(Receiver::instance.isDataReady());
  Bits::value_type result{};
  REQUIRE(Receiver::instance.readValue(result));
  REQUIRE(result == 0x5a);
  REQUIRE_FALSE(Receiver::instance.isDataReady());
  REQUIRE_FALSE(Receiver::instance.readValue(result));
  REQUIRE_FALSE(Receiver::instance.decoder.hasSignalError());
  REQUIRE(Transmitter::instance.send(0x33));
}
} // namespace

TEST_CASE("Pulse IO modules transfer a frame through injected pins and clock", "[pulse-io]") {
  transfer_frame<false>();
}

TEST_CASE("Pulse IO modules transfer an inverted frame", "[pulse-io]") {
  transfer_frame<true>();
}
