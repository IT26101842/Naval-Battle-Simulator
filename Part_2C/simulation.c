#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simulation.h"
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_2A/strategy.h"
#include "files.h"
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/physics.h"

// while battleships is reloding the other are attacking with degradation
static void process_escort_continuous_fire_part2c(
    double start_time, double end_time,
    double bx, double by,
    EscortShip escorts[], int n_escorts,
    double te_p[],
    double gamma_e[],
    int escort_shots[],
    double next_fire_time[],
    double *cumulative_damage,
    FILE *log_fp
) {
    while (*cumulative_damage < 1.0) {
        int earliest_idx = -1;
        double min_time = end_time + 0.0001;

        // searching for alive ships for next shoot
        for (int i = 0; i < n_escorts; i++) {
            if (escorts[i].is_destroyed == 1) {
                if (next_fire_time[i] <= end_time && next_fire_time[i] < min_time) {
                    min_time = next_fire_time[i];
                    earliest_idx = i;
                }
            }
        }

        if (earliest_idx == -1) break;

        int i = earliest_idx;
        double fire_time = next_fire_time[i];

        int type_idx = (escorts[i].type[1] - 'A');
        if (type_idx < 0 || type_idx > 3) type_idx = 0;

        double d = calculate_distance(escorts[i].x, escorts[i].y, bx, by);
        double angle;

        if (find_elevation_angle(d, escorts[i].max_velocity, escorts[i].min_angle, escorts[i].max_angle, &angle)) {
            //  IP_n = IP_0 * exp(-gamma * n)
            double ip_0 = escorts[i].impact_power;
            double current_ip = ip_0 * exp(-gamma_e[type_idx] * escort_shots[i]);
            escort_shots[i]++;

            *cumulative_damage += current_ip;

            char shooter[32];
            snprintf(shooter, sizeof(shooter), "Escort #%d (%s)", escorts[i].id, escorts[i].type);
            log_shot_event(log_fp, fire_time, shooter, "Battleship", d, angle, escorts[i].max_velocity, 1, current_ip, current_ip);
        }

        next_fire_time[i] = fire_time + te_p[type_idx];

        if (*cumulative_damage >= 1.0) break;
    }
}

