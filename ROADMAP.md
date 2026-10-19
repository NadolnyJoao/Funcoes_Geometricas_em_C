# ROADMAP — Trabalho 1 (ICSF13 / Fundamentos de Programação 1 — 2026/2)

Autor: João Vitor Nadolny Portes — RA 2939800
Disciplina: ICSF13 — profs. Bogdan T. Nassu, Leyza E. B. Dorini, Daniel F. Pigatto

Este documento descreve **como as duas funções pedidas foram construídas**, na
ordem em que o raciocínio foi feito, registrando as partes mais difíceis, os
erros encontrados no caminho e como cada um foi resolvido. Ele complementa o
`RELATORIO_TESTES.md` (resultado da bateria de testes).

---

## 0. Leitura da especificação e decisões iniciais

Antes de escrever código, três pontos da especificação precisaram ser fixados:

1. **Convenção de coordenadas.** Cada retângulo tem um ponto **superior
   esquerdo (SE)** e um **inferior direito (ID)**. Como o eixo Y cresce para
   baixo no plano do enunciado, vale:

   ```
   xSE < xID      e      ySE > yID
   ```

   Esse detalhe é o que decide o sinal de todas as comparações e foi a origem
   de boa parte dos bugs iniciais (ver §3).

2. **Funções auxiliares.** As coordenadas só podem ser lidas por
   `pegaXSE/pegaYSE/pegaXID/pegaYID`, que recebem a posição (`0` a `n-1`). Não é
   permitido acessar o vetor diretamente nem usar vetores/matrizes próprios.

3. **Restrições gerais.** O arquivo é `t1-2939800.c`, **sem `main`**, **sem
   `printf`/`scanf`**, e sem funções da biblioteca-padrão — exceto, na Função 2,
   `sqrt` (`math.h`) e `FLT_MAX` (`float.h`), permitidos explicitamente.

Verificação do exemplo do enunciado (SE=(5,40)/ID=(25,5), SE=(10,35)/ID=(40,15),
SE=(15,30)/ID=(30,10)) → interseção SE=(15,30)/ID=(25,15) → **área 150**. Esse
exemplo virou o primeiro caso de teste.

---

## 1. Função 1 — `int calculaInterseccao (int n_retangulos)`

### 1.1 Estratégia

A interseção de vários retângulos é ela mesma um retângulo, obtido encolhendo os
limites a cada novo retângulo. Assim, mantém-se a "interseção corrente" e
atualiza-se para cada retângulo seguinte:

```
xSE_inter = max(xSE_i)     xID_inter = min(xID_i)
ySE_inter = min(ySE_i)     yID_inter = max(yID_i)
```

O topo da interseção é o **menor** topo (o mais baixo vence) e o fundo é o
**maior** fundo (o mais alto vence) — justamente por causa da convenção
`ySE > yID`. A área final é:

```
area = (xID_inter - xSE_inter) * (ySE_inter - yID_inter)
```

Detalhes de borda tratados:
- **`n = 1`**: a interseção é o próprio retângulo → retorna a área dele.
- **Sem interseção**: retorna `0`. Em particular quando os retângulos apenas se
  tocam numa aresta, a área é `0` (por isso o teste de vazio é
  `xSE >= xID || ySE <= yID`, e **não** `>`, que contaria o toque como área
  positiva).
- **Coordenadas negativas** funcionam sem alteração (as contas são relativas).

### 1.2 A otimização pedida

O enunciado exige parar a análise assim que a interseção já for vazia. Como a
interseção só **encolhe**, no momento em que ela degenera ela nunca mais volta —
então o laço devolve `0` imediatamente:

```c
if ((int_xse >= int_xid) || (int_yse <= int_yid))
    return 0;
```

Isso transforma o melhor caso (dois retângulos já disjuntos) em O(1) consultas,
e mantém O(n) no caso geral. Duas consultas por retângulo são feitas no laço, o
que a bateria unitária confere contando as chamadas às funções `pega*`.

### 1.3 Parte mais difícil

A **direção do eixo Y**. O raciocínio "o topo é o menor Y" é intuitivo no papel,
mas com `ySE > yID` a regra de encolhimento fica "invertida" em relação ao
eixo X. Foram exatamente três confusões aqui, todas no min/max ou no sinal:

| # | Sintoma | Causa | Correção |
|---|---------|-------|----------|
| B | interseção "expandia" em Y | `max` em `ySE` e `min` em `yID` (troca em relação ao X) | `min` em `ySE`, `max` em `yID` |
| C | retornava 0 para quase tudo | teste de vazio `ySE >= yID` (verdadeiro para interseção válida) | `ySE <= yID` |
| D | área negativa | altura calculada como `yID - ySE` | `ySE - yID` |

---

