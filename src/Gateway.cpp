#include "bems_gateway/Gateway.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <utility>

namespace bems_gateway {

namespace {

constexpr double kOccupiedComfortSetpointC = 22.0;
constexpr double kUnoccupiedEcoSetpointC = 24.5;

std::string boolText(bool value) {
  return value ? "true" : "false";
}

std::string fixed(double value) {
  std::ostringstream output;
  output << std::fixed << std::setprecision(1) << value;
  return output.str();
}

}  // namespace

SimulatedBacnetClient::SimulatedBacnetClient(std::vector<ZoneSample> samples)
    : samples_(std::move(samples)) {}

std::vector<ZoneSample> SimulatedBacnetClient::readPresentValues() {
  return samples_;
}

AiRecommendation RuleBasedAiAdvisor::recommend(const ZoneSample& sample) const {
  const double target = sample.occupied ? kOccupiedComfortSetpointC : kUnoccupiedEcoSetpointC;
  const double confidence = sample.equipmentAvailable ? 0.91 : 0.48;
  const std::string mode = sample.occupied ? "occupied comfort" : "unoccupied economy";

  return {sample.zone, target, confidence, mode};
}

SafetyPolicy::SafetyPolicy(double minSetpointC, double maxSetpointC, double minConfidence)
    : minSetpointC_(minSetpointC),
      maxSetpointC_(maxSetpointC),
      minConfidence_(minConfidence) {}

SafetyDecision SafetyPolicy::evaluate(const ZoneSample& sample,
                                      const AiRecommendation& recommendation) const {
  const double bounded = std::clamp(recommendation.requestedSetpointC, minSetpointC_, maxSetpointC_);

  if (!sample.equipmentAvailable) {
    return {false, bounded, "hold: equipment unavailable"};
  }

  if (recommendation.confidence < minConfidence_) {
    return {false, bounded, "hold: recommendation confidence below threshold"};
  }

  if (bounded != recommendation.requestedSetpointC) {
    return {false, bounded, "hold: requested setpoint outside safety envelope"};
  }

  if (sample.temperatureC < 5.0 || sample.temperatureC > 45.0) {
    return {false, bounded, "hold: BACnet present value outside plausible range"};
  }

  return {true, bounded, "accept: local safety policy passed"};
}

void InMemoryCommandPublisher::publish(const GatewayCommand& command) {
  published_.push_back(command);
}

const std::vector<GatewayCommand>& InMemoryCommandPublisher::published() const {
  return published_;
}

void InMemoryTelemetrySink::record(const TelemetryRecord& telemetry) {
  records_.push_back(telemetry);
}

const std::vector<TelemetryRecord>& InMemoryTelemetrySink::records() const {
  return records_;
}

EdgeGateway::EdgeGateway(IBacnetClient& bacnet,
                         const IAiAdvisor& advisor,
                         const SafetyPolicy& safetyPolicy,
                         ICommandPublisher& commandPublisher,
                         ITelemetrySink& telemetrySink)
    : bacnet_(bacnet),
      advisor_(advisor),
      safetyPolicy_(safetyPolicy),
      commandPublisher_(commandPublisher),
      telemetrySink_(telemetrySink) {}

CycleReport EdgeGateway::runOnce() {
  auto samples = bacnet_.readPresentValues();
  bool fallbackUsed = false;

  if (samples.empty()) {
    samples = fallbackSamples();
    fallbackUsed = true;
  }

  CycleReport report{{}, {}, fallbackUsed};

  for (const auto& sample : samples) {
    const auto recommendation = advisor_.recommend(sample);
    const auto decision = safetyPolicy_.evaluate(sample, recommendation);

    if (decision.accepted) {
      GatewayCommand command{sample.zone, decision.boundedSetpointC, "bems.zone.setpoint"};
      commandPublisher_.publish(command);
      report.commands.push_back(command);
    }

    TelemetryRecord telemetry{
        sample.zone,
        sample.temperatureC,
        sample.humidityPct,
        recommendation.requestedSetpointC,
        decision.accepted,
        decision.reason};

    telemetrySink_.record(telemetry);
    report.telemetry.push_back(telemetry);
  }

  return report;
}

std::string toTelemetryPayload(const TelemetryRecord& telemetry) {
  std::ostringstream output;
  output << "{"
         << "\"zone\":\"" << telemetry.zone << "\","
         << "\"temperature_c\":" << fixed(telemetry.temperatureC) << ","
         << "\"humidity_pct\":" << fixed(telemetry.humidityPct) << ","
         << "\"requested_setpoint_c\":" << fixed(telemetry.requestedSetpointC) << ","
         << "\"command_published\":" << boolText(telemetry.commandPublished) << ","
         << "\"status\":\"" << telemetry.status << "\""
         << "}";
  return output.str();
}

std::string toCommandPayload(const GatewayCommand& command) {
  std::ostringstream output;
  output << "{"
         << "\"route_key\":\"" << command.routeKey << "\","
         << "\"zone\":\"" << command.zone << "\","
         << "\"setpoint_c\":" << fixed(command.setpointC)
         << "}";
  return output.str();
}

std::vector<ZoneSample> defaultCommissioningSamples() {
  return {
      {3001, "lab-east", 23.8, 41.0, true, true},
      {3002, "lab-west", 21.3, 44.5, false, true},
      {3003, "mechanical-room", 27.1, 50.0, false, false},
  };
}

std::vector<ZoneSample> fallbackSamples() {
  return {
      {9001, "simulated-zone", 22.7, 45.0, true, true},
  };
}

}  // namespace bems_gateway
