#include <cstdio>
#include <cuda_runtime.h>

#include "items.h"
#include "seed.cu.cc"
#include "util.cu.cc"

__device__ bool find_negatives(const Seed &seed) {
    RandGen shop_edition = seed.init_rand("edisho1");
    if (shop_edition.random() <= 0.997) return false;
    if (shop_edition.random() <= 0.997) return false;
    if (shop_edition.random() <= 0.997) return false;
    // if (shop_edition.random() <= 0.997) return false;
    return true;
}

__device__ bool negative_bluestorm(const Seed &seed) {
    RandGen shop_edition = seed.init_rand("edisho1");
    if (shop_edition.random() <= 0.997) return false;
    if (shop_edition.random() <= 0.997) return false;
    RandGen shop_rarity = seed.init_rand("rarity1sho");
    if (shop_rarity.random() <= 0.95) return false;
    if (shop_rarity.random() <= 0.95) return false;
    RandGen shop_rare = seed.init_rand("Joker3sho1");
    if (shop_rare.rand_item<Rare>() != Rare::Blueprint) return false;
    if (shop_rare.rand_item<Rare>() != Rare::Brainstorm) return false;
    return true;
}

__device__ bool ante1_cavendish(const Seed &seed) {
    RandGen cavendish = seed.init_rand(9, "cavendish");
    if (cavendish.random() >= 1.0 / 1000) return false;
    if (cavendish.random() >= 1.0 / 1000) return false;
    RandGen gros_michel = seed.init_rand(11, "gros_michel");
    if (gros_michel.random() >= 1.0 / 6) return false;

    RandGen shop_item = seed.init_rand("cdt1");
    RandGen shop_rarity = seed.init_rand("rarity1sho");
    RandGen shop_common = seed.init_rand("Joker1sho1");

    bool has_michel = false;
    for (int i = 0; i < 2; i++) {
      if (shop_item.random() * 28 > 20) continue;
      if (shop_rarity.random() > 0.7) continue;
      if (shop_common.rand_item<Common>() != Common::Gros_Michel) continue;
      has_michel = true;
    }
    if (!has_michel) return false;

    bool has_cavendish = false;
    for (int i = 0; i < 2; i++) {
      if (shop_item.random() * 28 > 20) continue;
      if (shop_rarity.random() > 0.7) continue;
      if (shop_common.rand_item<Common>() != Common::Cavendish) continue;
      has_cavendish = true;
    }
    if (!has_cavendish) return false;

    shop_item = seed.init_rand("cdt2");
    shop_rarity = seed.init_rand("rarity2sho");
    shop_common = seed.init_rand("Joker1sho2");
    for (int i = 0; i < 2; i++) {
      if (shop_item.random() * 28 > 20) continue;
      if (shop_rarity.random() > 0.7) continue;
      if (shop_common.rand_item<Common>() != Common::Cavendish) continue;
      return true;
    }

    return false;
}

__device__ bool trib_dna_poly(const Seed &seed) {
  RandGen soul_rand = seed.init_rand("soul_Tarot1");
  for (int i = 0; i < 5; i++) {
    if (soul_rand.random() > 0.997) {
      goto has_soul;
    }
  }
  return false;
has_soul:

  RandGen pack_rand = seed.init_rand("shop_pack1");
  if (pack_from_rand(pack_rand.random()) != Pack::Standard_Normal) return false;
  RandGen legendary = seed.init_rand("Joker4");
  if (legendary.rand_item<Legendary>() != Legendary::Triboulet) return false;
  
  RandGen rarity_buf = seed.init_rand("rarity1buf");
  RandGen rare_buf = seed.init_rand("Joker3buf1");
  for (int i = 0; i < 2; i++) {
    if (rarity_buf.random() <= 0.95) continue;
    if (rare_buf.rand_item<Rare>() != Rare::DNA) continue;
    goto has_dna;
  }
  return false;
has_dna:

  RandGen tag_rand = seed.init_rand("Tag1");
  Tag tag = tag_rand.rand_item<Tag>();
  for (int i = 2; !ante1_tag(tag); i++) {
    tag = seed.init_rand("Tag1_resample", i).rand_item<Tag>();
  }
  if (tag != Tag::Charm) return false;

  RandGen card_rand = seed.init_rand("frontsta1");
  RandGen edition_rand = seed.init_rand("standard_edition1");
  RandGen has_enhancment_rand = seed.init_rand("stdset1");
  RandGen enhancment_rand = seed.init_rand("Enhancedsta1");
  RandGen has_seal_rand = seed.init_rand("stdseal1");
  RandGen seal_type_rand = seed.init_rand("stdsealtype1");
  for (int i = 0; i < 3; i++) {
    Card card = card_rand.rand_item<Card>();
    bool polychrome = edition_rand.random() > 1 - 0.006 * 2;
    bool red_seal =
        has_seal_rand.random() > 1 - 0.02 * 10 && seal_type_rand.random() > 0.75;
    Enhancement enhancment = has_enhancment_rand.random() > 0.6
                                 ? enhancment_rand.rand_item<Enhancement>()
                                 : Enhancement::None;
    if (card_rank(card) == Rank::King  && polychrome && red_seal &&
        enhancment == Enhancement::Lucky) {
      return true;
    }
  }

  return false;
}

