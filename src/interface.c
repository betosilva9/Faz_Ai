#include "faz_ai.h"
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
int voltou = 0;
const char profissoes[][100] = {
    "Eletricista", "Encanador", "Pedreiro", "Pintor", "Carpinteiro","Marceneiro", "Serralheiro",
    "Azulejista", "Gesseiro", "Jardineiro", "Diarista", "Faxineiro", "Cozinheiro", "Confeiteiro", "Garcom", "Fotografo", "Videomaker",
    "Designer Grafico", "Desenvolvedor de Software", "Tecnico de Informatica", "Tecnico de Celular", "Tecnico de Computadores",
    "Instalador de Ar-Condicionado", "Tecnico de Refrigeracao", "Mecanico", "Eletricista Automotivo", "Lavador de Carros", "Funileiro",
    "Motoboy", "Motorista", "Professor Particular", "Tradutor", "Redator", "Social Media", "Cabeleireiro", "Barbeiro", "Manicure",
    "Maquiador", "Personal Trainer", "Professor de Musica", "Professor de Idiomas", "Costureiro", "Decorador", "Cerimonialista",
    "Cuidador de Idosos", "Baba", "Pet Sitter", "Adestrador de Caes", "Tecnico de Seguranca Eletronica"
}
;
int entradaConfirmada(const char texto[]) {
    voltou = 0;
    if (strcmp(texto, "0") != 0) {
        return 1;
    }
    voltou = 1;
    return 0;
}

void avisoVoltar(void) {
    printf("Digite somente 0 e pressione ENTER em qualquer campo (inclusive senha) para voltar ao menu anterior.\n");
}

/* Le nomes com espacos e impede que | estrague o arquivo de dados. */
int lerTexto(char rotulo[], char texto[], int tamanho) {
    int c, valido;
    do {
        printf("%s", rotulo);
        if (fgets(texto, tamanho, stdin) == NULL) {
            exit(0);
        }
        valido = strchr(texto, '\n') != NULL;
        if (!valido) {
            /* WHILE: descarta o que nao coube no vetor. */
            c = getchar();
            valido = c == '\n' || c == EOF;
            while (c != '\n' && c != EOF) {
                c = getchar();
            }
        }
        texto[strcspn(texto, "\r\n")] = '\0';
        valido = valido && texto[0] != '\0' && strchr(texto, '|') == NULL;
        if (!valido) {
            printf("Preencha o campo sem | e respeite o limite de tamanho.\n");
        }
    } while (!valido);
    /* DO WHILE: pede o campo pelo menos uma vez. */
    return entradaConfirmada(texto);
}

/* Guarda a senha verdadeira no vetor, mas mostra apenas * na tela. */
int lerSenha(char rotulo[], char senha[], int tamanho) {
    int i = 0;
    // Contador de caracteres guardados na senha
    int tecla;
    int invalida = 0;
#ifndef _WIN32
    struct termios antigo, novo;
    int terminal = tcgetattr(STDIN_FILENO, &antigo) == 0;
    if (terminal) {
        novo = antigo;
        novo.c_lflag &= ~(ECHO | ICANON);
        // Desliga a exibicao e le tecla por tecla
        novo.c_cc[VMIN] = 1;
        novo.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &novo);
    }
#endif
    printf("%s", rotulo);
    fflush(stdout);
    while (1) {
#ifdef _WIN32
        tecla = _getch();
        // Le a tecla sem exibir a senha no console
        if (tecla == 0 || tecla == 224) {
            _getch();
            // Descarta teclas especiais, como as setas
            continue;
        }
#else
        tecla = getchar();
        if (tecla == EOF) {
            if (terminal) {
                tcsetattr(STDIN_FILENO, TCSANOW, &antigo);
            }
            exit(0);
        }
#endif
        if (tecla == 13 || tecla == 10) {
            // Enter confirma uma senha nao vazia
            if (invalida) {
                printf("\nSenha invalida ou muito longa. Digite novamente.\n%s", rotulo);
                i = 0;
                invalida = 0;
                continue;
            }
            if (i > 0) {
                break;
            }
        } else if (tecla == 8 || tecla == 127) {
            // Backspace apaga a ultima letra
            if (i > 0) {
                i--;
                printf("\b \b");
                // Apaga tambem o asterisco na tela
            }
        } else if (tecla >= 32 && tecla <= 126 && tecla != '|' && i < tamanho - 1) {
            senha[i] = (char)tecla;
            // Guarda o caractere real para conferir no login
            i++;
            printf("*");
            // Mostra um asterisco para cada caractere digitado
        } else if (tecla >= 32 && tecla <= 126) {
            /* Nao transforma uma entrada como 0| em um comando de retorno. */
            invalida = 1;
        }
        fflush(stdout);
    }
    senha[i] = '\0';
    // Marca o fim da senha dentro do vetor
