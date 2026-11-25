#include "teamlist.h"
#include "team.h"
#include <stdlib.h>

// Nó da lista duplamente encadeada
struct node {
    Team *team;
    struct node *next;
    struct node *prev;
};

// Guarda o início, o fim e a quantidade de times na lista
struct teamlist {
    Node *first;
    Node *last;
    int size;
};

TeamList *tl_create(void) {
    TeamList *tl = (TeamList *)malloc(sizeof(*tl));
    if (tl == NULL)
        return NULL;

    tl->first = NULL;
    tl->last = NULL;
    tl->size = 0;

    return tl;
}

void tl_append(TeamList *tl, Team *t) {
    if (tl == NULL || t == NULL)
        return;

    Node *new_node = (Node *)malloc(sizeof(*new_node));
    if (new_node == NULL) {
        // Se falhar ao criar o nó, libera o time para não vazar memória
        team_free(t);
        return;
    }

    new_node->team = t;
    new_node->next = NULL;
    new_node->prev = tl->last;

    if (tl->size == 0)
        tl->first = new_node;
    else
        tl->last->next = new_node;

    tl->last = new_node;
    tl->size++;
}

/*
 * Retorna 1 se o time 'a' deve ficar na frente do time 'b' na classificação,
 * ou 0 caso contrário. Segue os critérios do campeonato na ordem de prioridade:
 * pontos, vitórias, saldo de gols, gols marcados e menor ID.
 */
static int team_comes_before(Team *a, Team *b) {
    if (a == NULL || b == NULL)
        return 0;

    // 1. Maior pontuação
    int pg_a = team_points(a);
    int pg_b = team_points(b);
    if (pg_a != pg_b)
        return pg_a > pg_b;

    // 2. Maior número de vitórias
    int v_a = team_wins(a);
    int v_b = team_wins(b);
    if (v_a != v_b)
        return v_a > v_b;

    // 3. Maior saldo de gols
    int s_a = team_goal_difference(a);
    int s_b = team_goal_difference(b);
    if (s_a != s_b)
        return s_a > s_b;

    // 4. Mais gols marcados
    int gm_a = team_goals_scored(a);
    int gm_b = team_goals_scored(b);
    if (gm_a != gm_b)
        return gm_a > gm_b;

    // 5. Menor ID (desempate final)
    return team_id(a) < team_id(b);
}

void tl_insert_ordered(TeamList *l, Team *t) {
    if (l == NULL || t == NULL)
        return;

    // Aqui não dá free em 't' se o malloc falhar, porque 't' ainda está na lista principal
    Node *new_node = (Node *)malloc(sizeof(*new_node));
    if (new_node == NULL)
        return;

    new_node->team = t;

    // Caso 1: lista vazia
    if (l->size == 0) {
        new_node->prev = NULL;
        new_node->next = NULL;
        l->first = new_node;
        l->last = new_node;
        l->size++;
        return;
    }

    // Avança na lista até achar o primeiro time que deve ficar atrás de 't'
    Node *current = l->first;
    while (current != NULL && team_comes_before(current->team, t)) {
        current = current->next;
    }

    // Caso 2: inserir no início (novo líder)
    if (current == l->first) {
        new_node->prev = NULL;
        new_node->next = l->first;
        l->first->prev = new_node;
        l->first = new_node;
    }
    // Caso 3: inserir no final (ficou atrás de todos)
    else if (current == NULL) {
        new_node->next = NULL;
        new_node->prev = l->last;
        l->last->next = new_node;
        l->last = new_node;
    }
    // Caso 4: inserir no meio (logo antes de 'current')
    else {
        new_node->next = current;
        new_node->prev = current->prev;
        current->prev->next = new_node;
        current->prev = new_node;
    }

    l->size++;
}

int tl_size(TeamList *tl) {
    return tl != NULL ? tl->size : 0;
}

Team *tl_get(TeamList *tl, int i) {
    if (tl == NULL || i < 0 || i >= tl->size)
        return NULL;

    Node *current = tl->first;
    for (int position = 0; position < i; position++)
        current = current->next;

    return current->team;
}

Team *tl_best_attack(TeamList *tl) {
    if (tl == NULL || tl->first == NULL)
        return NULL;

    Team *best_team = tl->first->team;
    int max_goals = team_goals_scored(best_team);

    Node *curr = tl->first->next;
    while (curr != NULL) {
        Team *t = curr->team;
        int goals = team_goals_scored(t);
        // Usa > (e não >=) para desempatar a favor de quem está mais bem classificado
        if (goals > max_goals) {
            best_team = t;
            max_goals = goals;
        }
        curr = curr->next;
    }

    return best_team;
}

Team *tl_best_defense(TeamList *tl) {
    if (tl == NULL || tl->first == NULL)
        return NULL;

    Team *best_team = tl->first->team;
    int min_goals = team_goals_conceded(best_team);

    Node *curr = tl->first->next;
    while (curr != NULL) {
        Team *t = curr->team;
        int goals = team_goals_conceded(t);
        // Usa < (e não <=) para desempatar a favor de quem está mais bem classificado
        if (goals < min_goals) {
            best_team = t;
            min_goals = goals;
        }
        curr = curr->next;
    }

    return best_team;
}

void tl_free(TeamList *tl) {
    if (tl == NULL)
        return;

    Node *current = tl->first;
    while (current != NULL) {
        // Guarda o próximo nó antes de dar free no atual
        Node *next = current->next;
        team_free(current->team);
        free(current);
        current = next;
    }

    free(tl);
}

void tl_free_nodes(TeamList *tl) {
    if (tl == NULL)
        return;

    Node *current = tl->first;
    while (current != NULL) {
        Node *next = current->next;
        // Dá free apenas no nó, sem apagar os structs Team da lista principal
        free(current);
        current = next;
    }

    free(tl);
}