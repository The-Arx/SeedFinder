#include <iostream>

#include "items.h"
#include "seed.cu.cc"
#include "util.cu.cc"

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
      std::cout << seed.seed << std::endl;
      return;
    }
  }
  return;
}

std::string edition_str(Edition edition) {
  switch (edition) {
    case Edition::Foil: return "foil";
    case Edition::Holographic: return "holographic";
    case Edition::Polychrome: return "polychrome";
    case Edition::Negative: return "negative";
    default: return "none";
  }
}

std::string edition_str(double edition) {
  return edition_str(joker_edition_from_rand(edition));
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
    tag = seed.init_rand("Tag1_resample", i).rand_item<Tag>();
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
  RandGen jud_edition = seed.init_rand("edijud1");
  Edition blueprint_edition = joker_edition_from_rand(jud_edition.random());
  RandGen soul_edition = seed.init_rand("edisou1");
  Edition perkeo_edition = joker_edition_from_rand(soul_edition.random());
  // if (blueprint_edition <= 0.96 && perkeo_edition <= 0.96) return;
  std::cout << edition_str(blueprint_edition) << " "
            << edition_str(perkeo_edition) << " " << seed.seed
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

  std::cout << seed.seed << " " << edition_str(michel_edition) << " "
            << edition_str(cavendish_edition) << std::endl;
}

void card_search(const Seed &seed) {
  RandGen pack_rand = seed.init_rand("shop_pack1");
  Pack pack = pack_from_rand(pack_rand.random());
  if (pack_type(pack) != PackType::Standard) return;
  int size = pack_size(pack);

  std::array<bool, 5> matches;
  int largest_card_match = -1;
  RandGen card_rand = seed.init_rand("frontsta1");
  for (int i = 0; i < size; i++) {
    matches[i] = card_rand.rand_item<Card>() == Card::S_K;
    if (matches[i]) {
      largest_card_match = i;
    }
  }
  if (largest_card_match < 0) return;

  int largest_edition_match = -1;
  RandGen edition_rand = seed.init_rand("standard_edition1");
  for (int i = 0; i <= largest_card_match; i++) {
    double edition = edition_rand.random();
    if (matches[i]) {
      if (edition > 1 - 0.006 * 2) {
        largest_edition_match = i;
      } else {
        matches[i] = false;
      }
    }
  }
  if (largest_edition_match < 0) return;


  // RandGen card_rand = seed.init_rand("frontsta1");
  // RandGen edition_rand = seed.init_rand("standard_edition1");
  RandGen has_enhancment_rand = seed.init_rand("stdset1");
  RandGen enhancment_rand = seed.init_rand("Enhancedsta1");
  RandGen has_seal_rand = seed.init_rand("stdseal1");
  RandGen seal_type_rand = seed.init_rand("stdsealtype1");
  for (int i = 0; i <= largest_edition_match; i++) {
    // Card card = card_rand.rand_item<Card>();
    // bool polychrome = edition_rand.random() > 1 - 0.006 * 2;
    bool red_seal =
        has_seal_rand.random() > 1 - 0.02 * 10 && seal_type_rand.random() > 0.75;
    Enhancement enhancment = has_enhancment_rand.random() > 0.6
                                 ? enhancment_rand.rand_item<Enhancement>()
                                 : Enhancement::None;
    if (matches[i] /* &&  card == Card::S_K  && polychrome */ && red_seal &&
        enhancment == Enhancement::Glass) {
      std::cout << seed.seed << std::endl;
    }
  }
}

void bugged_seeds(const Seed &seed) {
  RandGen rand = seed.init_rand("erratic");
  if (rand.is_bugged()) {
    std::cout << seed.seed << std::endl;
  }
}

__device__ void erratic_regular(const Seed &seed) {
  RandGen card_rand = seed.init_rand("erratic");
  char ranks[static_cast<int>(Rank::Count)] = {};
  char suits[static_cast<int>(Suit::Count)] = {};
  for (int i = 0; i < 52; i++) {
    Card card = card_rand.rand_item<Card>();
    if (++ranks[static_cast<int>(card_rank(card))] > 4) return;
    if (++suits[static_cast<int>(card_suit(card))] > 13) return;
  }
  std::cout << seed.seed << std::endl;
}

void royal_flush(const Seed &seed) {
  PRNG shuffle = seed.init_rand("nr1").prng();
  for (int i = 52; i > 52 - 8; i--) {
    if (static_cast<int>(shuffle.random() * i) < 52 - 8) return;
  }
  std::cout << seed.seed << std::endl;
}

bool check_shuffle(RandGen shuffle, bool cards[]) {
  for (int n = 52; n > 52 - 8; n--) {
    int index = shuffle.random() * n;
    if (!cards[index]) return false;
    cards[index] = cards[n - 1];
  }
  return true;
}

