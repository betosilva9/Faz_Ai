#include "faz_ai.h"
/* Troca protegida: o backup permanece ate a nova gravacao estar instalada.
Se houver backup pendente, o programa pede revisao em vez de sobrescreve-lo. */
int substituirArquivo(const char *caminho, const char *temporario) {
    char backup[100];
    FILE *arquivo;
    snprintf(backup, sizeof(backup), "%s.backup", caminho);
    arquivo = fopen(backup, "r");
    if (arquivo) {
        fclose(arquivo);
        printf("Confira o backup pendente: %s\n", backup);
        return 0;
    }
    if (rename(caminho, backup) != 0) {
        perror("Preservar original");
        return 0;
    }
    if (rename(temporario, caminho) != 0) {
        perror("Instalar arquivo atualizado");
        if (rename(backup, caminho) != 0) {
            printf("Recupere manualmente %s a partir de %s.\n", caminho, backup);
        }
        return 0;
    }
    if (remove(backup) != 0) {
        printf("Dados salvos, mas nao foi possivel remover %s.\n", backup);
    }
    return 1;
}

/* Diagnostico sem apagar nem adivinhar dados. A linha e o motivo sao
informados; o programa interrompe antes de qualquer alteracao. */
/* Confere identificadores repetidos e referencias entre arquivos.
Leitura sequencial e suficiente para o volume de um projeto didatico. */
static int conferirVinculos(void) {
    FILE *arquivo, *outro;
    char usuario[9][TAM], anterior[9][TAM], cliente[9][TAM];
    char servico[9][TAM];
    char pedido[8][TAM], repetido[8][TAM];
    char linha[2400], linha2[2400], *email;
    int f, numero, indice, erro = 0, i, codigoValido;
    const char *nomes[] = {
        USUARIOS, PROFISSIONAIS, TICKETS
    }
    ;
    for (f = 0; f < 3; f++) {
        arquivo = fopen(nomes[f], "r");
        if (!arquivo) {
            continue;
        }
        numero = 0;
        while (fgets(linha, sizeof(linha), arquivo)) {
            numero++;
            if (f == 0) {
                if (!lerUsuario(linha, usuario)) {
                    continue;
                }
                outro = fopen(USUARIOS, "r");
                if (!outro) {
                    fclose(arquivo);
                    return 0;
                }
                indice = 0;
                while (++indice < numero && fgets(linha2, sizeof(linha2), outro)) {
                    if (lerUsuario(linha2, anterior) && iguais(usuario[U_EMAIL], anterior[U_EMAIL]) && iguais(usuario[U_TIPO], anterior[U_TIPO])) {
                        printf("usuarios.txt, linha %d: email/tipo duplicados com linha %d.\n", numero, indice);
                        erro = 1;
                    }
                }
                fclose(outro);
            } else if (f == 1) {
                if (!lerServico(linha, servico)) {
                    continue;
                }
                if (!buscarConta(servico[S_PROFISSIONAL_ID], "profissional", usuario)) {
                    printf("profissionais.txt, linha %d: profissional inexistente.\n", numero);
                    erro = 1;
                }
            } else {
                if (!lerSolicitacao(linha, pedido)) {
                    continue;
                }
                codigoValido = strlen(pedido[P_CODIGO]) == 4;
                if (codigoValido) {
                    for (i = 0; i < 4; i++)
                    {
                        if (!strchr("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", pedido[P_CODIGO][i])) {
                            codigoValido = 0;
                        }
                    }
                } else if (strncmp(pedido[P_CODIGO], "TK", 2) == 0 && pedido[P_CODIGO][2]) {
                    codigoValido = 1;
                    for (i = 2; pedido[P_CODIGO][i]; i++)
                    {
                        if (!isdigit((unsigned char)pedido[P_CODIGO][i])) {
                            codigoValido = 0;
                        }
                    }
                    if (strspn(pedido[P_CODIGO] + 2, "0") == strlen(pedido[P_CODIGO] + 2)) {
                        codigoValido = 0;
                    }
                }
                if (!codigoValido) {
                    printf("tickets.txt, linha %d: codigo invalido.\n", numero);
                    erro = 1;
                }
                if (!buscarConta(pedido[P_CLIENTE_ID], "cliente", cliente)) {
                    printf("tickets.txt, linha %d: cliente inexistente.\n", numero);
                    erro = 1;
                }
                email = pedido[P_PROFISSIONAL_ID];
                if (strcmp(email, "SEM_PROFISSIONAL") != 0 && !buscarConta(email, "profissional", usuario)) {
                    printf("tickets.txt, linha %d: profissional inexistente.\n", numero);
                    erro = 1;
                }
                if (((iguais(pedido[P_SITUACAO], "ACEITO") || iguais(pedido[P_SITUACAO], "CONCLUIDO")) && strcmp(email, "SEM_PROFISSIONAL") == 0) ||
                (iguais(pedido[P_SITUACAO], "ABERTO") && strcmp(email, "SEM_PROFISSIONAL") != 0)) {
                    printf("tickets.txt, linha %d: situacao e responsavel inconsistentes.\n", numero);
                    erro = 1;
                }
                outro = fopen(TICKETS, "r");
                if (!outro) {
                    fclose(arquivo);
                    return 0;
                }
                indice = 0;
                while (++indice < numero && fgets(linha2, sizeof(linha2), outro)) {
                    if (lerSolicitacao(linha2, repetido) && iguais(pedido[P_CODIGO], repetido[P_CODIGO])) {
                        printf("tickets.txt, linha %d: codigo duplicado com linha %d.\n", numero, indice);
                        erro = 1;
                    }
                }
                fclose(outro);
            }
        }
        if (ferror(arquivo)) {
            erro = 1;
        }
        fclose(arquivo);
    }
    return !erro;
}

