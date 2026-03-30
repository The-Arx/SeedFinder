#pragma once
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