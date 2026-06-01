#include "benchmark.hpp"
#include "compile_options.hpp"
#include "context.hpp"
#include "stdlib.hpp"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <unordered_map>

bool ltrace = false;

enum class Command {
  LTRACE = 0,
  VERSION,
};

std::unordered_map<std::string, Command> commands = {
    {"--ltrace", Command::LTRACE},
    {"--version", Command::VERSION},
};

std::string load_src(char *path) {
  std::ifstream src(path);
  std::string res;
  std::string line;

  if (!src.is_open()) {
    std::cout << "Error: Cannot open file." << std::endl;
    return "";
  }

  while (std::getline(src, line)) {
    res += line;
    res += '\n';
  }

  return res;
}

void parse_args(int argc, char *argv[]) {
  if (argc == 3 && commands.find(argv[2]) != commands.end()) {
    switch (commands.at(argv[2])) {
    case Command::LTRACE:
      ltrace = true;
      break;
    default:
      std::cout << "Unexpected command: " << argv[2] << std::endl;
      break;
    }
  } else if (argc == 2 && commands.contains(argv[1])) {
    if (commands.at(argv[1]) == Command::VERSION) {
      std::cout << "SLang version: " << SLang::SLANG_VERSION << std::endl;
    }
  } else if (argc == 2) {
    SLang::SLContext ctx;
    SLang::StdLib::bind(ctx);

    std::string src = load_src(argv[1]);

    ctx.parse(src);

    ctx.execute();
  }
}

int main(int argc, char *argv[]) {
  SLANG_TIME(MAIN);

  parse_args(argc, argv);
  return EXIT_SUCCESS;
}
