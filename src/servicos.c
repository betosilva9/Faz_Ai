#include "faz_ai.h"
void oferecerServico(char email[]) {
    char p[9][TAM];
    char conta[9][TAM];
    double valor;
    if (!buscarConta(email, "profissional", conta)) {
        printf("Conta profissional nao encontrada.\n");
        return;
    }
    if (!perfilCompleto(conta)) {
        printf("Complete seu cadastro antigo em 2 - Meu perfil antes de oferecer servicos.\n");
        return;
    }
    strcpy(p[S_PROFISSIONAL_ID], conta[U_EMAIL]);
    strcpy(p[S_NOME], conta[U_NOME]);
    strcpy(p[S_CPF], conta[U_CPF]);
    strcpy(p[S_NASCIMENTO], conta[U_NASCIMENTO]);
    strcpy(p[S_CIDADE], conta[U_CIDADE]);
    strcpy(p[S_TELEFONE], conta[U_TELEFONE]);
    if (!escolherServico(p[S_TIPO])) {
        return;
    }
    if (!lerTexto("Descricao do servico: ", p[S_DESCRICAO], TAM)) {
        return;
    }
    do {
        if (!lerTexto("Valor do servico: ", p[S_PRECO], 32)) {
            return;
        }
        if (precoValido(p[S_PRECO], &valor)) {
            break;
        }
        printf("Informe um valor positivo ate 1000000.\n");
    } while (1);
    gravarServico(p);
}

/* Ticket: codigo, cliente, servico, cidade, descricao, status, profissional, endereco.
Os pedidos permanecem no arquivo, inclusive depois de concluidos. */
int novoCodigoTicket(char codigo[]) {
    const char caracteres[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    FILE *arquivo = fopen(TICKETS, "a+");
    char linha[2400], sobra;
    char t[8][TAM];
    int numero, maior = 0, i, digito;
    if (arquivo == NULL) {
        printf("Erro ao abrir os pedidos.\n");
        return 0;
    }
    rewind(arquivo);
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (!lerSolicitacao(linha, t)) {
            fclose(arquivo);
            printf("Arquivo de pedidos invalido. Confira antes de criar outro pedido.\n");
            return 0;
        }
        if (strlen(t[P_CODIGO]) == 4) {
            numero = 0;
            for (i = 0; i < 4; i++) {
                digito = 0;
                while (digito < 36 && caracteres[digito] != t[P_CODIGO][i]) {
                    digito++;
                }
                if (digito == 36) {
                    fclose(arquivo);
                    printf("Codigo de pedido invalido.\n");
                    return 0;
                }
                numero = numero * 36 + digito;
            }
            if (numero > maior) {
                maior = numero;
            }
        } else if (sscanf(t[P_CODIGO], "TK%d%c", &numero, &sobra) != 1 || numero <= 0) {
            fclose(arquivo);
            printf("Codigo de pedido invalido.\n");
            return 0;
        }
    }
    if (ferror(arquivo)) {
        fclose(arquivo);
        printf("Erro ao ler pedidos.\n");
        return 0;
    }
    fclose(arquivo);
    if (maior >= 1679615) {
        printf("Limite de codigos atingido.\n");
        return 0;
    }
    numero = maior + 1;
    codigo[4] = '\0';
    for (i = 3; i >= 0; i--) {
        codigo[i] = caracteres[numero % 36];
        numero = numero / 36;
    }
    return 1;
}

void criarTicket(char email[]) {
    char t[8][TAM];
    /* Os limites permitem reunir o endereco completo sem cortar os campos. */
    char rua[80], numero[16], bairro[60], complemento[64];
    strcpy(t[P_CLIENTE_ID], email);
    if (!escolherServico(t[P_SERVICO])) {
        return;
    }
    if (!lerTexto("Cidade do atendimento: ", t[P_CIDADE], 100)) {
        return;
    }
    if (!lerTexto("Rua: ", rua, sizeof(rua))) {
        return;
    }
    if (!lerTexto("Numero (ou S/N se nao houver): ", numero, sizeof(numero))) {
        return;
    }
    if (!lerTexto("Bairro: ", bairro, sizeof(bairro))) {
        return;
    }
    if (!lerTexto("Complemento (digite - se nao houver): ", complemento, sizeof(complemento))) {
        return;
    }
    snprintf(t[P_ENDERECO], sizeof(t[P_ENDERECO]), "%s, numero %s, bairro %s, complemento %s",
    rua, numero, bairro, complemento);
    if (!lerTexto("Descreva o pedido: ", t[P_DESCRICAO], TAM)) {
        return;
    }
    strcpy(t[P_SITUACAO], "ABERTO");
    strcpy(t[P_PROFISSIONAL_ID], "SEM_PROFISSIONAL");
    if (!novoCodigoTicket(t[P_CODIGO])) {
        return;
    }
    if (gravarSolicitacao(t)) {
        printf("Codigo do pedido: %s\n", t[P_CODIGO]);
    }
}

/* Um profissional atende somente pedidos da profissao e cidade cadastradas. */
int atendeTicket(char email[], char t[][TAM]) {
    FILE *arquivo = fopen(PROFISSIONAIS, "r");
    char linha[2400];
    char p[9][TAM];
    int atende = 0;
    char cidade[TAM];
    char profissional[9][TAM];
    int perfil = buscarConta(email, "profissional", profissional);
    if (arquivo == NULL) {
        return 0;
    }
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (!lerServico(linha, p)) {
            continue;
        }
        if (perfil && strcmp(profissional[U_CIDADE], "-") != 0) {
            strcpy(cidade, profissional[U_CIDADE]);
        } else {
            strcpy(cidade, p[S_CIDADE]);
        }
        if (iguais(p[S_PROFISSIONAL_ID], email) && iguais(p[S_TIPO], t[P_SERVICO]) && iguais(cidade, t[P_CIDADE])) {
            atende = 1;
        }
    }
    if (ferror(arquivo)) {
        atende = 0;
    }
    fclose(arquivo);
    return atende;
}

