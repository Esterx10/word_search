#ifndef JOGO_H
#define JOGO_H

#include "config.h"
#include "niveis.h"

/* Uma palavra escondida na grade */
typedef struct {
    const char *texto;
    int linha, coluna, d;    /* onde começa e para onde vai */
    int achada;
} Palavra;

/* Estado do jogo: a interface só LÊ essas variáveis */
extern Nivel nivel_atual;
extern int nivel_idx;                   /* qual nível está rolando (0, 1 ou 2) */
extern int tam;                         /* tamanho da grade do nível atual */
extern char grade[TAM_MAX][TAM_MAX];
extern Palavra pal[MAX_PALAVRAS];
extern int qtd_pal;
extern int ganhou;                      /* 1 quando achou todas as palavras */
extern int mostrar_gabarito;
extern int sel_ativa;                   /* 1 enquanto o jogador está arrastando */
extern int sel_l0, sel_c0, sel_l1, sel_c1;
extern int dir[8][2];                   /* tabela de direções */

void novo_jogo(int idx);                /* monta a grade do nível idx */
void jogo_atualizar(void);              /* mouse, teclado e passagem de nível */
int palavra_achada(const char *chave);

#endif