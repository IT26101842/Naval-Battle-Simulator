#include "files.h"

FILE* open_log_file(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        printf("❌ Error creating log file!\n");
        return NULL;
    }
    fprintf(f, "========================================\n");
    fprintf(f, "     PART 1-B: SIMULATION 1 LOG         \n");
    fprintf(f, "========================================\n\n");
    return f;
}

void close_log_file(FILE *log_file) {
    if (log_file != NULL) {
        fclose(log_file);
    }
}

// Function for record and write the battleship locations
void log_step_header(FILE *log_file, int step, int total_steps, double x, double y) {
    printf("\n--- Step %d / %d ---\n", step, total_steps);
    printf("Battleship moved to: (%.2f, %.2f)\n", x, y);
    if (log_file) {
        fprintf(log_file, "\n--- Step %d / %d ---\n", step, total_steps);
        fprintf(log_file, "Battleship moved to: (%.2f, %.2f)\n", x, y);
    }
}

// Function for record and write the Battleship was destroyed by the escortship
void log_escort_attack(FILE *log_file, int escort_id, int success) {
    if (success) {
        printf("💥 Escort Ship #%d destroyed the Battleship!\n", escort_id);
        if (log_file) fprintf(log_file, "💥 Escort Ship #%d destroyed the Battleship!\n", escort_id);
    }
}

// Function for write the Escortship was destroyed by the battleship
void log_battleship_counter(FILE *log_file, int escort_id) {
    printf("🎯 Battleship destroyed Escort Ship #%d!\n", escort_id);
    if (log_file) fprintf(log_file, "🎯 Battleship destroyed Escort Ship #%d!\n", escort_id);
}

// Function for record and write Victory and say battleship has won
void log_victory(FILE *log_file, int step) {
    printf("\n🏆 VICTORY: All escort ships have been destroyed at step %d!\n", step);
    printf("Halting simulation as no threats remain.\n");
    if (log_file) {
        fprintf(log_file, "\n🏆 VICTORY: All escort ships have been destroyed at step %d!\n", step);
        fprintf(log_file, "Halting simulation as no threats remain.\n");
    }
}

// Function for write the summary and print it on the terminal
void log_summary(FILE *log_file, Battleship *battleship, EscortShip escorts[], int num_escorts) {
    int destroyed_count = 0;
    for (int i = 0; i < num_escorts; i++) {
        if (escorts[i].is_destroyed == 0) destroyed_count++; // 0 = destroyed
    }

    printf("\n========================================\n");
    printf("         SIMULATION 1 SUMMARY           \n");
    printf("========================================\n");
    printf("Battleship Status : %s\n", (battleship->is_destroyed == 0) ? "DESTROYED (SUNK)" : "SURVIVED");
    printf("Final Position    : (%.2f, %.2f)\n", battleship->x, battleship->y);
    printf("Escorts Destroyed : %d / %d\n", destroyed_count, num_escorts);

    if (log_file) {
        fprintf(log_file, "\n========================================\n");
        fprintf(log_file, "         SIMULATION 1 SUMMARY           \n");
        fprintf(log_file, "========================================\n");
        fprintf(log_file, "Battleship Status : %s\n", (battleship->is_destroyed == 0) ? "DESTROYED (SUNK)" : "SURVIVED");
        fprintf(log_file, "Final Position    : (%.2f, %.2f)\n", battleship->x, battleship->y);
        fprintf(log_file, "Escorts Destroyed : %d / %d\n\n", destroyed_count, num_escorts);
        for (int i = 0; i < num_escorts; i++) {
            fprintf(log_file, "  Escort #%d at (%.2f, %.2f) - %s\n",
                    escorts[i].id, escorts[i].x, escorts[i].y,
                    (escorts[i].is_destroyed == 0) ? "DESTROYED" : "ACTIVE");
        }
    }
}
