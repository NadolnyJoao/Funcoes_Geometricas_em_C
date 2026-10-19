// Joao Vitor Nadolny Portes, 2939800
#include "trabalho1.h"
#include <float.h>
#include <math.h>
/*Essa primeira função vai pegar primeiramente o retangulo de indice 0, e ai
 entrar em um for para ficar comparando os demais retangulos ao Retangulo
 formado pelas interseccoes*/
int calculaInterseccao(int n_retangulos) {
  int int_xse = pegaXSE(0);
  int int_xid = pegaXID(0);
  int int_yse = pegaYSE(0);
  int int_yid = pegaYID(0);
  /*Esse é o for que vai ficar pegando um novo retangulo e comparando os lados
   * dele as coordenadas do retangulo que eu já tenho*/
  for (int i = 1; i < n_retangulos; i++) {
    int new_xse = pegaXSE(i);
    int new_xid = pegaXID(i);
    int new_yse = pegaYSE(i);
    int new_yid = pegaYID(i);
    // cada if atualiza um limite da interseção: para xSE e yID fica o maior
    // valor, e para xID e ySE fica o menor, encolhendo o retângulo a cada
    // comparação
    if (new_xse > int_xse)
      int_xse = new_xse;

    if (new_xid < int_xid)
      int_xid = new_xid;

    if (new_yse < int_yse)
      int_yse = new_yse;

    if (new_yid > int_yid)
      int_yid = new_yid;
    // Condição de parada (a otimização pedida no enunciado): quando os limites
    // se cruzam, a interseção esvaziou. Como ela só encolhe, nunca voltaria a
    // crescer, então já podemos retornar 0 sem analisar os retângulos restantes
    if ((int_xse >= int_xid) || (int_yse <= int_yid)) {
      return 0;
    }
  } // Este calculo vai fazer o papel de uma simples equação de área de
    // retangulo: Base x Altura, usando as informações que foram dadas no for
    // acima;
  int area = (int_xid - int_xse) * (int_yse - int_yid);
  return area;
} /*Aqui a coisa é um pouco mais complexa: o objetivo é achar o par de centros
   mais próximos. Com loops aninhados, cada retângulo i é comparado com todos os
   que vêm depois dele (j > i), evitando repetir comparações. No fim, fica
   guardado o par de menor distância entre todos os testados*/
unsigned int encontraParMaisProximo(int n_retangulos) {
  // Três variáveis: dist_minima guarda a menor distância encontrada durante
  // toda a busca, e par1/par2 só são usadas no final. O FLT_MAX é o maior float
  // possível, então serve de valor inicial para que qualquer distância real já
  // seja menor que ele na primeira comparação, não importa o quão longe estejam
  // os primeiros retângulos testados.
  float dist_minima = FLT_MAX;
  int par1 = 0, par2 = 0;

  // Esse for vai fazer o papel de Pegar o primeiro retângulo, e usar ele como o
  // ponto fixo de comparação;
  for (int i = 0; i < n_retangulos - 1; i++) {
    // Aqui são declarados os dois primeiros centros, o ponto central y e ponto
    // central x, usando um calculo de média comum;
    float centro_x1 = (pegaXSE(i) + pegaXID(i)) / 2.0;

    float centro_y1 = ((pegaYSE(i) + pegaYID(i)) / 2.0);
    // Este for vai pegar como base o retangulo fornecido pela estrutura acima,
    // e comparar a todos os outros retângulos, desconsiderando as comparações
    // que já foram feitas, por isso eles tem origens e paradas diferentes.
    for (int j = i + 1; j < n_retangulos; j++) {

      // Declaracao de mais dois pontos centrais, que vao ser usados para
      // encontrar a distancia entre eles abaixo

      float centro_x2 = (pegaXSE(j) + pegaXID(j)) / 2.0;

      float centro_y2 = ((pegaYSE(j) + pegaYID(j)) / 2.0);
      // Distância euclidiana entre os dois centros: a menor distância em linha
      // reta entre dois pontos no plano, "dist = √(Δx² + Δy²)"
      float dist = sqrt((centro_x2 - centro_x1) * (centro_x2 - centro_x1) +
                        (centro_y2 - centro_y1) * (centro_y2 - centro_y1));
      // Um comparador para declarar qual a menor distancia encontrada até
      // então, e declarar quais são os retangulos que formam essa distancia
      if (dist < dist_minima) {
        dist_minima = dist;
        par1 = i;
        par2 = j;
      }
    }
  } // Esse if vai declarar qual o menor e maior par e atribuir aos indices,
    // para dessa forma, poder manipular a memoria deles propriamente
  int menor_id, maior_id;
  if (par1 < par2) {
    menor_id = par1;
    maior_id = par2;
  } else {
    menor_id = par2;
    maior_id = par1;
  } // Usamos unsigned int porque a variável não tem sinal (não usamos
    // negativos) e porque isso garante que o deslocamento de bits seja bem
    // definido. O menor índice vai para os 16 bits da esquerda e, com um OR, é
    // "colado" ao maior índice, que fica nos 16 bits da direita.
  unsigned int resultado = (menor_id << 16) | maior_id;
  // Retorna dois valores em uma só variável;
  return resultado;
}
