#ifndef SIMULATION_H
#define SIMULATION_H

#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/details.h"

// Part 2-C Simulations with gamma parametrs
void run_part2c_static_simulation(Battleship b, EscortShip escorts[], int n_escorts,double tb_q, double te_p[], double gamma_b, double gamma_e[]);

void run_part2c_moving_simulation(Battleship b, EscortShip escorts[], int n_escorts,Point path[], int steps, double tb_q, double te_p[],double gamma_b, double gamma_e[]);

void run_part2c_jammed_simulation(Battleship b, EscortShip escorts[], int n_escorts,Point path[], int steps, int jam_step, double jam_min_angle,double tb_q, double te_p[], double gamma_b, double gamma_e[]);

#endif
