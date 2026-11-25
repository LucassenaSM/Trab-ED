# Fechando o Campeonato

Implementação em linguagem C para o fechamento do Campeonato Computacional de Futebol em execução em lote. O programa carrega os clubes de `times.csv` e os resultados das partidas de `partidas.csv`, apura o desempenho de cada clube, constrói a tabela de classificação ordenada, exibe o relatório e resumo no terminal e persiste a tabela em `classificacao.csv`.

---

## 1. Identificação dos Integrantes

- **Disciplina:** Estrutura de Dados  
- **Professor:** Prof. Thiago M. Paixão  
- **Integrantes:**
  - **Lucas Sena** — Matrícula: 20252BSI0038
  - **Nathan Figueiredo** — Matrícula: 20252BSI0020

---

## 2. Pré-requisitos, Compilação e Execução

O código foi desenvolvido em conformidade com o padrão C11 e projetado para o ambiente de teste oficial (**Linux Ubuntu 22.04 com GCC 11**), compilado via `make`.

### Pré-requisitos
- Compilador C: `gcc` (suporte a C11)
- Utilitário: `make`

### Comandos de Compilação e Execução

Compilar e executar diretamente:
```sh
make
```

Executar manualmente o binário gerado:
```sh
./campeonato
```

Limpar arquivos compilados (`.o`), binários (`campeonato`):
```sh
make clean
```

As diretivas de compilação no `Makefile` utilizam as flags `-Wall -Wextra -std=c11 -pedantic` com verificação estrita de boas práticas.

---

## 3. Arquivos de Entrada e Cenários de Teste

O programa é executado do início ao fim sem menus ou consultas interativas. Conforme a especificação, ele lê obrigatoriamente:
1. `times.csv`: Contém os times participantes (`ID,Nome`).
2. `partidas.csv`: Contém as partidas disputadas (`ID,Time1,Time2,GolsTime1,GolsTime2`).

São fornecidos 3 arquivos de cenário de teste. Para testar qualquer um dos cenários, basta copiá-lo para `partidas.csv`:

```sh
# Cenário 1: Campeonato não iniciado (0 partidas disputadas)
cp partidas_vazio.csv partidas.csv
./campeonato

# Cenário 2: Campeonato em andamento (20 das 45 partidas disputadas)
cp partidas_parcial.csv partidas.csv
./campeonato

# Cenário 3: Campeonato encerrado (todas as 45 partidas disputadas)
cp partidas_completo.csv partidas.csv
./campeonato
```

---

## 4. Estrutura do Repositório

