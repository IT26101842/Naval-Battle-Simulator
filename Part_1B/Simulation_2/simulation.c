#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simulation.h"
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/physics.h"
#include "files.h"

int find_elevation_angle(double distance, double velocity, double min_angle, double max_angle, double *angle_out) {
    if (velocity <= 0.0) return 0;

    double val = (distance * GRAVITY) / (velocity * velocity);
    if (val > 1.0) return 0; 

    double two_theta_rad = asin(val);
    double theta_deg = (two_theta_rad * 180.0 / PI) / 2.0;

    if (theta_deg >= min_angle && theta_deg <= max_angle) {
        *angle_out = theta_deg;
        return 1;
    }
    return 0;
}

void run_simulation_2(Battleship *battleship, 
                      EscortShip escorts[], 
                      int num_escorts, 
                      Point path[], 
                      int num_points, 
                      int jam_step, 
                      double canvas_size) {

    FILE *log_file = open_simulation_log("simulation2_log.txt");

    // battleship angle range before jam
    double b_min_angle = 0.0;
    double b_max_angle = 90.0;
    int is_jammed = 0;

    for (int i = 0; i < num_points; i++) {
        battleship->x = path[i].x;
        battleship->y = path[i].y;

        // jamming when the t value came
        if (i + 1 >= jam_step && !is_jammed) {
            is_jammed = 1;
            b_min_angle = ((double)rand() / RAND_MAX) * 30.0; // random value between 0 and 30
            b_max_angle = 90.0;
        }

        log_step_info(log_file, i + 1, num_points, battleship->x, battleship->y, is_jammed, b_min_angle, jam_step);

        // Attacking for battleship
        int hit_battleship = 0;
        for (int j = 0; j < num_escorts; j++) {
            if (escorts[j].is_destroyed == 1) {
                double dist = calculate_distance(escorts[j].x, escorts[j].y, battleship->x, battleship->y);
                double angle = 0.0;

                if (find_elevation_angle(dist, escorts[j].max_velocity, escorts[j].min_angle, escorts[j].max_angle, &angle)) {
                    double flight_time = calculate_time_of_flight(escorts[j].max_velocity, angle);
                    log_attack_event(log_file, "Escort", escorts[j].id, "Battleship", 0, dist, angle, flight_time);
                    battleship->is_destroyed = 0;
                    hit_battleship = 1;
                    break;
                }
            }
        }

        if (hit_battleship) {
            printf("\n🚨 Battleship was sunk! Simulation ended.\n");
            if (log_file) fprintf(log_file, "\n🚨 Battleship was sunk! Simulation ended.\n");
            break;
        }

        // Attacking for escortships
        for (int j = 0; j < num_escorts; j++) {
            if (escorts[j].is_destroyed == 1) {
                double dist = calculate_distance(battleship->x, battleship->y, escorts[j].x, escorts[j].y);
                double angle = 0.0;

                if (find_elevation_angle(dist, battleship->max_velocity, b_min_angle, b_max_angle, &angle)) {
                    double flight_time = calculate_time_of_flight(battleship->max_velocity, angle);
                    escorts[j].is_destroyed = 0;
                    log_attack_event(log_file, "Battleship", 0, "Escort", escorts[j].id, dist, angle, flight_time);
                }
            }
        }

        // checking all the escortships are destroyed
        int remaining = 0;
        for (int j = 0; j < num_escorts; j++) {
            if (escorts[j].is_destroyed == 1) remaining++;
        }

        if (remaining == 0) {
            printf("\n🏆 All Escort ships destroyed at step %d!\n", i + 1);
            if (log_file) fprintf(log_file, "\n🏆 All Escort ships destroyed at step %d!\n", i + 1);
            break;
        }
    }

    log_final_summary(log_file, battleship, escorts, num_escorts);
    close_simulation_log(log_file);
}