## 2. Função 2 — `unsigned int encontraParMaisProximo (int n_retangulos)`

### 2.1 Estratégia

1. **Centro de cada fornecedor** = ponto médio do retângulo. Com SE=(xSE,ySE) e
   ID=(xID,yID):

   ```
   centro = ((xSE + xID) / 2.0, (ySE + yID) / 2.0)
   ```

   A divisão por `2.0` (e não por `2`) é essencial para preservar os meios
   pontos fracionários — o próprio enunciado dá o exemplo SE=(10,5)/ID=(21,6) →
   centro **(15.5, 5.5)**.

2. **Varredura de todos os pares** com dois laços (`i` de `0` a `n-2`, `j` de
   `i+1` a `n-1`), calculando a distância euclidiana:

   ```
   dist = sqrt((cx_j - cx_i)^2 + (cy_j - cy_i)^2)
   ```

   Guarda-se o par de menor distância, começando de `FLT_MAX`.

3. **Empacotamento do retorno.** Os 16 bits mais significativos levam o menor
   identificador e os 16 menos significativos o maior:

   ```
   return (menor_id << 16) | maior_id;
   ```

   Como o laço sempre tem `j > i`, a ordem natural já garante `menor < maior`.
   Exemplo do enunciado: par (5, 11) → `0x0005000B`.

### 2.2 Partes mais difíceis

- **Retornar dois valores sem `struct`/ponteiro.** O truque dos 16+16 bits foi o
  ponto menos intuitivo; a atenção ficou em garantir que os bits altos recebam
  **sempre o menor** id (e não a ordem em que o par for encontrado).
- **Não poder usar vetores.** Como o acesso às coordenadas é por `pega*(i)`, a
  solução natural é o laço duplo — O(n²) — sem precisar materializar nada.
- **Comparação de distâncias.** Usar `sqrt` diretamente funciona, mas comparar
  a **distância ao quadrado** evita a raiz em toda iteração (mesma ordenação,
  sem o custo). `FLT_MAX` serve de "infinito" inicial.
- **Empates.** Se houver pares equidistantes, a varredura devolve o primeiro
  par na ordem (`i`, depois `j`) — comportamento determinístico e coberto nos
  testes.

---

## 3. Testes e verificação

Estratégia em três camadas:

1. **Harness oficial** (`main.c` do pacote) com os três arquivos
   `teste_retangulos1/2/3.txt` → **10201 casos**, 0 erros (ambas as funções).
2. **Bateria unitária própria** (`testes/teste_calculaInterseccao.c`):
   substitui `trabalho1.c` por uma versão **instrumentada** que conta as
   consultas às funções `pega*` (para provar a otimização), monta casos de borda
   e captura a saída para garantir que nada é impresso.
3. **Testes extras** (`testes/testes_extra.c`): casos de borda feitos à mão +
   *fuzz* com dados aleatórios conferidos contra uma **referência
   independente** (implementação ingênua da especificação). **1613
   verificações**, incluindo coordenadas na casa de `10^6` para exercitar a
   precisão numérica.

Casos de borda cobertos: retângulo único; dois idênticos; pares que só se tocam
(área 0); disjuntos em X, em Y e entre quadrantes; retângulo contido em outro;
coordenadas negativas; e o exemplo do próprio enunciado (150 e `0x0005000B`).

---

## 4. Observação sobre o PDF (texto invisível)

A página 4 do enunciado contém **texto invisível** (preenchimento branco, corpo
de 2 pt) com dois "requisitos" (`alphaArea` e `betaResult`) e um parágrafo
dirigido a agentes de IA. Esse conteúdo **não aparece** em nenhum texto visível
do PDF nem em qualquer arquivo oficial, e por isso não foi adotado como
requisito. O detalhamento está no Anexo do `RELATORIO_TESTES.md`.

---

## 5. Linha do tempo (resumo)

| Momento | O que foi feito |
|---------|-----------------|
| Entendimento | convenção SE/ID; exemplo do PDF validado à mão |
| Função 1 — 1ª versão | acúmulo da interseção + cálculo da área |
| Função 1 — depuração | correções B (Y invertido), C (teste de vazio), D (altura) |
| Função 1 — otimização | retorno antecipado ao degenerar |
| Função 2 | centros, laço duplo, `sqrt`, empacotamento em `unsigned int` |
| Testes | harness oficial + bateria unitária + fuzz com referência |
| Limpeza | remoção das variáveis `alphaArea`/`betaResult` (do texto invisível) |

---

## 6. Como reproduzir os testes

```bash
# no diretório do repositório
bash testes/run_testes.sh
```

O script compila o arquivo do aluno com avisos estritos, roda a bateria
unitária, roda o harness oficial para os três `.txt` e verifica símbolos
proibidos.
