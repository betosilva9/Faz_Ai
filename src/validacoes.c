#include "faz_ai.h"
int iguais(char a[], char b[]) {
    int i = 0;
    while (a[i] && b[i]) {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) {
            return 0;
        }
        i++;
    }
    return a[i] == b[i];
}

int emailValido(char email[]) {
    int i, arroba = -1, ponto = -1;
    for (i = 0; email[i]; i++) {
        if (isspace((unsigned char)email[i])) {
            return 0;
        }
        if (email[i] == '@') {
            if (arroba != -1) {
                return 0;
            }
            arroba = i;
        }
        if (email[i] == '.' && arroba != -1) {
            ponto = i;
        }
    }
    return arroba > 0 && ponto > arroba + 1 && ponto < i - 1;
}

/* Aceita valores positivos com ponto ou virgula, como 150,00. */
int precoValido(char texto[], double *valor) {
    int i, pontos = 0, digitos = 0;
    for (i = 0; texto[i]; i++) {
        if (isdigit((unsigned char)texto[i])) {
            digitos++;
        } else if (texto[i] == '.' || texto[i] == ',') {
            texto[i] = '.';
            pontos++;
        } else {
            return 0;
        }
    }
    if (digitos == 0 || pontos > 1) {
        return 0;
    }
    *valor = atof(texto);
    return *valor > 0 && *valor <= 1000000;
}

/* WHILE percorre o vetor; FOR nao e obrigatorio para percorrer textos. */
void apenasDigitos(char destino[], size_t tamanho, const char origem[]) {
    size_t i = 0, j = 0;
    while (origem[i] != '\0' && j + 1 < tamanho) {
        if (isdigit((unsigned char)origem[i])) {
            destino[j] = origem[i];
            j++;
        }
        i++;
    }
    destino[j] = '\0';
}

int telefoneValido(const char telefone[]) {
    int i, numeros = 0;
    /* CONTADOR de digitos encontrados. */
    for (i = 0; telefone[i] != '\0'; i++) {
        if (isdigit((unsigned char)telefone[i])) {
            numeros++;
        } else if (telefone[i] != ' ' && telefone[i] != '(' && telefone[i] != ')' && telefone[i] != '-') {
            return 0;
        }
    }
    return numeros == 10 || numeros == 11;
}

int cpfValido(const char *cpfDigitado) {
    /* Valida formato e digitos verificadores do CPF. */
    char cpf[16];
    int soma = 0, indice, digito;
    /* ACUMULADOR: soma os produtos do CPF. */
    for (indice = 0; cpfDigitado[indice] != '\0'; indice++)
    {
        if (!isdigit((unsigned char)cpfDigitado[indice]) && cpfDigitado[indice] != '.' &&
        cpfDigitado[indice] != '-')
        {
            return 0;
        }
    }
    apenasDigitos(cpf, sizeof(cpf), cpfDigitado);
    if (strlen(cpf) != 11 || strspn(cpf, "0123456789") != 11)
    {
        return 0;
    }
    for (indice = 1; indice < 11 && cpf[indice] == cpf[0]; indice++) {
    }
    if (indice == 11)
    {
        return 0;
    }
    for (indice = 0; indice < 9; indice++)
    {
        soma = soma + (cpf[indice] - '0') * (10 - indice);
    }
    if (soma % 11 < 2) {
        digito = 0;
    } else {
        digito = 11 - soma % 11;
    }
    if (digito != cpf[9] - '0')
    {
        return 0;
    }
    soma = 0;
    for (indice = 0; indice < 10; indice++)
    {
        soma = soma + (cpf[indice] - '0') * (11 - indice);
    }
    if (soma % 11 < 2) {
        digito = 0;
    } else {
        digito = 11 - soma % 11;
    }
    return digito == cpf[10] - '0';
}

int dataValida(const char *data) {
    /* Valida uma data no formato DD/MM/AAAA. */
    static const int dias[] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    }
    ;
    int dia, mes, ano, limite, indice;
    time_t agora = time(NULL);
    struct tm *hoje = localtime(&agora);
    if (strlen(data) != 10 || data[2] != '/' || data[5] != '/')
    {
        return 0;
    }
    for (indice = 0; indice < 10; indice++)
    {
        if (indice != 2 && indice != 5 && !isdigit((unsigned char)data[indice]))
        {
            return 0;
        }
    }
    dia = (data[0] - '0') * 10 + data[1] - '0';
    mes = (data[3] - '0') * 10 + data[4] - '0';
    ano = (data[6] - '0') * 1000 + (data[7] - '0') * 100 + (data[8] - '0') * 10 + data[9] - '0';
    /* Apenas valida; quem chamou a funcao pede a data completa novamente. */
    if (ano < 1900 || (hoje && ano > hoje->tm_year + 1900) || mes < 1 || mes > 12)
    {
        return 0;
    }
    limite = dias[mes - 1];
    if (mes == 2 && ((ano % 4 == 0 && ano % 100 != 0) || ano % 400 == 0))
    {
        limite = 29;
    }
    return dia >= 1 && dia <= limite;
}

/* CPF e exclusivo entre profissionais; cliente pode ter o mesmo email. */
int cpfPermitidoNaConta(char email[], char cpf[]) {
    FILE *arquivo;
    char linha[2400], cadastrado[16];
    char usuario[9][TAM];
    char servico[9][TAM];
    int f, valido;
    for (f = 0; f < 2; f++) {
        if (f == 0) {
            arquivo = fopen(USUARIOS, "r");
        } else {
            arquivo = fopen(PROFISSIONAIS, "r");
        }
        if (!arquivo) {
            if (errno == ENOENT) {
                continue;
            }
            perror("Consultar CPF");
            return -1;
        }
        while (fgets(linha, sizeof(linha), arquivo)) {
            if (f == 0) {
                valido = lerUsuario(linha, usuario);
                if (!valido) {
                    fclose(arquivo);
                    printf("char danificado.\n");
                    return -1;
                }
                if (!iguais(usuario[U_TIPO], "profissional") || strcmp(usuario[U_CPF], "-") == 0) {
                    continue;
                }
                apenasDigitos(cadastrado, sizeof(cadastrado), usuario[U_CPF]);
                if (!iguais(usuario[U_EMAIL], email) && strcmp(cadastrado, cpf) == 0) {
                    fclose(arquivo);
                    printf("CPF ja vinculado a outra conta.\n");
                    return 0;
                }
            } else {
                if (!lerServico(linha, servico)) {
                    fclose(arquivo);
                    printf("Anuncio danificado.\n");
                    return -1;
                }
                apenasDigitos(cadastrado, sizeof(cadastrado), servico[S_CPF]);
                if ((iguais(servico[S_PROFISSIONAL_ID], email) && strcmp(cadastrado, cpf) != 0) ||
                (!iguais(servico[S_PROFISSIONAL_ID], email) && strcmp(cadastrado, cpf) == 0)) {
                    fclose(arquivo);
                    printf("CPF nao corresponde ao vinculo dos anuncios.\n");
                    return 0;
                }
            }
        }
        if (ferror(arquivo)) {
            fclose(arquivo);
            perror("Consultar CPF");
            return -1;
        }
        fclose(arquivo);
    }
    return 1;
}

int perfilCompleto(char conta[][TAM]) {
    int i;
    for (i = 4; i < 9; i++) {
        if (strcmp(conta[i], "-") == 0) {
            return 0;
        }
    }
    return 1;
}

