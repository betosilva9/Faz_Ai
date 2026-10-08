#ifndef FAZ_AI_H
#define FAZ_AI_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#define TAM 256
#define USUARIOS "usuarios.txt"
#define PROFISSIONAIS "profissionais.txt"
#define TICKETS "tickets.txt"
#define TOTAL 49
extern const char profissoes[][100];
extern int voltou;
/* Cada registro e uma matriz: uma linha para cada campo de texto.
   Exemplo: conta[U_NOME] e o vetor que guarda o nome. */

/* Campos de usuario. */
#define U_NOME 0
#define U_EMAIL 1
#define U_SENHA 2
#define U_TIPO 3
#define U_CPF 4
#define U_NASCIMENTO 5
#define U_CIDADE 6
#define U_TELEFONE 7
#define U_HABILIDADES 8

/* Campos de servico. */
#define S_PROFISSIONAL_ID 0
#define S_NOME 1
#define S_CPF 2
#define S_NASCIMENTO 3
#define S_TIPO 4
#define S_DESCRICAO 5
#define S_CIDADE 6
#define S_TELEFONE 7
#define S_PRECO 8

/* Campos de solicitacao. */
#define P_CODIGO 0
#define P_CLIENTE_ID 1
#define P_SERVICO 2
#define P_CIDADE 3
#define P_DESCRICAO 4
#define P_SITUACAO 5
#define P_PROFISSIONAL_ID 6
#define P_ENDERECO 7

int entradaConfirmada(const char texto[]);
void avisoVoltar(void);
int lerTexto(char rotulo[], char texto[], int tamanho);
int lerSenha(char rotulo[], char senha[], int tamanho);
int lerNumero(void);
int separar(char linha[], char campos[][TAM], int total);
int separarConta(char linha[], char conta[][TAM]);
int iguais(char a[], char b[]);
int existeConta(char email[], char tipo[]);
int salvar(char caminho[], char campos[][TAM], int total);
int emailValido(char email[]);
int precoValido(char texto[], double *valor);
int escolherServico(char servico[]);
void apenasDigitos(char destino[], size_t tamanho, const char origem[]);
int telefoneValido(const char *telefone);
int cpfValido(const char *cpfDigitado);
int dataValida(const char *data);
int lerDadosProfissionais(char conta[][TAM]);
void cadastrarDadosConta(int tipo);
void cadastrarConta(void);
int cpfPermitidoNaConta(char email[], char cpf[]);
void recuperarPerfilAntigo(char conta[][TAM]);
int buscarConta(char email[], char tipo[], char conta[][TAM]);
int perfilCompleto(char conta[][TAM]);
int salvarPerfil(char conta[][TAM]);
void esperarVoltar(void);
void mostrarPerfilProfissional(char conta[][TAM], int proprio);
void meuPerfilProfissional(char conta[][TAM]);
void consultarProfissionais(void);
void oferecerServico(char email[]);
int separarTicket(char linha[], char t[][TAM]);
int novoCodigoTicket(char codigo[]);
void criarTicket(char email[]);
int atendeTicket(char email[], char t[][TAM]);
void listarTickets(char email[], int modo);
void atualizarTicket(char email[], int acao);
void painelCliente(char conta[][TAM]);
void painelProfissional(char conta[][TAM]);
void painel(char conta[][TAM]);
void entrarTipo(int tipo);
void entrar(void);
void redefinirSenhaTipo(int tipo);
void redefinirSenha(void);
int lerUsuario(char linha[], char registro[][TAM]);
int gravarUsuario(char registro[][TAM]);
int lerServico(char linha[], char registro[][TAM]);
int gravarServico(char registro[][TAM]);
int lerSolicitacao(char linha[], char registro[][TAM]);
int gravarSolicitacao(char registro[][TAM]);
int gerarHash(const char *senha, char hash[TAM]);
int conferirSenha(const char *senha, const char *hash);
int loginPermitido(void);
void loginFalhou(void);
void loginSucesso(void);
int verificarArquivos(void);
int substituirArquivo(const char *caminho, const char *temporario);
void editarPerfil(char conta[][TAM]);
void editarAnuncio(char email[]);
int gravarLinha(FILE *arquivo, char campos[][TAM], int total);
#endif
