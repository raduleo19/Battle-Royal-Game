// Copyright 2018 Rica Radu Leonard
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int hp;
    int stamina;
    int dmg;
    int x, y;
    int dry;
    int eliminated;
} elf;

typedef struct {
    int height;
    int dmg;
} cell;

void alloc_arr(cell ***glacier, int size) {
    *glacier = malloc(size * sizeof(cell *));
    for (int i = 0; i < size; ++i) {
        (*glacier)[i] = malloc(size * sizeof(cell));
    }
}

void realloc_arr(cell ***glacier, int size) {
    for (int i = 0; i < size; ++i) {
        (*glacier)[i] = realloc((*glacier)[i], size * sizeof(cell));
    }
    *glacier = realloc(*glacier, size * sizeof(cell *));
}

void free_memory(int num_players, elf *elfs, int *sorted_index, int radius,
                 int meltdowns, cell **glacier) {
    for (int i = 0; i < num_players; ++i) {
        free(elfs[i].name);
    }
    free(sorted_index);
    free(elfs);
    for (int i = 0; i < 2 * (radius - meltdowns) + 1; ++i) {
        free(glacier[i]);
    }
    free(glacier);
}

void int_swap(int *a, int *b) {
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

void print_scoreboard(int num_players, elf *elfs, int *sorted_index) {
    // Sort indexes of elfs by required criteria with bubble sort
    for (int i = 0; i < num_players; ++i) {
        for (int j = i + 1; j < num_players; ++j) {
            if (elfs[sorted_index[i]].dry < elfs[sorted_index[j]].dry) {
                int_swap(&sorted_index[i], &sorted_index[j]);
            } else if (elfs[sorted_index[i]].dry == elfs[sorted_index[j]].dry) {
                if (elfs[sorted_index[i]].eliminated <
                    elfs[sorted_index[j]].eliminated) {
                    int_swap(&sorted_index[i], &sorted_index[j]);
                } else if (elfs[sorted_index[i]].eliminated ==
                           elfs[sorted_index[j]].eliminated) {
                    if (strcmp(elfs[sorted_index[i]].name,
                               elfs[sorted_index[j]].name) > 0) {
                        int_swap(&sorted_index[i], &sorted_index[j]);
                    }
                }
            }
        }
    }

    printf("SCOREBOARD:\n");
    for (int i = 0; i < num_players; ++i) {
        int index = sorted_index[i];
        if (elfs[index].dry) {
            printf("%s\tDRY\t%d\n", elfs[index].name, elfs[index].eliminated);
        } else {
            printf("%s\tWET\t%d\n", elfs[index].name, elfs[index].eliminated);
        }
    }
}

int distance_squared(int first_x, int first_y, int second_x, int second_y) {
    return (first_x - second_x) * (first_x - second_x) +
           (first_y - second_y) * (first_y - second_y);
}

void send_back_home(elf *winner, elf *loser, int first) {
    printf("%s sent %s back home.\n", (*winner).name, (*loser).name);
    (*winner).eliminated++;
    (*winner).stamina += (*loser).stamina;
    (*winner).hp -= ((*loser).hp / (*winner).dmg + first) * (*loser).dmg;
    (*loser).stamina = 0;
    (*loser).dry = 0;
}

void move(int num_players, elf *elfs, int id, char move_dir, cell **glacier,
          int radius, int meltdowns, int *players_alive) {
    int new_x;
    int new_y;
    if (move_dir == 'U') {
        new_x = elfs[id].x - 1;
        new_y = elfs[id].y;
    } else if (move_dir == 'D') {
        new_x = elfs[id].x + 1;
        new_y = elfs[id].y;
    } else if (move_dir == 'L') {
        new_x = elfs[id].x;
        new_y = elfs[id].y - 1;
    } else if (move_dir == 'R') {
        new_x = elfs[id].x;
        new_y = elfs[id].y + 1;
    }
    if ((new_x - meltdowns >= 0 &&
         new_x - meltdowns < 2 * (radius - meltdowns) + 1) &&
        (new_y - meltdowns >= 0 &&
         new_y - meltdowns < 2 * (radius - meltdowns) + 1)) {
        int needed_stamina =
            abs(glacier[elfs[id].x - meltdowns][elfs[id].y - meltdowns].height -
                glacier[new_x - meltdowns][new_y - meltdowns].height);
        if (elfs[id].stamina >= needed_stamina) {
            elfs[id].stamina -= needed_stamina;
            elfs[id].x = new_x;
            elfs[id].y = new_y;
            if (distance_squared(new_x, new_y, radius, radius) >
                (radius - meltdowns) * (radius - meltdowns)) {
                elfs[id].dry = 0;
                printf("%s fell off the glacier.\n", elfs[id].name);
                --(*players_alive);
            }
            // If found better gloves, equip them
            if (glacier[new_x - meltdowns][new_y - meltdowns].dmg >
                elfs[id].dmg) {
                int_swap(&glacier[new_x - meltdowns][new_y - meltdowns].dmg,
                         &elfs[id].dmg);
            }
            // Check if there is an enemy
            for (int i = 0; i < num_players; ++i) {
                if (elfs[i].dry) {
                    if (i != id && elfs[i].x == new_x && elfs[i].y == new_y) {
                        --(*players_alive);
                        if (elfs[id].stamina >= elfs[i].stamina) {
                            if (((elfs[i].hp + elfs[id].dmg - 1) /
                                 elfs[id].dmg) -
                                    1 <
                                ((elfs[id].hp + elfs[i].dmg - 1) /
                                 elfs[i].dmg)) {
                                send_back_home(&elfs[id], &elfs[i], 0);
                            } else {
                                send_back_home(&elfs[i], &elfs[id], 1);
                                return;
                            }
                        } else {
                            if (((elfs[i].hp + elfs[id].dmg - 1) /
                                 elfs[id].dmg) >
                                ((elfs[id].hp + elfs[i].dmg - 1) /
                                 elfs[i].dmg) -
                                    1) {
                                send_back_home(&elfs[i], &elfs[id], 0);
                                return;
                            } else {
                                send_back_home(&elfs[id], &elfs[i], 1);
                            }
                        }
                    }
                }
            }
        }
    }
}

void snowstorm(int num_players, elf *elfs, int data_package,
               int *players_alive) {
    int dmg = (data_package >> 24) & 255;
    int radius = (data_package >> 16) & 255;
    int y = (data_package >> 8) & 255;
    int x = data_package & 255;
    for (int i = 0; i < num_players; ++i) {
        if (elfs[i].dry == 1) {
            if (distance_squared(x, y, elfs[i].x, elfs[i].y) <=
                radius * radius) {
                if (elfs[i].hp <= dmg) {
                    elfs[i].hp = 0;
                    elfs[i].dry = 0;
                    printf("%s was hit by snowstorm.\n", elfs[i].name);
                    --(*players_alive);
                } else {
                    elfs[i].hp -= dmg;
                }
            }
        }
    }
}

void meltdown(int num_players, elf *elfs, int radius, cell ***glacier,
              int *meltdowns, int stamina, int *players_alive) {
    // Shift data to North-West
    for (int i = 0; i < 2 * (radius - *meltdowns); ++i) {
        for (int j = 0; j < 2 * (radius - *meltdowns); ++j) {
            (*glacier)[i][j] = (*glacier)[i + 1][j + 1];
        }
    }

    free((*glacier)[2 * (radius - *meltdowns) - 1]);
    free((*glacier)[2 * (radius - *meltdowns)]);
    ++(*meltdowns);
    realloc_arr(glacier, 2 * (radius - *meltdowns) + 1);

    for (int i = 0; i < num_players; ++i) {
        if (elfs[i].dry) {
            if (distance_squared(elfs[i].x, elfs[i].y, radius, radius) >
                (radius - *meltdowns) * (radius - *meltdowns)) {
                elfs[i].dry = 0;
                printf("%s got wet because of global warming.\n", elfs[i].name);
                --(*players_alive);
            } else {
                elfs[i].stamina += stamina;
            }
        }
    }
}

int main() {
    freopen("snowfight.in", "r", stdin);
    freopen("snowfight.out", "w", stdout);
    int radius, num_players;
    cell **glacier;
    elf *elfs;

    scanf("%d%d", &radius, &num_players);
    alloc_arr(&glacier, 2 * radius + 1);
    elfs = malloc(num_players * sizeof(elf));

    for (int i = 0; i < 2 * radius + 1; ++i) {
        for (int j = 0; j < 2 * radius + 1; ++j) {
            scanf("%d%d", &glacier[i][j].height, &glacier[i][j].dmg);
        }
    }

    int meltdowns = 0;
    int players_alive = num_players;

    char name[200] = "";
    for (int i = 0; i < num_players; ++i) {
        int x, y;
        scanf("%s%d%d%d%d", name, &x, &y, &elfs[i].hp, &elfs[i].stamina);
        elfs[i].name = malloc(strlen(name) + 1);
        snprintf(elfs[i].name, strlen(name) + 1, "%s", name);
        elfs[i].dry = 1;
        elfs[i].dmg = glacier[x][y].dmg;
        glacier[x][y].dmg = -1;
        elfs[i].x = x;
        elfs[i].y = y;
        elfs[i].eliminated = 0;
        if (distance_squared(x, y, radius, radius) > radius * radius) {
            printf("%s has missed the glacier.\n", elfs[i].name);
            elfs[i].dry = 0;
            --players_alive;
        }
    }

    int *sorted_index = malloc(num_players * sizeof(int));
    for (int i = 0; i < num_players; ++i) {
        sorted_index[i] = i;
    }

    char command[17];
    while (scanf("%s", command) != EOF) {
        if (players_alive == 1) {
            for (int i = 0; i < num_players; ++i) {
                if (elfs[i].dry) {
                    printf("%s has won.\n", elfs[i].name);
                    break;
                }
            }
            break;
        }
        if (strcmp(command, "MOVE") == 0) {
            int id;
            char move_dir;
            scanf("%d ", &id);
            while (scanf("%c", &move_dir) && move_dir != '\n') {
                if (elfs[id].dry == 1) {
                    move(num_players, elfs, id, move_dir, glacier, radius,
                         meltdowns, &players_alive);
                }
            }
        } else if (strcmp(command, "SNOWSTORM") == 0) {
            int data_package;
            scanf("%d", &data_package);
            snowstorm(num_players, elfs, data_package, &players_alive);
        } else if (strcmp(command, "PRINT_SCOREBOARD") == 0) {
            print_scoreboard(num_players, elfs, sorted_index);
        } else if (strcmp(command, "MELTDOWN") == 0) {
            int stamina;
            scanf("%d", &stamina);
            meltdown(num_players, elfs, radius, &glacier, &meltdowns, stamina,
                     &players_alive);
        }
    }
    free_memory(num_players, elfs, sorted_index, radius, meltdowns, glacier);
    return 0;
}
