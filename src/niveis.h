#ifndef NIVEIS_H
#define NIVEIS_H

#define NUM_NIVEIS 3
#define MAX_PALAVRAS 14

typedef struct {
    int tam;                            /* tamanho da grade (tam x tam) */
    int num_dir;                        /* quantas direções da tabela este nível usa */
    int qtd_palavras;
    const char *palavras[MAX_PALAVRAS]; /* da maior para a menor */
    const char *texto;                  /* texto do nível; palavras do jogo entre *asteriscos* */
} Nivel;

extern Nivel niveis[NUM_NIVEIS];

#endif