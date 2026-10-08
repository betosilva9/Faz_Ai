#include "faz_ai.h"
/* Driver usa as funcoes reais, sempre em pastas temporarias. */
int main(int argc, char **argv) {
    char conta[9][TAM];
    char hash[TAM];
    if (argc < 2) return 2;
    if (!strcmp(argv[1], "hash")) {
        if (!gerarHash("Senha123", hash) || !conferirSenha("Senha123", hash) || conferirSenha("Errada", hash)) return 1;
        printf("%s\n", hash); return 0;
    }
    if (!strcmp(argv[1], "integridade")) return verificarArquivos() ? 0 : 1;
    if (!strcmp(argv[1], "cadastro")) { cadastrarDadosConta(1); return 0; }
    if (!strcmp(argv[1], "login")) { entrarTipo(1); return 0; }
    if (!strcmp(argv[1], "senha")) { redefinirSenhaTipo(1); return 0; }
    if (!strcmp(argv[1], "bloqueio")) {
        loginFalhou(); loginFalhou(); loginFalhou(); return loginPermitido() ? 1 : 0;
    }
    if (argc < 3) return 2;
    if (!strcmp(argv[1], "anuncio")) editarAnuncio(argv[2]);
    else if (!strcmp(argv[1], "oferta")) oferecerServico(argv[2]);
    else if (!strcmp(argv[1], "pedido")) criarTicket(argv[2]);
    else if (!strcmp(argv[1], "perfil")) {
        if (!buscarConta(argv[2], argc > 3 ? argv[3] : "cliente", conta)) return 1;
        editarPerfil(conta);
    } else if (!strcmp(argv[1], "atualizar") && argc > 3) atualizarTicket(argv[2], atoi(argv[3]));
    else return 2;
    return 0;
}
