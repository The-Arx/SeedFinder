#pragma once
#include <numeric>
#include <string>

#include "items.h"

bool ante1_tag(Tag tag) {
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

std::string edition_str(double edition) {
  if (edition > 0.997) return "negative";
  if (edition > 0.994) return "polychrome";
  if (edition > 0.98) return "holographic";
  if (edition > 0.94) return "foil";
  return "none";
}

constexpr std::array<double, static_cast<int>(Pack::Count)> PACK_WEIGHTS{
    4, 2, 0.5, 4, 2, 0.5, 0.6, 0.3, 0.07, 4, 2, 0.5, 1.2, 0.6, 0.15,
};
constexpr double PACK_WEIGHT_TOTAL =
    std::accumulate(PACK_WEIGHTS.begin(), PACK_WEIGHTS.end(), 0.0);

Pack pack_from_rand(double rand) {
  double total = 0;
  for (unsigned int i = 0; i < PACK_WEIGHTS.size(); i++) {
    total += PACK_WEIGHTS[i];
    if (total >= rand) {
      return static_cast<Pack>(i);
    }
  }
  return static_cast<Pack>(PACK_WEIGHTS.size() - 1);
}

enum class PackType {
  Arcana,
  Celestial,
  Spectral,
  Standard,
  Buffoon,
};

PackType pack_type(Pack pack) {
  return static_cast<PackType>(static_cast<int>(pack) / 3);
}

int pack_size(Pack pack) {
  switch (static_cast<int>(pack) % 3) {
    case 0:
      return 3;
    default:
      return 5;
  }
}

int pack_choices(Pack pack) {
  switch (static_cast<int>(pack) % 3) {
    case 2:
      return 2;
    default:
      return 1;
  }
}
