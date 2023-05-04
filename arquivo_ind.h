#ifndef ARQUIVO_IND_H
#define ARQUIVO_IND_H

#include "estrutura.h"

FILE *abre_arquivo_ind(char *nomeArquivoIndex, IndexCabecalho *indexCabecalho, int tipo);

void fecha_arquivo_ind(FILE *arquivo, IndexCabecalho *indexCabecalho);

void cria_arq_index(char *nomeArquivoIndex, IndexCampos *indexCampos, IndexCabecalho *indexCabecalho, int tipoDado);

void le_cab_indince(FILE *arquivo, IndexCabecalho *indexCabecalho);

IndexCampos *le_arq_indice(FILE *arquivo, IndexCampos *indexCampos, IndexCabecalho *indexCab, int tipo);


#endif