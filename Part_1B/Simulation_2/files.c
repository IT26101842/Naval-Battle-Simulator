#include "files.h"

FILE* open_simulation_log(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("❌ Error opening log file!\n");
        return NULL;
    }
    fprintf(file, "==========================================================\n");
    fprintf(file, "     NAVAL BATTLEFIELD - SIMULATION 2 COMBAT LOG          \n");
    fprintf(file, "==========================================================\n\n");
    return file;
}

void close_simulation_log(FILE *log_file) {
    if (log_file != NULL) {
        fclose(log_file);
    }
}

// Function for record and write battleship positions and after jammed detailes
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

// Function for record and write the hitiing details ( E -> B) (B -> E)
void log_attack_event(FILE *log_file, const char *attacker, int attacker_id, const char *target, int target_id, double distance, double angle, double flight_time) {
    printf("💥 %s #%d -> FIRED AT %s #%d | Distance: %.2f m | Angle: %.2f deg | Flight Time: %.2f s [HIT]\n",
           attacker, attacker_id, target, target_id, distance, angle, flight_time);

    if (log_file != NULL) {
        fprintf(log_file, "💥 %s #%d -> FIRED AT %s #%d | Distance: %.2f m | Angle: %.2f deg | Flight Time: %.2f s [HIT]\n",
                attacker, attacker_id, target, target_id, distance, angle, flight_time);
    }
}

// Function for write and record the simulation 2 summary
void log_final_summary(FILE *log_file, Battleship *battleship, EscortShip escorts[], int num_escorts) {
    int destroyed_count = 0;
    for (int i = 0; i < num_escorts; i++) {
        if (escorts[i].is_destroyed == 0) {
            destroyed_count++;
        }
    }

    printf("\n==========================================================\n");
    printf("                  SIMULATION 2 SUMMARY                    \n");
    printf("==========================================================\n");
    printf("Battleship Status : %s\n", (battleship->is_destroyed == 0) ? "DESTROYED" : "SURVIVED");
    printf("Final Position    : (%.2f, %.2f)\n", battleship->x, battleship->y);
    printf("Escorts Destroyed : %d / %d\n", destroyed_count, num_escorts);

    if (log_file != NULL) {
        fprintf(log_file, "\n==========================================================\n");
        fprintf(log_file, "                  SIMULATION 2 SUMMARY                    \n");
        fprintf(log_file, "==========================================================\n");
        fprintf(log_file, "Battleship Status : %s\n", (battleship->is_destroyed == 0) ? "DESTROYED" : "SURVIVED");
        fprintf(log_file, "Final Position    : (%.2f, %.2f)\n", battleship->x, battleship->y);
        fprintf(log_file, "Escorts Destroyed : %d / %d\n\n", destroyed_count, num_escorts);
        for (int i = 0; i < num_escorts; i++) {
            fprintf(log_file, "  Escort #%d [%s] at (%.2f, %.2f) - %s\n",
                    escorts[i].id, escorts[i].type, escorts[i].x, escorts[i].y,
                    (escorts[i].is_destroyed == 0) ? "DESTROYED" : "ACTIVE");
        }
    }
}
