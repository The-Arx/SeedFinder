#include <array>
#include <iostream>
#include <string>

#include "searchers.cpp"
#include "seed.cpp"

int main(int argc, char* argv[]) {
  std::string seed_str;
  if (argc > 1) {
    seed_str = argv[1];
    if (seed_str.size() > SEED_LENGTH) {
      std::cerr << "Seed must be at most 8 characters, got " << seed_str.size()
                << std::endl;
      return -1;
    }
  } else {
    seed_str = "";
  }
  seed_str.insert(0, SEED_LENGTH - seed_str.size(), '1');
  std::cerr << "Starting on seed " << seed_str << std::endl;

  Seed seed(seed_str);
  // for (int i = 0; i < 10000000; i++) {
  while (true) {
    card_search(seed);
    seed.next();
  }
  return 0;
}
