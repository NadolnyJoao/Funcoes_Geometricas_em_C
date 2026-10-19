/* Testes proprios para as duas funcoes do Trabalho 1 (RA 2939800).
 * - casos de borda feitos a mao para calculaInterseccao e encontraParMaisProximo
 * - fuzz com dados aleatorios, conferido contra implementacoes de referencia
 * Compila-se junto com trabalho1.c (auxiliares) e t1-2939800.c. */
#include <stdio.h>
#include <math.h>
#include "trabalho1.h"

static int total = 0, falhas = 0;

static void check_int(const char *nome, int obtido, int esperado) {
    total++;
    if (obtido != esperado) {
        falhas++;
        printf("  FALHA [%s]: esperava %d, obteve %d\n", nome, esperado, obtido);
    }
}

static void check_par(const char *nome, unsigned int obtido, unsigned int esperado) {
    total++;
    if (obtido != esperado) {
        falhas++;
        printf("  FALHA [%s]: esperava 0x%05X, obteve 0x%05X\n", nome, esperado, obtido);
    }
}

/* ---- referencias independentes -------------------------------------------- */

static int ref_interseccao(int n) {
    int xse = pegaXSE(0), xid = pegaXID(0), yse = pegaYSE(0), yid = pegaYID(0);
    for (int i = 1; i < n; i++) {
        if (pegaXSE(i) > xse) xse = pegaXSE(i);
        if (pegaXID(i) < xid) xid = pegaXID(i);
        if (pegaYSE(i) < yse) yse = pegaYSE(i);
        if (pegaYID(i) > yid) yid = pegaYID(i);
    }
    if (xse >= xid || yse <= yid) return 0;
    return (xid - xse) * (yse - yid);
}

/* Usa double (o codigo do aluno usa float). Compara distancias ao quadrado,
 * que para coordenadas inteiras sao multiplos exatos de 0,25. */
static unsigned int ref_par(int n) {
    double melhor = -1.0;
    int mi = 0, mj = 0;
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            double dx = ((double)(pegaXSE(i) + pegaXID(i)) - (pegaXSE(j) + pegaXID(j))) / 2.0;
            double dy = ((double)(pegaYSE(i) + pegaYID(i)) - (pegaYSE(j) + pegaYID(j))) / 2.0;
            double d2 = dx * dx + dy * dy;
            if (melhor < 0.0 || d2 < melhor) { melhor = d2; mi = i; mj = j; }
        }
    }
    return ((unsigned int)mi << 16) | (unsigned int)mj;
}

/* ---- gerador pseudo-aleatorio (LCG) --------------------------------------- */
static unsigned long long estado = 88172645463325252ULL;
static int rnd(int lo, int hi) {
    estado = estado * 6364136223846793005ULL + 1442695040888963407ULL;
    return lo + (int)((estado >> 33) % (unsigned)(hi - lo + 1));
}

/* Cria n retangulos aleatorios com xSE<xID e ySE>yID no intervalo [lo,hi]. */
static void gera(int n, int lo, int hi) {
    criaVetorRetangulos(n);
    for (int i = 0; i < n; i++) {
        int x1 = rnd(lo, hi), x2 = rnd(lo, hi);
        int y1 = rnd(lo, hi), y2 = rnd(lo, hi);
        int xse = x1 < x2 ? x1 : x2, xid = x1 < x2 ? x2 : x1;
        int yse = y1 > y2 ? y1 : y2, yid = y1 > y2 ? y2 : y1;
        adicionaRetangulo(i, xse, yse, xid, yid);
    }
}

/* ---- casos de borda: calculaInterseccao ----------------------------------- */
static void bordas_interseccao(void) {
    puts(" Casos de borda - calculaInterseccao:");

    /* Exemplo do enunciado: SE=(5,40) ID=(25,5); (10,35)(40,15); (15,30)(30,10) -> 150 */
    criaVetorRetangulos(3);
    adicionaRetangulo(0, 5, 40, 25, 5);
    adicionaRetangulo(1, 10, 35, 40, 15);
    adicionaRetangulo(2, 15, 30, 30, 10);
    check_int("exemplo do enunciado = 150", calculaInterseccao(3), 150);
    destroiVetorRetangulos();

    /* Um unico retangulo: area do proprio */
    criaVetorRetangulos(1);
    adicionaRetangulo(0, 0, 10, 4, 0);
    check_int("retangulo unico = 40", calculaInterseccao(1), 40);
    destroiVetorRetangulos();

    /* Dois identicos */
    criaVetorRetangulos(2);
    adicionaRetangulo(0, 2, 8, 7, 3);
    adicionaRetangulo(1, 2, 8, 7, 3);
    check_int("dois identicos = 25", calculaInterseccao(2), 25);
    destroiVetorRetangulos();

    /* Disjuntos em x */
    criaVetorRetangulos(2);
    adicionaRetangulo(0, 0, 10, 3, 0);
    adicionaRetangulo(1, 5, 10, 8, 0);
    check_int("disjuntos em x = 0", calculaInterseccao(2), 0);
    destroiVetorRetangulos();

    /* Disjuntos em y */
    criaVetorRetangulos(2);
    adicionaRetangulo(0, 0, 10, 5, 6);
    adicionaRetangulo(1, 0, 4, 5, 0);
    check_int("disjuntos em y = 0", calculaInterseccao(2), 0);
    destroiVetorRetangulos();

    /* Tocando-se apenas pela aresta (intersecao de area nula) */
    criaVetorRetangulos(2);
    adicionaRetangulo(0, 0, 10, 5, 0);
    adicionaRetangulo(1, 5, 10, 9, 0);
    check_int("tocando na aresta vertical = 0", calculaInterseccao(2), 0);
    destroiVetorRetangulos();

    criaVetorRetangulos(2);
    adicionaRetangulo(0, 0, 10, 5, 4);
    adicionaRetangulo(1, 0, 4, 5, 0);
    check_int("tocando na aresta horizontal = 0", calculaInterseccao(2), 0);
    destroiVetorRetangulos();

    /* Contido no outro: a intersecao e o menor */
    criaVetorRetangulos(2);
    adicionaRetangulo(0, 0, 20, 20, 0);
    adicionaRetangulo(1, 5, 15, 10, 5);
    check_int("contido = 50", calculaInterseccao(2), 50);
    destroiVetorRetangulos();

    /* Primeiro par sem intersecao, mas um par posterior com intersecao:
       a otimizacao deve encerrar retornando 0. */
    criaVetorRetangulos(3);
    adicionaRetangulo(0, 0, 10, 2, 0);      /* nao cruza com os seguintes em x */
    adicionaRetangulo(1, 5, 10, 9, 0);
    adicionaRetangulo(2, 6, 10, 8, 0);
    check_int("otimizacao: para cedo em 0", calculaInterseccao(3), 0);
    destroiVetorRetangulos();

    /* Coordenadas negativas */
    criaVetorRetangulos(2);
    adicionaRetangulo(0, -10, -2, -4, -8);
    adicionaRetangulo(1, -8, -3, -2, -12);
    check_int("coordenadas negativas = 20", calculaInterseccao(2), 20);
    destroiVetorRetangulos();
}

