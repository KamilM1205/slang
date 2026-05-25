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
};

std::unordered_map<std::string, Command> commands = {
    {"--ltrace", Command::LTRACE},
};

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
  }
}

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

int main(int argc, char *argv[]) {
  SLang::SLContext ctx;
  SLang::StdLib::bind(ctx);

  parse_args(argc, argv);

  std::string src = load_src(argv[1]);

  ctx.parse(src);

  ctx.execute();

  return EXIT_SUCCESS;
}
