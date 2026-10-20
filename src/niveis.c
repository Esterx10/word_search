#include "niveis.h"

/* Os dados dos 3 níveis: tamanho da grade, direções, palavras e o texto.
   No texto, as palavras do jogo ficam entre *asteriscos*. Quando a palavra tem acento,
   escreve-se *SEM_ACENTO|COM_ACENTO*. */
Nivel niveis[NUM_NIVEIS] = {
    { 8, 2, 5, { "HARDWARE", "MEMORIA", "KERNEL", "BATCH", "CPU" },
      "O *KERNEL* é o núcleo do sistema operacional e controla o *HARDWARE*, "
      "a *MEMORIA|MEMÓRIA* e a *CPU*. Em sistemas em lote, ou *BATCH*, os trabalhos "
      "são preparados antes e executados com pouca interação do usuário." },

    { 16, 3, 10, { "MULTIUSUARIO", "DISTRIBUIDOS", "MULTITAREFA", "MICROKERNEL", "MONOLITICO",
                   "EFICIENCIA", "EMBARCADOS", "ABSTRACAO", "HIBRIDO", "LINUX" },
      "Um sistema operacional oferece *ABSTRACAO|ABSTRAÇÃO* do hardware e busca "
      "*EFICIENCIA|EFICIÊNCIA* na utilização dos recursos. Um sistema como o *LINUX*, "
      "instalado em um servidor, pode ser *MULTIUSUARIO|MULTIUSUÁRIO*, *MULTITAREFA* e ter "
      "um kernel *MONOLITICO|MONOLÍTICO* modular. O kernel também pode ser um *MICROKERNEL*, "
      "que mantém no núcleo só o essencial, ou *HIBRIDO|HÍBRIDO*, que combina ideias dos dois "
      "modelos. Quanto ao propósito, existem sistemas em lote, de tempo real, *EMBARCADOS*, "
      "feitos para uma função específica, e *DISTRIBUIDOS|DISTRIBUÍDOS*." },

    { 20, 8, 14, { "MULTIPROGRAMACAO", "CONFIABILIDADE", "PROCESSAMENTO", "COMPARTILHADO",
                   "CONVENIENCIA", "GERENCIADOR", "ISOLAMENTO", "PEOPLEWARE", "SEGURANCA",
                   "INTERFACE", "MONITORES", "ROBUSTEZ", "SOCKETS", "NUVEM" },
      "O sistema operacional integra *PEOPLEWARE* (usuários), software e hardware, atuando como "
      "*INTERFACE* e *GERENCIADOR* de recursos. Seus objetivos são *CONVENIENCIA|CONVENIÊNCIA*, eficiência, "
      "*CONFIABILIDADE*, *ROBUSTEZ* e *SEGURANCA|SEGURANÇA*. O *ISOLAMENTO* impede que um programa interfira "
      "em outro, e o sistema oferece abstrações como processos e *SOCKETS*. Os *MONITORES* residentes "
      "automatizavam o carregamento de programas. Depois vieram o *PROCESSAMENTO* em lote, a "
      "*MULTIPROGRAMACAO|MULTIPROGRAMAÇÃO* e o tempo *COMPARTILHADO*. Hoje, a computação em *NUVEM* "
      "ampliou o papel dos sistemas operacionais." }
};