/* ---- casos de borda: encontraParMaisProximo ------------------------------- */
static void bordas_par(void) {
    puts(" Casos de borda - encontraParMaisProximo:");

    /* Dois retangulos: unico par possivel (0,1) */
    criaVetorRetangulos(2);
    adicionaRetangulo(0, 0, 10, 2, 0);
    adicionaRetangulo(1, 100, 10, 102, 0);
    check_par("dois retangulos => 0x00000001", encontraParMaisProximo(2), 0x00000001u);
    destroiVetorRetangulos();

    /* Empacotamento do enunciado: par (5,11) => 0x0005000B.
       Constroi 12 fornecedores com 5 e 11 bem proximos e os outros afastados. */
    criaVetorRetangulos(12);
    for (int i = 0; i < 12; i++)
        adicionaRetangulo(i, 1000 * i, 1000, 1000 * i + 10, 0);
    adicionaRetangulo(5, 5000, 300, 5010, 290);   /* centro (5005, 295) */
    adicionaRetangulo(11, 5003, 300, 5013, 290);  /* centro (5008, 295) */
    check_par("par (5,11) => 0x0005000B", encontraParMaisProximo(12), 0x0005000Bu);
    destroiVetorRetangulos();

    /* Tres centros: (2,2), (12,12) e (6,6). O par mais proximo e (0,2). */
    criaVetorRetangulos(3);
    adicionaRetangulo(0, 0, 4, 4, 0);       /* centro (2,2) */
    adicionaRetangulo(1, 10, 14, 14, 10);   /* centro (12,12) */
    adicionaRetangulo(2, 4, 8, 8, 4);       /* centro (6,6) */
    check_par("pars por ordem = 0x00000002", encontraParMaisProximo(3), 0x00000002u);
    destroiVetorRetangulos();
}

/* ---- fuzz ---------------------------------------------------------------- */
static void fuzz(void) {
    puts(" Fuzz:");

    int casos = 0;
    for (int t = 0; t < 600; t++) {
        int n = rnd(2, 14);

        gera(n, -40, 40);
        int a = calculaInterseccao(n), ra = ref_interseccao(n);
        unsigned int p = encontraParMaisProximo(n), rp = ref_par(n);
        if (a != ra) { falhas++; printf("  FALHA fuzz interseccao (n=%d): %d != %d\n", n, a, ra); }
        if (p != rp) { falhas++; printf("  FALHA fuzz par (n=%d): 0x%05X != 0x%05X\n", n, p, rp); }
        total += 2;
        destroiVetorRetangulos();
        casos++;
    }
    printf("  %d casos aleatorios de -40 a 40 (2 a 14 retangulos): OK\n", casos);

    /* Coordenadas grandes: base para a precisao do calculo de distancias. */
    int casos2 = 0;
    for (int t = 0; t < 200; t++) {
        int n = rnd(2, 10);
        gera(n, -1000000, 1000000);
        int a = calculaInterseccao(n), ra = ref_interseccao(n);
        unsigned int p = encontraParMaisProximo(n), rp = ref_par(n);
        if (a != ra) { falhas++; printf("  FALHA fuzz-grande interseccao (n=%d): %d != %d\n", n, a, ra); }
        if (p != rp) { falhas++; printf("  FALHA fuzz-grande par (n=%d): 0x%05X != 0x%05X\n", n, p, rp); }
        total += 2;
        destroiVetorRetangulos();
        casos2++;
    }
    printf("  %d casos aleatorios de -1e6 a 1e6 (2 a 10 retangulos): OK\n", casos2);
}

int main(void) {
    puts("=== Testes proprios - Trabalho 1 (RA 2939800) ===");
    bordas_interseccao();
    bordas_par();
    fuzz();
    printf("\n%d verificacoes, %d falhas.\n", total, falhas);
    puts(falhas == 0 ? "RESULTADO: OK" : "RESULTADO: COM FALHAS");
    return falhas == 0 ? 0 : 1;
}
