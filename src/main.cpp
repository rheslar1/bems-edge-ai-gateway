#include "bems_gateway/Gateway.hpp"

#include <iostream>

int main() {
  using namespace bems_gateway;

  SimulatedBacnetClient bacnet(defaultCommissioningSamples());
  RuleBasedAiAdvisor advisor;
  SafetyPolicy safetyPolicy(18.0, 26.0, 0.70);
  InMemoryCommandPublisher commandPublisher;
  InMemoryTelemetrySink telemetrySink;

  EdgeGateway gateway(bacnet, advisor, safetyPolicy, commandPublisher, telemetrySink);
  const auto report = gateway.runOnce();

  std::cout << "BEMS Edge AI Gateway commissioning cycle\n";
  std::cout << "fallback_used=" << (report.fallbackUsed ? "true" : "false") << '\n';
  std::cout << "commands=" << report.commands.size() << '\n';

  for (const auto& command : report.commands) {
    std::cout << "command " << toCommandPayload(command) << '\n';
  }

  for (const auto& telemetry : report.telemetry) {
    std::cout << "telemetry " << toTelemetryPayload(telemetry) << '\n';
  }

  return report.telemetry.empty() ? 1 : 0;
}
