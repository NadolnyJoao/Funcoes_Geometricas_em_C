// Joao Vitor Nadolny Portes, 2939800
#include "trabalho1.h"
#include <float.h>
#include <math.h>
int calculaInterseccao(int n_retangulos) {
  int int_xse = pegaXSE(0);
  int int_xid = pegaXID(0);
  int int_yse = pegaYSE(0);
  int int_yid = pegaYID(0);

  for (int i = 1; i < n_retangulos; i++) {
    int new_xse = pegaXSE(i);
    int new_xid = pegaXID(i);
    int new_yse = pegaYSE(i);
    int new_yid = pegaYID(i);

    if (new_xse > int_xse)
      int_xse = new_xse;

    if (new_xid < int_xid)
      int_xid = new_xid;

    if (new_yse < int_yse)
      int_yse = new_yse;

    if (new_yid > int_yid)
      int_yid = new_yid;

    if ((int_xse >= int_xid) || (int_yse <= int_yid)) {
      return 0;
    }
  }
  int area = (int_xid - int_xse) * (int_yse - int_yid);
  return area;
}
unsigned int encontraParMaisProximo(int n_retangulos) {

  float dist_minima = FLT_MAX;
  int par1 = 0, par2 = 0;

  for (int i = 0; i < n_retangulos - 1; i++) {

    float centro_x1 = (pegaXSE(i) + pegaXID(i)) / 2.0;

    float centro_y1 = ((pegaYSE(i) + pegaYID(i)) / 2.0);
    for (int j = i + 1; j < n_retangulos; j++) {

      float centro_x2 = (pegaXSE(j) + pegaXID(j)) / 2.0;

      float centro_y2 = ((pegaYSE(j) + pegaYID(j)) / 2.0);

      float dist = sqrt((centro_x2 - centro_x1) * (centro_x2 - centro_x1) +
                        (centro_y2 - centro_y1) * (centro_y2 - centro_y1));

      if (dist < dist_minima) {
        dist_minima = dist;
        par1 = i;
        par2 = j;
      }
    }
  }
  int menor_id, maior_id;
  if (par1 < par2) {
    menor_id = par1;
    maior_id = par2;
  } else {
    menor_id = par2;
    maior_id = par1;
  }
  unsigned int resultado = (menor_id << 16) | maior_id;
  return resultado;
}
