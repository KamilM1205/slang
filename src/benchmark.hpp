#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <chrono>
#include <memory>
#include <unordered_map>

namespace SLang {
class TimeBlock {
private:
  std::string m_name;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;

public:
  TimeBlock() = delete;
  TimeBlock(std::string name);
  auto name() -> std::string;
  ~TimeBlock();
};

class Benchmark {
private:
  std::unordered_map<std::string, std::chrono::milliseconds> blocks;

protected:
  static std::shared_ptr<Benchmark> _benchmark;

public:
  Benchmark() = default;
  Benchmark(Benchmark &bench) = delete;
  void operator=(const Benchmark &bench) = delete;
  ~Benchmark() = default;

  static auto instance() -> std::shared_ptr<Benchmark>;
  static void output_at_exit();
  void registrate(std::string name, std::chrono::milliseconds time);
  void output();
};
} // namespace SLang

#define SLANG_TIME(name) SLang::TimeBlock _time_block(#name);

#endif // !BENCHMARK_HPP
