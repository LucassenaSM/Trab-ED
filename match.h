#ifndef MATCH_H
#define MATCH_H

#include "team.h"
#include "teamlist.h"

// TAD que representa uma partida entre dois times, guardando o placar
// e os ponteiros para os respectivos structs Team.
typedef struct match Match;

/*
 * Cria uma nova partida buscando os times pelos IDs t1 e t2 na TeamList.
 * Recebe a lista de times, o id da partida, os ids dos times e os gols de cada um.
 * Retorna o ponteiro para a Match criada, ou NULL se algum dado for inválido
 * (IDs iguais/inexistentes, gols negativos ou erro de alocação).
 */
Match *match_create(TeamList *tl, int id, int t1, int t2, int golsT1, int golsT2);

// Funções de acesso - assumem que m não é NULL
int match_id(Match *m);
Team *match_t1(Match *m);
Team *match_t2(Match *m);
int match_golsT1(Match *m);
int match_golsT2(Match *m);

/*
 * Atualiza as estatísticas dos dois times (pontos, vitórias, empates,
 * derrotas e saldo de gols) com base no placar da partida.
 * Deve ser chamada apenas uma vez por partida para não duplicar os pontos.
 */
void match_apply_stats(Match *m);

/*
 * Libera a memória da struct Match.
 * Obs: não dá free nos times (Team), pois eles pertencem à TeamList.
 */
void match_free(Match *m);

#endif