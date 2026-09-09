#include <stdio.h>
#include <stdlib.h>
#include "simulation.h"
#include "strategy.h"
#include "files.h"
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/physics.h"

// checking the antiattacks of escortships
static void process_escort_return_fire(
    double current_time,
    double bx, double by,
    EscortShip escorts[], int n_escorts,
    int has_fired[],
    double *cumulative_damage,
    FILE *log_fp
) {
    for (int i = 0; i < n_escorts; i++) {
        // still active and previously not attackted
        if (escorts[i].is_destroyed == 1 && has_fired[i] == 0) {
            double d = calculate_distance(escorts[i].x, escorts[i].y, bx, by);
            double angle;

            // attacking if the battleship is in the escortship range
            if (find_elevation_angle(d, escorts[i].max_velocity, escorts[i].min_angle, escorts[i].max_angle, &angle)) {
                has_fired[i] = 1;
                *cumulative_damage += escorts[i].impact_power;

                char shooter[32];
                snprintf(shooter, sizeof(shooter), "Escort #%d", escorts[i].id);
                log_shot_event(log_fp, current_time, shooter, "Battleship", d, angle, escorts[i].max_velocity, 1, escorts[i].impact_power);
            }
        }
    }
}

// 1. Static Simulation (Part 1-A)
void run_part2a_static_simulation(Battleship b, EscortShip escorts[], int n_escorts, double tb_q) {
    FILE *log_fp = open_simulation_log("Part_2A/Sim_1A", "Static");
    if (log_fp) fprintf(log_fp, "SIMULATION: Part 1-A (Static Battleship)\nReload Time (TB_q): %.2f s\n\n", tb_q);

    TargetInfo order[MAX_ESCORT_SHIPS];
    int has_fired[MAX_ESCORT_SHIPS] = {0};
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

        char target_name[32];
        snprintf(target_name, sizeof(target_name), "Escort #%d", target_id);
        log_shot_event(log_fp, current_time, "Battleship", target_name, order[0].distance, angle, b.max_velocity, 1, 1.0);

        // step 1: attacking the target ship and activating the anti attacks
        process_escort_return_fire(current_time, b.x, b.y, escorts, n_escorts, has_fired, &cumulative_damage, log_fp);

        // step 2: recording the target ship has destroyed after the battleship attack
        escorts[target_idx].is_destroyed = 0;
        destroyed_count++;

        if (cumulative_damage >= 1.0) {
            b_survived = 0;
            printf("\n☠️ Battleship was SUNK by Escort return fire!\n");
            break;
        }

        current_time += tb_q;
    }

    if (cumulative_damage >= 1.0) b_survived = 0;
    log_simulation_summary(log_fp, b_survived, cumulative_damage, destroyed_count, n_escorts, current_time);
    close_simulation_log(log_fp);
}

