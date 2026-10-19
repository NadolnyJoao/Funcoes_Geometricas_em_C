/*============================================================================*/
/* Bateria de testes unitarios para calculaInterseccao                        */
/* Trabalho 1 - ICSF13 - Joao Vitor Nadolny Portes (2939800)                   */
/*----------------------------------------------------------------------------*/
/* Este arquivo NAO faz parte da entrega. Ele substitui trabalho1.c por uma    */
/* implementacao instrumentada das funcoes auxiliares, permitindo:             */
/*   - montar conjuntos de retangulos arbitrarios;                             */
/*   - contar quantas consultas sao feitas (testa a otimizacao pedida);        */
/*   - verificar que a funcao nao imprime nada (proibido pela especificacao).  */
/*============================================================================*/

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "trabalho1.h"

/*============================================================================*/
/* Implementacao instrumentada das funcoes auxiliares. */

#define MAX_RETANGULOS 64

static int rxse [MAX_RETANGULOS];
static int ryse [MAX_RETANGULOS];
static int rxid [MAX_RETANGULOS];
static int ryid [MAX_RETANGULOS];

static long consultas_totais;      /* total de chamadas a pega* */
static int  maior_pos_consultado;  /* maior indice de retangulo consultado */

int pegaXSE (int pos) { consultas_totais++; if (pos > maior_pos_consultado) maior_pos_consultado = pos; return rxse [pos]; }
int pegaYSE (int pos) { consultas_totais++; if (pos > maior_pos_consultado) maior_pos_consultado = pos; return ryse [pos]; }
int pegaXID (int pos) { consultas_totais++; if (pos > maior_pos_consultado) maior_pos_consultado = pos; return rxid [pos]; }
int pegaYID (int pos) { consultas_totais++; if (pos > maior_pos_consultado) maior_pos_consultado = pos; return ryid [pos]; }

void criaVetorRetangulos (int tamanho) { (void) tamanho; }
void destroiVetorRetangulos () {}
void adicionaRetangulo (int pos, int tlx, int tly, int brx, int bry)
{
    rxse [pos] = tlx; ryse [pos] = tly; rxid [pos] = brx; ryid [pos] = bry;
}
void mostraRetangulos (int n) { (void) n; }

/*============================================================================*/
/* Referencia independente (implementacao "ingenua" da especificacao).         */
/* Convencao, segundo o PDF e os dados dos professores:                        */
/*   xSE < xID  e  ySE > yID  (o ponto superior-esquerdo tem Y maior).         */

static int referencia (int n)
{
    int i;
    int xse, xid, yse, yid;

    if (n <= 0)
        return 0;

    xse = rxse [0]; xid = rxid [0]; yse = ryse [0]; yid = ryid [0];
    for (i = 1; i < n; i++)
    {
        if (rxse [i] > xse) xse = rxse [i];
        if (rxid [i] < xid) xid = rxid [i];
        if (ryse [i] < yse) yse = ryse [i];   /* menor Y superior */
        if (ryid [i] > yid) yid = ryid [i];   /* maior Y inferior  */
    }
    if (xse >= xid || yse <= yid)
        return 0;
    return (xid - xse) * (yse - yid);
}

/*============================================================================*/
/* Infra de relatorio. */

static int total_testes;
static int total_falhas;

/* Contagem de causas (um teste pode ter mais de uma causa). */
static int falhas_area;
static int falhas_saida;
static int falhas_otimizacao;

static void reporta_falha (const char* nome, const char* motivo)
{
    total_falhas++;
    printf ("  [FALHA] %s\n           -> %s\n", nome, motivo);
}

/*============================================================================*/
/* Executa calculaInterseccao capturando qualquer saida impressa em stdout.    */

static int saida_vazia;
static int ultimo_retorno;

static void chama (int n)
{
    FILE* tmp;
    int saved_stdout;
    long tamanho;

    consultas_totais = 0;
    maior_pos_consultado = -1;

    fflush (stdout);
    saved_stdout = dup (fileno (stdout));
    tmp = tmpfile ();
    dup2 (fileno (tmp), fileno (stdout));

    ultimo_retorno = calculaInterseccao (n);

    fflush (stdout);
    dup2 (saved_stdout, fileno (stdout));
    close (saved_stdout);

    fseek (tmp, 0, SEEK_END);
    tamanho = ftell (tmp);
    fclose (tmp);
    saida_vazia = (tamanho == 0);
}

