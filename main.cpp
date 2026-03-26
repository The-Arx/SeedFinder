#include <array>
#include <iomanip>
#include <iostream>
#include <string>

#include "items.cpp"
#include "seed.cpp"
#include "util.cpp"

int bluestorm_credit(Seed seed) {
  double rarity_sho = seed.init_rand("rarity1sho");
  if (seed.random(rarity_sho) <= 0.95) return 0;
  if (seed.random(rarity_sho) <= 0.95) return 0;
  double rare_sho = seed.init_rand("Joker3sho1");
  if (seed.rand_item<Rare>(rare_sho) != Rare::Blueprint) return 0;
  if (seed.rand_item<Rare>(rare_sho) != Rare::Brainstorm) return 0;
  double card_type = seed.init_rand("cdt1");
  // check is a joker
  if (seed.random(card_type) * 28 > 20) return 0;
  if (seed.random(card_type) * 28 > 20) return 0;
  double rarity_buf = seed.init_rand("rarity1buf");
  double common_buf = seed.init_rand("Joker1buf1");
  // check both slots in buffoon pack
  for (int i = 0; i < 2; i++) {
    if (seed.random(rarity_buf) > 0.7) continue;  // uncommon or rare
    if (seed.rand_item<Common>(common_buf) == Common::Credit_Card) {
      return 1;
    }
  }
  return 0;
}

int perkeo_blueprint(Seed seed) {
  double rarity_jud = seed.init_rand("rarity1jud");
  if (seed.random(rarity_jud) <= 0.95) return 0;

  double rare_jud = seed.init_rand("Joker3jud1");
  if (seed.rand_item<Rare>(rare_jud) != Rare::Blueprint) return 0;
  double tag_rand = seed.init_rand("Tag1");
  Tag tag = seed.rand_item<Tag>(tag_rand);
  for (int i = 2; !ante1_tag(tag); i++) {
    double resample = seed.init_rand("Tag1_resample" + std::to_string(i));
    tag = seed.rand_item<Tag>(resample);
  }
  if (tag != Tag::Charm) return 0;
  double legendary = seed.init_rand("Joker4");
  if (seed.rand_item<Legendary>(legendary) != Legendary::Perkeo) return 0;
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
    if (seed.rand_item<Tarot>(arcana_tarot) == Tarot::Judgement) {
      goto has_judgment;
    }
  }
  return 0;
has_judgment:
  double jud_edition = seed.init_rand("edijud1");
  double blueprint_edition = seed.random(jud_edition);
  double sou_edition = seed.init_rand("edisou1");
  seed.random(jud_edition);

  return 1;
}

const std::string chars = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string next_chars = chars.substr(1) + chars[0];
int main() {
  std::cout << std::setprecision(16);
  std::string seed_str = "11111111";
  std::array<int, 8> seed_num;
  for (int i = 0; i < 8; i++) {
    int index = chars.find(seed_str[i]);
    if (index == std::string::npos) {
      std::cerr << "Unexpected character " << seed_str[i] << " in initial seed";
      return -1;
    }
    seed_num[i] = index;
  }
  for (int i = 0; i < 100000000; i++) {
    if (perkeo_blueprint(Seed(seed_str)) > 0) {
      std::cout << seed_str << std::endl;
    }
    for (int j = 0; j < 8; j++) {
      seed_str[j] = next_chars[seed_num[j]];
      seed_num[j] += 1;
      if (seed_num[j] != chars.length()) break;
      seed_num[j] = 0;
    }
  }
  return 0;
}
