#include "simulation.h"
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/physics.h"
#include "files.h"

void run_simulation_1(Battleship *battleship, 
                      EscortShip escorts[], 
                      int num_escorts, 
                      Point path[], 
                      int num_points, 
                      double canvas_size) {
    
    FILE *log_file = open_log_file("simulation1_full_log.txt");

    for (int i = 0; i < num_points; i++) {
        battleship->x = path[i].x;
        battleship->y = path[i].y;

        log_step_header(log_file, i + 1, num_points, battleship->x, battleship->y);

        int hit_by_escort = 0;
        for (int j = 0; j < num_escorts; j++) {
            if (escorts[j].is_destroyed == 1) {
                if (is_in_escort_range(escorts[j], *battleship)) {
                    log_escort_attack(log_file, escorts[j].id, 1);
                    battleship->is_destroyed = 0; // 0 = destroy
                    hit_by_escort = 1;
                    break;
                }
            }
        }

        if (hit_by_escort) {
            printf("Battleship sunk! Ending simulation.\n");
            if (log_file) fprintf(log_file, "Battleship sunk! Ending simulation.\n");
            break;
        }

        for (int j = 0; j < num_escorts; j++) {
            if (escorts[j].is_destroyed == 1) {
                if (is_in_battleship_range(*battleship, escorts[j])) {
                    escorts[j].is_destroyed = 0; // 0 = destroy
                    log_battleship_counter(log_file, escorts[j].id);
                }
            }
        }

        int remaining_escorts = 0;
        for (int j = 0; j < num_escorts; j++) {
            if (escorts[j].is_destroyed == 1) {
                remaining_escorts++;
            }
        }

        if (remaining_escorts == 0) {
            log_victory(log_file, i + 1);
            break;
        }
    }

    log_summary(log_file, battleship, escorts, num_escorts);
    close_log_file(log_file);
    printf("\n✅ Log saved to 'simulation1_full_log.txt'\n");
}
