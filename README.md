# BEMS Edge AI Gateway

C++17 edge-runtime slice for a Building Energy Management System gateway. The
project models the control boundary between BACnet telemetry, AI setpoint
recommendations, local safety rules, RabbitMQ-style command publishing, and
cloud-ready telemetry payloads.

This is intentionally dependency-light so it can run on a normal development
machine before being ported to an i.MX93, BeagleBone-class Linux target, or a
Yocto image.

## Stack

- C++17
- CMake and CTest
- BACnet/IP polling boundary
- BEMS-ai recommendation boundary
- RabbitMQ command-route model
- Telemetry payload serialization
- Local simulator fallback
- Edge Linux / Yocto deployment path

## Quick Start

```bash
cmake -S . -B build
cmake --build build
./build/bems_edge_ai_gateway
ctest --test-dir build --output-on-failure
```

Expected demo output includes a commissioning cycle, the number of accepted
commands, JSON-like command payloads, and telemetry records for each zone.

## What The Code Demonstrates

- `IBacnetClient` isolates BACnet present-value acquisition.
- `IAiAdvisor` isolates BEMS-ai setpoint recommendations.
- `SafetyPolicy` blocks unsafe, low-confidence, or out-of-range commands before
  anything reaches the command bus.
- `ICommandPublisher` models a RabbitMQ route-key publishing boundary.
- `ITelemetrySink` records every control decision for observability.
- `EdgeGateway` coordinates one polling/control/publish cycle.
- `SimulatedBacnetClient` and fallback samples keep the gateway runnable without
  field hardware.

## Repository Layout

```text
include/bems_gateway/Gateway.hpp   Public gateway interfaces and data contracts
src/Gateway.cpp                    Gateway implementation and simulator data
src/main.cpp                       Commissioning/demo executable
tests/GatewayTests.cpp             CTest behavioral checks
docs/validation-plan.md            Build, test, and hardware evidence plan
.github/workflows/ci.yml           GitHub Actions build and test workflow
```

## Validation Evidence

The current host-side checks validate that:

- safe AI setpoint recommendations are published,
- unsafe setpoints are rejected,
- telemetry is recorded for each zone,
- fallback simulator data is used when BACnet returns no samples,
- command and telemetry payloads expose the expected fields.

## Remote

```text
git@github.com:rheslar1/bems-edge-ai-gateway.git
```

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
