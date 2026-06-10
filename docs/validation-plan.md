# Validation Plan

## Host Checks

```bash
cmake -S . -B build
cmake --build build
./build/bems_edge_ai_gateway
ctest --test-dir build --output-on-failure
```

## Current Test Coverage

- Accepts a safe AI recommendation and publishes one command.
- Rejects a setpoint outside the local safety envelope.
- Records telemetry for accepted and rejected decisions.
- Uses simulator fallback when the BACnet boundary returns no samples.
- Verifies command and telemetry payload fields.

## Static Analysis Targets

Add these checks once the repo is connected to its CI runner:

```bash
cppcheck --enable=warning,style,performance,portability --std=c++17 include src tests
clang-tidy -p build src/Gateway.cpp src/main.cpp tests/GatewayTests.cpp
```

## Hardware Evidence To Add

- BACnet/IP capture from a real or simulated BMS device.
- RabbitMQ management screenshot or log showing command route delivery.
- Gateway systemd status on an embedded Linux target.
- Yocto recipe and image build log.
- Watchdog/health telemetry output.
- Before/after command decision evidence for safe, unsafe, and degraded modes.

## Acceptance Criteria

- The gateway never publishes a command rejected by local safety policy.
- Every zone decision emits telemetry, even if no command is published.
- Degraded BACnet mode is visible through `fallbackUsed` and telemetry.
- The project builds without external service dependencies.
