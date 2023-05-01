#include "arquivo_ind.h"


FILE *abre_arquivo_ind(char *nomeArquivoIndex, IndexCabecalho *indexCabecalho) {
    FILE *arquivo;

    arquivo = fopen(nomeArquivoIndex, "wb");
    cria_cabecalho_ind(indexCabecalho);

    return arquivo;
}


void fecha_arquivo_ind(FILE *arquivo, IndexCabecalho *indexCabecalho) {
    atualiza_cab_ind_fechamento(arquivo, indexCabecalho);

    fclose(arquivo);
}


void cria_arq_index(char *nomeArquivoIndex, IndexCampos *indexCampos, IndexCabecalho *indexCabecalho, int tipoDado, int quant) {
    FILE *arquivo = abre_arquivo_ind(nomeArquivoIndex, indexCabecalho);

    // Escreve cabecalho do arquivo de indice:
    fwrite(&indexCabecalho->status, sizeof(char), 1, arquivo);

    if(tipoDado == 0) {
        for(int i = 0; i < quant; i++) {
            fwrite(&indexCampos[i].chaveInt, sizeof(int), 1, arquivo);
            fwrite(&indexCampos[i].byteOffset, sizeof(long long int), 1, arquivo);
        }
    } else if(tipoDado == 1) {
        for(int i = 0; i < quant; i++) {
            fwrite(indexCampos[i].chaveStr, sizeof(char), 12, arquivo);
            fwrite(&indexCampos[i].byteOffset, sizeof(long long int), 1, arquivo);
        }
    } else {
        erro_processamento();
    }

    fecha_arquivo_ind(arquivo, indexCabecalho);
}