__device__ bool quad_soul(const Seed &seed) {
  RandGen soul_rand = seed.init_rand("soul_Tarot1");
  int non_soul = 0;
  for (int i = 0; i < 5; i++) {
    if (soul_rand.random() <= 0.997 && ++non_soul > 1) {
      return false;
    }
  }
  
  RandGen tag_rand = seed.init_rand("Tag1");
  Tag first = tag_rand.rand_item<Tag>();
  Tag second = tag_rand.rand_item<Tag>();
  for (int i = 2; !ante1_tag(second); i++) {
    RandGen tag_resample = seed.init_rand("Tag1_resample", i);
    if (!ante1_tag(first)) first = tag_resample.rand_item<Tag>();
    second = tag_resample.rand_item<Tag>();
  }
  if (second != Tag::Charm) return false;


  RandGen shop_item = seed.init_rand("cdt1");
  RandGen shop_rarity = seed.init_rand("rarity1sho");
  RandGen shop_uncommon = seed.init_rand("Joker2sho1");
  for (int i = 0; i < 2; i++) {
    if (shop_item.random() * 28 > 20) continue;
    if (rarity_from_rand(shop_rarity.random()) != Rarity::Uncommon) continue;
    if (shop_uncommon.rand_item<Uncommon>() != Uncommon::Showman) continue;
    return true;
  }

  RandGen buf_rarity = seed.init_rand("rarity1buf");
  RandGen buf_uncommon = seed.init_rand("Joker2buf1");
  for (int i = 0; i < 2; i++) {
    if (rarity_from_rand(buf_rarity.random()) != Rarity::Uncommon) continue;
    if (buf_uncommon.rand_item<Uncommon>() != Uncommon::Showman) continue;
    return true;
  }

  RandGen pack_rand = seed.init_rand("shop_pack1");
  Pack pack = pack_from_rand(pack_rand.random());
  if (pack_type(pack) == PackType::Buffoon) {
    int size = pack_size(pack);
    for (int i = 0; i < size; i++) {
      if (rarity_from_rand(buf_rarity.random()) != Rarity::Uncommon) continue;
      if (buf_uncommon.rand_item<Uncommon>() != Uncommon::Showman) continue;
      return true;
    }
  }

  return false;
}

__device__ bool erratic_regular(const Seed &seed) {
  RandGen card_rand = seed.init_rand("erratic");
  char cards[static_cast<int>(Rank::Count)] = {0};
  for (int i = 0; i < 52; i++) {
    Card card = card_rand.rand_item<Card>();
    if (++cards[static_cast<int>(card_rank(card))] > 4) {
      return false;
    }
  }
  return true;
}

__device__ bool round2_score(const Seed &seed) {
  RandGen pack_rand = seed.init_rand("shop_pack1");
  if (pack_from_rand(pack_rand.random()) != Pack::Spectral_Mega) return false;
  
  RandGen soul_rand = seed.init_rand("soul_Spectral1");
  for (int i = 0; i < 4; i++) {
    if (soul_rand.random() > 0.997) {
      goto has_soul;
    }
    soul_rand.skip();
  }
  return false;
has_soul:

  RandGen legendary = seed.init_rand("Joker4");
  if (legendary.rand_item<Legendary>() != Legendary::Triboulet) return false;

  RandGen arcana_tarot = seed.init_rand("Spectralspe1");
  for (int i = 0; i < 3; i++) {
    if (arcana_tarot.rand_item<Spectral>() == Spectral::Immolate) {
      goto has_immolate;
    }
  }
  return false;
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
  return false;
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

  return has_dusk + has_sock + has_seltzer >= 2;
}

