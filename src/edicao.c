#include "faz_ai.h"
/* Email + tipo identificam a conta e nao sao editados: pedidos antigos
continuam vinculados. O perfil e salvo somente ao terminar o formulario. */
void editarPerfil(char conta[][TAM]) {
    char novo[9][TAM];
    int i;
    for (i = 0; i < 9; i++) {
        strcpy(novo[i], conta[i]);
    }
    if (!lerTexto("Novo nome: ", novo[U_NOME], 100)) {
        return;
    }
    if (iguais(novo[U_TIPO], "profissional")) {
        strcpy(novo[U_CIDADE], "-");
        strcpy(novo[U_TELEFONE], "-");
        strcpy(novo[U_HABILIDADES], "-");
        if (!lerDadosProfissionais(novo)) {
            return;
        }
    }
    if (salvarPerfil(novo)) {
        for (i = 0; i < 9; i++) {
            strcpy(conta[i], novo[i]);
        }
    }
}

/* O numero e a posicao atual no arquivo, usada somente neste menu.
Nao e um identificador permanente. Cada anuncio pertence ao email da conta. */
void editarAnuncio(char email[]) {
    FILE *arquivo, *novo;
    char servico[9][TAM];
    char linha[2400], escolha[32], sobra, confirmar[16];
    int numero = 0, alvo, acao, encontrou = 0, erro = 0;
    double valor;
    arquivo = fopen(PROFISSIONAIS, "r");
    if (!arquivo) {
        perror("Abrir anuncios");
        return;
    }
    while (fgets(linha, sizeof(linha), arquivo)) {
        numero++;
        if (!lerServico(linha, servico)) {
            fclose(arquivo);
            printf("Anuncio danificado. Confira o arquivo.\n");
            return;
        }
        if (iguais(servico[S_PROFISSIONAL_ID], email))
        {
            printf("%d - %s | %s | R$ %s\n", numero, servico[S_TIPO], servico[S_DESCRICAO], servico[S_PRECO]);
        }
    }
    if (ferror(arquivo)) {
        fclose(arquivo);
        perror("Ler anuncios");
        return;
    }
    fclose(arquivo);
    if (!lerTexto("Numero do anuncio: ", escolha, sizeof(escolha))) {
        return;
    }
    if (sscanf(escolha, "%d %c", &alvo, &sobra) != 1 || alvo < 1) {
        printf("Numero invalido.\n");
        return;
    }
    printf("1 - Editar descricao e preco\n2 - Excluir anuncio\n0 - Voltar\n");
    acao = lerNumero();
    if (acao != 1 && acao != 2) {
        return;
    }
    if (acao == 2) {
        if (!lerTexto("Digite EXCLUIR para confirmar: ", confirmar, sizeof(confirmar))) {
            return;
        }
        if (strcmp(confirmar, "EXCLUIR") != 0) {
            return;
        }
    }
    arquivo = fopen(PROFISSIONAIS, "r");
    if (!arquivo) {
        perror("Abrir anuncios");
        return;
    }
    novo = fopen("profissionais_novo.tmp", "w");
    if (!novo) {
        fclose(arquivo);
        perror("Preparar anuncios");
        return;
    }
    numero = 0;
    while (fgets(linha, sizeof(linha), arquivo)) {
        numero++;
        if (!lerServico(linha, servico)) {
            erro = 1;
            break;
        }
        if (numero == alvo && iguais(servico[S_PROFISSIONAL_ID], email)) {
            encontrou = 1;
            if (acao == 2) {
                continue;
            }
            if (!lerTexto("Nova descricao: ", servico[S_DESCRICAO], TAM)) {
                erro = 1;
                break;
            }
            do {
                if (!lerTexto("Novo preco: ", servico[S_PRECO], 32)) {
                    erro = 1;
                    break;
                }
                if (precoValido(servico[S_PRECO], &valor)) {
                    break;
                }
                printf("Informe um preco positivo ate 1000000.\n");
            } while (1);
            if (erro) {
                break;
            }
        }
        if (!gravarLinha(novo, servico, 9)) {
            erro = 1;
        }
    }
    if (ferror(arquivo)) {
        erro = 1;
    }
    fclose(arquivo);
    if (fclose(novo) != 0) {
        erro = 1;
    }
    if (erro || !encontrou) {
        remove("profissionais_novo.tmp");
        printf("Nada alterado. Confira o numero e se o anuncio pertence a voce.\n");
        return;
    }
    if (substituirArquivo(PROFISSIONAIS, "profissionais_novo.tmp")) {
        printf("Anuncio atualizado!\n");
    }
}
