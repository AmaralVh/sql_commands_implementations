#include "arquivo_ind.h"
#include <stdbool.h>


// Abre o arquivo de indices:
FILE *abre_arquivo_ind(char *nomeArquivoIndex, IndexCabecalho *indexCabecalho, int tipo) {
    FILE *arquivo;

    if(tipo == 1) {
        // Abertura para escrita:
        arquivo = fopen(nomeArquivoIndex, "wb");
         // Criacao do cabecalho do arq. de indices:
        cria_cabecalho_ind(indexCabecalho);
    } else if(tipo == 2) { // Abertura para leitura:
        arquivo = fopen(nomeArquivoIndex, "rb");
    }

    return arquivo;
}


// Fecha arquivo de indices:
void fecha_arquivo_ind(FILE *arquivo, IndexCabecalho *indexCabecalho) {
    atualiza_cab_ind_fechamento(arquivo, indexCabecalho);

    fclose(arquivo);
}


// Cria arquivo de index:
void cria_arq_index(char *nomeArquivoIndex, IndexCampos *indexCampos, IndexCabecalho *indexCabecalho, int tipoDado) {
    FILE *arquivo = abre_arquivo_ind(nomeArquivoIndex, indexCabecalho, 1);

    // Escreve cabecalho do arquivo de indice:
    fwrite(&indexCabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&indexCabecalho->quant, sizeof(int), 1, arquivo);

    // Se tipo de dado = inteiro:
    if(tipoDado == 0) {
        for(int i = 0; i < indexCabecalho->quant; i++) { // Escrita:
            fwrite(&indexCampos[i].chaveInt, sizeof(int), 1, arquivo);
            fwrite(&indexCampos[i].byteOffset, sizeof(long long int), 1, arquivo);
        }
    } else if(tipoDado == 1) { // Se tipo de dado = string:
        for(int i = 0; i < indexCabecalho->quant; i++) { // Escrita:
            fwrite(indexCampos[i].chaveStr, sizeof(char), 12, arquivo);
            fwrite(&indexCampos[i].byteOffset, sizeof(long long int), 1, arquivo);
        }
    } else {
        erro_processamento();
    }

    fecha_arquivo_ind(arquivo, indexCabecalho);
}


void le_cab_indince(FILE *arquivo, IndexCabecalho *indexCabecalho) {
    fread(&indexCabecalho->status, sizeof(char), 1, arquivo);
    fread(&indexCabecalho->quant, sizeof(int), 1, arquivo);
}


IndexCampos *le_arq_indice(FILE *arquivo, IndexCampos *indexCampos, IndexCabecalho *indexCab, int tipo) {
    indexCampos = (IndexCampos *) realloc(indexCampos, (indexCab->quant) * sizeof(IndexCampos));

    for(int i = 0; i < indexCab->quant; i++) {
        if(tipo == 0) {
            fread(&indexCampos[i].chaveInt, sizeof(int), 1, arquivo);
            fread(&indexCampos[i].byteOffset, sizeof(long long int), 1, arquivo);
        } else if(tipo == 1) {
            fread(indexCampos[i].chaveStr, sizeof(char), 12, arquivo);
            fread(&indexCampos[i].byteOffset, sizeof(long long int), 1, arquivo);
        }
    }

    return indexCampos;
}