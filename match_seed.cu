#include <cstdio>
#include <cuda_runtime.h>

#include "items.h"
#include "seed.cu.cc"
#include "util.cu.cc"

constexpr int HAND_SIZE = 8;

constexpr Card hand[8] = {Card::S_7, Card::C_4, Card::H_8, Card::H_J, Card::D_4, Card::S_9, Card::H_3, Card::C_3};
struct DrawOrder {
    int order[HAND_SIZE];
};
__constant__ constexpr DrawOrder draw_order = [] () {
    DrawOrder draw_order;
    int cards = 52;
    int hand_idx[HAND_SIZE];
    for (int i = 0; i < HAND_SIZE; i++) hand_idx[i] = static_cast<int>(hand[i]);

    for (int i = 0; i < HAND_SIZE; i++) {
        draw_order.order[i] = hand_idx[i];
        cards--;
        for (int j = i + 1; j < HAND_SIZE; j++) {
            if (hand_idx[j] == cards) hand_idx[j] = hand_idx[i];
        }
    }
    return draw_order;
}();

__device__ bool match_seed(const Seed &seed) {
    PRNG shuffle = seed.init_rand("nr1").prng();
    for (int i = 0; i < HAND_SIZE; i++) {
        if (shuffle.rand_int(52 - i) != draw_order.order[i]) return false;
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
        if (match_seed(seed)) {
            printf("%s\n", seed.seed);
        }
        seed.next();
    }
}

int main() {
    search_seeds<<<64,64>>>();

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