// 1. Static Simulation (Part 2-C)
void run_part2c_static_simulation(Battleship b, EscortShip escorts[], int n_escorts, 
                                  double tb_q, double te_p[], double gamma_b, double gamma_e[]) {
    FILE *log_fp = open_simulation_log("Part_2C/Sim_1A", "Static_2C");
    if (log_fp) {
        fprintf(log_fp, "SIMULATION: Part 2-C (Static + Gun Degradation)\n");
        fprintf(log_fp, "Reload Time (TB_q): %.2f s | Gamma_B: %.4f\n", tb_q, gamma_b);
        fprintf(log_fp, "Gamma_E -> EA: %.4f, EB: %.4f, EC: %.4f, ED: %.4f\n\n",
                gamma_e[0], gamma_e[1], gamma_e[2], gamma_e[3]);
    }

    TargetInfo order[MAX_ESCORT_SHIPS];
    double next_fire_time[MAX_ESCORT_SHIPS] = {0.0};
    int escort_shots[MAX_ESCORT_SHIPS] = {0};
    double escort_damage_received[MAX_ESCORT_SHIPS] = {0.0};
    
    int b_shots = 0;
    double cumulative_damage = 0.0;
    double current_time = 0.0;
    int destroyed_count = 0;
    int b_survived = 1;

    while (cumulative_damage < 1.0) {
        int count = get_attack_order(b.x, b.y, b.max_velocity, 0.0, 90.0, escorts, n_escorts, order);
        log_attack_order(log_fp, 0, order, count);

        if (count == 0) {
            printf("\n✅ No more targets within Battleship's firing range.\n");
            break;
        }

        int target_id = order[0].id;
        int target_idx = -1;
        for (int i = 0; i < n_escorts; i++) {
            if (escorts[i].id == target_id) { target_idx = i; break; }
        }

        double angle;
        find_elevation_angle(order[0].distance, b.max_velocity, 0.0, 90.0, &angle);

        // Battleship Gun Degradation: IP_n = 1.0 * exp(-gamma_b * b_shots)
        double b_current_ip = 1.0 * exp(-gamma_b * b_shots);
        b_shots++;

        char target_name[32];
        snprintf(target_name, sizeof(target_name), "Escort #%d", target_id);
        log_shot_event(log_fp, current_time, "Battleship", target_name, order[0].distance, angle, b.max_velocity, 1, b_current_ip, b_current_ip);

        // giving chance for target ship for shoot in current time
        if (next_fire_time[target_idx] <= current_time) {
            int type_idx = (escorts[target_idx].type[1] - 'A');
            if (type_idx < 0 || type_idx > 3) type_idx = 0;

            double ed = order[0].distance;
            double e_angle;
            if (find_elevation_angle(ed, escorts[target_idx].max_velocity, escorts[target_idx].min_angle, escorts[target_idx].max_angle, &e_angle)) {
                double e_current_ip = escorts[target_idx].impact_power * exp(-gamma_e[type_idx] * escort_shots[target_idx]);
                escort_shots[target_idx]++;
                cumulative_damage += e_current_ip;

                char shooter[32];
                snprintf(shooter, sizeof(shooter), "Escort #%d (%s)", escorts[target_idx].id, escorts[target_idx].type);
                log_shot_event(log_fp, current_time, shooter, "Battleship", ed, e_angle, escorts[target_idx].max_velocity, 1, e_current_ip, e_current_ip);
            }
            next_fire_time[target_idx] = current_time + te_p[type_idx];
        }

        // after adding the damage checking if the battleship is destroyed
        escort_damage_received[target_idx] += b_current_ip;
        if (escort_damage_received[target_idx] >= 1.0) {
            escorts[target_idx].is_destroyed = 0;
            destroyed_count++;
        }

        if (cumulative_damage >= 1.0) {
            b_survived = 0;
            printf("\n☠️ Battleship was SUNK!\n");
            break;
        }

        // other are shooting continuously
        double next_b_time = current_time + tb_q;
        process_escort_continuous_fire_part2c(current_time, next_b_time, b.x, b.y, escorts, n_escorts, 
                                             te_p, gamma_e, escort_shots, next_fire_time, &cumulative_damage, log_fp);

        if (cumulative_damage >= 1.0) {
            b_survived = 0;
            printf("\n☠️ Battleship was SUNK by Escort continuous fire!\n");
            break;
        }

        current_time = next_b_time;
    }

    if (cumulative_damage >= 1.0) b_survived = 0;
    log_simulation_summary(log_fp, b_survived, cumulative_damage, destroyed_count, n_escorts, current_time);
    close_simulation_log(log_fp);

    // logging the damaged ship for log file
    log_damaged_escorts_report("Static_2C", escorts, escort_damage_received, n_escorts);
}

