#include "team.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Guarda os dados e as estatísticas acumuladas de um time
struct team {
    int id;
    char *name;
    int v;   // vitórias
    int e;   // empates
    int d;   // derrotas
    int gm;  // gols marcados
    int gs;  // gols sofridos
};

Team *team_create(int id, const char *name) {
    if (name == NULL)
        return NULL;

    Team *new_team = (Team *)malloc(sizeof(Team));
    if (new_team == NULL)
        return NULL;

    new_team->id = id;

    // Aloca espaço para uma cópia do nome (+1 por causa do '\0' no final)
    size_t name_len = strlen(name);
    new_team->name = (char *)malloc((name_len + 1) * sizeof(char));
    if (new_team->name == NULL) {
        // Se falhar ao alocar o nome, libera a struct já alocada para não vazar memória
        free(new_team);
        return NULL;
    }
    memcpy(new_team->name, name, name_len + 1);

    new_team->v = 0;
    new_team->e = 0;
    new_team->d = 0;
    new_team->gm = 0;
    new_team->gs = 0;

    return new_team;
}

int team_id(Team *t) {
    return t != NULL ? t->id : 0;
}

const char *team_name(Team *t) {
    return t != NULL ? t->name : NULL;
}

int team_wins(Team *t) {
    return t != NULL ? t->v : 0;
}

int team_draws(Team *t) {
    return t != NULL ? t->e : 0;
}

int team_losses(Team *t) {
    return t != NULL ? t->d : 0;
}

int team_goals_scored(Team *t) {
    return t != NULL ? t->gm : 0;
}

int team_goals_conceded(Team *t) {
    return t != NULL ? t->gs : 0;
}

int team_goal_difference(Team *t) {
    if (t == NULL)
        return 0;
    return t->gm - t->gs;
}

int team_points(Team *t) {
    if (t == NULL)
        return 0;
    return 3 * t->v + t->e;
}

Team *team_update_stats(Team *t, int golsT1, int golsT2) {
    if (t == NULL)
        return NULL;

    // golsT1 são os gols feitos por este time e golsT2 os gols sofridos
    t->gm += golsT1;
    t->gs += golsT2;

    if (golsT1 > golsT2) {
        t->v++;
    } else if (golsT1 < golsT2) {
        t->d++;
    } else {
        t->e++;
    }

    return t;
}

void team_free(Team *t) {
    if (t == NULL)
        return;

    // Libera primeiro o nome (alocado dentro da struct) e depois a própria struct
    free(t->name);
    free(t);
}