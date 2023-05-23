#ifndef ARQUIVO_CSV_H
#define ARQUIVO_CSV_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "estrutura.h"


void le_cabecalho_csv(FILE *arquivo);

int le_arquivo_csv(FILE *arquivo, Campos *dados);

void le_campos_variaveis_csv(FILE *arquivo, Campos *dados, int opcao);

void trata_dados_csv(Campos *dados);


#endif