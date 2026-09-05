#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdio.h>
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/details.h"

// main simulation function
void run_battle_simulation(
    Battleship *b, 
    EscortShip escorts[], 
    int n_escorts, 
    Point path[], 
    int steps, 
    int jam_step, 
    FILE *log
);

// 3 wrapper functions for each simulation
void run_part1a_c(Battleship *b, EscortShip escorts[], int n_escorts, FILE *log);
void run_part1b_sim1_c(Battleship *b, EscortShip escorts[], int n_escorts, Point path[], int steps, FILE *log);
void run_part1b_sim2_c(Battleship *b, EscortShip escorts[], int n_escorts, Point path[], int steps, int jam_step, FILE *log);
int find_elevation_angle(double distance, double velocity, double min_angle, double max_angle, double *angle_out);
#endif