template<typename K, typename V>
class ItemMap {
  public:
  V jokers[static_cast<int>(K::Count)];
  __device__ ItemMap(): jokers{} {}
  __device__ V& operator[](K item) {
      return jokers[static_cast<size_t>(item)];
  }
};

template<typename T>
requires std::is_enum_v<T> && (static_cast<size_t>(T::Count) <= 64)
class ItemBitset {
  // static_assert(static_cast<size_t>(T::Count) <= 64, "ItemBitset only supports enums with up to 64 items");
  using Storage = std::conditional_t<static_cast<size_t>(T::Count) <= 32, uint32_t, uint64_t>;
  Storage bits = 0;

  public:
  __device__ bool has(T item) {
    return bits & (Storage{1} << static_cast<size_t>(item));
  }
  __device__ bool pop(T item) {
    Storage bit = Storage{1} << static_cast<size_t>(item);
    if (bits & bit) {
      bits &= ~bit;
      return true;
    }
    return false;
  }
  __device__ void add(T item) {
    bits |= Storage{1} << static_cast<size_t>(item);
  }
  __device__ void remove(T item) {
    bits &= ~(Storage{1} << static_cast<size_t>(item));
  }
  __device__ void clear() {
    bits = 0;
  }
};


__device__ bool quad_jokers(const Seed &seed) {
  RandGen pack_rand = seed.init_rand("shop_pack1");
  if (pack_from_rand(pack_rand.random()) != Pack::Buffoon_Jumbo) return false;

  RandGen buf_rarity = seed.init_rand("rarity1buf");
  for (int i = 0; i < 4; i++) {
    if (rarity_from_rand(buf_rarity.random()) != Rarity::Uncommon) return false;
  }

  ItemMap<Uncommon, bool> jokers;
  jokers[Uncommon::Glass_Joker] = true;
  jokers[Uncommon::Stone_Joker] = true;
  jokers[Uncommon::Steel_Joker] = true;
  jokers[Uncommon::Lucky_Cat] = true;

  RandGen buf_uncommon = seed.init_rand("Joker2buf1");
  for (int i = 0; i < 4; i++) {
    jokers[buf_uncommon.rand_item<Uncommon>()] = true;
  }

  return jokers[Uncommon::Arrowhead] && jokers[Uncommon::Bloodstone] && jokers[Uncommon::Onyx_Agate] && jokers[Uncommon::Rough_Gem];
}

__device__ bool double_trib(const Seed &seed) {
  RandGen pack_rand = seed.init_rand("shop_pack1");
  if (pack_from_rand(pack_rand.random()) != Pack::Spectral_Mega) return false;
  RandGen soul_rand = seed.init_rand("soul_Spectral1");
  for (int i = 0; i < 4; i++) {
    if (soul_rand.random() > 0.997) {
      goto has_soul;
    }
    soul_rand.skip();
  }
  return false;
has_soul:

  RandGen legendary = seed.init_rand("Joker4");
  if (legendary.rand_item<Legendary>() != Legendary::Triboulet) return false;

  RandGen arcana_tarot = seed.init_rand("Spectralspe1");
  for (int i = 0; i < 3; i++) {
    if (arcana_tarot.rand_item<Spectral>() == Spectral::Ankh) {
      goto has_ankh;
    }
  }
  return false;
has_ankh:
  return true;
}

__device__ bool double_charm(const Seed &seed) {
  RandGen tag_gen = seed.init_rand("Tag1");
  int resample = 1;
  int remaining = 2;
  while (true) {
    for (int i = remaining; i > 0; i--) {
      Tag tag = tag_gen.rand_item<Tag>();
      if (tag == Tag::Charm) {
        if (--remaining == 0) return true;
      } else if (ante1_tag(tag)) {
        return false;
      }
    }
    tag_gen = seed.init_rand("Tag1_resample", ++resample);
  }
}

