# BEMS Edge AI Gateway Design Package

## Purpose

C++ edge runtime coordinating BACnet polling, local safety rules, RabbitMQ command transport, and cloud-ready telemetry.

This package defines the project as an implementation-ready embedded system. It covers system architecture, requirements, interface boundaries, runtime design, validation evidence, and phased delivery.

## Project Profile

| Field | Value |
| --- | --- |
| Repository | `rheslar1/bems-edge-ai-gateway` |
| Primary stack | C++17, C++ Design Patterns, SOLID, C++, BACnet/IP, RabbitMQ, Docker, i.MX93 |
| Review proof point | Resilient edge control with simulator-safe fallbacks and observable health checks. |

## Artifacts

- [System Design](system-design.md)
- [Requirements](requirements.md)
- [Interface Control](interface-control.md)
- [Runtime Design](runtime-design.md)
- [Validation Plan](validation-plan.md)
- [Implementation Roadmap](implementation-roadmap.md)
- [Draw.io UML](diagrams/system-design.drawio)
- [PNG UML](diagrams/system-design.png)
