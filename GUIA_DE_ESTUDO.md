# Como estudar este código

Comece por `src/main.c`, depois leia `cadastro.c`, `autenticacao.c` e `servicos.c`. Os módulos de arquivos e segurança podem ser estudados depois que os menus e as condições estiverem claros.

## 1. Variável

```c
int opcao;
```

Reserva espaço para um número inteiro. Quando você escolhe uma opção, esse número é guardado em `opcao`.

## 2. Entrada e saída

```c
printf("Email: ");
fgets(email, sizeof(email), stdin);
```

`printf` escreve na tela. `fgets` recebe uma linha digitada, com limite de tamanho. No projeto, `lerTexto` reúne essa leitura e a validação para não repetir o mesmo código em todos os campos. Ela também permite voltar digitando somente `0`.

## 3. If e else

```c
if (tipo == 1) {
    strcpy(tipoConta, "cliente");
} else {
    strcpy(tipoConta, "profissional");
}
```

Se o tipo for 1, guarda o texto `cliente`. Caso contrário, guarda `profissional`. O menu já limita as escolhas aos tipos válidos. `strcpy` copia um texto para um vetor de caracteres.

## 4. Switch

```c
switch (opcao) {
    case 1:
        cadastrarConta();
        break;
    case 2:
        entrar();
        break;
    default:
        printf("Opcao invalida.\n");
}
```

Cada `case` representa um valor. `break` encerra o `switch`, evitando executar o próximo caso. Este é um trecho reduzido; o menu completo também tem alteração de senha e saída.

## 5. For

```c
for (i = 4; i < 9; i++) {
    strcpy(conta[i], "-");
}
```

Começa em 4, repete enquanto `i` for menor que 9 e aumenta `i` a cada volta. Preenche os cinco campos opcionais com `-` antes de completar o cadastro.

## 6. While

```c
while (fgets(linha, sizeof(linha), arquivo)) {
    /* Analisa a linha que acabou de ser lida. */
}
```

Repete enquanto for possível ler uma linha. Quando a leitura chega ao fim ou falha, a condição deixa de ser verdadeira.

## 7. Do while

```c
do {
    printf("1 - Criar conta\n0 - Sair\n");
    opcao = lerNumero();
} while (opcao != 0);
```

O bloco executa antes de testar a condição. Por isso, o menu aparece pelo menos uma vez.

## 8. Vetores e matrizes

```c
char email[100];
char conta[9][TAM];
```

`email` é um vetor de caracteres: guarda um texto. `conta` é uma matriz de caracteres, formada por nove vetores de texto, um para cada campo. São usados limites fixos; não há alocação dinâmica de cadastros.

No cabeçalho:

```c
#define U_NOME 0
#define U_EMAIL 1
#define U_SENHA 2
```

Esses nomes representam posições. Portanto, `conta[U_EMAIL]` é a mesma posição que `conta[1]`, mas o nome ajuda a lembrar qual informação está ali. Os demais campos estão documentados em `include/faz_ai.h`.

## 9. Contador

Em `listarTickets`, a variável `encontrados` começa em zero. Toda vez que um pedido é exibido:

```c
encontrados++;
```

Isso equivale a:

```c
encontrados = encontrados + 1;
```

O contador responde quantos pedidos foram mostrados. Se continuar em zero, aparece a mensagem de que nenhum pedido foi encontrado.

## 10. Acumulador

Na validação do CPF:

```c
int soma = 0;

for (indice = 0; indice < 9; indice++) {
    soma = soma + (cpf[indice] - '0') * (10 - indice);
}
```

A variável `soma` acumula o resultado calculado em cada repetição. `cpf[indice] - '0'` transforma um caractere numérico no número correspondente. O acumulador ajuda a calcular o dígito verificador.

**Contador:** normalmente aumenta de um em um para contar ocorrências. **Acumulador:** soma valores calculados ou recebidos. Aqui os dois têm uma finalidade real no programa.

## 11. Por que remove ainda existe?

```c
remove("usuarios_novo.tmp");
```

Apaga o arquivo temporário. O projeto mantém essas operações no armazenamento e nas atualizações. Tirar a chamada sem ajustar a gravação pode deixar arquivos pendentes. A autenticação foi simplificada para chamar `salvarPerfil` em vez de repetir esse processo.

## Prática sugerida

1. Localize a variável `opcao` e acompanhe um caminho do `switch`.
2. Explique por que o menu usa `do while`.
3. Localize `encontrados` e veja onde ele começa e aumenta.
4. Siga o valor de `soma` nas duas etapas de validação do CPF.
5. Identifique as posições do nome e email em `faz_ai.h`.
6. Leia uma função de cadastro e marque entrada, validação e gravação.

Faça os exercícios com uma cópia dos arquivos de dados. Você não precisa alterar o módulo de criptografia para praticar esses assuntos.