template<typename T, size_t N>
__device__ void gen_jokers(const Seed &seed, const char (&key)[N], ItemBitset<T> &bitset, int count) {
  RandGen joker_rand = seed.init_rand(key);
  int resample = 1;
  while (true) {
    int num_reroll = 0;
    for (int i = 0; i < count; i++) {
      T joker = joker_rand.rand_item<T>();
      if (bitset.has(joker)) {
        num_reroll++;
      } else {
        bitset.add(joker);
      }
    }
    if (num_reroll == 0) break;
    count = num_reroll;
    joker_rand = seed.init_rand(key, ++resample);
  }
}

__device__ bool round1_score(const Seed &seed) {
  RandGen legendary_gen = seed.init_rand("Joker4");
  if (legendary_gen.rand_item<Legendary>() != Legendary::Triboulet) return false;
  
  if (!double_charm(seed)) return false;

  // ignoring both packs having soul because its both very rare and a downside
  bool first_pack;
  RandGen soul_rand = seed.init_rand("soul_Tarot1");
  for (int i = 0; i < 10; i++) {
    if (soul_rand.random() > 0.997) {
      first_pack = i < 5;
      goto has_soul;
    }
  }
  return false;
has_soul:

  RandGen tarot_gen = seed.init_rand("Tarotar11");
  ItemBitset<Tarot> packs[2];
  int tarots = 9;
  int pack1items = 5 - first_pack;
  int resample = 1;
  while (true) {
    int pack = 0;
    int num_rerolls[2] = {};
    for (int i = 0; i < tarots; i++) {
      if (i == pack1items) pack = 1;
      Tarot tarot = tarot_gen.rand_item<Tarot>();
      if (packs[pack].has(tarot)) {
        num_rerolls[pack]++;
      } else {
        packs[pack].add(tarot);
      }
    }
    tarots = num_rerolls[0] + num_rerolls[1];
    if (tarots == 0) break;
    pack1items = num_rerolls[0];
    tarot_gen = seed.init_rand("Tarotar11_resample", ++resample);
  }
  int num_judgment = 0;
  int choice1 = 2 - first_pack;
  if (packs[0].pop(Tarot::Judgement)) {
    num_judgment++; choice1--;
    if (choice1 > 0 && packs[0].pop(Tarot::The_Fool)) { num_judgment++; choice1--; }
  }
  int choice2 = 1 + first_pack;
  if (packs[1].pop(Tarot::Judgement)) { num_judgment++; choice2--; }
  if (choice2 > 0 && num_judgment > 0 && packs[1].pop(Tarot::The_Fool)) { num_judgment++; choice2--; }
  bool emp1 = choice1 > 0 && packs[0].has(Tarot::The_Emperor);
  bool emp2 = choice2 > 0 && packs[1].has(Tarot::The_Emperor);
  if (emp1 || emp2) {
    int emp_count = (emp1 + emp2) * 2;
    bool pack1_fool = packs[0].pop(Tarot::The_Fool);
    bool pack2_fool = packs[1].pop(Tarot::The_Fool);
    int emp1count = emp1 * 2;
    RandGen emp_gen = seed.init_rand("Tarotemp1");
    while (true) {
      int emp = 0;
      int num_rerolls[2] = {};
      for (int i = 0; i < emp_count; i++) {
        if (i == emp1count) emp = 1;
        Tarot tarot = emp_gen.rand_item<Tarot>();
        if (packs[emp].has(tarot)) {
          num_rerolls[emp]++;
        } else {
          packs[emp].add(tarot);
        }
      }
      emp_count = num_rerolls[0] + num_rerolls[1];
      if (emp_count == 0) break;
      emp1count = num_rerolls[0];
      emp_gen = seed.init_rand("Tarotemp1_resample", ++resample);
    }
    if (emp1) {
      if (packs[0].has(Tarot::Judgement)) {
        num_judgment++;
        if (pack1_fool) num_judgment++;
        if (pack2_fool) {
          pack2_fool = false;
          num_judgment++;
        }
      }
      if (!pack1_fool && num_judgment && packs[0].has(Tarot::The_Fool)) {
        num_judgment++;
      }
    }
    if (emp2) {
      if (packs[1].has(Tarot::Judgement)) {
        num_judgment++;
        if (pack2_fool) num_judgment++;
      }
      if (!pack2_fool && num_judgment && packs[1].has(Tarot::The_Fool)) {
        num_judgment++;
      }
    }
  }

  RandGen rarity_rand = seed.init_rand("rarity1jud");
  int uncommon_count = 0;
  int rare_count = 0;
  for (int i = 0; i < num_judgment; i++) {
    double rarity = rarity_rand.random();
    if (rarity <= 0.7) continue;
    if (rarity <= 0.95) {
      uncommon_count++;
    } else {
      rare_count++;
    }
  }

  ItemBitset<Uncommon> uncommons;
  // requirements not met
  uncommons.add(Uncommon::Glass_Joker);
  uncommons.add(Uncommon::Stone_Joker);
  uncommons.add(Uncommon::Steel_Joker);
  uncommons.add(Uncommon::Lucky_Cat);
  gen_jokers<Uncommon>(seed, "Joker2jud1", uncommons, uncommon_count);
  int retrigger_jokers = uncommons.has(Uncommon::Dusk)
                       + uncommons.has(Uncommon::Sock_and_Buskin)
                       + uncommons.has(Uncommon::Seltzer);

  ItemBitset<Rare> rares;
  gen_jokers<Rare>(seed, "Joker3jud1", rares, rare_count);
  int copy_jokers = rares.has(Rare::Blueprint) + rares.has(Rare::Brainstorm);

  return retrigger_jokers >= 1 && copy_jokers >= 1 && retrigger_jokers + copy_jokers >= 3 ;
}

