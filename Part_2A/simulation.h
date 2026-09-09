#ifndef SIMULATION_H
#define SIMULATION_H

#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/details.h"

// Part 2A Simulations
void run_part2a_static_simulation(Battleship b, EscortShip escorts[], int n_escorts, double tb_q);
void run_part2a_moving_simulation(Battleship b, EscortShip escorts[], int n_escorts, Point path[], int steps, double tb_q);
void run_part2a_jammed_simulation(Battleship b, EscortShip escorts[], int n_escorts, Point path[], int steps, int jam_step, double jam_min_angle, double tb_q);

#endif