// 2. Moving Simulation (Part 2-C)
void run_part2c_moving_simulation(Battleship b, EscortShip escorts[], int n_escorts, 
                                  Point path[], int steps, double tb_q, double te_p[], 
                                  double gamma_b, double gamma_e[]) {
    FILE *log_fp = open_simulation_log("Part_2C/Sim_1B_1", "Moving_2C");
    if (log_fp) {
        fprintf(log_fp, "SIMULATION: Part 2-C (Moving + Gun Degradation)\n");
        fprintf(log_fp, "Steps: %d | Reload Time: %.2f s | Gamma_B: %.4f\n\n", steps, tb_q, gamma_b);
    }

    TargetInfo order[MAX_ESCORT_SHIPS];
    double next_fire_time[MAX_ESCORT_SHIPS] = {0.0};
    int escort_shots[MAX_ESCORT_SHIPS] = {0};
    double escort_damage_received[MAX_ESCORT_SHIPS] = {0.0};
    
    int b_shots = 0;
    double cumulative_damage = 0.0;
    double current_time = 0.0;
    int destroyed_count = 0;
    int b_survived = 1;

    for (int s = 0; s < steps; s++) {
        b.x = path[s].x;
        b.y = path[s].y;

        printf("\n📍 Battleship moved to Step %d: (%.2f, %.2f)\n", s + 1, b.x, b.y);
        if (log_fp) fprintf(log_fp, "\n📍 Battleship moved to Step %d: (%.2f, %.2f)\n", s + 1, b.x, b.y);

        int count = get_attack_order(b.x, b.y, b.max_velocity, 0.0, 90.0, escorts, n_escorts, order);
        log_attack_order(log_fp, s + 1, order, count);

        if (count > 0) {
            int target_id = order[0].id;
            int target_idx = -1;
            for (int i = 0; i < n_escorts; i++) {
                if (escorts[i].id == target_id) { target_idx = i; break; }
            }

            double angle;
            find_elevation_angle(order[0].distance, b.max_velocity, 0.0, 90.0, &angle);

            double b_current_ip = 1.0 * exp(-gamma_b * b_shots);
            b_shots++;

            char target_name[32];
            snprintf(target_name, sizeof(target_name), "Escort #%d", target_id);
            log_shot_event(log_fp, current_time, "Battleship", target_name, order[0].distance, angle, b.max_velocity, 1, b_current_ip, b_current_ip);

            if (next_fire_time[target_idx] <= current_time) {
                int type_idx = (escorts[target_idx].type[1] - 'A');
                if (type_idx < 0 || type_idx > 3) type_idx = 0;

                double ed = order[0].distance;
                double e_angle;
                if (find_elevation_angle(ed, escorts[target_idx].max_velocity, escorts[target_idx].min_angle, escorts[target_idx].max_angle, &e_angle)) {
                    double e_current_ip = escorts[target_idx].impact_power * exp(-gamma_e[type_idx] * escort_shots[target_idx]);
                    escort_shots[target_idx]++;
                    cumulative_damage += e_current_ip;

                    char shooter[32];
                    snprintf(shooter, sizeof(shooter), "Escort #%d (%s)", escorts[target_idx].id, escorts[target_idx].type);
                    log_shot_event(log_fp, current_time, shooter, "Battleship", ed, e_angle, escorts[target_idx].max_velocity, 1, e_current_ip, e_current_ip);
                }
                next_fire_time[target_idx] = current_time + te_p[type_idx];
            }

            escort_damage_received[target_idx] += b_current_ip;
            if (escort_damage_received[target_idx] >= 1.0) {
                escorts[target_idx].is_destroyed = 0;
                destroyed_count++;
            }
        }

        if (cumulative_damage >= 1.0) {
            b_survived = 0;
            printf("\n☠️ Battleship was SUNK at Step %d!\n", s + 1);
            break;
        }

        double next_b_time = current_time + tb_q;
        process_escort_continuous_fire_part2c(current_time, next_b_time, b.x, b.y, escorts, n_escorts, 
                                             te_p, gamma_e, escort_shots, next_fire_time, &cumulative_damage, log_fp);

        if (cumulative_damage >= 1.0) {
            b_survived = 0;
            printf("\n☠️ Battleship was SUNK at Step %d!\n", s + 1);
            break;
        }

        current_time = next_b_time;
    }

    if (cumulative_damage >= 1.0) b_survived = 0;
    log_simulation_summary(log_fp, b_survived, cumulative_damage, destroyed_count, n_escorts, current_time);
    close_simulation_log(log_fp);

    log_damaged_escorts_report("Moving_2C", escorts, escort_damage_received, n_escorts);
}

