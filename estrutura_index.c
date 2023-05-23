#include <stdio.h>
#include "estrutura_index.h"


// Alocacao do indice:
IndexCampos *aloca_indice() {
    IndexCampos *indexCampos = (IndexCampos *) calloc(1, sizeof(IndexCampos));

    if (indexCampos == NULL) {
        erro_processamento();
    }

    return indexCampos;
}


// Realocacao de indice (uma posicao a mais):
void realoca_indice(IndexCampos *indexCampos, int n) {
    indexCampos = (IndexCampos *) realloc(indexCampos, (n+1) * sizeof(IndexCampos));
}


// Aloca cabecalho do arq. de indice:
IndexCabecalho *aloca_cabecalho_index() {
    IndexCabecalho *indexCabecalho = (IndexCabecalho *) calloc(1, sizeof(IndexCabecalho));
    if(indexCabecalho == NULL) {
        erro_processamento();
    }
    return indexCabecalho;
}


// Cria o cabecalho do arq. de indice:
void cria_cabecalho_ind(IndexCabecalho *indexCabecalho) {
    indexCabecalho->status = '0'; // Inicia como inconsistente (arq. aberto p/ escrita).
}


// Atualiza o cabecalho dos indices no fechamento do arquivo:
void atualiza_cab_ind_fechamento(FILE *arquivo, IndexCabecalho *indexCabecalho) {
    indexCabecalho->status = '1'; // Marca como consistente.

    // Escreve cabecalho atualizado no inicio do arquivo:
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&indexCabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&indexCabecalho->quant, sizeof(int), 1, arquivo);
}


// Preenche a string do indice com cifrao ate completar 12 bytes:
void preenche_cifrao_ind(IndexCampos *indexCampos, IndexCabecalho *indCab) {   
    // Para cada indice:
    for(int i = 0; i < indCab->quant; i++) {
        int tamanhoString = strlen(indexCampos[i].chaveStr);
        
        // Se tam < 12, preenchemos com '$':
        if(tamanhoString < 12) {
            int j;

            for(j = tamanhoString; j < 12; j++) {
                indexCampos[i].chaveStr[j] = '$';
            }
            indexCampos->chaveStr[j] = '\0';
        }
    }
}


// Funcao para dar shift em uma posicao dos indices a partir de 'i':
void shift_index(IndexCampos *indexCampos, int i, IndexCabecalho *indCab, int direcao) {
    // Para indices inteiros:

    if(direcao == 0){               // shift pra baixo
        if(indCab->tipo == 0) {             // para inteiros
            for(int k = indCab->quant - 1; k > i; k--) {
                indexCampos[k].chaveInt = indexCampos[k-1].chaveInt;
                indexCampos[k].byteOffset = indexCampos[k-1].byteOffset;
            }
        } else if(indCab->tipo == 1) { // Para indices string:
            for(int k = indCab->quant - 1; k > i; k--) {
                strcpy(indexCampos[k].chaveStr, indexCampos[k-1].chaveStr);
                indexCampos[k].byteOffset = indexCampos[k-1].byteOffset;
            }
        }
    } else if (direcao == 1){       // shift pra cima
        if(indCab->tipo == 0) {             // para inteiros
            for(int k = i; k < indCab->quant - 1 ; k++) {
                indexCampos[k].chaveInt = indexCampos[k+1].chaveInt;
                indexCampos[k].byteOffset = indexCampos[k+1].byteOffset;
            }
        } else if(indCab->tipo == 1) { // Para indices string:
            for(int k = i; k < indCab->quant - 1 ; k++) {
                strcpy(indexCampos[k].chaveStr, indexCampos[k+1].chaveStr);
                indexCampos[k].byteOffset = indexCampos[k+1].byteOffset;
            }
        }
    }
}


// Insere no array de indices inteiros:
void insere_index_int(IndexCampos *indexCampos, int *ponteiro, IndexCabecalho *indCab, long long int byteOffset) {
    int i = 0;

    // Incrementa 'i' ate achar a posicao certa para inserir indice ordenadamente:
    while(i < (indCab->quant - 1) && *ponteiro > indexCampos[i].chaveInt) {
        i++;
    }
    while(*ponteiro == indexCampos[i].chaveInt && byteOffset > indexCampos[i].byteOffset) {
        i++;
    }

    // Chama funcao de shift, se preciso:
    if(i < indCab->quant - 1) {
        shift_index(indexCampos, i, indCab, 0);
    }

    // Insere index e byte offset:
    indexCampos[i].chaveInt = *ponteiro;
    indexCampos[i].byteOffset = byteOffset;
}


