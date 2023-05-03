#ifndef ARQUIVO_IND_H
#define ARQUIVO_IND_H

#include "estrutura.h"

void le_cab_indince(FILE *arq, IndexCabecalho *indexCabecalho);

FILE *abre_arquivo_ind(char *nomeArquivoIndex, IndexCabecalho *indexCabecalho, int tipo);

void fecha_arquivo_ind(FILE *arquivo, IndexCabecalho *indexCabecalho);

void cria_arq_index(char *nomeArquivoIndex, IndexCampos *indexCampos, IndexCabecalho *indexCabecalho, int tipoDado);

void busca_bin(IndexCampos *indexCampos, char *campoIndexado, IndexCabecalho *indexCabecalho, char *nomeArqIndex );

#endif