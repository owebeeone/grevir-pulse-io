// Adapted from Ardoinus pwm_serial_comms.h; MIT license, see LICENSE.txt.
#pragma once

#include <grevir/core/module.hpp>
#include <grevir/pulse_codec/decoder.hpp>
#include <grevir/pulse_codec/encoder.hpp>

namespace ardo {

// Pin supplies get() and a Core GPIO claim; Clock supplies now() in the
// selected waveform time unit. Application sets up the pin before polling.
template <typename Pin, typename Collector, typename Clock, typename Tick,
          Tick settle_period, Tick stop_period, Tick bit_period,
          bool inverted = false, bool long_on_is_1 = false,
          setl::TimeUnit units = setl::PWE_UNIT>
class PweReceiverModule : public ModuleBase<Parameters<Pin>> {
public:
  using DecoderType = setl::PweDecoder<Collector, inverted, long_on_is_1, Tick, units>;
  using value_type = typename DecoderType::value_type;

  static void runLoop() {
    instance.instanceLoop();
  }

  void instanceLoop() {
    decoder.signalChanged(Clock::now(), Pin::get());
  }

  bool isDataReady() const {
    return decoder.pollDataReady();
  }

  bool readValue(value_type& result) {
    return decoder.readValue(result);
  }

  static PweReceiverModule instance;
  DecoderType decoder{
    setl::PweWaveformParams1to3<Tick, settle_period, stop_period, bit_period, units>};
};

template <typename Pin, typename Collector, typename Clock, typename Tick,
          Tick settle_period, Tick stop_period, Tick bit_period,
          bool inverted, bool long_on_is_1, setl::TimeUnit units>
PweReceiverModule<Pin, Collector, Clock, Tick, settle_period, stop_period,
                  bit_period, inverted, long_on_is_1, units>
  PweReceiverModule<Pin, Collector, Clock, Tick, settle_period, stop_period,
                    bit_period, inverted, long_on_is_1, units>::instance;

// Pin supplies set(bool) and a Core GPIO claim. send() refuses a new value
// while the current frame is active; runLoop() applies the encoder output.
template <typename Pin, typename Collector, typename Clock, typename Tick,
          Tick settle_period, Tick stop_period, Tick bit_period,
          bool inverted = false, bool long_on_is_1 = false,
          setl::TimeUnit units = setl::PWE_UNIT>
class PweTransmitterModule : public ModuleBase<Parameters<Pin>> {
public:
  using EncoderType = setl::PweEncoder<Collector, inverted, long_on_is_1, Tick, units>;
  using value_type = typename EncoderType::value_type;

  static void runSetup() {
    Pin::set(EncoderType::off_level);
  }

  static void runLoop() {
    instance.instanceLoop();
  }

  void instanceLoop() {
    Pin::set(encoder.poll(Clock::now()).current_output);
  }

  bool send(const value_type& value) {
    return encoder.send(value);
  }

  static PweTransmitterModule instance;
  EncoderType encoder{
    setl::PweWaveformParams1to3<Tick, settle_period, stop_period, bit_period, units>};
};

template <typename Pin, typename Collector, typename Clock, typename Tick,
          Tick settle_period, Tick stop_period, Tick bit_period,
          bool inverted, bool long_on_is_1, setl::TimeUnit units>
PweTransmitterModule<Pin, Collector, Clock, Tick, settle_period, stop_period,
                     bit_period, inverted, long_on_is_1, units>
  PweTransmitterModule<Pin, Collector, Clock, Tick, settle_period, stop_period,
                       bit_period, inverted, long_on_is_1, units>::instance;

} // namespace ardo