__device__ bool ante1_showman(const Seed &seed) {
  RandGen shop_item = seed.init_rand("cdt1");
  RandGen shop_rarity = seed.init_rand("rarity1sho");
  RandGen shop_uncommon = seed.init_rand("Joker2sho1");
  for (int i = 0; i < 12; i++) {
    if (shop_item.random() * 28 > 20) continue;
    if (rarity_from_rand(shop_rarity.random()) != Rarity::Uncommon) continue;
    if (shop_uncommon.rand_item<Uncommon>() != Uncommon::Showman) continue;
    return true;
  }

  int buffoon_slots = 2;
  RandGen pack_rand = seed.init_rand("shop_pack1");
  for (int i = 0; i < 5; i++) {
    Pack pack = pack_from_rand(pack_rand.random());
    if (pack_type(pack) != PackType::Buffoon) continue;
    buffoon_slots += pack_size(pack);
  }

  RandGen buf_rarity = seed.init_rand("rarity1buf");
  RandGen buf_uncommon = seed.init_rand("Joker2buf1");
  for (int i = 0; i < buffoon_slots; i++) {
    if (rarity_from_rand(buf_rarity.random()) != Rarity::Uncommon) continue;
    if (buf_uncommon.rand_item<Uncommon>() != Uncommon::Showman) continue;
    return true;
  }
  return false;
}

__device__ bool quad_showman(const Seed &seed) {
  uint32_t first_small = 0xffffffff;
  int last_big = -1;
  int slots = 0;
  int last_big_i = -1;
  RandGen pack_rand = seed.init_rand("shop_pack2");
  for (int i = 0; i < 6; i++) {
    Pack pack = pack_from_rand(pack_rand.random());
    if (pack_type(pack) != PackType::Buffoon) continue;
    if (pack == Pack::Buffoon_Normal) {
      if (first_small == 0xffffffff) {
        first_small = slots;
        if (last_big_i + 1 == i && i % 2 == 1) {
          first_small -= 4;
        }
      }
      slots += 2;
    } else {
      last_big = slots;
      last_big_i = i;
      slots += 4;
    }
  }
  if (last_big < 0) return false;
  int streak = 0;
  bool has_showman = false;
  RandGen buf_rarity = seed.init_rand("rarity2buf");
  RandGen buf_uncommon = seed.init_rand("Joker2buf2");
  for (int i = 0; i < last_big || streak > 0; i++) {
    if (rarity_from_rand(buf_rarity.random()) == Rarity::Uncommon
        && buf_uncommon.rand_item<Uncommon>() == Uncommon::Showman
        && (streak > 0 || i % 4 == 0 || (i % 2 == 0 && first_small < i))) {
      if (++streak == 4) return has_showman || ante1_showman(seed);
    } else if (streak > 0) {
      has_showman = true;
      streak = 0;
    }
  }
  return false;
}

