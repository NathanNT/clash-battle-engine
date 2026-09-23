# 0001: Incremental modularization

Date: 2026-09-18  
Status: accepted

## Context

The engine, Viewer, and tests had grown into a few very large files. Changing
them required broad searches and made regressions more likely during TH18
integration.

## Decision

Extract domain responsibilities in small increments that preserve public APIs
and serialized formats. Keep a simple C++20 Core library. Do not introduce an
ECS, dependency-injection framework, or plugin system.

Durable documentation lives in `docs/`. Current work state lives in `state/`.
Structured data and validators remain the source of truth for exhaustive
inventories.

## Consequences

Every extraction must pass the complete test suite. Adapters remain downstream
of Core. Functional changes and mechanical extraction are not mixed in the
same increment.
