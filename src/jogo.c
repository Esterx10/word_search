#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include "jogo.h"

/* ===== Estado do jogo ===== */

Nivel nivel_atual;
int nivel_idx = 0;
int tam;
char grade[TAM_MAX][TAM_MAX];
Palavra pal[MAX_PALAVRAS];
int qtd_pal;
int ganhou = 0;
int mostrar_gabarito = 0;
int sel_ativa = 0;
int sel_l0, sel_c0, sel_l1, sel_c1;

static int num_dir;                 /* quantas direções o nível atual usa */
static double tempo_vitoria = 0;

int dir[8][2] = {
    {0, 1},    /* 0: direita */
    {1, 0},    /* 1: baixo */
    {1, 1},    /* 2: diagonal descendo para a direita */
    {0, -1},   /* 3: esquerda */
    {-1, 0},   /* 4: cima */
    {-1, -1},  /* 5: diagonal subindo para a esquerda */
    {1, -1},   /* 6: diagonal descendo para a esquerda */
    {-1, 1}    /* 7: diagonal subindo para a direita */
};

/* ===== Montar a grade ===== */

static void limpar_grade(void) {
    qtd_pal = 0;
    sel_ativa = 0;
    for (int linha = 0; linha < tam; linha++) {
        for (int coluna = 0; coluna < tam; coluna++) {
            grade[linha][coluna] = '.';
        }
    }
}

static void preencher_vazios(void) {
    for (int linha = 0; linha < tam; linha++) {
        for (int coluna = 0; coluna < tam; coluna++) {
            if (grade[linha][coluna] == '.') {
                grade[linha][coluna] = 'A' + rand() % 26;
            }
        }
    }
}

static int cabe(const char *palavra, int linha, int coluna, int d) {
    for (int i = 0; palavra[i] != '\0'; i++) {
        int l = linha + dir[d][0] * i;
        int c = coluna + dir[d][1] * i;
        if (l < 0 || l >= tam || c < 0 || c >= tam) return 0;          /* saiu da grade */
        if (grade[l][c] != '.' && grade[l][c] != palavra[i]) return 0; /* bateu em outra letra */
    }
    return 1;
}

static void escrever(const char *palavra, int linha, int coluna, int d) {
    for (int i = 0; palavra[i] != '\0'; i++) {
        grade[linha + dir[d][0] * i][coluna + dir[d][1] * i] = palavra[i];
    }
}

static int inserir(const char *palavra) {
    for (int tentativa = 0; tentativa < 1000; tentativa++) {
        int d = rand() % num_dir;
        int linha = rand() % tam;
        int coluna = rand() % tam;
        if (cabe(palavra, linha, coluna, d)) {
            escrever(palavra, linha, coluna, d);

            pal[qtd_pal].texto = palavra;
            pal[qtd_pal].linha = linha;
            pal[qtd_pal].coluna = coluna;
            pal[qtd_pal].d = d;
            pal[qtd_pal].achada = 0;
            qtd_pal++;

            return 1;
        }
    }
    return 0;
}

/* Confere se a grade tem as direções que o nível exige:
   - direita, baixo e diagonal (as 3 primeiras da tabela), cada uma que o nível usa;
   - no nível de 8 direções, também pelo menos uma palavra invertida (direções 3 a 7). */
static int tem_direcoes_obrigatorias(void) {
    int usada[8] = { 0 };

    for (int p = 0; p < qtd_pal; p++) {
        usada[pal[p].d] = 1;
    }

    for (int d = 0; d < num_dir && d < 3; d++) {
        if (!usada[d]) return 0;
    }

    if (num_dir == 8) {
        int invertida = 0;
        for (int d = 3; d < 8; d++) {
            if (usada[d]) invertida = 1;
        }
        if (!invertida) return 0;
    }

    return 1;
}

void novo_jogo(int idx) {
    nivel_idx = idx;
    ganhou = 0;

    nivel_atual = niveis[idx];
    tam = nivel_atual.tam;
    num_dir = nivel_atual.num_dir;

    int ok;
    int tentativas = 0;
    do {
        limpar_grade();
        ok = 1;
        for (int i = 0; i < nivel_atual.qtd_palavras; i++) {
            if (!inserir(nivel_atual.palavras[i])) {
                ok = 0;
                break;
            }
        }
        if (ok && !tem_direcoes_obrigatorias()) ok = 0;   /* faltou alguma direção: sorteia de novo */
        tentativas++;
    } while (!ok && tentativas < 1000);

    preencher_vazios();
}

/* ===== Jogar: mouse e vitória ===== */

static int celula_sob_mouse(int *linha, int *coluna) {
    Vector2 m = GetMousePosition();
    int cel = AREA_GRADE / tam;
    int area = cel * tam;

    if (m.x < GRADE_X || m.y < GRADE_Y) return 0;
    if (m.x >= GRADE_X + area || m.y >= GRADE_Y + area) return 0;

    *coluna = (int)(m.x - GRADE_X) / cel;
    *linha = (int)(m.y - GRADE_Y) / cel;
    return 1;
}

static void verificar_selecao(void) {
    for (int p = 0; p < qtd_pal; p++) {
        if (pal[p].achada) continue;

        int len = (int)strlen(pal[p].texto);
        int l0 = pal[p].linha;
        int c0 = pal[p].coluna;
        int l1 = l0 + dir[pal[p].d][0] * (len - 1);
        int c1 = c0 + dir[pal[p].d][1] * (len - 1);

        int normal = (sel_l0 == l0 && sel_c0 == c0 && sel_l1 == l1 && sel_c1 == c1);
        int inverso = (sel_l0 == l1 && sel_c0 == c1 && sel_l1 == l0 && sel_c1 == c0);

        if (normal || inverso) {
            pal[p].achada = 1;
            return;
        }
    }
}

static int contar_achadas(void) {
    int total = 0;
    for (int p = 0; p < qtd_pal; p++) {
        if (pal[p].achada) total++;
    }
    return total;
}

int palavra_achada(const char *chave) {
    for (int p = 0; p < qtd_pal; p++) {
        if (strcmp(pal[p].texto, chave) == 0) return pal[p].achada;
    }
    return 0;
}

void jogo_atualizar(void) {
    int l, c;

    if (IsKeyPressed(KEY_G)) mostrar_gabarito = !mostrar_gabarito;

    /* mouse: apertar, arrastar e soltar */
    if (!ganhou && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && celula_sob_mouse(&l, &c)) {
        sel_ativa = 1;
        sel_l0 = sel_l1 = l;
        sel_c0 = sel_c1 = c;
    }
    if (sel_ativa && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && celula_sob_mouse(&l, &c)) {
        sel_l1 = l;
        sel_c1 = c;
    }
    if (sel_ativa && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        verificar_selecao();
        sel_ativa = 0;
    }

    /* vitória e passagem de nível */
    if (!ganhou && qtd_pal > 0 && contar_achadas() == qtd_pal) {
        ganhou = 1;
        tempo_vitoria = GetTime();
    }
    if (ganhou) {
        if (nivel_idx < NUM_NIVEIS - 1) {
            if (GetTime() - tempo_vitoria > 2.0) novo_jogo(nivel_idx + 1);
        } else if (IsKeyPressed(KEY_ENTER)) {
            novo_jogo(0);
        }
    }
}