#ifndef _WIN32
    if (terminal) {
        tcsetattr(STDIN_FILENO, TCSANOW, &antigo);
    }
#endif
    printf("\n");
    return entradaConfirmada(senha);
}

/* Usa um vetor de caracteres para ler e conferir a opcao numerica. */
int lerNumero(void) {
    char texto[32], sobra;
    int numero;
    while (1) {
        if (!lerTexto("\nEscolha: ", texto, sizeof(texto))) {
            return 0;
        }
        if (sscanf(texto, "%d %c", &numero, &sobra) == 1 &&
        numero > 0 && numero <= 50) {
            return numero;
        }
        printf("Digite um numero de 1 a 50, ou somente 0 para voltar.\n");
    }
}

/* Separa uma linha em uma matriz: campos[0], campos[1] e assim por diante. */
int escolherServico(char servico[]) {
    int i, opcao;
    do {
        avisoVoltar();
        for (i = 0; i < TOTAL; i++) {
            printf("%2d - %s\n", i + 1, profissoes[i]);
        }
        printf("50 - Outro servico\n0 - Voltar\n\n");
        opcao = lerNumero();
        if (opcao == 0) {
            return 0;
        }
        if (opcao == 50) {
            if (!lerTexto("Servico: ", servico, 100)) {
                continue;
            }
        } else {
            strcpy(servico, profissoes[opcao - 1]);
        }
        return opcao;
    } while (1);
}

void esperarVoltar(void) {
    printf("0 - Voltar ao menu anterior\n");
    while (lerNumero() != 0) {
        printf("Digite somente 0 e ENTER para voltar.\n");
    }
}

/* O perfil publico mostra qualificacoes e servicos, sem CPF, nascimento ou senha. */
void mostrarPerfilProfissional(char conta[][TAM], int proprio) {
    FILE *arquivo;
    char linha[2400];
    char p[9][TAM];
    recuperarPerfilAntigo(conta);
    printf("\n=== PERFIL PROFISSIONAL ===\nNome: %s\nCidade: %s\n", conta[U_NOME], conta[U_CIDADE]);
    if (strcmp(conta[U_HABILIDADES], "-") == 0) {
        printf("Habilidades e experiencias: Ainda nao informadas\n");
    } else {
        printf("Habilidades e experiencias: %s\n", conta[U_HABILIDADES]);
    }
    if (proprio)
    {
        printf("Email: %s\nCPF: %s\nNascimento: %s\nTelefone: %s\n",
        conta[U_EMAIL], conta[U_CPF], conta[U_NASCIMENTO], conta[U_TELEFONE]);
    }
    arquivo = fopen(PROFISSIONAIS, "r");
    if (arquivo != NULL) {
        while (fgets(linha, sizeof(linha), arquivo)) {
            if (lerServico(linha, p) && iguais(p[S_PROFISSIONAL_ID], conta[U_EMAIL]))
            {
                printf("\nServico: %s\nDescricao: %s\nValor: R$ %s\n", p[S_TIPO], p[S_DESCRICAO], p[S_PRECO]);
            }
        }
        fclose(arquivo);
    }
}

void meuPerfilProfissional(char conta[][TAM]) {
    int opcao;
    char atualizado[9][TAM];
    int i;
    do {
        mostrarPerfilProfissional(conta, 1);
        if (perfilCompleto(conta)) {
            esperarVoltar();
            return;
        }
        printf("Cadastro antigo: faltam dados no perfil.\n1 - Completar cadastro\n0 - Voltar\n");
        opcao = lerNumero();
        if (opcao == 0) {
            return;
        }
        if (opcao != 1) {
            printf("Opcao invalida.\n");
            continue;
        }
        for (i = 0; i < 9; i++) {
            strcpy(atualizado[i], conta[i]);
        }
        if (lerDadosProfissionais(atualizado) && salvarPerfil(atualizado)) {
            for (i = 0; i < 9; i++) {
                strcpy(conta[i], atualizado[i]);
            }
        }
    } while (1);
}

