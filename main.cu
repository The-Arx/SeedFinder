#include <cstdio>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#include "items.h"
#include "seed.cu"

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

__global__ void search_seeds() {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int totalThreads = gridDim.x * blockDim.x;
    long total = NUM_SEEDS;
    long start_seed = total * tid / totalThreads;
    long end_seed = total * (tid + 1) / totalThreads;
    Seed seed(start_seed);
    for (long i = start_seed; i < end_seed; i++) {
        if (find_negatives(seed)) {
            printf("%s\n", seed.seed);
        }
        seed.next();
    }
}

int main() {
    search_seeds<<<160,256>>>();

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
