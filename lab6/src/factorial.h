#ifndef FACTORIAL_H
#define FACTORIAL_H

#include "arpa/inet.h"

struct FactorialArgs {
    uint64_t begin;
    uint64_t end;
    uint64_t mod;
    uint64_t socket;
};

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod);

#endif