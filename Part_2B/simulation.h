#ifndef SIMULATION_H
#define SIMULATION_H

#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/details.h"

// Part 2-B Siulations
void run_part2b_static_simulation(Battleship b, EscortShip escorts[], int n_escorts, double tb_q, double te_p[]);
void run_part2b_moving_simulation(Battleship b, EscortShip escorts[], int n_escorts, Point path[], int steps, double tb_q, double te_p[]);
void run_part2b_jammed_simulation(Battleship b, EscortShip escorts[], int n_escorts, Point path[], int steps, int jam_step, double jam_min_angle, double tb_q, double te_p[]);

#endif
