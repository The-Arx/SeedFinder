#pragma once
#include "items.cpp"

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