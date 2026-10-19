#!/usr/bin/env bash
#==============================================================================
# Roda a bateria de testes das duas funcoes do Trabalho 1 (RA 2939800)
#   1) compilacao do arquivo do aluno com avisos estritos
#   2) testes unitarios proprios (testes/teste_calculaInterseccao.c)
#   3) testes extras de borda + fuzz (testes/testes_extra.c)
#   4) harness oficial dos professores (main.c + trabalho1.c) com os 3 arquivos
#      de dados .txt do pacote, com as duas funcoes ligadas
#   5) verificacao de simbolos proibidos (printf, scanf, etc.)
# Uso: bash testes/run_testes.sh
#==============================================================================

set -u
cd "$(dirname "$0")/.."
BASE="$(pwd)"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

FALHAS=0

echo "=============================================================="
echo " Bateria de testes - Trabalho 1 (RA 2939800)"
echo "=============================================================="

#------------------------------------------------------------------------------
echo
echo "[1/5] Compilacao do arquivo do aluno (t1-2939800.c)"
if gcc -std=c17 -Wall -Wextra -c "t1-2939800.c" -o "$TMP/aluno.o" 2> "$TMP/comp.log"; then
    if [ -s "$TMP/comp.log" ]; then
        echo "  Compilou, mas com avisos:"
        sed 's/^/    /' "$TMP/comp.log"
    else
        echo "  OK - compilou sem nenhum aviso"
    fi
else
    echo "  FALHA - nao compilou:"
    sed 's/^/    /' "$TMP/comp.log"
    FALHAS=$((FALHAS+1))
fi

#------------------------------------------------------------------------------
echo
echo "[2/5] Testes unitarios proprios (calculaInterseccao)"
if gcc -std=c17 -Wall -I "$BASE" \
        "$BASE/testes/teste_calculaInterseccao.c" \
        "$BASE/t1-2939800.c" -lm -o "$TMP/unit" 2> "$TMP/unit_comp.log"; then
    [ -s "$TMP/unit_comp.log" ] && { echo "  avisos do compilador:"; sed 's/^/    /' "$TMP/unit_comp.log"; }
    "$TMP/unit"
    [ $? -ne 0 ] && FALHAS=$((FALHAS+1))
else
    echo "  FALHA - os testes nao compilaram:"
    sed 's/^/    /' "$TMP/unit_comp.log"
    FALHAS=$((FALHAS+1))
fi

#------------------------------------------------------------------------------
echo
echo "[3/5] Testes extras de borda + fuzz (testes/testes_extra.c)"
if gcc -std=c17 -Wall -I "$BASE" \
        "$BASE/testes/testes_extra.c" \
        "$BASE/trabalho1.c" "$BASE/t1-2939800.c" -lm -o "$TMP/extra" 2> "$TMP/extra_comp.log"; then
    [ -s "$TMP/extra_comp.log" ] && { echo "  avisos do compilador:"; sed 's/^/    /' "$TMP/extra_comp.log"; }
    "$TMP/extra"
    [ $? -ne 0 ] && FALHAS=$((FALHAS+1))
else
    echo "  FALHA - os testes nao compilaram:"
    sed 's/^/    /' "$TMP/extra_comp.log"
    FALHAS=$((FALHAS+1))
fi

#------------------------------------------------------------------------------
echo
echo "[4/5] Harness oficial dos professores (main.c) - as duas funcoes"
for arq in teste_retangulos1.txt teste_retangulos2.txt teste_retangulos3.txt; do
    sed -e "s|#define ARQUIVO_RETANGULOS .*|#define ARQUIVO_RETANGULOS \"$arq\"|" \
        "$BASE/main.c" > "$TMP/main_oficial.c"

    if ! gcc -std=c17 -I "$BASE" "$TMP/main_oficial.c" "$BASE/trabalho1.c" \
             "$BASE/t1-2939800.c" -lm -o "$TMP/oficial" 2> "$TMP/of_comp.log"; then
        echo "  $arq: FALHA DE COMPILACAO"
        sed 's/^/    /' "$TMP/of_comp.log"
        FALHAS=$((FALHAS+1))
        continue
    fi

    "$TMP/oficial" > "$TMP/saida.txt" 2>&1
    erros=$(grep -c "Erro no teste" "$TMP/saida.txt")
    total=$(grep -o "[0-9]* testes executados" "$TMP/saida.txt" | grep -o "[0-9]*")
    if [ "$erros" -eq 0 ]; then
        echo "  $arq: OK - ${total} testes, 0 erros"
    else
        echo "  $arq: FALHA - ${erros} de ${total} testes com erro. Primeiros erros:"
        grep -A1 "Erro no teste" "$TMP/saida.txt" | head -12 | sed 's/^/    /'
        echo "    ..."
        FALHAS=$((FALHAS+1))
    fi
done

#------------------------------------------------------------------------------
echo
echo "[5/5] Simbolos usados pelo objeto do aluno (devem ser apenas as pegas* e sqrt)"
simbolos=$(nm -u "$TMP/aluno.o" 2>/dev/null | awk '{print $2}' | sort | tr '\n' ' ')
echo "  simbolos indefinidos: ${simbolos:-nenhum}"
proibidos=$(nm -u "$TMP/aluno.o" 2>/dev/null | awk '{print $2}' \
            | grep -E '^(printf|puts|putchar|scanf|fscanf|fgets|malloc|free|exit|abort|rand|srand)$' | tr '\n' ' ')
if [ -n "$proibidos" ]; then
    echo "  FALHA - biblioteca-padrao nao permitida: $proibidos"
    FALHAS=$((FALHAS+1))
else
    echo "  OK - nenhuma funcao proibida"
fi

echo
echo "=============================================================="
if [ "$FALHAS" -eq 0 ]; then
    echo " RESULTADO FINAL: BATERIA APROVADA"
else
    echo " RESULTADO FINAL: BATERIA REPROVADA ($FALHAS bloco(s) com falha)"
fi
echo "=============================================================="
exit $FALHAS