// 3. Jammed Gun Simulation (Part 2-C)
void run_part2c_jammed_simulation(Battleship b, EscortShip escorts[], int n_escorts, 
                                  Point path[], int steps, int jam_step, double jam_min_angle, 
                                  double tb_q, double te_p[], double gamma_b, double gamma_e[]) {
    FILE *log_fp = open_simulation_log("Part_2C/Sim_1B_2", "Jammed_2C");
    if (log_fp) {
        fprintf(log_fp, "SIMULATION: Part 2-C (Jammed Gun + Degradation)\n");
        fprintf(log_fp, "Jam Step: %d | Min Angle: %.2f deg | Gamma_B: %.4f\n\n", jam_step, jam_min_angle, gamma_b);
    }

    TargetInfo order[MAX_ESCORT_SHIPS];
    double next_fire_time[MAX_ESCORT_SHIPS] = {0.0};
    int escort_shots[MAX_ESCORT_SHIPS] = {0};
    double escort_damage_received[MAX_ESCORT_SHIPS] = {0.0};
    
    int b_shots = 0;
    double cumulative_damage = 0.0;
    double current_time = 0.0;
    int destroyed_count = 0;
    int b_survived = 1;

    for (int s = 0; s < steps; s++) {
        b.x = path[s].x;
        b.y = path[s].y;

        double current_min_angle = 0.0;
        if ((s + 1) >= jam_step) {
            current_min_angle = jam_min_angle;
            if ((s + 1) == jam_step) {
                printf("\n⚠️ [WARNING] Gun JAMMED! Min angle restricted to: %.2f deg\n", jam_min_angle);
                if (log_fp) fprintf(log_fp, "\n⚠️ [WARNING] Gun JAMMED! Min angle restricted to: %.2f deg\n", jam_min_angle);
            }
        }

        printf("\n📍 Battleship moved to Step %d: (%.2f, %.2f)", s + 1, b.x, b.y);
        if (current_min_angle > 0.0) printf(" [Gun Min Angle: %.2f°]\n", current_min_angle);
        else printf("\n");

        if (log_fp) {
            fprintf(log_fp, "\n📍 Battleship moved to Step %d: (%.2f, %.2f)", s + 1, b.x, b.y);
            if (current_min_angle > 0.0) fprintf(log_fp, " [Gun Min Angle: %.2f°]\n", current_min_angle);
            else fprintf(log_fp, "\n");
        }

        int count = get_attack_order(b.x, b.y, b.max_velocity, current_min_angle, 90.0, escorts, n_escorts, order);
        log_attack_order(log_fp, s + 1, order, count);

        if (count > 0) {
            int target_id = order[0].id;
            int target_idx = -1;
            for (int i = 0; i < n_escorts; i++) {
                if (escorts[i].id == target_id) { target_idx = i; break; }
            }

            double angle;
            find_elevation_angle(order[0].distance, b.max_velocity, current_min_angle, 90.0, &angle);

            double b_current_ip = 1.0 * exp(-gamma_b * b_shots);
            b_shots++;

            char target_name[32];
            snprintf(target_name, sizeof(target_name), "Escort #%d", target_id);
            log_shot_event(log_fp, current_time, "Battleship", target_name, order[0].distance, angle, b.max_velocity, 1, b_current_ip, b_current_ip);

            if (next_fire_time[target_idx] <= current_time) {
                int type_idx = (escorts[target_idx].type[1] - 'A');
                if (type_idx < 0 || type_idx > 3) type_idx = 0;

                double ed = order[0].distance;
                double e_angle;
                if (find_elevation_angle(ed, escorts[target_idx].max_velocity, escorts[target_idx].min_angle, escorts[target_idx].max_angle, &e_angle)) {
                    double e_current_ip = escorts[target_idx].impact_power * exp(-gamma_e[type_idx] * escort_shots[target_idx]);
                    escort_shots[target_idx]++;
                    cumulative_damage += e_current_ip;

                    char shooter[32];
                    snprintf(shooter, sizeof(shooter), "Escort #%d (%s)", escorts[target_idx].id, escorts[target_idx].type);
                    log_shot_event(log_fp, current_time, shooter, "Battleship", ed, e_angle, escorts[target_idx].max_velocity, 1, e_current_ip, e_current_ip);
                }
                next_fire_time[target_idx] = current_time + te_p[type_idx];
            }

            escort_damage_received[target_idx] += b_current_ip;
            if (escort_damage_received[target_idx] >= 1.0) {
                escorts[target_idx].is_destroyed = 0;
                destroyed_count++;
            }
        }

        if (cumulative_damage >= 1.0) {
            b_survived = 0;
            printf("\n☠️ Battleship was SUNK at Step %d!\n", s + 1);
            break;
        }

        double next_b_time = current_time + tb_q;
        process_escort_continuous_fire_part2c(current_time, next_b_time, b.x, b.y, escorts, n_escorts, 
                                             te_p, gamma_e, escort_shots, next_fire_time, &cumulative_damage, log_fp);

        if (cumulative_damage >= 1.0) {
            b_survived = 0;
            printf("\n☠️ Battleship was SUNK at Step %d!\n", s + 1);
            break;
        }

        current_time = next_b_time;
    }

    if (cumulative_damage >= 1.0) b_survived = 0;
    log_simulation_summary(log_fp, b_survived, cumulative_damage, destroyed_count, n_escorts, current_time);
    close_simulation_log(log_fp);

    log_damaged_escorts_report("Jammed_2C", escorts, escort_damage_received, n_escorts);
}
