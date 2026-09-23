#include <GrevirPulseIO.h>
#include <grevir/core/application.hpp>
#include <grevir/peripherals/gpio/input.hpp>
#include <grevir/peripherals/gpio/output.hpp>
#include <cstdint>

namespace {
struct Backend {
  static void pinMode(unsigned, ardo::gpio::InputPinMode);
  static void pinMode(unsigned, ardo::gpio::OutputPinMode);
  static bool digitalRead(unsigned);
  static void digitalWrite(unsigned, bool);
};
struct Clock {
  static setl::Time<std::uint32_t, setl::TimeUnit::MICROS> now();
};
using Bits = setl::PweBitCollector<8>;
using Receiver = ardo::PweReceiverModule<ardo::InputPin<Backend, 4>, Bits,
                                         Clock, std::uint32_t, 10, 200, 100>;
using Transmitter = ardo::PweTransmitterModule<ardo::OutputPin<Backend, 5>, Bits,
                                               Clock, std::uint32_t, 10, 200, 100>;
using App = ardo::Application<Receiver, Transmitter>;
static_assert(!App::has_conflict);
} // namespace

void instantiate_pulse_modules() {
  App::runSetup();
  App::runLoop();
  Receiver::instance.isDataReady();
  Bits::value_type result{};
  Receiver::instance.readValue(result);
  Transmitter::instance.send(result);
}
