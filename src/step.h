#pragma once


#include <stdint.h>


void step_init();

int32_t step_get_count(unsigned i);
float step_get_velocity(unsigned i);

void step_reset();
void step_callback();
