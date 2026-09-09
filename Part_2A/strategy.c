#include <stdio.h>
#include <math.h>
#include "strategy.h"
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/physics.h"

int find_elevation_angle(double distance, double velocity, double min_angle, double max_angle, double *angle_out) {
    if (velocity <= 0.0) return 0;

    double g = 9.8;
    double val = (distance * g) / (velocity * velocity);

    if (val > 1.0) return 0; // out of range

    double two_theta_rad = asin(val);
    double deg_theta = (two_theta_rad * 180.0 / M_PI) / 2.0;

    // checking the first angle in the range
    if (deg_theta >= min_angle && deg_theta <= max_angle) {
        *angle_out = deg_theta;
        return 1;
    }

    //second alternative angle (90-theta)
    double alt_deg = 90.0 - deg_theta;
    if (alt_deg >= min_angle && alt_deg <= max_angle) {
        *angle_out = alt_deg;
        return 1;
    }

    return 0;
}

int get_attack_order(
    double bx, double by,
    double b_velocity, double min_angle, double max_angle,
    EscortShip escorts[], int n_escorts,
    TargetInfo order_out[]
) {
    int count = 0;

    // selecting the active ships in the range
    for (int i = 0; i < n_escorts; i++) {
        if (escorts[i].is_destroyed != 1) continue;

        double d = calculate_distance(bx, by, escorts[i].x, escorts[i].y);
        double angle;

        if (find_elevation_angle(d, b_velocity, min_angle, max_angle, &angle)) {
            order_out[count].id = escorts[i].id;
            order_out[count].impact_power = escorts[i].impact_power;
            order_out[count].distance = d;
            count++;
        }
    }

    // bubble sorting according to gressdy priority
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int should_swap = 0;

            // Priority 1: first impact_power high one
            if (order_out[j + 1].impact_power > order_out[j].impact_power) {
                should_swap = 1;
            } 
            // Priority 2: if the impact_power equal then the short distance one
            else if (order_out[j + 1].impact_power == order_out[j].impact_power &&
                     order_out[j + 1].distance < order_out[j].distance) {
                should_swap = 1;
            }

            if (should_swap) {
                TargetInfo temp = order_out[j];
                order_out[j] = order_out[j + 1];
                order_out[j + 1] = temp;
            }
        }
    }

    return count;
}
