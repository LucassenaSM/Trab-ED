#ifndef TEAM_H
#define TEAM_H

// TAD que representa um time e suas estatísticas no campeonato.
typedef struct team Team;

/*
 * Cria um novo time com o id e nome informados, iniciando todas as estatísticas em 0.
 * Faz uma cópia própria da string name (alocada dinamicamente).
 * Retorna o ponteiro para o Team criado, ou NULL se name for NULL ou faltar memória.
 */
Team *team_create(int id, const char *name);

// Retorna o nome do time (não dar free nesse ponteiro, pois pertence ao struct).
const char *team_name(Team *t);

// Funções de acesso das estatísticas - assumem que t não é NULL
int team_id(Team *t);
int team_wins(Team *t);
int team_draws(Team *t);
int team_losses(Team *t);
int team_goals_scored(Team *t);
int team_goals_conceded(Team *t);

// Calcula e retorna o saldo de gols (gols marcados - gols sofridos).
int team_goal_difference(Team *t);

// Calcula e retorna a pontuação total (3 por vitória + 1 por empate).
int team_points(Team *t);

/*
 * Atualiza os dados do time após uma partida, somando os gols marcados (golsT1),
 * os gols sofridos (golsT2) e incrementando vitórias, empates ou derrotas.
 * Retorna o próprio ponteiro t, ou NULL se t for NULL.
 */
Team *team_update_stats(Team *t, int golsT1, int golsT2);

// Libera toda a memória alocada pelo time (incluindo a cópia do nome).
void team_free(Team *t);

#endif