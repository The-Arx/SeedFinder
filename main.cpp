#include <array>
#include <iostream>
#include <string>

#include "items.cpp"
#include "seed.cpp"
#include "util.cpp"

int bluestorm_credit(Seed seed) {
  RandGen rarity_sho = seed.init_rand("rarity1sho");
  if (rarity_sho.random() <= 0.95) return 0;
  if (rarity_sho.random() <= 0.95) return 0;
  RandGen rare_sho = seed.init_rand("Joker3sho1");
  if (rare_sho.rand_item<Rare>() != Rare::Blueprint) return 0;
  if (rare_sho.rand_item<Rare>() != Rare::Brainstorm) return 0;
  RandGen card_type = seed.init_rand("cdt1");
  // check is a joker
  if (card_type.random() * 28 > 20) return 0;
  if (card_type.random() * 28 > 20) return 0;
  RandGen rarity_buf = seed.init_rand("rarity1buf");
  RandGen common_buf = seed.init_rand("Joker1buf1");
  // check both slots in buffoon pack
  for (int i = 0; i < 2; i++) {
    if (rarity_buf.random() > 0.7) continue;  // uncommon or rare
    if (common_buf.rand_item<Common>() == Common::Credit_Card) {
      return 1;
    }
  }
  return 0;
}

void perkeo_blueprint(Seed seed) {
  RandGen rarity_jud = seed.init_rand("rarity1jud");
  if (rarity_jud.random() <= 0.95) return;

  RandGen rare_jud = seed.init_rand("Joker3jud1");
  if (rare_jud.rand_item<Rare>() != Rare::Blueprint) return;
  RandGen tag_rand = seed.init_rand("Tag1");
  Tag tag = tag_rand.rand_item<Tag>();
  for (int i = 2; !ante1_tag(tag); i++) {
    tag = seed.init_rand("Tag1_resample" + std::to_string(i)).rand_item<Tag>();
  }
  if (tag != Tag::Charm) return;
  RandGen legendary = seed.init_rand("Joker4");
  if (legendary.rand_item<Legendary>() != Legendary::Perkeo) return;
  RandGen soul_rand = seed.init_rand("soul_Tarot1");
  for (int i = 0; i < 5; i++) {
    if (soul_rand.random() > 0.997) {
      goto has_soul;
    }
  }
  return;
has_soul:
  RandGen arcana_tarot = seed.init_rand("Tarotar11");
  for (int i = 0; i < 4; i++) {
    // this ignores rerolling if the same tarot apears twice
    if (arcana_tarot.rand_item<Tarot>() == Tarot::Judgement) {
      goto has_judgment;
    }
  }
  return;
has_judgment:
  double blueprint_edition = seed.init_rand("edijud1").random();
  double perkeo_edition = seed.init_rand("edisou1").random();
  if (blueprint_edition <= 0.96 && perkeo_edition <= 0.96) return;
  std::cout << edition_str(blueprint_edition) << " "
            << edition_str(perkeo_edition) << " " << seed.seed << std::endl;
}

const std::string chars = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string next_chars = chars.substr(1) + chars[0];
int main(int argc, char* argv[]) {
  std::string seed_str;
  if (argc > 1) {
    seed_str = argv[1];
    if (seed_str.size() > 8) {
      std::cerr << "Seed must be at most 8 characters, got " << seed_str.size() << std::endl;
      return -1;
    }
    seed_str.insert(0, 8 - seed_str.size(), '1');
  } else {
    seed_str = "11111111";
  }
  std::cerr << "Starting on seed " << seed_str << std::endl;
  std::array<int, 8> seed_num;
  for (int i = 0; i < 8; i++) {
    int index = chars.find(seed_str[i]);
    if (index == std::string::npos) {
      std::cerr << "Unexpected character " << seed_str[i] << " in initial seed" << std::endl;
      return -1;
    }
    seed_num[i] = index;
  }
  // for (int i = 0; i < 100000000; i++) {
  while (true) {
    perkeo_blueprint(Seed(seed_str));
    for (int j = 0; j < 8; j++) {
      seed_str[j] = next_chars[seed_num[j]];
      seed_num[j] += 1;
      if (seed_num[j] != chars.length()) break;
      seed_num[j] = 0;
    }
  }
  return 0;
}
