#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include "config.h"
#include "jogo.h"
#include "interface.h"

int main(void) {
    srand(time(NULL));   /* sem isso o sorteio daria sempre as mesmas letras: usa a hora atual como ponto de partida */
    ChangeDirectory(GetApplicationDirectory());   /* acha a pasta fonts/ ao lado do programa */

    InitWindow(LARGURA, ALTURA, "Caca-Palavras");
    SetTargetFPS(60);
    interface_iniciar();
    novo_jogo(0);

    while (!WindowShouldClose()) {
        jogo_atualizar();
        interface_desenhar();
    }

    interface_encerrar();
    CloseWindow();
    return 0;
}