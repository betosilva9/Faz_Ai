#include "faz_ai.h"
int lerDadosProfissionais(char conta[][TAM]) {
    int permitido;
    if (strcmp(conta[U_CPF], "-") == 0) {
        do {
            if (!lerTexto("CPF: ", conta[U_CPF], 16)) {
                return 0;
            }
            if (!cpfValido(conta[U_CPF])) {
                printf("CPF invalido.\n");
                continue;
            }
            apenasDigitos(conta[U_CPF], 16, conta[U_CPF]);
            permitido = cpfPermitidoNaConta(conta[U_EMAIL], conta[U_CPF]);
            if (permitido == 1) {
                break;
            }
            if (permitido == -1) {
                return 0;
            }
        } while (1);
    }
    if (strcmp(conta[U_NASCIMENTO], "-") == 0) {
        do {
            if (!lerTexto("Nascimento (DD/MM/AAAA): ", conta[U_NASCIMENTO], 11)) {
                return 0;
            }
            if (dataValida(conta[U_NASCIMENTO])) {
                break;
            }
            printf("Data invalida. Digite a data completa novamente, ou 0 para voltar.\n");
        } while (1);
    }
    if (strcmp(conta[U_CIDADE], "-") == 0) {
        do {
            if (!lerTexto("Cidade de atendimento: ", conta[U_CIDADE], 100)) {
                return 0;
            }
        } while (strcmp(conta[U_CIDADE], "-") == 0);
    }
    if (strcmp(conta[U_TELEFONE], "-") == 0) {
        do {
            if (!lerTexto("Telefone com DDD: ", conta[U_TELEFONE], 20)) {
                return 0;
            }
            if (telefoneValido(conta[U_TELEFONE])) {
                break;
            }
            printf("Use 10 ou 11 numeros.\n");
        } while (1);
    }
    if (strcmp(conta[U_HABILIDADES], "-") == 0) {
        do {
            if (!lerTexto("Descreva suas habilidades e experiencias: ", conta[U_HABILIDADES], TAM)) {
                return 0;
            }
        } while (strcmp(conta[U_HABILIDADES], "-") == 0);
    }
    return 1;
}

/* Matriz da conta com dados pessoais e perfil profissional. */
void cadastrarDadosConta(int tipo) {
    char conta[9][TAM];
    int i;
    char confirmar[64], hash[TAM];
    if (tipo == 1) {
        strcpy(conta[U_TIPO], "cliente");
    } else {
        strcpy(conta[U_TIPO], "profissional");
    }
    for (i = 4; i < 9; i++) {
        strcpy(conta[i], "-");
    }
    if (!lerTexto("Nome: ", conta[U_NOME], 100)) {
        return;
    }
    do {
        if (!lerTexto("Email: ", conta[U_EMAIL], 100)) {
            return;
        }
        if (!emailValido(conta[U_EMAIL])) {
            printf("Email invalido.\n");
        } else if (existeConta(conta[U_EMAIL], conta[U_TIPO])) {
            printf("Email ja cadastrado para este tipo de conta.\n");
        }
    } while (!emailValido(conta[U_EMAIL]) || existeConta(conta[U_EMAIL], conta[U_TIPO]));
    do {
        /* Oculta a senha do cadastro; o do while exige pelo menos 6 caracteres. */
        if (!lerSenha("Senha (minimo 6 caracteres): ", conta[U_SENHA], 64)) {
            return;
        }
    } while (strlen(conta[U_SENHA]) < 6);
    if (!lerSenha("Confirme a senha: ", confirmar, sizeof(confirmar))) {
        return;
    }
    if (strcmp(confirmar, conta[U_SENHA]) != 0) {
        printf("Senhas diferentes. Cadastro nao salvo.\n");
        return;
    }
    if (!gerarHash(conta[U_SENHA], hash)) {
        printf("Erro ao proteger senha.\n");
        return;
    }
    strcpy(conta[U_SENHA], hash);
    if (tipo == 2 && !lerDadosProfissionais(conta)) {
        return;
    }
    gravarUsuario(conta);
}

void cadastrarConta(void) {
    int tipo;
    do {
        printf("\n1 - Cliente\n2 - Profissional\n0 - Voltar\n");
        printf("Cliente: solicitar servicos. Profissional: oferecer e realizar trabalhos.\n");
        printf("As contas sao separadas. Para mudar de finalidade, saia e entre na outra conta.\n");
        printf("O mesmo email pode ser usado uma vez como cliente e uma vez como profissional.\n");
        avisoVoltar();
        tipo = lerNumero();
        if (tipo == 0) {
            return;
        }
        if (tipo != 1 && tipo != 2) {
            printf("Tipo invalido.\n");
            continue;
        }
        cadastrarDadosConta(tipo);
        if (!voltou) {
            return;
        }
    } while (1);
}

