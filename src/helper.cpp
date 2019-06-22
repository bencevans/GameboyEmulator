// Copyright (C) Dock Studios Ltd, Inc - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential
// Written by Matt Comben <matthew@dockstudios.co.uk>, May 2019

#ifdef HELPER_INCLUDED
#define HELPER_INCLUDED
#include <iostream>
#include "helper.h"


void Helper::reset_cout() {
     std::cout.flags(Helper::RESET_FLAGS);
}
void Helper::init() {
    Helper::RESET_FLAGS = std::cout.flags();
}
#endif
