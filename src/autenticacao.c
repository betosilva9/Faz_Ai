#include "faz_ai.h"

/* Procura a conta e confere a senha antes de abrir o painel. */
void entrarTipo(int tipo) {
    char email[100], senha[64], tipoConta[20], hash[TAM];
    char conta[9][TAM];
    int encontrou;

    if (!loginPermitido()) {
        return;
    }

    if (tipo == 1) {
        strcpy(tipoConta, "cliente");
    } else {
        strcpy(tipoConta, "profissional");
    }

    if (!lerTexto("Email: ", email, sizeof(email))) {
        return;
    }
    if (!lerSenha("Senha: ", senha, sizeof(senha))) {
        return;
    }

    encontrou = buscarConta(email, tipoConta, conta);
    if (encontrou == 0) {
        loginFalhou();
        printf("Email ou senha incorretos para o tipo de conta escolhido.\n");
        return;
    }

    if (!conferirSenha(senha, conta[U_SENHA])) {
        loginFalhou();
        printf("Email ou senha incorretos para o tipo de conta escolhido.\n");
        return;
    }

    /* Contas antigas tambem recebem hash depois do login correto. */
    if (strncmp(conta[U_SENHA], "pbkdf2$", 7) != 0) {
        if (!gerarHash(senha, hash)) {
            printf("Erro ao proteger senha antiga.\n");
            return;
        }
        strcpy(conta[U_SENHA], hash);
        if (!salvarPerfil(conta)) {
            return;
        }
    }

    loginSucesso();
    painel(conta);
}

/* DO WHILE: mostra o menu novamente ate escolher 0. */
void entrar(void) {
    int tipo;

    do {
        printf("\n=== ENTRAR ===\n");
        printf("1 - Cliente\n2 - Profissional\n0 - Voltar\n");
        avisoVoltar();
        tipo = lerNumero();

        switch (tipo) {
            case 1:
                    entrarTipo(1);
                    break;
            case 2:
                    entrarTipo(2);
                    break;
            case 0:
                    break;
            default:
                    printf("Tipo invalido.\n");
        }
    } while (tipo != 0);
}

/* Reaproveita a gravacao do perfil, sem repetir a troca dos arquivos aqui. */
void redefinirSenhaTipo(int tipo) {
    char email[100], nome[100], tipoConta[20];
    char senhaAtual[64], senhaNova[64], confirmar[64], hash[TAM];
    char conta[9][TAM];
    int encontrou;

    printf("\n=== ALTERAR SENHA ===\n");
    if (!loginPermitido()) {
        return;
    }
    if (tipo == 1) {
        strcpy(tipoConta, "cliente");
    } else {
        strcpy(tipoConta, "profissional");
    }

    if (!lerTexto("Email cadastrado: ", email, sizeof(email))) {
        return;
    }
    if (!lerTexto("Nome cadastrado: ", nome, sizeof(nome))) {
        return;
    }
    if (!lerSenha("Senha atual: ", senhaAtual, sizeof(senhaAtual))) {
        return;
    }

    encontrou = buscarConta(email, tipoConta, conta);
    if (encontrou == 0) {
        loginFalhou();
        printf("Dados ou senha atual incorretos.\n");
        return;
    }
    if (!iguais(nome, conta[U_NOME]) || !conferirSenha(senhaAtual, conta[U_SENHA])) {
        loginFalhou();
        printf("Dados ou senha atual incorretos.\n");
        return;
    }

    do {
        if (!lerSenha("Nova senha (minimo 6 caracteres): ", senhaNova, sizeof(senhaNova))) {
            return;
        }
        if (strlen(senhaNova) < 6) {
            printf("A senha precisa ter pelo menos 6 caracteres.\n");
        }
    } while (strlen(senhaNova) < 6);

    if (!lerSenha("Confirme a nova senha: ", confirmar, sizeof(confirmar))) {
        return;
    }
    if (strcmp(senhaNova, confirmar) != 0) {
        printf("Senhas diferentes. Nenhuma alteracao foi salva.\n");
        return;
    }
    if (!gerarHash(senhaNova, hash)) {
        printf("Erro ao proteger senha.\n");
        return;
    }

    strcpy(conta[U_SENHA], hash);
    if (salvarPerfil(conta)) {
        loginSucesso();
        printf("Senha redefinida! Entre usando a nova senha.\n");
    }
}

void redefinirSenha(void) {
    int tipo;

    do {
        printf("\n=== ALTERAR SENHA ===\n");
        printf("1 - Cliente\n2 - Profissional\n0 - Voltar\n");
        avisoVoltar();
        tipo = lerNumero();

        if (tipo == 1 || tipo == 2) {
            redefinirSenhaTipo(tipo);
        } else if (tipo != 0) {
            printf("Tipo invalido.\n");
        }
    } while (tipo != 0);
}