/* O cliente escolhe um profissional pelo email exibido na lista. */
void consultarProfissionais(void) {
    FILE *arquivo;
    char linha[2400], email[100];
    char conta[9][TAM];
    int encontrados;
    do {
        arquivo = fopen(USUARIOS, "r");
        encontrados = 0;
        printf("\n=== PROFISSIONAIS ===\n");
        if (arquivo != NULL) {
            while (fgets(linha, sizeof(linha), arquivo)) {
                if (lerUsuario(linha, conta) && iguais(conta[U_TIPO], "profissional")) {
                    recuperarPerfilAntigo(conta);
                    printf("Nome: %s | Cidade: %s | Email: %s\n", conta[U_NOME], conta[U_CIDADE], conta[U_EMAIL]);
                    encontrados++;
                }
            }
            fclose(arquivo);
        }
        if (!encontrados) {
            printf("Nenhum profissional cadastrado.\n");
            esperarVoltar();
            return;
        }
        if (!lerTexto("Email do profissional para ver o perfil (0 para voltar): ", email, sizeof(email))) {
            return;
        }
        if (!buscarConta(email, "profissional", conta)) {
            printf("char nao encontrado.\n");
            continue;
        }
        mostrarPerfilProfissional(conta, 0);
        esperarVoltar();
    } while (1);
}

/* Reutiliza os dados do perfil; a oferta pede apenas informacoes do servico. */
void painelCliente(char conta[][TAM]) {
    int opcao;
    do {
        printf("\nOla, %s! MENU DO CLIENTE\n", conta[U_NOME]);
        printf("Conta exclusiva para solicitar servicos e acompanhar seus pedidos.\n");
        printf("Para trabalhar, saia da conta e entre com uma conta de profissional.\n");
        printf("1 - Solicitar servico\n2 - Meu perfil\n3 - Meus pedidos\n4 - Perfis dos profissionais\n5 - Editar perfil\n6 - Cancelar pedido\n");
        printf("0 - Voltar ao menu inicial (sair da conta)\n");
        avisoVoltar();
        opcao = lerNumero();
        switch (opcao) {
            case 1:
                criarTicket(conta[U_EMAIL]);
                break;
            case 2:
                printf("Nome: %s\nEmail: %s\nTipo: %s\n", conta[U_NOME], conta[U_EMAIL], conta[U_TIPO]);
                break;
            case 3:
                listarTickets(conta[U_EMAIL], 1);
                break;
            case 4:
                consultarProfissionais();
                break;
            case 5:
                editarPerfil(conta);
                break;
            case 6:
                atualizarTicket(conta[U_EMAIL], 3);
                break;
            case 0:
                printf("Conta encerrada.\n");
                break;
            default:
                printf("Opcao invalida no menu do cliente.\n");
        }
    } while (opcao != 0);
}

void painelProfissional(char conta[][TAM]) {
    int opcao;
    do {
        printf("\nOla, %s! MENU DO PROFISSIONAL\n", conta[U_NOME]);
        printf("Conta exclusiva para oferecer servicos e realizar trabalhos.\n");
        printf("Para contratar um servico, digite 0 para sair e entre com uma conta de cliente.\n");
        printf("2 - Meu perfil\n3 - Oferecer servico\n4 - Pedidos disponiveis\n"
        "5 - Aceitar pedido\n6 - Meus atendimentos\n7 - Concluir servico\n8 - Editar perfil\n9 - Editar/excluir anuncio\n10 - Cancelar atendimento\n");
        printf("0 - Voltar ao menu inicial (sair da conta)\n");
        avisoVoltar();
        opcao = lerNumero();
        switch (opcao) {
            case 2:
                meuPerfilProfissional(conta);
                break;
            case 3:
                oferecerServico(conta[U_EMAIL]);
                break;
            case 4:
                listarTickets(conta[U_EMAIL], 2);
                break;
            case 5:
                atualizarTicket(conta[U_EMAIL], 1);
                break;
            case 6:
                listarTickets(conta[U_EMAIL], 3);
                break;
            case 7:
                atualizarTicket(conta[U_EMAIL], 2);
                break;
            case 8:
                editarPerfil(conta);
                break;
            case 9:
                editarAnuncio(conta[U_EMAIL]);
                break;
            case 10:
                atualizarTicket(conta[U_EMAIL], 4);
                break;
            case 0:
                printf("Conta encerrada.\n");
                break;
            default:
                printf("Opcao invalida no menu do profissional. Para contratar, saia e entre como cliente.\n");
        }
    } while (opcao != 0);
}

void painel(char conta[][TAM]) {
    if (iguais(conta[U_TIPO], "cliente")) {
        painelCliente(conta);
    } else if (iguais(conta[U_TIPO], "profissional")) {
        painelProfissional(conta);
    } else {
        printf("Tipo de conta invalido. Acesso negado.\n");
    }
}

/* Confere email e senha nos registros salvos. */