/*============================================================================*/

static void caso (const char* nome, int n, int esperado)
{
    char motivo [512];

    total_testes++;
    chama (n);

    motivo [0] = 0;
    if (ultimo_retorno != esperado)
        { sprintf (motivo + strlen (motivo), "area esperada = %d, retornada = %d. ", esperado, ultimo_retorno); falhas_area++; }
    if (!saida_vazia)
        { sprintf (motivo + strlen (motivo), "a funcao imprimiu saida (printf e proibido pela especificacao). "); falhas_saida++; }

    if (motivo [0])
        reporta_falha (nome, motivo);
}

static void caso_otimizacao (const char* nome, int n, int esperado, int pos_maior_permitida)
{
    char motivo [512];

    total_testes++;
    chama (n);

    motivo [0] = 0;
    if (ultimo_retorno != esperado)
        { sprintf (motivo + strlen (motivo), "area esperada = %d, retornada = %d. ", esperado, ultimo_retorno); falhas_area++; }
    if (maior_pos_consultado > pos_maior_permitida)
        { sprintf (motivo + strlen (motivo), "otimizacao ausente: consultou ate o retangulo %d, mas poderia parar no %d (%ld consultas). ", maior_pos_consultado, pos_maior_permitida, consultas_totais); falhas_otimizacao++; }

    if (motivo [0])
        reporta_falha (nome, motivo);
}

static void monta (int n, const int* c)
{
    int i;
    for (i = 0; i < n; i++)
        adicionaRetangulo (i, c [4 * i], c [4 * i + 1], c [4 * i + 2], c [4 * i + 3]);
}

/*============================================================================*/

static void testes_do_pdf ()
{
    /* Exemplo oficial do enunciado: esperado 150. */
    const int pdf [] = { 5, 40, 25, 5,  10, 35, 40, 15,  15, 30, 30, 10 };
    monta (3, pdf);
    caso ("PDF: exemplo oficial das 3 regioes", 3, 150);

    /* Unico retangulo: a interseccao e ele mesmo. */
    const int um [] = { 5, 40, 25, 5 };
    monta (1, um);
    caso ("retangulo unico (area 20x35 = 700)", 1, 700);
}

static void testes_basicos ()
{
    /* Dois retangulos sobrepostos parcialmente: 5x5. */
    const int dois [] = { 0, 10, 10, 0,  5, 15, 20, 5 };
    monta (2, dois);
    caso ("2 retangulos sobrepostos (5x5)", 2, 25);

    /* Retangulos identicos. */
    const int iguais [] = { 0, 10, 10, 0,  0, 10, 10, 0 };
    monta (2, iguais);
    caso ("2 retangulos identicos", 2, 100);

    /* Um dentro do outro. */
    const int dentro [] = { 0, 20, 20, 0,  5, 15, 15, 5 };
    monta (2, dentro);
    caso ("2 retangulos (um dentro do outro)", 2, 100);

    /* Disjuntos na horizontal. */
    const int disj_h [] = { 0, 10, 5, 0,  20, 30, 15, 5 };
    monta (2, disj_h);
    caso ("2 retangulos disjuntos (horizontal)", 2, 0);

    /* Disjuntos na vertical. */
    const int disj_v [] = { 0, 10, 20, 0, 10,  0, 10, -20, -10 };
    monta (2, disj_v);
    caso ("2 retangulos disjuntos (vertical)", 2, 0);

    /* Encostando exatamente na borda (area zero). */
    const int borda [] = { 0, 10, 10, 0, 0,  10, 10, 20, 0, 10 };
    monta (2, borda);
    caso ("bordas se encostam (area zero)", 2, 0);

    /* Retangulo degenerado (largura zero). */
    const int degen [] = { 5, 10, 5, 0 };
    monta (1, degen);
    caso ("retangulo degenerado (largura 0)", 1, 0);

    /* Coordenadas negativas. */
    const int neg [] = { -10, -5, 5, -20,  -8, -7, 10, -18 };
    monta (2, neg);
    caso ("coordenadas negativas", 2, 143);

    /* Interseccao que nao contem a origem do sistema. */
    const int longe [] = { 100, 200, 150, 100,  120, 180, 140, 110,  110, 190, 160, 120 };
    monta (3, longe);
    caso ("3 retangulos", 3, 1200);
}

