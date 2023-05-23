#ifndef ESTRUTURA_INDEX_H
#define ESTRUTURA_INDEX_H

#include "estrutura.h"

// estrutura de cabecalho para o arquivo de indices
struct indexCabecalho {
    char status;
    int quant;
    int tipo;
};

// estrutura de campos para o arquivo de indices
struct indexCampos {
    // union para guardar a chave de busca do arquivo de indices
    // (inteiro ou string de tamanho 12 bytes):
    union {
        int chaveInt;
        char chaveStr[13];
    };
    long long int byteOffset;
};


typedef struct indexCabecalho IndexCabecalho;
typedef struct indexCampos IndexCampos;


IndexCampos *aloca_indice();

void realoca_indice(IndexCampos *indexCampos, int n);

IndexCabecalho *aloca_cabecalho_index();

void cria_cabecalho_ind(IndexCabecalho *indexCabecalho);

void atualiza_cab_ind_fechamento(FILE *arquivo, IndexCabecalho *indexCabecalho);

void preenche_cifrao_ind(IndexCampos *indexCampos, IndexCabecalho *indCab);

void shift_index(IndexCampos *indexCampos, int i, IndexCabecalho *indCab, int direcao);

void seleciona_index(IndexCampos *indexCampos, Campos *campos, char *campoIndexado, IndexCabecalho *indCab, long long int byteOffset);

void insere_index(long long int byteOffset);

void apaga_index(IndexCabecalho *indCab, long long int byteOffset, IndexCampos *indCampos);

void atualiza_ind(IndexCampos *indCampos, IndexCabecalho *indCab, long long int bOffset, ChavesBusca *atualiz, int atuIndexada, long long int novoBOffset);

void atualiza_ind_bOffset(IndexCampos *indCampos, IndexCabecalho *indCab, long long int bOffset, Campos *campos, char *campoIndexado, long long int novoBOffset);

#endif