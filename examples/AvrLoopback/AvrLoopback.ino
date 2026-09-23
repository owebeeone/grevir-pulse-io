#include <GrevirArduinoAVR.h>
#include <GrevirPulseIO.h>

struct MicrosClock {
  using TimeType = setl::Time<unsigned long, setl::TimeUnit::MICROS>;

  static TimeType now() {
    return TimeType(ardo::CoreIF::micros());
  }
};

using Bits = setl::PweBitCollector<8>;
using Receive = ardo::PweReceiverModule<
  ardo::arduino::InputPin<4, ardo::gpio::InputPinMode::Untied>,
  Bits, MicrosClock, unsigned long, 10, 200, 100>;
using Transmit = ardo::PweTransmitterModule<
  ardo::arduino::OutputPin<5>, Bits, MicrosClock, unsigned long, 10, 200, 100>;
using App = ardo::ArduinoAvrApplication<Receive, Transmit>;

volatile unsigned char last_received = 0;
unsigned char next_value = 0;

void setup() {
  App::runSetup();
}

void loop() {
  App::runLoop();
  if (Transmit::instance.send(next_value)) {
    ++next_value;
  }
  Bits::value_type value{};
  if (Receive::instance.readValue(value)) {
    last_received = value;
  }
}
