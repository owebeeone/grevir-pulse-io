#include <GrevirPulseIO.h>
#include <grevir/core/application.hpp>
#include <grevir/peripherals/gpio/input.hpp>
#include <grevir/peripherals/gpio/output.hpp>
#include <cstdint>

namespace {
struct Clock {
  inline static std::uint32_t tick = 0;
  static setl::Time<std::uint32_t, setl::TimeUnit::MICROS> now() {
    return setl::Time<std::uint32_t, setl::TimeUnit::MICROS>(tick);
  }
};
struct GPIO {
  inline static bool line = false;
  static void pinMode(unsigned, ardo::gpio::InputPinMode) {}
  static void pinMode(unsigned, ardo::gpio::OutputPinMode) {}
  static bool digitalRead(unsigned) { return line; }
  static void digitalWrite(unsigned, bool level) { line = level; }
};
using Bits = setl::PweBitCollector<8>;
using Receive = ardo::PweReceiverModule<ardo::InputPin<GPIO, 4>, Bits,
                                        Clock, std::uint32_t, 10, 200, 100>;
using Transmit = ardo::PweTransmitterModule<ardo::OutputPin<GPIO, 5>, Bits,
                                            Clock, std::uint32_t, 10, 200, 100>;
using App = ardo::Application<Receive, Transmit>;
}

int main() {
  App::runSetup();
  for (; Clock::tick < 250; ++Clock::tick) {
    App::runLoop();
  }
  if (!Transmit::instance.send(0x5a)) {
    return 1;
  }
  for (; Clock::tick < 1400; ++Clock::tick) {
    App::runLoop();
  }
  Bits::value_type result{};
  return Receive::instance.readValue(result) && result == 0x5a ? 0 : 2;
}
