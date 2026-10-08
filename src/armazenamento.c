#include "faz_ai.h"
int separar(char linha[], char campos[][TAM], int total) {
    int i, campo = 0, coluna = 0;
    int j;
    /* Dois FOR limpam as linhas e colunas da matriz antes da leitura. */
    for (i = 0; i < total; i++) {
        for (j = 0; j < TAM; j++) {
            campos[i][j] = '\0';
        }
    }
    for (i = 0; linha[i] != '\0' && linha[i] != '\n' && linha[i] != '\r'; i++) {
        if (linha[i] == '|') {
            campo++;
            coluna = 0;
            if (campo >= total) {
                return 0;
            }
        } else {
            if (coluna >= TAM - 1) {
                return 0;
            }
            campos[campo][coluna++] = linha[i];
        }
    }
    for (i = 0; i < total; i++)
    {
        if (campos[i][0] == '\0') {
            return 0;
        }
    }
    return campo + 1 == total;
}

/* Conta: nome, email, senha, tipo, CPF, nascimento, cidade, telefone, habilidades/experiencias.
Cadastros antigos de quatro campos continuam sendo aceitos. */
int separarConta(char linha[], char conta[][TAM]) {
    int i;
    if (separar(linha, conta, 9)) {
        return 1;
    }
    if (!separar(linha, conta, 4)) {
        return 0;
    }
    for (i = 4; i < 9; i++) {
        strcpy(conta[i], "-");
    }
    return 1;
}

int existeConta(char email[], char tipo[]) {
    FILE *arquivo = fopen(USUARIOS, "r");
    char linha[2400], campos[9][TAM];
    if (arquivo == NULL) {
        return 0;
    }
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (separarConta(linha, campos) && iguais(campos[1], email) && iguais(campos[3], tipo)) {
            fclose(arquivo);
            return 1;
        }
    }
    fclose(arquivo);
    return 0;
}

/* Salva os campos separados por |, no mesmo formato dos arquivos originais. */
/* Acrescenta um registro usando uma copia temporaria; evita gravacao parcial. */
int salvar(char caminho[], char campos[][TAM], int total) {
    FILE *original, *novo;
    char temporario[100];
    int c, ultimo = '\n', erro = 0, existia;
    snprintf(temporario, sizeof(temporario), "%s.novo", caminho);
    original = fopen(caminho, "rb");
    existia = original != NULL;
    if (!original && errno != ENOENT) {
        perror(caminho);
        return 0;
    }
    novo = fopen(temporario, "wb");
    if (!novo) {
        if (original) {
            fclose(original);
        }
        perror(temporario);
        return 0;
    }
    if (original) {
        while ((c = fgetc(original)) != EOF) {
            ultimo = c;
            if (fputc(c, novo) == EOF) {
                erro = 1;
            }
        }
        if (ferror(original)) {
            erro = 1;
        }
        fclose(original);
    }
    if (ultimo != '\n' && fputc('\n', novo) == EOF) {
        erro = 1;
    }
    if (!gravarLinha(novo, campos, total)) {
        erro = 1;
    }
    if (fclose(novo) != 0) {
        erro = 1;
    }
    if (erro) {
        remove(temporario);
        printf("Falha na gravacao. Original preservado.\n");
        return 0;
    }
    if (existia) {
        if (!substituirArquivo(caminho, temporario)) {
            return 0;
        }
    } else if (rename(temporario, caminho) != 0) {
        perror(caminho);
        return 0;
    }
    printf("Cadastro salvo!\n");
    return 1;
}

void recuperarPerfilAntigo(char conta[][TAM]) {
    FILE *arquivo;
    char linha[2400];
    char p[9][TAM];
    if (!iguais(conta[U_TIPO], "profissional") || strcmp(conta[U_CPF], "-") != 0) {
        return;
    }
    arquivo = fopen(PROFISSIONAIS, "r");
    if (arquivo == NULL) {
        return;
    }
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (lerServico(linha, p) && iguais(p[S_PROFISSIONAL_ID], conta[U_EMAIL])) {
            strcpy(conta[U_CPF], p[S_CPF]);
            strcpy(conta[U_NASCIMENTO], p[S_NASCIMENTO]);
            strcpy(conta[U_CIDADE], p[S_CIDADE]);
            strcpy(conta[U_TELEFONE], p[S_TELEFONE]);
            break;
        }
    }
    fclose(arquivo);
}

