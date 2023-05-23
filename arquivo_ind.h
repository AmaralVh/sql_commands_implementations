#ifndef ARQUIVO_IND_H
#define ARQUIVO_IND_H

#include "estrutura_index.h"

FILE *abre_arquivo_ind(char *nomeArquivoIndex, IndexCabecalho *indexCabecalho, int tipo);

void escreve_arq_ind(FILE *arquivo, IndexCabecalho *indexCabecalho, IndexCampos *indexCampos);

void fecha_arquivo_ind(FILE *arquivo);

void cria_arq_index(char *nomeArquivoIndex, IndexCampos *indexCampos, IndexCabecalho *indexCabecalho);

void le_cab_indice(FILE *arquivo, IndexCabecalho *indexCabecalho);

IndexCampos *le_arq_indice(FILE *arquivo, IndexCampos *indexCampos, IndexCabecalho *indexCab);

#endif