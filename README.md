# BEMS Edge AI Gateway

C++ edge runtime coordinating BACnet polling, local safety rules, RabbitMQ command transport, and cloud-ready telemetry.

## Portfolio Purpose

This repository is an Embedded Systems project scaffold for the Rheslar portfolio. It is designed to become a hardware-backed project with build output, validation logs, and reviewable implementation evidence.

## Stack

- C++
- BACnet/IP
- RabbitMQ
- Docker
- i.MX93

## Quick Start

```bash
cmake -S . -B build
cmake --build build
./build/bems_edge_ai_gateway
python -m unittest discover -s tests
```

## Implementation Slices

- Native starter executable that exposes the project identity, stack, and validation target.
- Architecture document with control boundaries, data flow, safety assumptions, and evidence plan.
- Unit smoke test that keeps source, docs, and CI files present as the repo grows.
- GitHub Actions workflow for configure, build, executable smoke run, and repository validation.

## Evidence Target

Resilient edge control with simulator-safe fallbacks and observable health checks.

## Remote

Intended public repository: https://github.com/rheslar1/bems-edge-ai-gateway
