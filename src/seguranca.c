/* A regra do sistema continua simples; a criptografia fica isolada aqui.
PBKDF2-HMAC-SHA256: salt aleatorio de 16 bytes, 600000 iteracoes.
Nunca implementar um hash caseiro para guardar senhas. */
#include "faz_ai.h"
#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>
#else
#include <openssl/evp.h>
#include <openssl/rand.h>
#endif
#define ITERACOES 600000
static int falhas = 0;
static time_t bloqueadoAte = 0;
int loginPermitido(void) {
    if (time(NULL) < bloqueadoAte) {
        printf("Limite de 3 tentativas. Aguarde 60 segundos.\n");
        return 0;
    }
    if (bloqueadoAte != 0) {
        falhas = 0;
        bloqueadoAte = 0;
    }
    return 1;
}

void loginFalhou(void) {
    falhas++;
    if (falhas >= 3) {
        bloqueadoAte = time(NULL) + 60;
        printf("Acesso bloqueado por 60 segundos nesta execucao.\n");
    }
}

void loginSucesso(void) {
    falhas = 0;
    bloqueadoAte = 0;
}

static int derivar(const char *senha, unsigned char salt[16], unsigned char chave[32]) {
#ifdef _WIN32
    BCRYPT_ALG_HANDLE algoritmo = NULL;
    NTSTATUS resultado;
    if (BCryptOpenAlgorithmProvider(&algoritmo, BCRYPT_SHA256_ALGORITHM, NULL,
    BCRYPT_ALG_HANDLE_HMAC_FLAG) != 0) {
        return 0;
    }
    resultado = BCryptDeriveKeyPBKDF2(algoritmo, (PUCHAR)senha, (ULONG)strlen(senha),
    salt, 16, ITERACOES, chave, 32, 0);
    BCryptCloseAlgorithmProvider(algoritmo, 0);
    return resultado == 0;
#else
    return PKCS5_PBKDF2_HMAC(senha, (int)strlen(senha), salt, 16, ITERACOES,
    EVP_sha256(), 32, chave) == 1;
#endif
}

static void hexadecimal(const unsigned char *bytes, int tamanho, char *texto) {
    int i;
    for (i = 0; i < tamanho; i++) {
        sprintf(texto + i * 2, "%02x", bytes[i]);
    }
}

static int decodificar(const char *texto, unsigned char *bytes, int tamanho) {
    int i;
    unsigned int valor;
    for (i = 0; i < tamanho; i++) {
        if (!isxdigit((unsigned char)texto[i*2]) || !isxdigit((unsigned char)texto[i*2+1])) {
            return 0;
        }
        if (sscanf(texto + i*2, "%2x", &valor) != 1) {
            return 0;
        }
        bytes[i] = (unsigned char)valor;
    }
    return 1;
}

int gerarHash(const char *senha, char hash[TAM]) {
    unsigned char salt[16], chave[32];
    char saltTexto[33], chaveTexto[65];
#ifdef _WIN32
    if (BCryptGenRandom(NULL, salt, 16, BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
        return 0;
    }
#else
    if (RAND_bytes(salt, 16) != 1) {
        return 0;
    }
#endif
    if (!derivar(senha, salt, chave)) {
        return 0;
    }
    hexadecimal(salt, 16, saltTexto);
    hexadecimal(chave, 32, chaveTexto);
    snprintf(hash, TAM, "pbkdf2$600000$%s$%s", saltTexto, chaveTexto);
    memset(chave, 0, sizeof(chave));
    return 1;
}

int conferirSenha(const char *senha, const char *hash) {
    unsigned char salt[16], esperado[32], calculado[32];
    unsigned int diferenca = 0;
    int i;
    /* Migra uma senha legada somente depois de autenticar com sucesso. */
    if (strncmp(hash, "pbkdf2$", 7) != 0) {
        return strcmp(senha, hash) == 0;
    }
    if (strlen(hash) != 111 || strncmp(hash, "pbkdf2$600000$", 14) != 0 || hash[46] != '$') {
        return 0;
    }
    if (!decodificar(hash + 14, salt, 16) || !decodificar(hash + 47, esperado, 32)) {
        return 0;
    }
    if (!derivar(senha, salt, calculado)) {
        return 0;
    }
    for (i = 0; i < 32; i++) {
        diferenca |= esperado[i] ^ calculado[i];
    }
    memset(calculado, 0, sizeof(calculado));
    return diferenca == 0;
}