```text
.
|-- Makefile                 # Script de compilação, linkedição, execução e limpeza
|-- README.md                # Documentação da solução, instruções e TADs
|-- main.c                   # Fluxo principal (Carga, Apuração, Classificação, Relatório)
|-- team.h                   # Interface do TAD Team
|-- team.c                   # Implementação concreta e privada do TAD Team
|-- teamlist.h               # Interface do TAD TeamList (lista duplamente encadeada)
|-- teamlist.c               # Implementação do TAD TeamList e ordenação
|-- match.h                  # Interface do TAD Match
|-- match.c                  # Implementação concreta e privada do TAD Match
|-- matchlist.h              # Interface do TAD MatchList (lista encadeada de partidas)
|-- matchlist.c              # Implementação do TAD MatchList e busca da maior goleada
|-- times.csv                # Arquivo com os 10 clubes do campeonato
|-- partidas.csv             # Arquivo ativo de partidas lido pelo programa
|-- partidas_vazio.csv       # Cenário com 0 partidas
|-- partidas_parcial.csv     # Cenário com 20 partidas
|-- partidas_completo.csv    # Cenário completo com 45 partidas
`-- classificacao.csv        # Arquivo CSV de saída gerado pelo programa
```

---

## 5. Tipos Abstratos de Dados (TADs)

Todas as estruturas de dados foram desenhadas de forma opaca: os tipos `struct` são declarados nos arquivos de cabeçalho (`.h`) via `typedef struct ...`, enquanto a definição completa de seus campos reside unicamente nos arquivos de implementação (`.c`). O acesso aos campos é realizado exclusivamente por meio de funções públicas.

### 5.1. TAD `Team` (`team.h` / `team.c`)
Representa um time de futebol e suas estatísticas.
- **Campos privados:** `id`, `name`, `v` (vitórias), `e` (empates), `d` (derrotas), `gm` (gols marcados), `gs` (gols sofridos).
- **Cálculo sob demanda:** Os campos de saldo de gols (`S`) e pontos ganhos (`PG`) **nunca são armazenados** na struct, sendo sempre calculados sob demanda pelas funções `team_goal_difference()` ($GM - GS$) e `team_points()` ($3V + E$).
- **Principais funções:**
  - `team_create(id, name)`: Aloca dinamicamente o time e a string do nome.
  - `team_update_stats(t, gols_pro, gols_contra)`: Atualiza `v`, `e`, `d`, `gm` e `gs`.
  - `team_points(t)`: Retorna $3V + E$.
  - `team_goal_difference(t)`: Retorna $GM - GS$.
  - `team_free(t)`: Desaloca o nome e a estrutura do time.

### 5.2. TAD `TeamList` (`teamlist.h` / `teamlist.c`)
Coleção de times implementada como uma **lista duplamente encadeada** com nós contendo ponteiros `prev` e `next`.
- **Principais funções:**
  - `tl_create()`: Inicializa a lista duplamente encadeada.
  - `tl_append(tl, t)`: Insere um time no final da lista (ordem de inserção/ID).
  - `tl_insert_ordered(l, t)`: Insere um time mantendo a lista ordenada pelos 5 critérios oficiais de classificação.
  - `tl_best_attack(tl)`: Localiza o time com maior número de gols marcados ($GM$). Como a lista fornecida já está ordenada por classificação, empates preservam o time melhor classificado.
  - `tl_best_defense(tl)`: Localiza o time com menor número de gols sofridos ($GS$), preservando o time melhor classificado em caso de empate.
  - `tl_free(tl)`: Libera os nós e também os objetos `Team` (usado na lista original).
  - `tl_free_nodes(tl)`: Libera apenas os nós da lista, preservando os objetos `Team` (usado na lista ordenada).

### 5.3. TAD `Match` (`match.h` / `match.c`)
Representa uma partida entre dois times.
- **Campos privados:** `id`, ponteiros para os dois times (`t1`, `t2`), e o placar (`golsT1`, `golsT2`).
- **Principais funções:**
  - `match_create(tl, id, t1_id, t2_id, g1, g2)`: Localiza os ponteiros dos times em `tl` pelos IDs e aloca a partida.
  - `match_apply_stats(m)`: Aplica o resultado bilateralmente aos times via `team_update_stats()`.
  - `match_free(m)`: Libera a estrutura da partida (sem liberar os times, que pertencem à `TeamList`).

### 5.4. TAD `MatchList` (`matchlist.h` / `matchlist.c`)
Coleção de partidas disputadas mantida como uma **lista encadeada**.
- **Principais funções:**
  - `ml_create()`: Inicializa a lista encadeada de partidas.
  - `ml_append(ml, m)`: Insere a partida no final da lista, mantendo a ordem original de leitura do arquivo.
  - `ml_biggest_victory(ml)`: Percorre a lista determinando a partida de maior saldo de gols ($|golsT1 - golsT2|$). Em caso de empate, mantém a primeira partida lida do arquivo.
  - `ml_free(ml)`: Libera todas as partidas e os nós da lista.

---

## 6. Decisões de Implementação e Algoritmos

### 6.1. As Quatro Etapas do Programa
1. **Carga:** Abre e faz o parse de `times.csv` e `partidas.csv`, populando as listas `tl` (times) e `ml` (partidas).
2. **Apuração:** Percorre as partidas da lista `ml` e invoca `match_apply_stats()`, que atualiza as vitórias, empates, derrotas, gols marcados e sofridos de cada time.
3. **Classificação:** Constrói uma nova lista duplamente encadeada `l` através da inserção ordenada dos times já apurados.
4. **Relatório:** Grava a tabela no arquivo `classificacao.csv`, imprime a tabela formatada no terminal e exibe o resumo com as estatísticas do campeonato.
