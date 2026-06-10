# BEMS Edge AI Gateway Architecture

## Goal

Provide a reviewable C++17 edge gateway slice for a BEMS deployment: collect
BACnet zone data, ask an AI advisory boundary for setpoints, enforce local
safety policy, publish accepted commands, and emit telemetry for cloud or
operator dashboards.

## Runtime Flow

```text
BACnet devices or simulator
  -> IBacnetClient
  -> EdgeGateway
  -> IAiAdvisor
  -> SafetyPolicy
  -> ICommandPublisher
  -> ITelemetrySink
```

`EdgeGateway::runOnce()` is the orchestration point. It reads zone present
values, falls back to simulator data if the BACnet boundary is empty, evaluates
one recommendation per zone, publishes only accepted commands, and records every
decision as telemetry.

## Key Boundaries

| Boundary | Interface | Current implementation |
| --- | --- | --- |
| BACnet acquisition | `IBacnetClient` | `SimulatedBacnetClient` |
| AI recommendation | `IAiAdvisor` | `RuleBasedAiAdvisor` |
| Local command safety | `SafetyPolicy` | min/max setpoint, confidence, equipment, plausible temperature checks |
| RabbitMQ command route | `ICommandPublisher` | `InMemoryCommandPublisher` |
| Observability | `ITelemetrySink` | `InMemoryTelemetrySink` |

## Safety Rules

The starter policy rejects commands when:

- equipment is unavailable,
- AI confidence is below the threshold,
- requested setpoint is outside the configured comfort/safety envelope,
- BACnet present-value temperature is outside a plausible range.

These checks are intentionally local. A future cloud or AI service can recommend
actions, but the edge gateway keeps the final authority to hold unsafe commands.

## Simulator Fallback

If the BACnet boundary returns no samples, the gateway uses `fallbackSamples()`
and marks `CycleReport::fallbackUsed`. This keeps CI and commissioning demos
repeatable while still making degraded mode visible to tests and telemetry.

## SOLID / C++ Design Notes

- Single Responsibility: acquisition, advisory, safety, command publishing, and
  telemetry are separate types.
- Open/Closed: production BACnet, RabbitMQ, or telemetry implementations can be
  added without changing `EdgeGateway`.
- Liskov Substitution: tests substitute a fixed advisor through `IAiAdvisor`.
- Interface Segregation: each interface exposes one focused behavior.
- Dependency Inversion: `EdgeGateway` depends on abstractions for external
  boundaries.

## Deployment Path

The current code is host-buildable C++17. Hardware-backed expansion should add:

- real BACnet/IP client implementation,
- RabbitMQ or MQTT publisher implementation,
- BEMS-ai service client or local ONNX inference boundary,
- systemd unit for the gateway,
- Yocto recipe and image integration,
- health-check endpoint or watchdog heartbeat,
- captured telemetry and command evidence from a real BEMS or simulator stack.

<!-- cpp17-solid-implementation:start -->
## C++17, Design Patterns, and SOLID Implementation

This repository includes a host-buildable C++17 implementation, not only documentation. The implementation applies:

- Strategy pattern for validation rules.
- Adapter interfaces for input samples and telemetry/reporting.
- Composite validation for combining safety and readiness checks.
- Facade orchestration through the project runtime class.
- SOLID boundaries between profile data, input acquisition, validation, telemetry encoding, and tests.
<!-- cpp17-solid-implementation:end -->

<!-- deep-architecture-links:start -->
## Deep Architecture and UML

- [Deep architecture](docs/deep-architecture.md)
- [Full UML Draw.io source](docs/diagrams/full-system-uml.drawio)
- [Full UML PNG export](docs/diagrams/full-system-uml.png)
<!-- deep-architecture-links:end -->
