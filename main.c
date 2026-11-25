#include "match.h"
#include "matchlist.h"
#include "team.h"
#include "teamlist.h"
#include <stdio.h>

/* 
 * Calcula quantos bytes extras de continuação UTF-8 existem na string.
 * Necessário porque printf("%-15s") conta bytes e não caracteres visuais:
 * letras acentuadas ocupam 2+ bytes, o que desalinharia as colunas da tabela.
 * Em UTF-8, bytes de continuação sempre começam com os bits 10xxxxxx (0x80).
 */
static int utf8_continuation_bytes(const char *s) {
    int n = 0;
    if (s == NULL)
        return 0;

    for (const unsigned char *p = (const unsigned char *)s; *p; p++) {
        if ((*p & 0xC0) == 0x80)
            n++;
    }
    return n;
}

int main(void) {
    /* =========================================================================
     * ETAPA 1: CARGA E PARSING DOS DADOS
     * ========================================================================= */
    FILE *f = fopen("times.csv", "rt");
    if (f == NULL)
        return 1;

    /* O especificador '*' lê e descarta a linha de cabeçalho sem alocar buffer */
    if (fscanf(f, "%*[^\n]\n") < 0) {
        fclose(f);
        return 1;
    }

    TeamList *tl = tl_create();
    if (tl == NULL) {
        fclose(f);
        return 1;
    }

    int id;
    char name[50];
    /* [^\r\n] evita capturar o '\r' caso o CSV use quebra de linha padrão Windows (CRLF) */
    while (fscanf(f, "%d,%49[^\r\n]", &id, name) == 2) {
        Team *t = team_create(id, name);
        if (t == NULL) {
            tl_free(tl);
            fclose(f);
            return 1;
        }
        tl_append(tl, t);
    }
    fclose(f);

    FILE *fp = fopen("partidas.csv", "rt");
    if (fp == NULL) {
        tl_free(tl);
        return 1;
    }

    if (fscanf(fp, "%*[^\n]\n") < 0) {
        fclose(fp);
        tl_free(tl);
        return 1;
    }

    MatchList *ml = ml_create();
    if (ml == NULL) {
        fclose(fp);
        tl_free(tl);
        return 1;
    }

    int idMatch, t1, t2, golsT1, golsT2;
    while (fscanf(fp, "%d,%d,%d,%d,%d", &idMatch, &t1, &t2, &golsT1, &golsT2) == 5) {
        /* match_create valida internamente se os IDs dos times existem em 'tl' */
        Match *m = match_create(tl, idMatch, t1, t2, golsT1, golsT2);
        if (m != NULL) {
            ml_append(ml, m);
        }
    }
    fclose(fp);

    /* =========================================================================
     * ETAPA 2: APURAÇÃO DAS ESTATÍSTICAS
     * ========================================================================= */
    /* Percorre o histórico de partidas atualizando pontos, gols e saldos nos structs Team */
    for (int i = 0; i < ml_size(ml); i++) {
        match_apply_stats(ml_get(ml, i));
    }

    /* =========================================================================
     * ETAPA 3: CLASSIFICAÇÃO DOS TIMES
     * =========================================================================
     * Cria uma visão ordenada ('l') sem duplicar os structs Team na memória.
     * Os ponteiros são compartilhados com a lista original ('tl') e inseridos
     * já na posição correta conforme os critérios de desempate do TAD TeamList.
     */
    TeamList *l = tl_create();
    if (l == NULL) {
        ml_free(ml);
        tl_free(tl);
        return 1;
    }

    for (int i = 0; i < tl_size(tl); i++) {
        Team *t = tl_get(tl, i);
        tl_insert_ordered(l, t);
    }

    /* =========================================================================
     * ETAPA 4: EXPORTAÇÃO E RELATÓRIO FINAL
     * ========================================================================= */
    FILE *classification = fopen("classificacao.csv", "wt");
    if (classification != NULL) {
        fprintf(classification, "Pos,ID,Time,V,E,D,GM,GS,S,PG\n");
        for (int i = 0; i < tl_size(l); i++) {
            Team *t = tl_get(l, i);
            fprintf(classification, "%d,%d,%s,%d,%d,%d,%d,%d,%d,%d\n",
                    i + 1, team_id(t), team_name(t),
                    team_wins(t), team_draws(t), team_losses(t),
                    team_goals_scored(t), team_goals_conceded(t),
                    team_goal_difference(t), team_points(t));
        }
        fclose(classification);
    }

    printf("Pos ID Time            V   E   D   GM  GS  S   PG\n");
    for (int i = 0; i < tl_size(l); i++) {
        Team *t = tl_get(l, i);
        printf("%-3d %-2d %-15s", i + 1, team_id(t), team_name(t));
        /* Compensa o deslocamento causado por caracteres acentuados (UTF-8) */
        for (int j = 0; j < utf8_continuation_bytes(team_name(t)); j++)
            putchar(' ');
        printf(" %-3d %-3d %-3d %-3d %-3d %-3d %-3d\n",
               team_wins(t), team_draws(t), team_losses(t),
               team_goals_scored(t), team_goals_conceded(t),
               team_goal_difference(t), team_points(t));
    }

    printf("\n===== Resumo do campeonato =====\n");

    int matches_count = ml_size(ml);
    /* Campeonato de 10 times em turno único totaliza 45 jogos (combinação 10, 2 a 2) */
    printf("Partidas disputadas: %d de 45\n", matches_count);

    if (matches_count > 0) {
        /* Como 'l' já está ordenada, o índice 0 garante o líder/campeão */
        Team *leader = tl_get(l, 0);
        int points = team_points(leader);
        Team *bestAttackTeam = tl_best_attack(l);
        Team *bestDefenseTeam = tl_best_defense(l);
        Match *biggestVictory = ml_biggest_victory(ml);

        if (matches_count == 45)
            printf("Campeão: %s (%d pontos)\n", team_name(leader), points);
        else
            printf("Líder: %s (%d pontos)\n", team_name(leader), points);

        printf("Melhor ataque: %s (%d gols marcados)\n",
               team_name(bestAttackTeam), team_goals_scored(bestAttackTeam));
        printf("Melhor defesa: %s (%d gols sofridos)\n",
               team_name(bestDefenseTeam), team_goals_conceded(bestDefenseTeam));
        printf("Maior goleada: %s %d x %d %s\n",
               team_name(match_t1(biggestVictory)),
               match_golsT1(biggestVictory),
               match_golsT2(biggestVictory),
               team_name(match_t2(biggestVictory)));
    } else {
        printf("Nenhuma partida foi disputada.\n");
    }

    /* =========================================================================
     * LIBERAÇÃO DE MEMÓRIA
     * =========================================================================
     * Como 'l' e 'tl' apontam para os mesmos structs Team, liberamos primeiro
     * apenas a estrutura de nós de 'l' para evitar double-free ao liberar 'tl'.
     */
    ml_free(ml);
    tl_free_nodes(l);
    tl_free(tl);

    return 0;
}