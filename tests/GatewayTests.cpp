#include "bems_gateway/Gateway.hpp"

#include <cassert>
#include <string>
#include <utility>
#include <vector>

namespace {

class FixedAdvisor final : public bems_gateway::IAiAdvisor {
 public:
  explicit FixedAdvisor(bems_gateway::AiRecommendation recommendation)
      : recommendation_(std::move(recommendation)) {}

  bems_gateway::AiRecommendation recommend(const bems_gateway::ZoneSample& sample) const override {
    auto recommendation = recommendation_;
    recommendation.zone = sample.zone;
    return recommendation;
  }

 private:
  bems_gateway::AiRecommendation recommendation_;
};

void acceptsSafeRecommendations() {
  using namespace bems_gateway;

  SimulatedBacnetClient bacnet({{1001, "office", 23.1, 42.0, true, true}});
  FixedAdvisor advisor({"office", 22.0, 0.95, "test"});
  SafetyPolicy policy(18.0, 26.0, 0.70);
  InMemoryCommandPublisher publisher;
  InMemoryTelemetrySink telemetry;

  EdgeGateway gateway(bacnet, advisor, policy, publisher, telemetry);
  const auto report = gateway.runOnce();

  assert(report.commands.size() == 1);
  assert(publisher.published().size() == 1);
  assert(report.telemetry.size() == 1);
  assert(report.telemetry.front().commandPublished);
  assert(report.telemetry.front().status.find("accept") != std::string::npos);
}

void rejectsUnsafeRecommendations() {
  using namespace bems_gateway;

  SimulatedBacnetClient bacnet({{1002, "atrium", 24.0, 48.0, true, true}});
  FixedAdvisor advisor({"atrium", 31.0, 0.96, "unsafe setpoint"});
  SafetyPolicy policy(18.0, 26.0, 0.70);
  InMemoryCommandPublisher publisher;
  InMemoryTelemetrySink telemetry;

  EdgeGateway gateway(bacnet, advisor, policy, publisher, telemetry);
  const auto report = gateway.runOnce();

  assert(report.commands.empty());
  assert(publisher.published().empty());
  assert(report.telemetry.size() == 1);
  assert(!report.telemetry.front().commandPublished);
  assert(report.telemetry.front().status.find("safety envelope") != std::string::npos);
}

void fallsBackToSimulatorWhenBacnetIsUnavailable() {
  using namespace bems_gateway;

  SimulatedBacnetClient bacnet({});
  RuleBasedAiAdvisor advisor;
  SafetyPolicy policy(18.0, 26.0, 0.70);
  InMemoryCommandPublisher publisher;
  InMemoryTelemetrySink telemetry;

  EdgeGateway gateway(bacnet, advisor, policy, publisher, telemetry);
  const auto report = gateway.runOnce();

  assert(report.fallbackUsed);
  assert(report.telemetry.size() == 1);
  assert(report.telemetry.front().zone == "simulated-zone");
  assert(!report.commands.empty());
}

void payloadsExposeRouteAndTelemetryFields() {
  const bems_gateway::GatewayCommand command{"office", 22.0, "bems.zone.setpoint"};
  const auto commandPayload = bems_gateway::toCommandPayload(command);
  assert(commandPayload.find("\"route_key\":\"bems.zone.setpoint\"") != std::string::npos);
  assert(commandPayload.find("\"setpoint_c\":22.0") != std::string::npos);

  const bems_gateway::TelemetryRecord telemetry{
      "office", 23.0, 44.0, 22.0, true, "accept: local safety policy passed"};
  const auto telemetryPayload = bems_gateway::toTelemetryPayload(telemetry);
  assert(telemetryPayload.find("\"command_published\":true") != std::string::npos);
  assert(telemetryPayload.find("\"temperature_c\":23.0") != std::string::npos);
}

}  // namespace

int main() {
  acceptsSafeRecommendations();
  rejectsUnsafeRecommendations();
  fallsBackToSimulatorWhenBacnetIsUnavailable();
  payloadsExposeRouteAndTelemetryFields();
  return 0;
}
