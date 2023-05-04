#ifndef ESTRUTURA_H
#define ESTRUTURA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// estrutura de cabecalho do arquivo de dados
struct cabecalho {
    char status;
    long long int proxByteOffset;
    int nroRegArq;
    int nroRegRem;
};

// estrutura de campos do arquivo de dados
struct campos {
    char removido;
    int idCrime;
    int numeroArtigo;
    char dataCrime[11];
    char marcaCelular[13];
    char *lugarCrime;
    char *descricaoCrime;
};

// estrutura de cabecalho para o arquivo de indices
struct indexCabecalho {
    char status;
    int quant;
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

// estrutura para guardar os campos usados para busca
struct chavesBusca {
    int tipo;
    char campoBusca[40];
    union {
        int chaveInt;
        char chaveStr[100];
    };
};


typedef struct campos Campos;
typedef struct cabecalho Cabecalho;
typedef struct indexCabecalho IndexCabecalho;
typedef struct indexCampos IndexCampos;
typedef struct chavesBusca ChavesBusca;


void erro_processamento();

Campos *aloca_campos();

ChavesBusca *aloca_chaves_busca(int numPares);

IndexCampos *aloca_indice();

void realoca_indice(IndexCampos *indexCampos, int n);

Cabecalho *aloca_cabecalho();

IndexCabecalho *aloca_cabecalho_index();

void cria_cabecalho_ind(IndexCabecalho *indexCabecalho);

void aloca_campos_variaveis(Campos *dados, int i);

void desaloca_campos_variaveis(Campos *dados);

char *copia_campo_variavel(char *origem);

FILE *abre_arquivo(char *nome_arquivo, int tipo, Cabecalho *cabecalho);

void escreve_cabecalho(FILE *arquivo, Cabecalho *cabecalho);

void cria_cabecalho(Cabecalho *cabecalho);

void atualiza_cabecalho_escrita(Cabecalho *cabecalho, int deslocamentoOffset);

void atualiza_cabecalho_fechamento(FILE *arquivo, Cabecalho *cabecalho);

void atualiza_cab_ind_fechamento(FILE *arquivo, IndexCabecalho *indexCabecalho);

FILE *abre_arquivo(char *nome_arquivo, int tipo, Cabecalho *cabecalho);

void fecha_arquivo(FILE *arquivo, int tipo, Cabecalho *cabecalho);

void preenche_cifrao_ind(IndexCampos *indexCampos, IndexCabecalho *indCab);

void preenche_cifrao_chav_str(ChavesBusca *chavesBusca, int index);

int seleciona_tipo(char *tipoDado);

void seleciona_index(IndexCampos *indexCampos, Campos *campos, char *campoIndexado, int tipo, IndexCabecalho *indCab, long long int byteOffset);


#endif