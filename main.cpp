#include <cmath>
#include <cstdint>
#include <iostream>
#include <span>
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
  Seed(std::string seed);
  double init_rand(std::string key);
  double pseudoseed(double &rand_seed);
  double random(double &rand_seed);
  Item rand_choice(double &rand_seed, std::span<const Item> choices);
};

Seed::Seed(std::string seed) {
  this->seed = seed;
  this->hashed_seed = pseudohash(seed);
}

double Seed::init_rand(std::string key) { return pseudohash(key + seed); }

double Seed::pseudoseed(double &rand_seed) {
  rand_seed =
      std::round(std::fmod(2.134453429141 + rand_seed * 1.72431234, 1.0) *
                 std::pow(10.0, 13.0)) /
      std::pow(10.0, 13.0);
  return (rand_seed + hashed_seed) / 2.0;
}

double Seed::random(double &rand_seed) {
  return first_rand(pseudoseed(rand_seed));
}

Item Seed::rand_choice(double &rand_seed, std::span<const Item> choices) {
  return choices[choices.size() * this->random(rand_seed)];
}

int bluestorm_credit(Seed seed) {
  double rarity_sho = seed.init_rand("rarity1sho");
  if (seed.random(rarity_sho) <= 0.95) return 0;
  if (seed.random(rarity_sho) <= 0.95) return 0;
  double rare_sho = seed.init_rand("Joker3sho1");
  if (seed.rand_choice(rare_sho, RARE_JOKERS) != Item::Blueprint) return 0;
  if (seed.rand_choice(rare_sho, RARE_JOKERS) != Item::Brainstorm) return 0;
  double card_type = seed.init_rand("cdt1");
  // check is a joker
  if (seed.random(card_type) * 28 > 20) return 0;
  if (seed.random(card_type) * 28 > 20) return 0;
  double rarity_buf = seed.init_rand("rarity1buf");
  double common_buf = seed.init_rand("Joker1buf1");
  // check both slots in buffoon pack
  for (int i = 0; i < 2; i++) {
    if (seed.random(rarity_buf) > 0.7) continue;  // uncommon or rare
    if (seed.rand_choice(common_buf, COMMON_JOKERS) == Item::Credit_Card) {
      return 1;
    }
  }
  return 0;
}

int perkeo_blueprint(Seed seed) {
  double rarity_jud = seed.init_rand("rarity1jud");
  if (seed.random(rarity_jud) <= 0.95) return 0;

  double rare_jud = seed.init_rand("Joker3jud1");
  if (seed.rand_choice(rare_jud, RARE_JOKERS) != Item::Blueprint) return 0;

  double tag_rand = seed.init_rand("Tag1");
  Item tag = seed.rand_choice(tag_rand, TAGS);
  for (int i = 2; !ante1_tag(tag); i++) {
    double resample = seed.init_rand("Tag1_resample" + std::to_string(i));
    tag = seed.rand_choice(resample, TAGS);
  }
  if (tag != Item::Charm_Tag) return 0;
  double legendary = seed.init_rand("Joker4");
  if (seed.rand_choice(legendary, LEGENDARY_JOKERS) != Item::Perkeo) return 0;
  double soul_rand = seed.init_rand("soul_Tarot1");
  for (int i = 0; i < 5; i++) {
    if (seed.random(soul_rand) > 0.997) {
      goto has_soul;
    }
  }
  return 0;
has_soul:
  double arcana_tarot = seed.init_rand("Tarotar11");
  for (int i = 0; i < 4; i++) {
    // this ignores rerolling if the same tarot apears twice
    if (seed.rand_choice(arcana_tarot, TAROTS) == Item::Judgement) {
      goto has_judgment;
    }
  }
  return 0;
has_judgment:
  double jud_edition = seed.init_rand("edijud1");
  double sou_edition = seed.init_rand("edisou1");

  return 1;
}

const std::string chars = "123456789ABCDEFGHIJKLMNPQRSTUVWXYZ";
#include <iomanip>
int main() {
  std::cout << std::setprecision(16);
  std::string seed_str = "11111113";
  std::array<int, 8> seed_num;
  for (int i = 0; i < 8; i++) {
    int index = chars.find(seed_str[i]);
    if (index == std::string::npos) {
      std::cerr << "Unexpected character " << seed_str[i] << " in initial seed";
      return -1;
    }
    seed_num[i] = index;
  }
  for (int i = 0; i < 1000000000; i++) {
    if (perkeo_blueprint(Seed(seed_str)) > 0) {
      std::cout << seed_str << std::endl;
    }
    for (int j = 0; j < 8; j++) {
      seed_num[j] = (seed_num[j] + 1) % chars.length();
      seed_str[j] = chars[seed_num[j]];
      if (seed_num[j] != 0) break;
    }
  }
  return 0;
}
