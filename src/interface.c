#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "jogo.h"
#include "interface.h"

/* ===== Fontes e cores ===== */

   static int fonte_texto[NUM_NIVEIS] = { 36, 32, 30 };   /* tamanho da letra do texto em cada nível */

static Font fonte_letras;     /* fonte das letras da grade */
static Font fonte_normal;     /* fonte do texto */
static Color cor_texto = { 45, 25, 70, 255 };       /* roxo bem escuro: texto normal */
static Color cor_palavra = { 110, 30, 170, 255 };   /* roxo escuro: palavras do jogo e título */

static Font carregar_fonte(const char *arquivo, int tamanho) {
    if (!FileExists(arquivo)) return GetFontDefault();   /* sem o arquivo, usa a fonte padrão */

    int codigos[224];
    for (int i = 0; i < 224; i++) codigos[i] = 32 + i;   /* inclui os acentos */

    Font f = LoadFontEx(arquivo, tamanho, codigos, 224);
    SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
    return f;
}

void interface_iniciar(void) {
    fonte_letras = carregar_fonte("fonts/LilitaOne-Regular.ttf", 64);
    fonte_normal = carregar_fonte("fonts/Poppins-Regular.ttf", 32);
}

void interface_encerrar(void) {
    UnloadFont(fonte_letras);
    UnloadFont(fonte_normal);
}

/* ===== Texto simples ===== */

static int largura_texto(const char *txt, int tamanho) {
    return (int)MeasureTextEx(fonte_normal, txt, (float)tamanho, 1).x;
}

static void escrever_texto(const char *txt, int x, int y, int tamanho, Color cor) {
    Vector2 pos = { (float)x, (float)y };
    DrawTextEx(fonte_normal, txt, pos, (float)tamanho, 1, cor);
}

static void texto_centralizado(Font f, const char *txt, int tamanho, int cx, int y, Color cor) {
    Vector2 med = MeasureTextEx(f, txt, (float)tamanho, 1);
    Vector2 pos = { cx - med.x / 2, (float)y };
    DrawTextEx(f, txt, pos, (float)tamanho, 1, cor);
}

/* ===== Texto do nível (com as palavras marcadas) ===== */

/* Separa um pedaço do texto em: antes, palavra marcada e depois.
   Ex.: "*MEMORIA|MEMÓRIA*," -> pre="" chave="MEMORIA" exib="MEMÓRIA" suf=","
   Devolve 1 se tinha palavra marcada, 0 se era só texto comum. */
static int separar_token(const char *tok, char *pre, char *chave, char *exib, char *suf) {
    const char *a = strchr(tok, '*');
    if (a == NULL) return 0;
    const char *b = strchr(a + 1, '*');
    if (b == NULL) return 0;

    int n = (int)(a - tok);
    memcpy(pre, tok, n);
    pre[n] = '\0';

    char dentro[96];
    n = (int)(b - a - 1);
    memcpy(dentro, a + 1, n);
    dentro[n] = '\0';

    char *barra = strchr(dentro, '|');
    if (barra != NULL) {
        *barra = '\0';
        strcpy(chave, dentro);
        strcpy(exib, barra + 1);
    } else {
        strcpy(chave, dentro);
        strcpy(exib, dentro);
    }

    strcpy(suf, b + 1);
    return 1;
}

/* Lê o próximo pedaço do texto (até o próximo espaço). Devolve 0 se o texto acabou. */
static int proximo_token(const char *t, int *i, char *tok) {
    while (t[*i] == ' ') (*i)++;
    if (t[*i] == '\0') return 0;

    int n = 0;
    while (t[*i] != ' ' && t[*i] != '\0' && n < 95) tok[n++] = t[(*i)++];
    tok[n] = '\0';
    return 1;
}

/* Largura de um pedaço na tela (sem contar os asteriscos). */
static int largura_token(const char *tok, int fonte) {
    char pre[96], chave[96], exib[96], suf[96];

    if (!separar_token(tok, pre, chave, exib, suf)) return largura_texto(tok, fonte);
    return largura_texto(pre, fonte) + largura_texto(exib, fonte) + largura_texto(suf, fonte);
}

/* Desenha um pedaço; se for palavra do jogo já achada, fica cinza e riscada. */
static void desenhar_token(const char *tok, int x, int y, int fonte) {
    char pre[96], chave[96], exib[96], suf[96];

    if (!separar_token(tok, pre, chave, exib, suf)) {
        escrever_texto(tok, x, y, fonte, cor_texto);
        return;
    }

    int wp = largura_texto(pre, fonte);
    int wm = largura_texto(exib, fonte);
    int achada = palavra_achada(chave);

    escrever_texto(pre, x, y, fonte, cor_texto);
    escrever_texto(exib, x + wp, y, fonte, achada ? GRAY : cor_palavra);
    escrever_texto(suf, x + wp + wm, y, fonte, cor_texto);

    if (achada) {
        Vector2 ini = { (float)(x + wp), (float)(y + fonte * 55 / 100) };
        Vector2 fim = { (float)(x + wp + wm), (float)(y + fonte * 55 / 100) };
        DrawLineEx(ini, fim, 2, RED);
    }
}

