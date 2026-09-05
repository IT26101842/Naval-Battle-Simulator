#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simulation.h"
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/physics.h"
#include "files.h"

// function to find angle of a shot
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

void run_battle_simulation(
    Battleship *b, 
    EscortShip escorts[], 
    int n_escorts, 
    Point path[], 
    int steps, 
    int jam_step, 
    FILE *log
) {
    int is_jammed = 0;
    double b_min_angle = 0.0, b_max_angle = 90.0;
    double cumulative_damage = 0.0;
    int b_sunk = 0;
    int end_step = steps;

    // array for records of shots that get for escortships
    int has_fired[MAX_ESCORT_SHIPS] = {0};

    for (int step = 1; step <= steps; step++) {
        b->x = path[step - 1].x;
        b->y = path[step - 1].y;

        // jamming condition
        if (jam_step > 0 && step >= jam_step && !is_jammed) {
            is_jammed = 1;
            b_min_angle = ((double)rand() / RAND_MAX) * 30.0;
            b_max_angle = 90.0;
        }

        log_step_info(log, step, steps, b->x, b->y, is_jammed, b_min_angle, jam_step);

        // shots from escortship to battleship
        for (int i = 0; i < n_escorts; i++) {
            if (escorts[i].is_destroyed != 1 || has_fired[i]) continue;

            double d = calculate_distance(escorts[i].x, escorts[i].y, b->x, b->y);
            double angle;
            if (find_elevation_angle(d, escorts[i].max_velocity, escorts[i].min_angle, escorts[i].max_angle, &angle)) {
                has_fired[i] = 1;
                cumulative_damage += escorts[i].impact_power; // calculating the impact
                
                double t = calculate_time_of_flight(escorts[i].max_velocity, angle);
                log_attack_event(log, "Escort", escorts[i].id, "Battleship", 0, d, angle, t);
                printf("💥 Impact Power: +%.2f | Cumulative Damage: %.2f%%\n", 
                       escorts[i].impact_power, cumulative_damage * 100.0);
                if (log) {
                    fprintf(log, "💥 Impact Power: +%.2f | Cumulative Damage: %.2f%%\n", 
                            escorts[i].impact_power, cumulative_damage * 100.0);
                }

                // if the damage more than 100% it will destroyed
                if (cumulative_damage >= 1.0) {
                    b_sunk = 1;
                    b->is_destroyed = 0;
                    end_step = step;
                    printf("🚨 Battleship was SUNK at step %d!\n", step);
                    if (log) fprintf(log, "🚨 Battleship was SUNK at step %d!\n", step);
                    break;
                }
            }
        }

        // stop the battle step if the battleship is already sunk
        if (b_sunk) break;

        // shots from battleship to escortships
        for (int i = 0; i < n_escorts; i++) {
            if (escorts[i].is_destroyed != 1) continue;

            double d = calculate_distance(b->x, b->y, escorts[i].x, escorts[i].y);
            double angle;
            if (find_elevation_angle(d, b->max_velocity, b_min_angle, b_max_angle, &angle)) {
                escorts[i].is_destroyed = 0; 
                double t = calculate_time_of_flight(b->max_velocity, angle);
                log_attack_event(log, "Battleship", 0, "Escort", escorts[i].id, d, angle, t);
            }
        }

        // checking the live escortships count
        int escorts_alive = 0;
        for (int i = 0; i < n_escorts; i++) {
            if (escorts[i].is_destroyed == 1) escorts_alive++;
        }
        if (escorts_alive == 0) {
            printf("\n🏆 All Escort ships destroyed at step %d!\n", step);
            if (log) fprintf(log, "\n🏆 All Escort ships destroyed at step %d!\n", step);
            end_step = step;
            break;
        }
    }

    log_summary(log, b_sunk, cumulative_damage, end_step);
}

void run_part1a_c(Battleship *b, EscortShip escorts[], int n, FILE *log) {
    Point p = {b->x, b->y};
    run_battle_simulation(b, escorts, n, &p, 1, -1, log);
}

void run_part1b_sim1_c(Battleship *b, EscortShip escorts[], int n, Point path[], int steps, FILE *log) {
    run_battle_simulation(b, escorts, n, path, steps, -1, log);
}

void run_part1b_sim2_c(Battleship *b, EscortShip escorts[], int n, Point path[], int steps, int jam_step, FILE *log) {
    run_battle_simulation(b, escorts, n, path, steps, jam_step, log);
}
