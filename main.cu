#include <cstdio>
#include <cuda_runtime.h>

#include "items.h"
#include "seed.cu"

__device__ bool find_negatives(const Seed &seed) {
    RandGen shop_edition = seed.init_rand(7, "edisho1");
    if (shop_edition.random() <= 0.997) return false;
    if (shop_edition.random() <= 0.997) return false;
    if (shop_edition.random() <= 0.997) return false;
    if (shop_edition.random() <= 0.997) return false;
    return true;
}

__device__ bool negative_bluestorm(const Seed &seed) {
    RandGen shop_edition = seed.init_rand(7, "edisho1");
    if (shop_edition.random() <= 0.997) return false;
    if (shop_edition.random() <= 0.997) return false;
    RandGen shop_rarity = seed.init_rand(10, "rarity1sho");
    if (shop_rarity.random() <= 0.95) return false;
    if (shop_rarity.random() <= 0.95) return false;
    RandGen shop_rare = seed.init_rand(10, "Joker3sho1");
    if (shop_rare.rand_item<Rare>() != Rare::Blueprint) return false;
    if (shop_rare.rand_item<Rare>() != Rare::Brainstorm) return false;
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
        if (negative_bluestorm(seed)) {
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