__device__ bool quad_joker_ante1(const Seed &seed) {
  RandGen pack_rand = seed.init_rand("shop_pack1");
  int last_big = -1;
  int slots = 2;
  for (int i = 0; i < 3; i++) {
    Pack pack = pack_from_rand(pack_rand.random());
    if (pack_type(pack) != PackType::Buffoon) continue;
    if (pack == Pack::Buffoon_Normal) {
      slots += 2;
    } else {
      last_big = slots;
      slots += 4;
    }
  }
  if (last_big < 0) return false;
  int streak = 0;
  bool has_showman = false;
  RandGen buf_rarity = seed.init_rand("rarity1buf");
  RandGen buf_uncommon = seed.init_rand("Joker2buf1");
  for (int i = 0; i < last_big || streak > 0; i++) {
    if (rarity_from_rand(buf_rarity.random()) == Rarity::Uncommon
        && buf_uncommon.rand_item<Uncommon>() == Uncommon::Showman
        && (streak > 0 || i % 2 == 0)) {
      if (++streak == 3) goto find_showman;
    } else if (streak > 0) {
      has_showman = true;
      streak = 0;
    }
  }
  return false;
  find_showman:
  if (has_showman) return true;
  RandGen shop_item = seed.init_rand("cdt1");
  RandGen shop_rarity = seed.init_rand("rarity1sho");
  RandGen shop_uncommon = seed.init_rand("Joker2sho1");
  for (int i = 0; i < 6; i++) {
    if (shop_item.random() * 28 > 20) continue;
    if (rarity_from_rand(shop_rarity.random()) != Rarity::Uncommon) continue;
    if (shop_uncommon.rand_item<Uncommon>() != Uncommon::Showman) continue;
    return true;
  }
  return false;
}

__device__ bool negative_skip(const Seed &seed) {
  int misses = 5;
  int doubles = 0;
  bool starts_with_neg = false;
  bool starts_with_neg_set = false;
  for (int i = 2; i <= 8; i++) {
    RandGen tag_gen = seed.init_rand("Tag", i);
    for (int j = i == 8; j < 2; j++) {
      Tag tag = tag_gen.rand_item<Tag>();
      doubles++;
      if (tag != Tag::Double) {
        if (tag != Tag::Negative) {
          misses -= doubles;
          if (!starts_with_neg) {
            starts_with_neg_set = true;
            starts_with_neg = false;
            misses -= 2;
          }
          if (misses < 0) return false;
        } else if (!starts_with_neg_set) {
          starts_with_neg_set = true;
          starts_with_neg = true;
        }
        doubles = 0;
      }
    }
  }
  misses -= doubles;
  if (misses < 0) return false;
  if (!starts_with_neg || misses >= 2) return true;
  RandGen tag_gen = seed.init_rand("Tag1");
  int resample = 1;
  int remaining = 2;
  int tags[2] = {};
  while (true) {
    int tag_index = tags[0] != 0; // 1 if first is found already else 0
    for (int i = remaining; i > 0; i--) {
      Tag tag = tag_gen.rand_item<Tag>();
      if (tag == Tag::Double) {
        tags[tag_index] = 1;
        if(!--remaining) goto end;
      } else if (ante1_tag(tag)) {
        tags[tag_index] = -1;
        if(!--remaining) goto end;
      }
      tag_index++;
    }
    tag_gen = seed.init_rand("Tag1_resample", ++resample);
  }
end:
  return tags[1] == 1 && (tags[0] == 1 || misses == 1);
}

__device__ int most_souls(const Seed &seed) {
  // ignores spectral packs
  int souls = 0;
  for (int ante = 1; ante <= 8; ante++) {
    RandGen pack_rand = seed.init_rand("shop_pack", ante);
    RandGen soul_rand = seed.init_rand("soul_Tarot", ante);
    for (int i = ante == 1 ? 3 : 0; i < 6; i++) {
      Pack pack = pack_from_rand(pack_rand.random());
      if (pack_type(pack) != PackType::Arcana) continue;
      for (int j = pack_size(pack); j > 0; j--) {
        if (soul_rand.random() > 0.997) {
          souls++;
          break;
        }
      }
    }
  }
  return souls;
}

