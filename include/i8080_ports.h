#ifndef PORTS_H
#define PORTS_H

#include <stdint.h>
#include "arcade.h"

// i8080 Port API
void set_arcade_system(arcade_system *system);
uint8_t read_port(uint8_t port_number);
void write_port(uint8_t port_number, uint8_t port_data);

#endif