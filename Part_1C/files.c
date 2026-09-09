#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "files.h"

FILE* open_simulation_log(const char *folder, const char *prefix) {
    // checking the folder it is available? or create one
    mkdir("logs", 0777);
    mkdir(folder, 0777);

    // searching the new interation number that doesnt exists
    char filepath[256];
    int run_id = 1;
    while (run_id < 10000) {
        snprintf(filepath, sizeof(filepath), "%s/%s_%04d.log", folder, prefix, run_id);
        FILE *check = fopen(filepath, "r");
        if (check == NULL) break; // if there is not that file that number can be used
        fclose(check);
        run_id++;
    }

    FILE *f = fopen(filepath, "w");
    if (f == NULL) {
        printf("⚠️ Log file open failed: %s\n", filepath);
    } else {
        printf("📄 Log file created: %s\n", filepath);
    }
    return f;
}

void close_simulation_log(FILE *log_file) {
    if (log_file != NULL) {
        fclose(log_file);
    }
}

// Function for write and record the battleship jammed details
void log_step_info(FILE *log_file, int step, int total_steps, double bx, double by, int is_jammed, double min_angle, int jam_step) {
    printf("\n--- Step %d / %d ---\n", step, total_steps);
    printf("Battleship Position: (%.2f, %.2f)\n", bx, by);

    if (is_jammed) {
        if (step == jam_step) {
            printf("⚠️ ALERT: Battleship Gun is JAMMED! Min Angle: %.2f deg | Max Angle: 90.00 deg\n", min_angle);
        } else {
            printf("ℹ️ Gun Status: JAMMED (Elevation Range: %.2f - 90.00 deg)\n", min_angle);
        }
    }

    if (log_file != NULL) {
        fprintf(log_file, "\n--- Step %d / %d ---\n", step, total_steps);
        fprintf(log_file, "Battleship Position: (%.2f, %.2f)\n", bx, by);
        if (is_jammed) {
            if (step == jam_step) {
                fprintf(log_file, "⚠️ ALERT: Battleship Gun is JAMMED! Min Angle: %.2f deg | Max Angle: 90.00 deg\n", min_angle);
            } else {
                fprintf(log_file, "ℹ️ Gun Status: JAMMED (Elevation Range: %.2f - 90.00 deg)\n", min_angle);
            }
        }
    }
}

// function for write and record the attacks that happens
void log_attack_event(FILE *log_file, const char *attacker, int attacker_id, const char *target, int target_id, double distance, double angle, double flight_time) {
    printf("💥 %s #%d -> FIRED AT %s #%d | Distance: %.2f m | Angle: %.2f deg | Flight Time: %.2f s [HIT]\n",
           attacker, attacker_id, target, target_id, distance, angle, flight_time);

    if (log_file != NULL) {
        fprintf(log_file, "💥 %s #%d -> FIRED AT %s #%d | Distance: %.2f m | Angle: %.2f deg | Flight Time: %.2f s [HIT]\n",
                attacker, attacker_id, target, target_id, distance, angle, flight_time);
    }
}

// function for write and record the summary details
void log_summary(FILE *log_file, int b_sunk, double cumulative_damage, int end_step, int destroyed_escorts, int total_escorts) {
    printf("\n==================== SIMULATION SUMMARY ====================\n");
    if (b_sunk) {
        printf("Battleship Status      : DESTROYED (Sunk at step %d)\n", end_step);
    } else {
        printf("Battleship Status      : SURVIVED\n");
    }
    printf("Total Damage Taken     : %.2f%%\n", cumulative_damage * 100.0);
    printf("Remaining Health       : %.2f%%\n", (cumulative_damage >= 1.0) ? 0.0 : (1.0 - cumulative_damage) * 100.0);
    printf("Destroyed Escort Ships : %d / %d\n", destroyed_escorts, total_escorts);
    printf("============================================================\n");

    if (log_file != NULL) {
        fprintf(log_file, "\n==================== SIMULATION SUMMARY ====================\n");
        if (b_sunk) {
            fprintf(log_file, "Battleship Status      : DESTROYED (Sunk at step %d)\n", end_step);
        } else {
            fprintf(log_file, "Battleship Status      : SURVIVED\n");
        }
        fprintf(log_file, "Total Damage Taken     : %.2f%%\n", cumulative_damage * 100.0);
        fprintf(log_file, "Remaining Health       : %.2f%%\n", (cumulative_damage >= 1.0) ? 0.0 : (1.0 - cumulative_damage) * 100.0);
        fprintf(log_file, "Destroyed Escort Ships : %d / %d\n", destroyed_escorts, total_escorts);
        fprintf(log_file, "============================================================\n");
    }
}
