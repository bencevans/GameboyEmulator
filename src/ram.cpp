#include "ram.h"

void RAM::initialise() {
    // Initialise memory to 0.
    // @TODO This is NOT what is done on the real console -
    // internal and high RAM should be left as random values.
    for (int me = 0; me < 0x10000; me ++)
        memory[me] = 0;
}