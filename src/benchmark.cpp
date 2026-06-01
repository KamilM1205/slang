#include "benchmark.hpp"
#include "compile_options.hpp"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>

std::shared_ptr<SLang::Benchmark> SLang::Benchmark::_benchmark = nullptr;

SLang::TimeBlock::TimeBlock(std::string name) {
  if constexpr (SLang::SLANG_USE_BENCHMARK) {
    m_name = name;
    m_start_time = std::chrono::high_resolution_clock::now();
  }
}

auto SLang::TimeBlock::name() -> std::string { return m_name; }

SLang::TimeBlock::~TimeBlock() {
  if constexpr (SLang::SLANG_USE_BENCHMARK) {
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - m_start_time);
    Benchmark::instance()->registrate(m_name, time);
  }
}

auto SLang::Benchmark::instance() -> std::shared_ptr<Benchmark> {
  if (!_benchmark) {
    _benchmark = std::shared_ptr<Benchmark>(new Benchmark());

    if (std::atexit(Benchmark::output_at_exit) != 0) {
      std::cerr << "Cannot registrate benchmark exit detection." << std::endl;
    }
  }

  return _benchmark;
}

void SLang::Benchmark::registrate(std::string name,
                                  std::chrono::milliseconds time) {
  if constexpr (SLang::SLANG_USE_BENCHMARK) {
    blocks[name] = std::move(time);
  }
}

void SLang::Benchmark::output_at_exit() { Benchmark::instance()->output(); }

void SLang::Benchmark::output() {
  std::cout << "Benchmark result:" << std::endl;
  for (auto &[k, v] : blocks) {
    std::cout << '\t' << k << "'s time: " << v << std::endl;
  }
}