/* modo 1: pedidos do cliente; 2: disponiveis; 3: aceitos pelo profissional. */
void listarTickets(char email[], int modo) {
    FILE *arquivo = fopen(TICKETS, "r");
    char linha[2400];
    char t[8][TAM];
    int mostrar, encontrados = 0; // CONTADOR de pedidos exibidos.
    if (arquivo == NULL) {
        printf("Nenhum pedido disponivel.\n");
        return;
    }
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (!lerSolicitacao(linha, t)) {
            continue;
        }
        mostrar = 0;
        switch (modo) {
            case 1:
                mostrar = iguais(t[P_CLIENTE_ID], email);
                break;
            case 2:
                if (iguais(t[P_SITUACAO], "ABERTO") && atendeTicket(email, t)) {
                    mostrar = 1;
            }
            break;
            case 3:
                mostrar = iguais(t[P_PROFISSIONAL_ID], email);
                break;
        }
        if (mostrar) {
            encontrados++;
            printf("\nPedido: %s\nServico: %s\nCidade: %s\nEndereco: %s\nDescricao: %s\n"
            "Status: %s\nCliente: %s\nProfissional: %s\n",
            t[P_CODIGO], t[P_SERVICO], t[P_CIDADE], t[P_ENDERECO], t[P_DESCRICAO], t[P_SITUACAO], t[P_CLIENTE_ID], t[P_PROFISSIONAL_ID]);
        }
    }
    if (ferror(arquivo)) {
        printf("Erro ao ler os pedidos.\n");
    }
    fclose(arquivo);
    if (!encontrados) {
        printf("Nenhum pedido encontrado para esta opcao.\n");
    }
}

/* Acao 1 aceita um pedido aberto; acao 2 conclui um pedido do profissional. */
void atualizarTicket(char email[], int acao) {
    FILE *arquivo, *novo, *backup;
    char codigo[TAM], linha[2400];
    char t[8][TAM];
    int alterou = 0, erro = 0, i;
    if (acao == 1) {
        listarTickets(email, 2);
    } else if (acao == 3) {
        listarTickets(email, 1);
    } else {
        listarTickets(email, 3);
    }
    if (!lerTexto("Codigo do pedido (0 para voltar): ", codigo, TAM)) {
        return;
    }
    backup = fopen("tickets_backup.tmp", "r");
    if (backup != NULL) {
        fclose(backup);
        printf("Existe tickets_backup.tmp. Confira o backup antes de continuar.\n");
        return;
    }
    arquivo = fopen(TICKETS, "r");
    if (arquivo == NULL) {
        printf("Nenhum pedido disponivel.\n");
        return;
    }
    novo = fopen("tickets_novo.tmp", "w");
    if (novo == NULL) {
        fclose(arquivo);
        printf("Erro ao preparar atualizacao.\n");
        return;
    }
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (!lerSolicitacao(linha, t)) {
            erro = 1;
            break;
        }
        if (iguais(t[P_CODIGO], codigo)) {
            if (alterou) {
                erro = 1;
                break;
            }
            if (acao == 1 && iguais(t[P_SITUACAO], "ABERTO") && atendeTicket(email, t)) {
                strcpy(t[P_SITUACAO], "ACEITO");
                strcpy(t[P_PROFISSIONAL_ID], email);
                alterou = 1;
            } else if ((acao == 3 || acao == 4) &&
            (iguais(t[P_SITUACAO], "ABERTO") || iguais(t[P_SITUACAO], "ACEITO")) &&
            ((acao == 3 && iguais(t[P_CLIENTE_ID], email)) || (acao == 4 && iguais(t[P_PROFISSIONAL_ID], email)))) {
                strcpy(t[P_SITUACAO], "CANCELADO");
                alterou = 1;
            } else if (acao == 2 && iguais(t[P_SITUACAO], "ACEITO") && iguais(t[P_PROFISSIONAL_ID], email)) {
                strcpy(t[P_SITUACAO], "CONCLUIDO");
                alterou = 1;
            }
        }
        for (i = 0; i < 8; i++) {
            if (i == 7) {
                if (fprintf(novo, "%s\n", t[i]) < 0) {
                    erro = 1;
                }
            } else {
                if (fprintf(novo, "%s|", t[i]) < 0) {
                    erro = 1;
                }
            }
        }
    }
    if (ferror(arquivo)) {
        erro = 1;
    }
    fclose(arquivo);
    if (fclose(novo) != 0) {
        erro = 1;
    }
    if (erro || !alterou) {
        remove("tickets_novo.tmp");
        printf("Pedido nao atualizado: confira o codigo, o status e sua permissao.\n");
        return;
    }
    if (rename(TICKETS, "tickets_backup.tmp") != 0) {
        remove("tickets_novo.tmp");
        printf("Erro ao preservar pedidos.\n");
        return;
    }
    if (rename("tickets_novo.tmp", TICKETS) != 0) {
        if (rename("tickets_backup.tmp", TICKETS) != 0)
        {
            printf("Recupere tickets.txt a partir de tickets_backup.tmp.\n");
        }
        printf("Erro ao atualizar pedidos.\n");
        return;
    }
    remove("tickets_backup.tmp");
    if (acao == 1) {
        printf("Pedido aceito! Voce e o profissional responsavel.\n");
    } else if (acao == 2) {
        printf("char concluido!\n");
    } else {
        printf("Pedido cancelado!\n");
    }
}

/* Cada tipo de conta possui seu proprio menu e suas proprias acoes. */
