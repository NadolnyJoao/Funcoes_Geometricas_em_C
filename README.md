# Funções geométricas em C — Trabalho 1 (ICSF13)

Trabalho 1 da disciplina **ICSF13 — Fundamentos de Programação 1** (2026/2),
UTFPR. Autor: **João Vitor Nadolny Portes — RA 2939800**.

Implementação, em C, de duas funções que operam sobre retângulos lidos através
de funções auxiliares `pegaXSE/pegaYSE/pegaXID/pegaYID`:

- `int calculaInterseccao (int n_retangulos)` — área da interseção de vários
  retângulos (com a otimização de parar assim que a interseção fica vazia);
- `unsigned int encontraParMaisProximo (int n_retangulos)` — par de retângulos
  cujos centros estão mais próximos, com os dois identificadores empacotados em
  um `unsigned int` (16 bits + 16 bits).

Restrições do enunciado respeitadas: sem `main`, sem `printf`/`scanf`, sem
vetores/matrizes, e sem biblioteca-padrão além de `sqrt` e `FLT_MAX` (permitidos
na Função 2).

## Estrutura

| Arquivo | Origem | Descrição |
|---------|--------|-----------|
| `t1-2939800.c` | **entrega** | implementação das duas funções pedidas |
| `main.c` | professores | harness de teste |
| `trabalho1.h` / `trabalho1.c` | professores | protótipos e funções auxiliares |
| `teste_retangulos1/2/3.txt` | professores | dados de teste (1 / 200 / 10000 casos) |
| `Trabalho1.pdf` | professores | especificação |
| `t1-pacote.zip` | professores | pacote original |
| `ROADMAP.md` | próprio | processo de construção das duas funções |
| `RELATORIO_TESTES.md` | próprio | resultados da bateria de testes |
| `testes/` | próprio | testes unitários e *fuzz* |

## Compilar e executar (harness dos professores)

```bash
gcc -std=c17 -I. main.c trabalho1.c t1-2939800.c -lm -o trabalho1
./trabalho1
```

No `main.c` escolha o arquivo de dados pela macro `ARQUIVO_RETANGULOS` e quais
funções testar pelas macros `TESTA_INTERSECCAO` / `TESTA_PAR_MAIS_PROXIMO`. O
projeto também abre normalmente na IDE **Code::Blocks** (basta adicionar
`main.c`, `trabalho1.c`, `trabalho1.h` e `t1-2939800.c`).

## Testes

```bash
bash testes/run_testes.sh
```

Resultado esperado: **BATERIA APROVADA** (harness oficial com 10201 casos sem
erro, testes unitários e checagem de símbolos proibidos). Os detalhes estão em
`RELATORIO_TESTES.md`.