void dna_glass(const Seed &seed) {
  RandGen pack_rand = seed.init_rand("shop_pack1");
  if (pack_from_rand(pack_rand.random()) != Pack::Standard_Normal) return;

  std::array<bool, 3> matches;
  int largest_card_match = -1;
  RandGen edition_rand = seed.init_rand("standard_edition1");
  for (int i = 0; i < 3; i++) {
    matches[i] = edition_rand.random() > 1 - 0.006 * 2;
    if (matches[i]) {
      largest_card_match = i;
    }
  }
  if (largest_card_match < 0) return;

  int largest_edition_match = -1;
  RandGen card_rand = seed.init_rand("frontsta1");
  for (int i = 0; i <= largest_card_match; i++) {
    Card card = card_rand.rand_item<Card>();
    if (matches[i]) {
      if (card_rank(card) == Rank::King) {
        largest_edition_match = i;
      } else {
        matches[i] = false;
      }
    }
  }
  if (largest_edition_match < 0) return;


  // RandGen card_rand = seed.init_rand("frontsta1");
  // RandGen edition_rand = seed.init_rand("standard_edition1");
  RandGen has_enhancment_rand = seed.init_rand("stdset1");
  RandGen enhancment_rand = seed.init_rand("Enhancedsta1");
  RandGen has_seal_rand = seed.init_rand("stdseal1");
  RandGen seal_type_rand = seed.init_rand("stdsealtype1");
  for (int i = 0; i <= largest_edition_match; i++) {
    // Card card = card_rand.rand_item<Card>();
    // bool polychrome = edition_rand.random() > 1 - 0.006 * 2;
    bool red_seal =
        has_seal_rand.random() > 1 - 0.02 * 10 && seal_type_rand.random() > 0.75;
    Enhancement enhancment = has_enhancment_rand.random() > 0.6
                                 ? enhancment_rand.rand_item<Enhancement>()
                                 : Enhancement::None;
    if (matches[i] /* &&  card == Card::S_K  && polychrome */ && red_seal &&
        enhancment == Enhancement::Glass) {
      goto has_card;
    }
  }
  return;
  has_card:

  RandGen rarity_buf = seed.init_rand("rarity1buf");
  RandGen rare_buf = seed.init_rand("Joker3buf1");
  // check both slots in buffoon pack
  for (int i = 0; i < 2; i++) {
    if (rarity_buf.random() <= 0.95) continue;
    if (rare_buf.rand_item<Rare>() == Rare::DNA) {
      std::cout << seed.seed << std::endl;
      return;
    }
  }
}

void round2_score(const Seed &seed) {
  RandGen pack_rand = seed.init_rand("shop_pack1");
  if (pack_from_rand(pack_rand.random()) != Pack::Spectral_Mega) return;
  
  RandGen soul_rand = seed.init_rand("soul_Spectral1");
  for (int i = 0; i < 4; i++) {
    if (soul_rand.random() > 0.997) {
      goto has_soul;
    }
    soul_rand.skip();
  }
  return;
has_soul:

  RandGen legendary = seed.init_rand("Joker4");
  if (legendary.rand_item<Legendary>() != Legendary::Triboulet) return;

  RandGen arcana_tarot = seed.init_rand("Spectralspe1");
  for (int i = 0; i < 3; i++) {
    if (arcana_tarot.rand_item<Spectral>() == Spectral::Immolate) {
      goto has_immolate;
    }
  }
  return;
has_immolate:

  RandGen rarity_buf = seed.init_rand("rarity1buf");
  RandGen rare_buf = seed.init_rand("Joker3buf1");
  for (int i = 0; i < 2; i++) {
    if (rarity_buf.random() <= 0.95) continue;
    switch (rare_buf.rand_item<Rare>()) {
      case Rare::Blueprint:
      case Rare::Brainstorm:
        goto has_copy;
      default:
        break;
    }
  }
  return;
has_copy:

  bool has_dusk = false;
  bool has_sock = false;
  bool has_seltzer = false;

  RandGen shop_item = seed.init_rand("cdt1");
  RandGen shop_rarity = seed.init_rand("rarity1sho");
  RandGen shop_uncommon = seed.init_rand("Joker2sho1");
  for (int i = 0; i < 4; i++) {
    if (shop_item.random() * 28 > 20) continue;
    if (rarity_from_rand(shop_rarity.random()) != Rarity::Uncommon) continue;
    switch (shop_uncommon.rand_item<Uncommon>()) {
      case Uncommon::Dusk:
        has_dusk = true;
        break;
      case Uncommon::Sock_and_Buskin:
        has_sock = true;
        break;
      case Uncommon::Seltzer:
        has_seltzer = true;
        break;
      default:
        break;
    }
  }

  if (has_dusk + has_sock + has_seltzer < 2) return;

  std::cout << seed.seed << std::endl;
}