__device__ bool ante1_souls(const Seed &seed) {
  Pack packs[3];
  for (int ante = 1; ante <= 2; ante++) {
    bool has_arcana = false;
    bool has_spectral = false;
    RandGen pack_rand = seed.init_rand("shop_pack", ante);
    for (int i = ante - 1; i < 3; i++) {
      Pack pack = pack_from_rand(pack_rand.random());
      switch(pack_type(pack)) {
        case PackType::Arcana:
          has_arcana = true;
          break;
        case PackType::Spectral:
          has_spectral = true;
          break;
        default:
          return false;
      }
      packs[i] = pack;
    }

    if (has_arcana) {
      RandGen soul_rand = seed.init_rand("soul_Tarot", ante);
      for (int i = ante - 1; i < 3; i++) {
        Pack pack = packs[i];
        if (pack_type(pack) != PackType::Arcana) continue;
        int j = pack_size(pack);
        while (true) {
          if (soul_rand.random() > 0.997) break;
          j--;
          if (j == 0) return false;
        }
      }
    }

    // todo: fix black hole
    if (has_spectral) {
      RandGen soul_rand = seed.init_rand("soul_Spectral", ante);
      for (int i = ante - 1; i < 3; i++) {
        Pack pack = packs[i];
        if (pack_type(pack) != PackType::Spectral) continue;
        int j = pack_size(pack);
        while (true) {
          if (soul_rand.random() > 0.997) break;
          j--;
          if (j == 0) return false;
        }
      }
    }
  }

  return true;
}

// __constant__ constexpr uint8_t initial_deck[52] = {
//   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
//   19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 
//   36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
// };
__device__ bool bad_shuffle(const Seed &seed) {
  PRNG shuffle = seed.init_rand("nr1").prng();
  uint8_t deck[52] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 
    34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };
  int suits[4] = {};
  int good_suits[4] = {};
  bool bad_suits[4] = {};
  int good_suit = -1;
  int cards_left = 52;
  while (cards_left > 52 - 8) {
    int idx = shuffle.rand_int(cards_left);
    int card = deck[idx];
    deck[idx] = deck[--cards_left];
    int suit = card / 13;
    suits[suit] += 1;
    int rank = card % 13;
    if (rank > 8) {
      if (++good_suits[suit] == 4) {
        good_suit = suit;
      }
    }
    if (rank == static_cast<int>(Rank::Ace) || rank == static_cast<int>(Rank::Nine)) {
      bad_suits[suit] = true;
    }
  }
  if (good_suit == -1 || suits[good_suit] > 4) return false;
  for (int i = 0; i < 4; i++) {
    if (i == good_suit) continue; 
    if (suits[i] >= 4 || bad_suits[i]) return false;
  }
  while (cards_left > 52 - 8 - 20) {
    int idx = shuffle.rand_int(cards_left);
    int card = deck[idx];
    deck[idx] = deck[--cards_left];
    if (card / 13 == good_suit) return false;
    int rank = card % 13;
    if (rank == static_cast<int>(Rank::Ace) || rank == static_cast<int>(Rank::Nine)) return false;
  }

  return true;
}

__global__ void search_seeds() {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int totalThreads = gridDim.x * blockDim.x;
    long start = 0;
    long total = NUM_SEEDS;
    long start_seed = total * tid / totalThreads + start;
    long end_seed = total * (tid + 1) / totalThreads + start;
    Seed seed(start_seed);
    for (long i = start_seed; i < end_seed; i++) {
      if (bad_shuffle(seed)) {
        printf("%s\n", seed.seed);
      }
      seed.next();
    }
}

int main() {
    search_seeds<<<256,256>>>();

    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) {
        printf("Launch error: %s\n", cudaGetErrorString(err));
    }

    err = cudaDeviceSynchronize();
    if (err != cudaSuccess) {
        printf("Sync error: %s\n", cudaGetErrorString(err));
    }

    return 0;
}