int verificarArquivos(void) {
    const char *nomes[] = {
        USUARIOS, PROFISSIONAIS, TICKETS
    }
    ;
    FILE *arquivo;
    char linha[2400], campos[9][TAM];
    int f, numero, valido, erros = 0;
    double preco;
    for (f = 0; f < 3; f++) {
        arquivo = fopen(nomes[f], "r");
        if (!arquivo) {
            if (errno != ENOENT) {
                perror(nomes[f]);
                return 0;
            }
            continue;
            /* Primeira execucao: arquivos ainda nao existem. */
        }
        numero = 0;
        while (fgets(linha, sizeof(linha), arquivo)) {
            numero++;
            if (f == 0) {
                valido = separarConta(linha, campos);
            } else if (f == 1) {
                valido = separar(linha, campos, 9);
            } else {
                valido = separarTicket(linha, campos);
            }
            if (valido && f == 0)
            {
                valido = emailValido(campos[1]) &&
                (iguais(campos[3], "cliente") || iguais(campos[3], "profissional"));
            }
            if (valido && f == 1) {
                valido = emailValido(campos[0]) && precoValido(campos[8], &preco);
            }
            if (valido && f == 2)
            {
                valido = emailValido(campos[1]) &&
                (iguais(campos[5], "ABERTO") || iguais(campos[5], "ACEITO") ||
                iguais(campos[5], "CONCLUIDO") || iguais(campos[5], "CANCELADO"));
            }
            if (!valido) {
                printf("%s, linha %d: registro danificado (campos, tipo, email, preco ou situacao).\n", nomes[f], numero);
                erros++;
            }
        }
        if (ferror(arquivo)) {
            perror(nomes[f]);
            erros++;
        }
        fclose(arquivo);
    }
    if (!erros && !conferirVinculos()) {
        erros++;
    }
    if (erros) {
        printf("Inicializacao interrompida. Restaure a linha usando uma copia valida; nenhum registro foi apagado.\n");
    }
    return erros == 0;
}
