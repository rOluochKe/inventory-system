# Inventory Management System

A CLI inventory management application written in C++17 for a small
electronics spare-parts shop. Stores products, suppliers, and stock
transactions, and persists all data to disk.

## Requirements

- C++17 compiler (g++ >= 9 or clang++ >= 10)
- CMake >= 3.15

## Build

- cmake -S . -B build
- cmake --build build

## Run

./build/inventory_app

## Test

ctest --test-dir build --output-on-failure

## Project structure

- `include/inventory/` — public headers
- `src/` — implementation
- `tests/` — unit tests
- `data/` — sample data file

## Status

1. Phase 1 — initial build skeleton.
