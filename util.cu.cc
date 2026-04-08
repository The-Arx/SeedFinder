#pragma once
#include <numeric>
#include <array>

#include "cuda.h"
#include "items.h"

__device__ bool ante1_tag(Tag tag) {
  switch (tag) {
    case Tag::Buffoon:
    case Tag::Ethereal:
    case Tag::Garbage:
    case Tag::Handy:
    case Tag::Meteor:
    case Tag::Negative:
    case Tag::Orbital:
    case Tag::Standard:
    case Tag::Top_up:
      return false;
    default:
      return true;
  }
}

__device__ Edition joker_edition_from_rand(double rand) {
  if (rand <= 0.96) return Edition::None;
  if (rand <= 0.98) return Edition::Foil;
  if (rand <= 0.994) return Edition::Holographic;
  if (rand <= 0.997) return Edition::Polychrome;
  return Edition::Negative;
}

__device__ Edition card_edition_from_rand(double rand) {
  if (rand <= 0.92) return Edition::None;
  if (rand <= 0.94) return Edition::Foil;
  if (rand <= 0.988) return Edition::Holographic;
  return Edition::Polychrome;
}

__device__ Rarity rarity_from_rand(double rand) {
  if (rand <= 0.7) return Rarity::Common;
  if (rand <= 0.95) return Rarity::Uncommon;
  return Rarity::Rare;
}

constexpr __constant__ double PACK_WEIGHTS[static_cast<int>(Pack::Count)] {
    4, 2, 0.5, 4, 2, 0.5, 4, 2, 0.5, 1.2, 0.6, 0.15, 0.6, 0.3, 0.07,
};
constexpr int PACK_WEIGHTS_LEN = sizeof(PACK_WEIGHTS) / sizeof(PACK_WEIGHTS[0]);
constexpr double PACK_WEIGHT_TOTAL =
    std::accumulate(std::begin(PACK_WEIGHTS), std::end(PACK_WEIGHTS), 0.0);

__device__ Pack pack_from_rand(double rand) {
  rand *= PACK_WEIGHT_TOTAL;
  double total = 0;
  for (unsigned int i = 0; i < PACK_WEIGHTS_LEN; i++) {
    total += PACK_WEIGHTS[i];
    if (total >= rand) {
      return static_cast<Pack>(i);
    }
  }
  return static_cast<Pack>(PACK_WEIGHTS_LEN - 1);
}

__device__ PackType pack_type(Pack pack) {
  return static_cast<PackType>(static_cast<int>(pack) / 3);
}

__device__ int pack_size(Pack pack) {
  bool big_packs = static_cast<int>(pack) < static_cast<int>(Pack::Buffoon_Normal);
  switch (static_cast<int>(pack) % 3) {
    case 0:
      return 2 + big_packs;
    default:
      return 4 + big_packs;
  }
}

__device__ int pack_choices(Pack pack) {
  switch (static_cast<int>(pack) % 3) {
    case 2:
      return 2;
    default:
      return 1;
  }
}

__device__ Rank card_rank(Card card) {
  return static_cast<Rank>(static_cast<int>(card) % static_cast<int>(Rank::Count));
}

__device__ Suit card_suit(Card card) {
  return static_cast<Suit>(static_cast<int>(card) / static_cast<int>(Rank::Count));
}
