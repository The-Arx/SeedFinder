#include <cstdio>
#include <cuda_runtime_api.h>
#include <device_launch_parameters.h>

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
      // if (shop_item.random() * 30 > 20) return; // ghost deck
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
    // if (shop_item.random() * 30 > 20) return; // ghost deck
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

__global__ void search_seeds() {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int totalThreads = gridDim.x * blockDim.x;
    long total = NUM_SEEDS;
    long start_seed = total * tid / totalThreads;
    long end_seed = total * (tid + 1) / totalThreads;
    Seed seed(start_seed);
    for (long i = start_seed; i < end_seed; i++) {
        if (erratic_regular(seed)) {
            printf("%s\n", seed.seed);
        }
        seed.next();
    }
}

int main() {
    search_seeds<<<1024,256>>>();

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
