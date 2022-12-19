#ifndef NEURON_H
#define NEURON_H

#include "global.h"
#include "vector.h"

mvector* mneuron_create_randomized(u_int32_t m, u_int32_t n, u_int32_t p);

void backprop_group(mvector* network, mvector* batches,
        u_int32_t m, u_int32_t n, u_int32_t p, u_int32_t batch_count);

#endif
