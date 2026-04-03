#include "items.h"
#include "seed.cpp"
#include "util.cpp"

void bluestorm_credit(const Seed &seed) {
  RandGen rarity_sho = seed.init_rand("rarity1sho");
  if (rarity_sho.random() <= 0.95) return;
  if (rarity_sho.random() <= 0.95) return;
  RandGen rare_sho = seed.init_rand("Joker3sho1");
  if (rare_sho.rand_item<Rare>() != Rare::Blueprint) return;
  if (rare_sho.rand_item<Rare>() != Rare::Brainstorm) return;
  RandGen card_type = seed.init_rand("cdt1");
  // check is a joker
  if (card_type.random() * 28 > 20) return;
  if (card_type.random() * 28 > 20) return;
  RandGen rarity_buf = seed.init_rand("rarity1buf");
  RandGen common_buf = seed.init_rand("Joker1buf1");
  // check both slots in buffoon pack
  for (int i = 0; i < 2; i++) {
    if (rarity_buf.random() > 0.7) continue;  // uncommon or rare
    if (common_buf.rand_item<Common>() == Common::Credit_Card) {
      std::cout << seed.to_string() << std::endl;
      return;
    }
  }
  return;
}

void perkeo_blueprint(const Seed &seed) {
  RandGen rarity_jud = seed.init_rand("rarity1jud");
  if (rarity_jud.random() <= 0.95) return;
  RandGen rare_jud = seed.init_rand("Joker3jud1");
  if (rare_jud.rand_item<Rare>() != Rare::Blueprint) return;
  RandGen legendary = seed.init_rand("Joker4");
  if (legendary.rand_item<Legendary>() != Legendary::Perkeo) return;
  RandGen tag_rand = seed.init_rand("Tag1");
  Tag tag = tag_rand.rand_item<Tag>();
  for (int i = 2; !ante1_tag(tag); i++) {
    tag = seed.init_rand("Tag1_resample" + std::to_string(i)).rand_item<Tag>();
  }
  if (tag != Tag::Charm) return;
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
  // if (blueprint_edition <= 0.96 && perkeo_edition <= 0.96) return;
  std::cout << edition_str(blueprint_edition) << " "
            << edition_str(perkeo_edition) << " " << seed.to_string()
            << std::endl;
}

void ante1_cavendish(const Seed &seed) {
    RandGen cavendish = seed.init_rand("cavendish");
    if (cavendish.random() >= 1.0 / 1000) return;
    RandGen gros_michel = seed.init_rand("gros_michel");
    if (gros_michel.random() >= 1.0 / 6) return;

    RandGen shop_item = seed.init_rand("cdt1");
    RandGen shop_edition = seed.init_rand("edisho1");
    RandGen shop_rarity = seed.init_rand("rarity1sho");
    RandGen shop_common = seed.init_rand("Joker1sho1");

    bool has_michel = false;
    double michel_edition = 0.0;
    for (int i = 0; i < 2; i++) {
      if (shop_item.random() * 28 > 20) continue;
      // if (shop_item.random() * 30 > 20) return; // ghost deck
      double edition = shop_edition.random();
      if (shop_rarity.random() > 0.7) continue;
      if (shop_common.rand_item<Common>() != Common::Gros_Michel) continue;
      if (has_michel) continue;
      has_michel = true;
      michel_edition = edition;
    }
    if (!has_michel) return;

    bool has_cavendish = false;
    double cavendish_edition = 0.0;
    for (int i = 0; i < 2; i++) {
      if (shop_item.random() * 28 > 20) continue;
      double edition = shop_edition.random();
      if (shop_rarity.random() > 0.7) continue;
      if (shop_common.rand_item<Common>() != Common::Cavendish) continue;
      if (has_cavendish) continue;
      has_cavendish = true;
      cavendish_edition = edition;
    }
    if (!has_cavendish) return;

    std::cout << seed << " " << edition_str(michel_edition) << " " << edition_str(cavendish_edition) << std::endl;
}

void bugged_seeds(const Seed &seed) {
  RandGen rand = seed.init_rand("erratic");
  if (rand.is_bugged()) {
    std::cout << seed << std::endl;
  }
}