static void testes_otimizacao ()
{
    /* Retangulos 0 e 1 disjuntos: nao e preciso consultar o 2 ou o 3. */
    const int cedo [] = { 0, 10, 5, 0, 0,  20, 30, 15, 5, 10,  0, 10, 5, 0, 0,  0, 10, 5, 0, 0 };
    monta (5, cedo);
    caso_otimizacao ("otimizacao: par disjunto ja no inicio (5 retangulos)", 5, 0, 1);

    /* 0 e 1 se sobrepoe; a interseccao so fica vazia no retangulo 2.
       O retangulo 3 (valido) nao precisa ser consultado. */
    const int meio [] = {
        0, 40, 40, 0, 0,        /* 0: 40x40 */
        10, 30, 30, 10,         /* 1: dentro do 0 */
        50, 60, 60, 50,         /* 2: disjunto de (0 e 1) */
        15, 25, 25, 15          /* 3: nunca chega a ser consultado */
    };
    monta (4, meio);
    caso_otimizacao ("otimizacao: vazio so apos o 3o retangulo (4 retangulos)", 4, 0, 2);
}

static void testes_aleatorios ()
{
    int i;
    unsigned int seed = 2939800u;
    int falhas_aleatorios = 0;
    int n_casos = 500;

    for (i = 0; i < n_casos; i++)
    {
        int n, j, esperado, obtido;
        char motivo [256];

        n = 2 + (seed % 9);          /* de 2 a 10 retangulos */
        seed = seed * 1103515245 + 12345;

        for (j = 0; j < n; j++)
        {
            int a, b, c, d, t;
            seed = seed * 1103515245 + 12345; a = (int) ((seed >> 16) % 60u) - 30;
            seed = seed * 1103515245 + 12345; b = (int) ((seed >> 16) % 60u) - 30;
            seed = seed * 1103515245 + 12345; c = (int) ((seed >> 16) % 60u) - 30;
            seed = seed * 1103515245 + 12345; d = (int) ((seed >> 16) % 60u) - 30;
            if (a > c) { t = a; a = c; c = t; }   /* xSE < xID   */
            if (b < d) { t = b; b = d; d = t; }   /* ySE > yID   */
            if (a == c) c = a + 1 + (j % 5);
            if (b == d) d = b - 1 - (j % 5);
            adicionaRetangulo (j, a, b, c, d);
        }

        esperado = referencia (n);
        total_testes++;
        chama (n);
        obtido = ultimo_retorno;

        motivo [0] = 0;
        if (obtido != esperado)
            { sprintf (motivo + strlen (motivo), "area esperada = %d, retornada = %d. ", esperado, obtido); falhas_area++; }
        if (!saida_vazia)
            { sprintf (motivo + strlen (motivo), "saida impressa na stdout. "); falhas_saida++; }

        if (motivo [0])
        {
            falhas_aleatorios++;
            if (falhas_aleatorios <= 5)
                reporta_falha ("aleatorio", motivo);
        }
    }

    if (falhas_aleatorios > 5)
    {
        total_falhas += falhas_aleatorios - 5;
        printf ("  [FALHA] ... e mais %d casos aleatorios com erro (total: %d/%d).\n",
                falhas_aleatorios - 5, falhas_aleatorios, n_casos);
    }
    else if (falhas_aleatorios == 0)
        printf ("  [ OK ] 500 casos aleatorios\n");
}

/*============================================================================*/

int main ()
{
    printf ("Bateria de testes - calculaInterseccao (RA 2939800)\n\n");

    testes_do_pdf ();
    testes_basicos ();
    testes_otimizacao ();
    testes_aleatorios ();

    printf ("\nResumo: %d testes executados, %d falhas, %d aprovados.\n",
            total_testes, total_falhas, total_testes - total_falhas);

    printf ("Causas das falhas: area=%d, saida_proibida=%d, otimizacao=%d\n",
            falhas_area, falhas_saida, falhas_otimizacao);

    if (total_falhas == 0)
    {
        printf ("RESULTADO: APROVADO\n");
        return 0;
    }
    printf ("RESULTADO: REPROVADO\n");
    return 1;
}
