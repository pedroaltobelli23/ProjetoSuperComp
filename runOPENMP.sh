#!/bin/bash

# Certifique-se de que o seu executável está no mesmo diretório ou ajuste o caminho conforme necessário
EXECUTAVEL="../../findclickOPENMP"

# Compila em openMP
g++ -g -Wall -fopenmp -o findclickOPENMP findclickOPENMP.cpp

# Navegue até o diretório contendo os grafos
cd ClusterScripts/grafos

# Loop por todos os arquivos que correspondem ao padrão grafoX.txt
for arquivo in grafo*.txt; do
    # Verifique se o arquivo existe antes de executar o programa
    if [ -e "$arquivo" ]; then
        saida_arquivo="../outputs/OPENMP/OPENMP${arquivo/grafo/out}"
        echo "Processando $arquivo... Saída em $saida_arquivo"
        $EXECUTAVEL "$arquivo" > "$saida_arquivo" 2>&1
    else
        echo "Arquivo $arquivo não encontrado."
    fi
done