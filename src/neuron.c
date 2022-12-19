#include "include/neuron.h"

mvector* mneuron_create_network(u_int32_t m, u_int32_t n, u_int32_t d)
{
    u_int32_t weights = 0;
    u_int32_t biases = 0;
    for (u_int32_t l = 0; l < d-1; l++)
    {
        biases += l;
        weights += mlerp(m, n, l) * mlerp(m, n, l+1);
    }

    return mvector_create(weights+biases, sizeof(float));
}

mvector* mneuron_prop(mvector* network, mvector* inputs,
        u_int32_t m, u_int32_t n, u_int32_t d)
{
    mvector* xs = mvector_create(mtrap(m,n,d), sizeof(float));

    mvector_copy(xs, inputs, 0, 0, mcount(inputs));

    u_int32_t layer_i = 0;
    u_int32_t x_i = 0;

    for (u_int32_t l = 0; l < d-1; l++)
    {
        u_int32_t h0 = mlerp(m, n, l);
        u_int32_t h1 = mlerp(m, n, l+1);

        u_int32_t neuron_i = layer_i;

        for (u_int32_t i = 0; i < h1; i++)
        {
            float b = *mvector_at(float, network, neuron_i);
            float w_sum = 0;
            for (u_int32_t j = 0; j < h0; j++)
            {
                w_sum += *mvector_at(float, network, neuron_i+j+1) *
                    *mvector_at(float, xs, x_i+j);
            }

            neuron_i += h0 + 1;


        }

        x_i += h0;
        layer_i += l + h0*h1;
    }


    return mvector_sub(xs, x_i, n);
}

void mneuron_backprop(mvector* network,
        mvector* inputs, mvector* outputs,
        u_int32_t m, u_int32_t n, u_int32_t d)
{
    
}

