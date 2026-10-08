#include "faz_ai.h"
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
int main(void) {
    int opcao; // VARIAVEL: guarda a escolha do usuario.
    if (!verificarArquivos()) {
        return 1;
    }
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    HANDLE tela = GetStdHandle(STD_OUTPUT_HANDLE);
    // Acessa o console do Windows
#endif
    // DO WHILE: mostra o menu pelo menos uma vez.
    do {
#ifdef _WIN32
        system("cls");
        // Limpa a tela antes de mostrar a home
        SetConsoleTextAttribute(tela, 9);
        // Titulo azul
#else
        printf("\033[2J\033[H\033[94m");
        // Limpa e colore em outros terminais
#endif
        printf("\n\t============================\n");
        printf("\t          FAZ AI\n");
        printf("\t============================\n");
#ifdef _WIN32
        SetConsoleTextAttribute(tela, 15);
        // Texto e opcoes em branco
#else
        printf("\033[97m");
#endif
        printf("\n\tConectando voce ao servico certo!\n");
        printf("\n\t1 - Criar conta\n\t2 - Entrar\n\t3 - Alterar senha\n\t0 - Sair\n\n");
        avisoVoltar();
        printf("No menu inicial, 0 encerra o programa.\n");
        opcao = lerNumero();
        // SWITCH: escolhe a acao conforme o numero digitado.
        switch (opcao) {
            case 1:
                cadastrarConta();
                break;
            case 2:
                entrar();
                break;
            case 3:
                redefinirSenha();
                break;
            case 0:
                printf("Ate logo!\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
        if (opcao != 0 && !voltou) {
            int tecla;
            printf("\nPressione ENTER ou digite 0 e ENTER para voltar...");
            /* Consome a linha inteira para nao deixar ENTER no proximo menu. */
            do {
                tecla = getchar();
            } while (tecla != '\n' && tecla != EOF);
        }
    } while (opcao != 0);
#ifdef _WIN32
    SetConsoleTextAttribute(tela, 7);
    // Restaura a cor padrao do console
#else
    printf("\033[0m");
#endif
    return 0;
}
