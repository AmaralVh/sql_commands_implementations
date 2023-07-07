#ifndef ARQUIVO_BIN_H
#define ARQUIVO_BIN_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "estrutura.h"
#include "arquivo_ind.h"

int le_cabecalho_bin(FILE *arquivo, Cabecalho *cabecalho);

void escreve_cabecalho(FILE *arquivo, Cabecalho *cabecalho);

void atualiza_cabecalho_escrita(Cabecalho *cabecalho, int deslocamentoOffset);

void atualiza_cabecalho_fechamento(FILE *arquivo, Cabecalho *cabecalho);

FILE *abre_arquivo(char *nome_arquivo, int tipo, Cabecalho *cabecalho);

void fecha_arquivo(FILE *arquivo, int tipo, Cabecalho *cabecalho);

void erro_reg_inexistente();

int registro_inexistente(FILE *arquivo, Cabecalho *cabecalho);

int le_arquivo_bin(FILE *arquivo, Campos *dados);

int le_campos_variaveis_bin(FILE *arquivo, Campos *dados, int opcao, int *tamRegBytes);

void escreve_arquivo_bin(FILE *arquivo, Campos *dados, Cabecalho *cabecalho);

void print_de_arquivo_bin(Campos *dados);

int num_registros_bin(Cabecalho *cabecalho);

void binarioNaTela(char *nomeArquivoBinario);

void marca_removido(FILE *arqBin, Campos *campos, long long int byteOffset);

long long int **busca_sequencial(FILE *arqBin, Campos *campos, Cabecalho *cab, ChavesBusca *chavesBusca, int numPares, int func, long long int **byteOffsets, int busca);

long long int **percorre_ind_iguais(IndexCampos *indCampos, ChavesBusca *chavBusca, int meio, int index, FILE *arqBin, Campos *campos, int numPares, int func, long long int **byteOffsets, int busca);

int busca_binaria(IndexCampos *indexCampos, int quantInd, ChavesBusca *chavesBusca, int index, Campos *campos, int func);

void insere_arq_dados(FILE *arqBin, Campos *insercao, Cabecalho *cab);

void completa_reg_cifrao(int tamRegAtual, int tamRegNovo, FILE *arqBin);

#endif