# Faz Aí — versão para estudar C básico

Esta versão mantém os menus, as cores, os cadastros, os anúncios, os pedidos e os arquivos de dados do projeto enviado. A lógica foi reescrita com comandos mais explícitos para facilitar a leitura.

## O que foi simplificado

- Os registros de usuários, profissionais e pedidos usam matrizes de texto em vez de structs próprias.
- As mensagens usam `printf`, com `\n` para pular linha.
- As condições usam `if/else`, sem operador ternário `? :`.
- Os blocos têm chaves e instruções separadas.
- A cópia dos campos usa `for` e `strcpy`, sem `memcpy`.
- A alteração de senha reaproveita `salvarPerfil`, sem repetir a rotina de arquivos na autenticação.
- As posições das matrizes têm nomes definidos no cabeçalho para evitar números sem explicação.

O objetivo é tornar a lógica mais fácil de estudar. Isso pode aumentar o número de linhas: separar as instruções deixa cada passo visível.

## Onde estão os assuntos pedidos

| Assunto | Exemplo real | Local |
|---|---|---|
| Variáveis | `int opcao` guarda a escolha do usuário. | `src/main.c`, função `main` |
| Entrada | `lerTexto` usa `fgets` para receber texto, inclusive espaços. | `src/interface.c` |
| Saída | `printf` mostra títulos, opções e mensagens. | `src/main.c` e demais módulos |
| `if/else` | Escolhe entre conta de cliente e profissional. | `src/cadastro.c`, `cadastrarDadosConta` |
| `switch` | Encaminha cada opção do menu. | `src/main.c`, `main` |
| `for` | Percorre os campos de um cadastro. | `src/cadastro.c`, `cadastrarDadosConta` |
| `while` | Lê uma linha de arquivo por vez. | `src/armazenamento.c`, `buscarConta` |
| `do while` | Repete o menu até escolher sair. | `src/main.c`, `main` |
| Vetores | `char email[100]` guarda um texto; cada posição guarda um caractere. | `src/autenticacao.c`, `entrarTipo` |
| Contador | `encontrados++` conta os pedidos mostrados. | `src/servicos.c`, `listarTickets` |
| Acumulador | `soma = soma + ...` acumula produtos para conferir o CPF. | `src/validacoes.c`, `cpfValido` |

Leia `GUIA_DE_ESTUDO.md` para exemplos explicados.

## Como executar no Windows

1. Extraia o ZIP completo para uma pasta nova.
2. Abra a pasta `Faz_Ai_basico` no VS Code.
3. Abra o terminal nessa pasta e execute:

```powershell
.\compilar.bat
.\Faz_Ai.exe
```

É necessário ter GCC/MinGW-w64 no PATH, com os headers e a biblioteca de importação `bcrypt` do Windows. O comando usado pelo arquivo de compilação é:

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude src/*.c -o Faz_Ai.exe -lbcrypt
```

O executável antigo não deve ser usado. Compile os fontes desta versão. O pacote não contém um novo executável Windows: a compilação e os testes foram realizados no Linux.

## Pastas e arquivos

| Local | Conteúdo |
|---|---|
| `src/main.c` | Home e menu inicial. |
| `src/interface.c` | Entrada, senha com asteriscos, menus e perfis. |
| `src/cadastro.c` | Cadastro de clientes e profissionais. |
| `src/autenticacao.c` | Login e alteração de senha. |
| `src/validacoes.c` | Email, preço, telefone, CPF e data. |
| `src/servicos.c` | Ofertas, pedidos e situações. |
| `src/edicao.c` | Edição de perfil e edição/exclusão de anúncios. |
| `src/armazenamento.c` | Leitura e gravação dos arquivos. |
| `src/integridade.c` | Diagnóstico de registros danificados e vínculos. |
| `src/seguranca.c` | Apoio para hash e bloqueio de tentativas. |
| `include/faz_ai.h` | Constantes e declarações das funções. |
| `testes/` | Verificação automatizada, separada do programa. |
| Pasta principal | `compilar.bat` e os três arquivos de dados `.txt`. |

Execute pela pasta principal, onde estão `usuarios.txt`, `profissionais.txt` e `tickets.txt`. Seus conteúdos foram preservados em relação ao ZIP anexado. Se você já cadastrou novos dados no computador depois de enviar o ZIP, mantenha uma cópia desses dados antes de trocar de pasta; este pacote contém somente os dados recebidos no anexo.

## O que continua funcionando

- Cadastro e login separados por tipo de conta, inclusive com o mesmo email nos dois tipos.
- Retorno com `0`, sem salvar formulário incompleto.
- Senhas ocultas com asteriscos e confirmação no cadastro.
- Edição do nome do cliente; edição de nome, cidade, telefone e habilidades do profissional.
- Vários anúncios por profissional, edição de descrição/preço e exclusão do próprio anúncio.
- Pedido por profissão e cidade; aceite pelo profissional compatível; conclusão e cancelamento autorizado.
- Histórico de pedidos preservado.
- Diagnóstico de registros danificados, duplicidade e referências inexistentes.

As opções são as mesmas: cliente 5 edita perfil e 6 cancela pedido; profissional 8 edita perfil, 9 edita/exclui anúncio e 10 cancela atendimento. Os emails permanecem como identificadores e não são editados. A exclusão é de anúncios, não de contas. Os pedidos continuam relacionados ao tipo de serviço, sem selecionar um anúncio específico nem fixar preço de contratação.

## Partes que vão além dos assuntos básicos

Um programa com arquivos persistentes, senha protegida e cores no terminal não pode ser explicado apenas com variáveis e laços. Por isso, foram mantidos alguns recursos de apoio:

- Funções e matrizes de caracteres organizam o sistema.
- `FILE *`, `fopen`, `fgets`, `fprintf` e `fclose` permitem ler e gravar os dados.
- `remove` apaga um arquivo temporário ou backup; `rename` troca seu nome. Não equivalem a mensagens na tela.
- As funções de texto da biblioteca `string.h` copiam e comparam strings.
- A ocultação de senha e as cores usam recursos do sistema operacional.
- `seguranca.c` usa PBKDF2-HMAC-SHA256, salt aleatório e 600.000 iterações. Essa parte é um módulo de apoio mais avançado; não foi trocada por um hash caseiro ou senha em texto simples.
- A consulta da data atual usa `struct tm`, da biblioteca padrão. As structs próprias de cadastro foram retiradas.

As senhas existentes continuam válidas. A alteração exige a senha atual. O limite de tentativas vale durante a execução: três erros bloqueiam o acesso por 60 segundos. Ao fechar o programa, esse controle em memória reinicia.

Use uma execução por vez. Os arquivos locais podem ser alterados por quem tem acesso à pasta; este continua sendo um projeto acadêmico local, sem servidor ou controle de concorrência.

## Testes

A versão foi compilada no Linux com C11, `-Wall -Wextra -Wpedantic -Werror`, sem avisos. Passou nos testes de cadastro, login, migração de senha, alteração de senha, bloqueio, perfil, anúncios, pedidos, permissões, integridade e proteção de backup.

Para repetir no Linux, com GCC, Python 3 e os arquivos de desenvolvimento do OpenSSL:

```bash
python3 testes/testar.py
```

Os testes criam dados fictícios em uma pasta temporária. Para compilar o programa no Linux:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude src/*.c -lcrypto -o Faz_Ai
./Faz_Ai
```
