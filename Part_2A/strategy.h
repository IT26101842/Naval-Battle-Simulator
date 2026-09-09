#ifndef STRATEGY_H
#define STRATEGY_H

#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/details.h"

// structure for get details of target ship
typedef struct {
    int id;
    double impact_power;
    double distance;
} TargetInfo;

// calculate the angle
int find_elevation_angle(double distance, double velocity, double min_angle, double max_angle, double *angle_out);

// creating the attack order
int get_attack_order(
    double bx, double by,
    double b_velocity, double min_angle, double max_angle,
    EscortShip escorts[], int n_escorts,
    TargetInfo order_out[]
);

#endif