// 2. Moving Simulation (Part 1-B Sim 1)
void run_part2a_moving_simulation(Battleship b, EscortShip escorts[], int n_escorts, Point path[], int steps, double tb_q) {
    FILE *log_fp = open_simulation_log("Part_2A/Sim_1B_1", "Moving");
    if (log_fp) fprintf(log_fp, "SIMULATION: Part 1-B Sim 1 (Moving Battleship)\nSteps: %d, Reload Time: %.2f s\n\n", steps, tb_q);

    int has_fired[MAX_ESCORT_SHIPS] = {0};
    double cumulative_damage = 0.0;
    double current_time = 0.0;
    int destroyed_count = 0;
    int b_survived = 1;

    for (int s = 0; s < steps; s++) {
        b.x = path[s].x;
        b.y = path[s].y;

        printf("\n📍 Battleship moved to Step %d: (%.2f, %.2f)\n", s + 1, b.x, b.y);
        if (log_fp) fprintf(log_fp, "\n📍 Battleship moved to Step %d: (%.2f, %.2f)\n", s + 1, b.x, b.y);

        TargetInfo order[MAX_ESCORT_SHIPS];
        int count = get_attack_order(b.x, b.y, b.max_velocity, 0.0, 90.0, escorts, n_escorts, order);
        log_attack_order(log_fp, s + 1, order, count);

        int target_idx = -1;
        if (count > 0) {
            int target_id = order[0].id;
            for (int i = 0; i < n_escorts; i++) {
                if (escorts[i].id == target_id) { target_idx = i; break; }
            }

            double angle;
            find_elevation_angle(order[0].distance, b.max_velocity, 0.0, 90.0, &angle);

            char target_name[32];
            snprintf(target_name, sizeof(target_name), "Escort #%d", target_id);
            log_shot_event(log_fp, current_time, "Battleship", target_name, order[0].distance, angle, b.max_velocity, 1, 1.0);
        }

        // activating the anti attacks
        process_escort_return_fire(current_time, b.x, b.y, escorts, n_escorts, has_fired, &cumulative_damage, log_fp);

        // destroying the target ship
        if (target_idx != -1) {
            escorts[target_idx].is_destroyed = 0;
            destroyed_count++;
        }

        if (cumulative_damage >= 1.0) {
            b_survived = 0;
            printf("\n☠️ Battleship was SUNK at Step %d!\n", s + 1);
            break;
        }

        current_time += tb_q;
    }

    if (cumulative_damage >= 1.0) b_survived = 0;
    log_simulation_summary(log_fp, b_survived, cumulative_damage, destroyed_count, n_escorts, current_time);
    close_simulation_log(log_fp);
}

// 3. Jammed Gun Simulation (Part 1-B Sim 2)
void run_part2a_jammed_simulation(Battleship b, EscortShip escorts[], int n_escorts, Point path[], int steps, int jam_step, double jam_min_angle, double tb_q) {
    FILE *log_fp = open_simulation_log("Part_2A/Sim_1B_2", "Jammed");
    if (log_fp) fprintf(log_fp, "SIMULATION: Part 1-B Sim 2 (Jammed Gun)\nJam at Step: %d, Min Angle: %.2f deg, Reload Time: %.2f s\n\n", jam_step, jam_min_angle, tb_q);

    int has_fired[MAX_ESCORT_SHIPS] = {0};
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
            printf("\n⚠️ [WARNING] Gun JAMMED! Min angle restricted to: %.2f deg\n", current_min_angle);
            if (log_fp) fprintf(log_fp, "\n⚠️ [WARNING] Gun JAMMED! Min angle restricted to: %.2f deg\n", current_min_angle);
        }

        TargetInfo order[MAX_ESCORT_SHIPS];
        int count = get_attack_order(b.x, b.y, b.max_velocity, current_min_angle, 90.0, escorts, n_escorts, order);
        log_attack_order(log_fp, s + 1, order, count);

        int target_idx = -1;
        if (count > 0) {
            int target_id = order[0].id;
            for (int i = 0; i < n_escorts; i++) {
                if (escorts[i].id == target_id) { target_idx = i; break; }
            }

            double angle;
            find_elevation_angle(order[0].distance, b.max_velocity, current_min_angle, 90.0, &angle);

            char target_name[32];
            snprintf(target_name, sizeof(target_name), "Escort #%d", target_id);
            log_shot_event(log_fp, current_time, "Battleship", target_name, order[0].distance, angle, b.max_velocity, 1, 1.0);
        }

        // activating the anti attacks
        process_escort_return_fire(current_time, b.x, b.y, escorts, n_escorts, has_fired, &cumulative_damage, log_fp);

        // destroying the target ship
        if (target_idx != -1) {
            escorts[target_idx].is_destroyed = 0;
            destroyed_count++;
        }

        if (cumulative_damage >= 1.0) {
            b_survived = 0;
            printf("\n☠️ Battleship was SUNK at Step %d!\n", s + 1);
            break;
        }

        current_time += tb_q;
    }

    if (cumulative_damage >= 1.0) b_survived = 0;
    log_simulation_summary(log_fp, b_survived, cumulative_damage, destroyed_count, n_escorts, current_time);
    close_simulation_log(log_fp);
}
