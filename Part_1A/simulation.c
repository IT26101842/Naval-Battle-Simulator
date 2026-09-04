#include <stdio.h>
#include "simulation.h"
#include "physics.h"
#include "files.h"

#define GRID_SIZE 20

// Function for display the canvas,battleship,escortships and the sea
void display_canvas(Battlefield *bf) {
    char grid[GRID_SIZE][GRID_SIZE];

    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            grid[r][c] = '.';
        }
    }

    // for loop for put escortship in canvas
    for (int i = 0; i < bf->num_escorts; i++) {
        int r = (int)((bf->escorts[i].y / bf->canvas_size) * (GRID_SIZE - 1));
        int c = (int)((bf->escorts[i].x / bf->canvas_size) * (GRID_SIZE - 1));
        r = (GRID_SIZE - 1) - r;
        if (r >= 0 && r < GRID_SIZE && c >= 0 && c < GRID_SIZE) {
            grid[r][c] = 'E';
        }
    }

    // putting battleship in the canvas
    int b_r = (int)((bf->battleship.y / bf->canvas_size) * (GRID_SIZE - 1));
    int b_c = (int)((bf->battleship.x / bf->canvas_size) * (GRID_SIZE - 1));
    b_r = (GRID_SIZE - 1) - b_r;
    if (b_r >= 0 && b_r < GRID_SIZE && b_c >= 0 && b_c < GRID_SIZE) {
        grid[b_r][b_c] = 'B';
    }

    printf("\n--- BATTLEFIELD CANVAS (0,0) to (%.0f, %.0f) ---\n", bf->canvas_size, bf->canvas_size);
    printf("   +");
    for (int c = 0; c < GRID_SIZE; c++) printf("--");
    printf("+\n");

    for (int r = 0; r < GRID_SIZE; r++) {
        printf("   |");
        for (int c = 0; c < GRID_SIZE; c++) {
            printf(" %c", grid[r][c]);
        }
        printf(" |\n");
    }

    printf("   +");
    for (int c = 0; c < GRID_SIZE; c++) printf("--");
    printf("+\n");
    printf("   Legend: [B] = Battleship | [E] = Escort Ship | [.] = Sea\n\n");
}

void run_part1_A(Battlefield *bf) {
    save_initial_conditions("part1a_initial_conditions.txt", bf);
    display_canvas(bf);

    FILE *results_file = fopen("part1a_battle_log.txt", "w");
    if (results_file) {
        fprintf(results_file, "=== BATTLE ENGAGEMENT LOG ===\n\n");
    }

    // Escort Ship Attacks records
    printf("=========================================\n");
    printf("          ESCORT SHIP ATTACKS            \n");
    printf("-----------------------------------------\n");
    if (results_file) fprintf(results_file, "--- ESCORT SHIP ATTACKS ---\n");

    int battleship_sunk = 0;
    int sunk_by_id = -1;

    for (int i = 0; i < bf->num_escorts; i++) {
        if (is_in_escort_range(bf->escorts[i], bf->battleship)) {
            printf("💥 Escort Ship ID %d fired and HIT Battleship!\n", bf->escorts[i].id);
            if (results_file) {
                fprintf(results_file, "Escort Ship ID %d HIT Battleship!\n", bf->escorts[i].id);
            }
            battleship_sunk = 1;
            sunk_by_id = bf->escorts[i].id;
            bf->battleship.is_destroyed = 1;
            break;
        }
    }

    if (!battleship_sunk) {
        printf("  (No Escort Ships had Battleship in range)\n");
        if (results_file) fprintf(results_file, "  (None)\n");
    } else {
        printf("\n💀 Result: Battleship has been SUNK by Escort Ship ID %d!\n", sunk_by_id);
        if (results_file) fprintf(results_file, "\nResult: Battleship SUNK by Escort ID %d\n", sunk_by_id);
    }

    // Battleship Attacks records
    printf("\n=========================================\n");
    printf("          BATTLESHIP ATTACKS             \n");
    printf("-----------------------------------------\n");
    if (results_file) fprintf(results_file, "\n--- BATTLESHIP ATTACKS ---\n");

    if (battleship_sunk) {
        printf("  (Battleship destroyed. Counter-attack aborted.)\n");
        if (results_file) fprintf(results_file, "  (Battleship destroyed. Counter-attack aborted.)\n");
    } else {
        int hits = 0;
        double max_time = 0.0;

        for (int i = 0; i < bf->num_escorts; i++) {
            if (is_in_battleship_range(bf->battleship, bf->escorts[i])) {
                bf->escorts[i].is_destroyed = 1;
                hits++;
                double time_to_hit = calculate_time_of_flight(bf->battleship.max_velocity, 45.0);
                if (time_to_hit > max_time) max_time = time_to_hit;

                printf("🎯 Battleship destroyed Escort ID %d (Flight Time: %.2f s)\n", bf->escorts[i].id, time_to_hit);
                if (results_file) {
                    fprintf(results_file, "Escort ID %d destroyed | Time: %.2f s\n", bf->escorts[i].id, time_to_hit);
                }
            }
        }

        if (hits == 0) {
            printf("  (No Escort Ships were in Battleship's range)\n");
            if (results_file) fprintf(results_file, "  (No Escort Ships in range)\n");
        } else {
            printf("\nSummary: Total Escorts Destroyed = %d | Battle Time = %.2f s\n", hits, max_time);
            if (results_file) {
                fprintf(results_file, "\nSummary: Destroyed = %d | Battle Time = %.2f s\n", hits, max_time);
            }
        }
    }

    if (results_file) fclose(results_file);
    save_final_conditions("part1a_final_conditions.txt", bf);
    printf("\nOutputs successfully saved to text files.\n");
}