/* Desenha o texto do nível, linha por linha, cada linha centralizada na coluna. */
static void desenhar_texto(void) {
    const char *t = nivel_atual.texto;
    int fonte = fonte_texto[nivel_idx];
    int passo = fonte * 125 / 100;
    int espaco = largura_texto(" ", fonte);
    int centro = TEXTO_X + TEXTO_LARGURA / 2;
    int y = TEXTO_Y;
    int i = 0;
    char tok[96];

    while (1) {
        int comeco = i;
        int largura_linha = 0;
        int qtd = 0;
        int j = i;

        /* 1) descobre quantas palavras cabem nesta linha e qual a largura dela */
        while (1) {
            int antes = j;
            if (!proximo_token(t, &j, tok)) break;

            int w = largura_token(tok, fonte);
            int nova = largura_linha + (qtd > 0 ? espaco : 0) + w;
            if (qtd > 0 && nova > TEXTO_LARGURA) {   /* não coube: fica pra próxima linha */
                j = antes;
                break;
            }
            largura_linha = nova;
            qtd++;
        }
        if (qtd == 0) break;

        /* 2) desenha a linha, começando de modo que ela fique no meio da coluna */
        int x = centro - largura_linha / 2;
        j = comeco;
        for (int q = 0; q < qtd; q++) {
            proximo_token(t, &j, tok);
            desenhar_token(tok, x, y, fonte);
            x += largura_token(tok, fonte) + espaco;
        }

        i = j;
        y += passo;
    }
}

/* ===== Grade e marcações ===== */

static void pintar_palavra(int p, Color cor) {
    int cel = AREA_GRADE / tam;

    for (int i = 0; i < (int)strlen(pal[p].texto); i++) {
        int l = pal[p].linha + dir[pal[p].d][0] * i;
        int c = pal[p].coluna + dir[pal[p].d][1] * i;
        DrawRectangle(GRADE_X + c * cel, GRADE_Y + l * cel, cel, cel, cor);
    }
}

static void desenhar_gabarito(void) {
    for (int p = 0; p < qtd_pal; p++) {
        pintar_palavra(p, Fade(PURPLE, 0.5f));
    }
}

static void desenhar_achadas(void) {
    for (int p = 0; p < qtd_pal; p++) {
        if (pal[p].achada) pintar_palavra(p, Fade(GREEN, 0.55f));
    }
}

static void desenhar_selecao(void) {
    int dl = sel_l1 - sel_l0;
    int dc = sel_c1 - sel_c0;
    int cel = AREA_GRADE / tam;

    if (!(dl == 0 || dc == 0 || abs(dl) == abs(dc))) return;   /* não é uma linha reta */

    int passos = abs(dl) > abs(dc) ? abs(dl) : abs(dc);
    int sl = (dl > 0) - (dl < 0);
    int sc = (dc > 0) - (dc < 0);

    for (int i = 0; i <= passos; i++) {
        int l = sel_l0 + sl * i;
        int c = sel_c0 + sc * i;
        DrawRectangle(GRADE_X + c * cel, GRADE_Y + l * cel, cel, cel, Fade(ORANGE, 0.5f));
    }
}

static void desenhar_grade(void) {
    int cel = AREA_GRADE / tam;
    int fonte = cel * 75 / 100;

    for (int linha = 0; linha < tam; linha++) {
        for (int coluna = 0; coluna < tam; coluna++) {
            int x = GRADE_X + coluna * cel;
            int y = GRADE_Y + linha * cel;
            DrawRectangleLines(x, y, cel, cel, GRAY);

            char letra[2] = { grade[linha][coluna], '\0' };
            Vector2 med = MeasureTextEx(fonte_letras, letra, (float)fonte, 1);
            Vector2 pos = { x + (cel - med.x) / 2, y + (cel - fonte) / 2.0f };
            DrawTextEx(fonte_letras, letra, pos, (float)fonte, 1, DARKGRAY);
        }
    }
}

/* ===== Título e mensagem de vitória ===== */

static void desenhar_titulo(void) {
    texto_centralizado(fonte_letras, "CAÇA-PALAVRAS", 54, TEXTO_X + TEXTO_LARGURA / 2, 28, cor_palavra);
}

static void desenhar_vitoria(void) {
    DrawRectangle(0, 0, LARGURA, ALTURA, Fade(BLACK, 0.4f));    /* escurece o fundo */

    float pw = 640, ph = 220;
    Rectangle borda = { (LARGURA - pw) / 2 - 6, (ALTURA - ph) / 2 - 6, pw + 12, ph + 12 };
    Rectangle painel = { (LARGURA - pw) / 2, (ALTURA - ph) / 2, pw, ph };
    DrawRectangleRounded(borda, 0.2f, 12, cor_palavra);
    DrawRectangleRounded(painel, 0.2f, 12, RAYWHITE);

    int topo = (int)painel.y;
    if (nivel_idx < NUM_NIVEIS - 1) {
        texto_centralizado(fonte_letras, "Nível completo!", 60, LARGURA / 2, topo + 80, cor_palavra);
    } else {
        texto_centralizado(fonte_letras, "Parabéns! Você venceu!", 48, LARGURA / 2, topo + 48, cor_palavra);
        texto_centralizado(fonte_normal, "Aperte ENTER para jogar de novo", 24, LARGURA / 2, topo + 140, cor_texto);
    }
}

/* ===== Um quadro completo ===== */

void interface_desenhar(void) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (mostrar_gabarito) desenhar_gabarito();
    desenhar_achadas();
    if (sel_ativa) desenhar_selecao();
    desenhar_grade();
    desenhar_titulo();
    desenhar_texto();

    if (ganhou) desenhar_vitoria();
    EndDrawing();
}