// Insere no array de indices string:
void insere_index_str(IndexCampos *indexCampos, char *ponteiro, IndexCabecalho *indCab, long long int byteOffset) {
    int i = 0;

    while(i < (indCab->quant - 1) && strcmp(ponteiro, indexCampos[i].chaveStr) > 0) {
        i++;
    }
    while(strcmp(ponteiro, indexCampos[i].chaveStr) == 0 && byteOffset > indexCampos[i].byteOffset) {
        i++;
    }
    
    
    // Chama funcao de shift, se precisar:
    if(i < indCab->quant - 1) {
        shift_index(indexCampos, i, indCab, 0);
    }
    
    // Insere index e byte offser:
    strcpy(indexCampos[i].chaveStr, ponteiro);
    indexCampos[i].byteOffset = byteOffset;
}


// Funcao que seleciona qual o campo que sera indexado:
void seleciona_index(IndexCampos *indexCampos, Campos *campos, char *campoIndexado, IndexCabecalho *indCab, long long int byteOffset) {
    
    // Se tipo = inteiro (0):
    if(indCab->tipo == 0) {
        int *ponteiro; // Ponteiro para guardar campo a ser indexado.

        if(strcmp(campoIndexado, "idCrime") == 0) { // se campo e idCrime
            ponteiro = &(campos->idCrime);
        } else if(strcmp(campoIndexado, "numeroArtigo") == 0) { // se campo é numeroArtigo
            ponteiro = &(campos->numeroArtigo);
        } else {
            erro_processamento();
        }

        // Se campo nao for nulo, chama insercao, e incrementa 'quant':
        if(inteiro_nulo(*ponteiro) == 0) {  // funcao que verifica se o numero do ponteiro é nulo
            indCab->quant = indCab->quant + 1;
            insere_index_int(indexCampos, ponteiro, indCab, byteOffset);
        }

    // Se tipo = string (1):
    } else if(indCab->tipo == 1) { 
        // Trunca a chave, se maior que 12:
        trunca_chaves(campos);

        char *ponteiro; // Ponteiro para guardar campo a ser indexado.

        if(strcmp(campoIndexado, "dataCrime") == 0) {
            ponteiro = campos->dataCrime;
        } else if(strcmp(campoIndexado, "marcaCelular") == 0) {
            ponteiro = campos->marcaCelular;
        } else if(strcmp(campoIndexado, "lugarCrime") == 0) {
            ponteiro = campos->lugarCrime;
        } else if(strcmp(campoIndexado, "descricaoCrime") == 0) {
            ponteiro = campos->descricaoCrime;
        } else {
            erro_processamento();
        }
        
        // Se campo nao for nulo, chama insercao, e incrementa 'quant':
        if(string_nula(ponteiro) == 0) {
            indCab->quant = indCab->quant + 1;
            insere_index_str(indexCampos, ponteiro, indCab, byteOffset);
        }
    }
}


// Apaga index de um array realizando o shift:
void apaga_index(IndexCabecalho *indCab, long long int byteOffset, IndexCampos *indCampos) {
    // Apagando do indCampos
    for(int i = 0; i < indCab->quant; i++) {
        if(indCampos[i].byteOffset == byteOffset) {
            shift_index(indCampos, i, indCab, 1);
        }
    }
}


// Atualiza o indice se o registro atualizado foi atualizado no mesmo lugar do antigo (altera somente o campo):
void atualiza_ind(IndexCampos *indCampos, IndexCabecalho *indCab, long long int bOffset, ChavesBusca *atualiz, int atuIndexada, long long int novoBOffset) {
    int indJaExiste = 0;

    // Busca ate encontrar o indice a ser atualizado:
    for(int i = 0; i < indCab->quant; i++) {
        if(indCampos[i].byteOffset == bOffset) {
            apaga_index(indCab, bOffset, indCampos); // Remove o indice
            indCab->quant--;
            indJaExiste = 1;
        }
    }


    indCab->quant++;
    // Insere o index na nova posicao correta:
    if(atualiz[atuIndexada].tipo == 0) {
        insere_index_int(indCampos, &(atualiz[atuIndexada].chaveInt), indCab, novoBOffset);
    }
    else {
        insere_index_str(indCampos, atualiz[atuIndexada].chaveStr, indCab, novoBOffset);
    }
}


// Atualiza o indice segundo o novo byteoffset do registro a ser atualizado:
void atualiza_ind_bOffset(IndexCampos *indCampos, IndexCabecalho *indCab, long long int bOffset, Campos *campos, char *campoIndexado, long long int novoBOffset) {

    // Busca ate encontrar o indice a ser atualizado:
    for(int i = 0; i < indCab->quant; i++) {
        if(indCampos[i].byteOffset == bOffset) {
            apaga_index(indCab, bOffset, indCampos);
            indCab->quant--;
        }
    }

    // Realiza a copia do registro para ser processado pela funcao que insere um index no array
    // (essa funcao altera os dados, entao mandamos uma copia dos dados originais):
    Campos *copiaCampos = copia_campos(campos);

    // Insere o index na nova posicao correta do array (com o novo byteOffset):
    seleciona_index(indCampos, copiaCampos, campoIndexado, indCab, novoBOffset);
}