int buscarConta(char email[], char tipo[], char conta[][TAM]) {
    FILE *arquivo = fopen(USUARIOS, "r");
    char linha[2400];
    if (arquivo == NULL) {
        return 0;
    }
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (lerUsuario(linha, conta) && iguais(conta[U_EMAIL], email) && iguais(conta[U_TIPO], tipo)) {
            fclose(arquivo);
            recuperarPerfilAntigo(conta);
            return 1;
        }
    }
    fclose(arquivo);
    return 0;
}

int salvarPerfil(char conta[][TAM]) {
    FILE *arquivo, *novo, *backup;
    char linha[2400];
    char atual[9][TAM];
    int encontrou = 0, erro = 0;
    backup = fopen("usuarios_backup.tmp", "r");
    if (backup != NULL) {
        fclose(backup);
        printf("Existe usuarios_backup.tmp. Confira o backup antes de continuar.\n");
        return 0;
    }
    arquivo = fopen(USUARIOS, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir os cadastros.\n");
        return 0;
    }
    novo = fopen("usuarios_novo.tmp", "w");
    if (novo == NULL) {
        fclose(arquivo);
        printf("Erro ao preparar o perfil.\n");
        return 0;
    }
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (lerUsuario(linha, atual) && iguais(atual[U_EMAIL], conta[U_EMAIL]) && iguais(atual[U_TIPO], conta[U_TIPO])) {
            encontrou++;
            if (!gravarLinha(novo, conta, 9)) {
                erro = 1;
            }
        } else if (fputs(linha, novo) == EOF) {
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
    if (erro || encontrou != 1) {
        remove("usuarios_novo.tmp");
        printf("Perfil nao salvo. Original preservado.\n");
        return 0;
    }
    if (rename(USUARIOS, "usuarios_backup.tmp") != 0) {
        remove("usuarios_novo.tmp");
        printf("Erro ao preservar cadastro.\n");
        return 0;
    }
    if (rename("usuarios_novo.tmp", USUARIOS) != 0) {
        if (rename("usuarios_backup.tmp", USUARIOS) != 0)
        {
            printf("Recupere usuarios.txt a partir de usuarios_backup.tmp.\n");
        }
        printf("Erro ao salvar perfil.\n");
        return 0;
    }
    remove("usuarios_backup.tmp");
    printf("Perfil salvo!\n");
    return 1;
}

int separarTicket(char linha[], char t[][TAM]) {
    if (separar(linha, t, 8)) {
        return 1;
    }
    if (separar(linha, t, 7)) {
        strcpy(t[7], "Nao informado (pedido antigo)");
        return 1;
    }
    return 0;
}

/* Le os campos do registro diretamente na matriz. */
int lerUsuario(char linha[], char registro[][TAM]) {
    return separarConta(linha, registro);
}

int gravarUsuario(char registro[][TAM]) {
    return salvar(USUARIOS, registro, 9);
}

/* Le os campos do registro diretamente na matriz. */
int lerServico(char linha[], char registro[][TAM]) {
    return separar(linha, registro, 9);
}

int gravarServico(char registro[][TAM]) {
    return salvar(PROFISSIONAIS, registro, 9);
}

/* Le os campos do registro diretamente na matriz. */
int lerSolicitacao(char linha[], char registro[][TAM]) {
    return separarTicket(linha, registro);
}

int gravarSolicitacao(char registro[][TAM]) {
    return salvar(TICKETS, registro, 8);
}

/* FOR grava cada campo; IF/ELSE escolhe separador ou fim de linha. */
int gravarLinha(FILE *arquivo, char campos[][TAM], int total) {
    int i, resultado;
    for (i = 0; i < total; i++) {
        if (i == total - 1) {
            resultado = fprintf(arquivo, "%s\n", campos[i]);
        } else {
            resultado = fprintf(arquivo, "%s|", campos[i]);
        }
        if (resultado < 0) {
            return 0;
        }
    }
    return 1;
}
