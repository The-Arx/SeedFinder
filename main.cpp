#include <cmath>
#include <iostream>
#include <string>

#include "items.cpp"
#include "prng.cpp"

#define MATH_PI 3.14159265358979323846

double pseudohash(std::string string) {
  double num = 1;
  for (int i = string.length(); i > 0; i--) {
    num = std::fmod(
        (1.1239285023 / num) * string[i - 1] * MATH_PI + MATH_PI * i, 1.0);
  }
  return num;
}

class Seed {
  double hashed_seed;

 public:
  std::string seed;
  Seed(std::string seed) {
    this->seed = seed;
    hashed_seed = pseudohash(seed);
  }
  double init_rand(std::string key);
  double pseudoseed(double &rand_seed);
  double pseudorand(double &rand_seed);
};

double Seed::init_rand(std::string key) { return pseudohash(key + seed); }

double Seed::pseudoseed(double &rand_seed) {
  rand_seed =
      std::round(std::fmod(2.134453429141 + rand_seed * 1.72431234, 1.0) *
                 std::pow(10.0, 13.0)) /
      std::pow(10.0, 13.0);
  return (rand_seed + hashed_seed) / 2.0;
}

double Seed::pseudorand(double &rand_seed) {
  PRNG prng;
  prng.seed(pseudoseed(rand_seed));
  return prng.random();
}

int score_seed(Seed seed) {
  double rarity_sho = seed.init_rand("rarity1sho");
  if (seed.pseudorand(rarity_sho) <= 0.95) return 0;
  if (seed.pseudorand(rarity_sho) <= 0.95) return 0;
  double rare_sho = seed.init_rand("Joker3sho1");
  if (RARE_JOKERS[RARE_JOKERS.size() * seed.pseudorand(rare_sho)] !=
      Item::Blueprint) {
    return 0;
  }
  if (RARE_JOKERS[RARE_JOKERS.size() * seed.pseudorand(rare_sho)] !=
      Item::Brainstorm) {
    return 0;
  }
  double rarity_buf = seed.init_rand("rarity1buf");
  double common_buf = seed.init_rand("Joker1buf1");
  // check both slots in buffoon pack
  for (int i = 0; i < 2; i++) {
    if (seed.pseudorand(rarity_buf) > 0.7) continue;  // uncommon or rare
    if (COMMON_JOKERS[COMMON_JOKERS.size() * seed.pseudorand(common_buf)] ==
        Item::Credit_Card) {
      return 1;
    }
  }
  return 0;
}

#include <iomanip>
int main() {
  // std::cout << std::setprecision(16);
  // for (int i = 0; i < 10000000; i++) {
  //   Seed seed(std::to_string(i));
  //   if (score_seed(seed)) {
  //     std::cout << seed.seed << std::endl;
  //   }
  // }
  score_seed(Seed("7504449"));
  return 0;
}