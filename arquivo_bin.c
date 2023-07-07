#include "arquivo_bin.h"


// Funcao para leitura do cabecalho:
int le_cabecalho_bin(FILE *arquivo, Cabecalho *cabecalho){

    // leitura do cabecalho em binario
    fread(&cabecalho->status, sizeof(char), 1, arquivo);
    fread(&cabecalho->proxByteOffset, sizeof(long long int), 1, arquivo);
    fread(&cabecalho->nroRegArq, sizeof(int), 1, arquivo);
    fread(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);

    // retorna numero de arquivos no binario:
    return cabecalho->nroRegArq;
}


// Escrevendo cabecalho no arquivo binario: 
void escreve_cabecalho(FILE *arquivo, Cabecalho *cabecalho) {
    fwrite(&cabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho->proxByteOffset, sizeof(long long int), 1, arquivo);
    fwrite(&cabecalho->nroRegArq, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);
}


// Atualizacao perante o byte offset e numero de arquivos
void atualiza_cabecalho_escrita(Cabecalho *cabecalho, int deslocamentoOffset) {
    cabecalho->proxByteOffset = cabecalho->proxByteOffset + deslocamentoOffset;
    cabecalho->nroRegArq = cabecalho->nroRegArq + 1;
}


// Para a questao de fechar o escopo do cabecalho, necessita de uma ultima atualizacao necessaria
void atualiza_cabecalho_fechamento(FILE *arquivo, Cabecalho *cabecalho) {
    cabecalho->status = '1';

    fseek(arquivo, 0, SEEK_SET);

    escreve_cabecalho(arquivo, cabecalho);
}


// Abrindo arquivo:
FILE *abre_arquivo(char *nome_arquivo, int tipo, Cabecalho *cabecalho) {
    FILE *arquivo;

    // Fizemos um switch para cada tipo de opcao para abertura de arquivo
    switch (tipo){
    case 1:     // o primeiro para leitura:
        arquivo = fopen(nome_arquivo, "r");
        break;

    case 2:     // Abertura para escrever no binario pela primeira vez.
        arquivo = fopen(nome_arquivo, "wb");
        cria_cabecalho(cabecalho);
        break;

    case 3:     // Abertura para ler no binario.
        arquivo = fopen(nome_arquivo, "rb");
        break;

    case 4:     // Abertura para ler e escrever no binario
        arquivo = fopen(nome_arquivo, "rb+");
        cabecalho->status = '0';
        break;

    case 5:     // Abre para escrever no binario que ja existe.
        arquivo = fopen(nome_arquivo, "wb");
        break;
    
    default:
        break;
    }

    // arquivo inexistente
    if(arquivo == NULL) {
        erro_processamento();
    }

    return arquivo;
}


// Fechamento e atualizacao do cabecalho:
void fecha_arquivo(FILE *arquivo, int tipo, Cabecalho *cabecalho) {
    if (tipo == 2){
        atualiza_cabecalho_fechamento(arquivo, cabecalho);
    }

    fclose(arquivo);
}


// Print de erro de registro inexistente:
void erro_reg_inexistente() {
    printf("Registro inexistente.\n");
}


// Verifica consistencia ou se arquivo esta vazio:
int registro_inexistente(FILE *arquivo, Cabecalho *cabecalho) {
    int inexistente = 0;
    
    // Caso não tenha registro, quant_registros = 0 (arq vazio):
    int quant_registros = le_cabecalho_bin(arquivo, cabecalho);

    // Se arquivo vazio, ou inconsistente:
    if(quant_registros == 0 || cabecalho->status != '1') {  
        erro_reg_inexistente();
        inexistente = 1;
    }

    return inexistente;
}


// Leitura de campos variaveis no binario
int le_campos_variaveis_bin(FILE *arquivo, Campos *dados, int opcao, int *tamRegBytes){
    int leuCampo = 0;
    int contador = 0;
    char c;

    // Caso do Lugar Crime
    if (opcao == 1){
        do{ // leitura até o |

            fread(&c, sizeof(char), 1, arquivo);   
            if (c != '|'){  // caso chegue no barramento
                if (contador > 0){
                    dados->lugarCrime = (char *)realloc(dados->lugarCrime, (contador + 1) * sizeof(char));
                }
                dados->lugarCrime[contador] = c;
                contador++;
            }
        } while (c != '|');
        dados->lugarCrime = (char *)realloc(dados->lugarCrime, (contador + 1) * sizeof(char));
        dados->lugarCrime[contador] = '\0';     // atualização do ultimo char

        // caso para Descricao do Crime
    }else{
        do{ // leitura até o |
            fread(&c, sizeof(char), 1, arquivo);

            if (c != '|'){      // caso chegue no barramento
                if (contador > 0){
                    dados->descricaoCrime = (char *)realloc(dados->descricaoCrime, (contador + 1) * sizeof(char));
                }
                dados->descricaoCrime[contador] = c;
                contador++;
            }
        } while (c != '|');
        dados->descricaoCrime = (char *)realloc(dados->descricaoCrime, (contador + 1) * sizeof(char));
        dados->descricaoCrime[contador] = '\0';     // atualização do ultimo char
    }

    if(contador > 0) {
        leuCampo = 1;
    }

    *tamRegBytes = *tamRegBytes + contador;

    // retorna se o campo existe ou nao
    return leuCampo;
}


// Chamando funcao de leitura com campos variaveis e fixos
int le_arquivo_bin(FILE *arquivo, Campos *dados){
    char delimitador;
    int leuLugar;
    int leuDescricao;
    int tamRegBytes = 0;

    // leitura de Tamanho fixo
    fread(&dados->removido, sizeof(char), 1, arquivo);
    fread(&dados->idCrime, sizeof(int), 1, arquivo);
    fread(&dados->dataCrime, sizeof(char), 10, arquivo);
    fread(&dados->numeroArtigo, sizeof(int), 1, arquivo);
    fread(&dados->marcaCelular, sizeof(char), 12, arquivo);

    tamRegBytes = tamRegBytes + 31 + 3;

    // leitura de campos variaveis
    leuLugar = le_campos_variaveis_bin(arquivo, dados, 1, &tamRegBytes);
    leuDescricao = le_campos_variaveis_bin(arquivo, dados, 2, &tamRegBytes);
    fread(&delimitador, sizeof(char), 1, arquivo);

    // casos NULOS
    if(leuLugar == 0) {
        strcpy(dados->lugarCrime, "NULO");
    }
    if(leuDescricao == 0) {
        strcpy(dados->descricaoCrime, "NULO");
    }

    return tamRegBytes;
}


// Escrita no arquivo binario:
void escreve_arquivo_bin(FILE *arquivo, Campos *dados, Cabecalho *cabecalho){
    if (cabecalho->nroRegArq == 0){
        escreve_cabecalho(arquivo, cabecalho);
    }

    char delimitador_campo = '|';
    char delimitador_registro = '#';

    fwrite(&dados->removido, sizeof(char), 1, arquivo);
    fwrite(&dados->idCrime, sizeof(int), 1, arquivo);
    fwrite(dados->dataCrime, sizeof(char), 10, arquivo);
    fwrite(&dados->numeroArtigo, sizeof(int), 1, arquivo);
    fwrite(dados->marcaCelular, sizeof(char), 12, arquivo);
    fwrite(dados->lugarCrime, sizeof(char), strlen(dados->lugarCrime), arquivo);
    fwrite(&delimitador_campo, sizeof(char), 1, arquivo);
    fwrite(dados->descricaoCrime, sizeof(char), strlen(dados->descricaoCrime), arquivo);
    fwrite(&delimitador_campo, sizeof(char), 1, arquivo);
    fwrite(&delimitador_registro, sizeof(char), 1, arquivo);

    // Calcula o deslocamento do byte offset de acordo com os bytes ocupados pelos campos fixos
    // (31), pelos dois campos variaveis e pelos delimitadores de campos e de registro (3):
    int deslocamentoOffset = 31 + strlen(dados->lugarCrime) + strlen(dados->descricaoCrime) + 3;

    atualiza_cabecalho_escrita(cabecalho, deslocamentoOffset);
}


// Prints para a opcao 2 onde escreve todos os dados na tela
void print_de_arquivo_bin(Campos *dados){
    if(dados->removido == '0') {
        printf("%d, ", dados->idCrime);
        printf("%s, ", dados->dataCrime);

        if (dados->numeroArtigo == 0 || dados->numeroArtigo == (-1))
            printf("NULO, ");
        else
            printf("%d, ", dados->numeroArtigo);

        printf("%s, ", dados->lugarCrime);
        printf("%s, ", dados->descricaoCrime);
        printf("%s", dados->marcaCelular);
        printf("\n");
    }
}

// Retorna um inteiro atrelado ao conteudo do arquivo binario:
void binarioNaTela(char *nomeArquivoBinario) {
    unsigned long i, cs;
    unsigned char *mb;
    size_t fl;
    FILE *fs;
    if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))){
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    cs = 0;
    for (i = 0; i < fl; i++){
        cs += (unsigned long)mb[i];
    }
    printf("%lf\n", (cs / (double)100));
    free(mb);
    fclose(fs);
}

// verifica se int de idCrime e numeroArtigo sao iguais
int compara_campos_int(Campos *campos, ChavesBusca *chavesBusca, int j) {
    int igual = 0;

    if(strcmp(chavesBusca[j].campoBusca, "idCrime") == 0) {
        if(chavesBusca[j].chaveInt == campos->idCrime) {
            igual = 1;  // sao iguais
        }
    } else if(strcmp(chavesBusca[j].campoBusca, "numeroArtigo") == 0) {
        if(chavesBusca[j].chaveInt == campos->numeroArtigo) {
            igual = 1;  // sao iguais
        }
    }

    return igual;
}

// verifica se strings sao iguais
int compara_campos_str(Campos *campos, ChavesBusca *chavesBusca, int j) {
    int igual = 0;

    if(strcmp(chavesBusca[j].campoBusca, "dataCrime") == 0) {
        if(strcmp(chavesBusca[j].chaveStr, campos->dataCrime) == 0) {
            igual = 1;
        } 
    } else if(strcmp(chavesBusca[j].campoBusca, "lugarCrime") == 0) {
        if(strcmp(chavesBusca[j].chaveStr, campos->lugarCrime) == 0) {
            igual = 1;
        } 
    } else if(strcmp(chavesBusca[j].campoBusca, "descricaoCrime") == 0) {
        if(strcmp(chavesBusca[j].chaveStr, campos->descricaoCrime) == 0) {
            igual = 1;
        } 
    } else if(strcmp(chavesBusca[j].campoBusca, "marcaCelular") == 0) {
        if(strcmp(chavesBusca[j].chaveStr, campos->marcaCelular) == 0) {
            igual = 1;
        } 
    }

    return igual;
}


// marca registro como removido e escreve no arquivo binario
void marca_removido(FILE *arqBin, Campos *campos, long long int byteOffset) {
    campos->removido = '1';     // seta pra 1, no removido

    // Marca registro como apagado no arquivo de dados:
    fseek(arqBin, byteOffset, SEEK_SET);
    fwrite(&campos->removido, sizeof(char), 1, arqBin);
}

// busca sequencial que retorna byteoffset
long long int **busca_sequencial(FILE *arqBin, Campos *campos, Cabecalho *cab, ChavesBusca *chavesBusca, int numPares, int func, long long int **byteOffsets, int busca) {
    int i = 0;
    int numEncontrados = 0;
    long long int bOffset = 17;
    campos->numEncontrados = 0;
    
    // se registro existe
    if(!registro_inexistente(arqBin, cab)) {
        while(i < cab->nroRegArq) {
            int igual = 1;

            int tamRegBytes = le_arquivo_bin(arqBin, campos);

            // trata casos nulos
            trata_dados_binario_para_estrutura(campos);

            // se campo nao foi removido
            if(campos->removido == '0') {
                // se tipos = inteiro
                for(int j = 0; j < numPares; j++) {
                    if(chavesBusca[j].tipo == 0) {
                        igual = compara_campos_int(campos, chavesBusca, j);
                    } else if(chavesBusca[j].tipo == 1) {
                        igual = compara_campos_str(campos, chavesBusca, j);
                    }

                    if(igual == 0) {
                        break;
                    }
                }
            } else {
                igual = 0;
            }
            
            if(igual == 1) {    // sao iguais
                numEncontrados++;
                campos->numEncontrados++;

                // alocando byteoffset
                if(numEncontrados > 0) {
                    byteOffsets[busca] = (long long int *) realloc(byteOffsets[busca], (numEncontrados+1) * sizeof(long long int));
                }

                // Guarda o byteOffset do registro que encontrou:
                byteOffsets[busca][numEncontrados-1] = bOffset;

                // Caso de recuperação de registro para print:
                if(func == 4) {
                    print_de_arquivo_bin(campos);
                } 
            }
            i++;
            // atualiza o byteoffset
            bOffset = bOffset + tamRegBytes;
        }
    } 

    if(numEncontrados == 0) {
        if(func == 4) {
            erro_reg_inexistente();
        }
    }

    return byteOffsets;
}


// percorre todos os indices iguais em ordem de byteOffset, realizando a busca:
long long int **percorre_ind_iguais(IndexCampos *indCampos, ChavesBusca *chavBusca, int meio, int index, FILE *arqBin, Campos *campos, int numPares, int func, long long int **byteOffsets, int busca) {
    int primeiro = meio;
    int ultimo = meio;
    int igual = 1;
    int numEncontrados = 0;
    campos->numEncontrados = 0;

    if(chavBusca[index].tipo == 0) {
        // Encontra o inicio dos indices iguais ao procurado:
        while(indCampos[primeiro - 1].chaveInt == chavBusca[index].chaveInt) {
            primeiro--;
        }
        // Encontra o fim dos indices iguais ao procurado:
        while(indCampos[ultimo + 1].chaveInt == chavBusca[index].chaveInt) {
            ultimo++;
        }
    } else if(chavBusca[index].tipo == 1) {
        // Encontra o inicio dos indices iguais ao procurado:
        while(strcmp(indCampos[primeiro - 1].chaveStr, chavBusca[index].chaveStr) == 0) {
            primeiro--;
        }
        // Encontra o fim dos indices iguais ao procurado:
        while(strcmp(indCampos[ultimo + 1].chaveStr, chavBusca[index].chaveStr) == 0) {
            ultimo++;
        }
    }

    // Percorre todos os indices iguais a chave indexada:
    for(int i = primeiro; i <= ultimo; i++) {
        fseek(arqBin, indCampos[i].byteOffset, SEEK_SET);

        // le o registro:
        int tamRegBytes = le_arquivo_bin(arqBin, campos);

        // Trata o registro com cifrao:
        trata_dados_binario_para_estrutura(campos);

        // Compara se todos os campos dele batem com a busca:
        for(int j = 0; j < numPares; j++) {
            if(j != index) {
                if(chavBusca[j].tipo == 0) {
                    igual = compara_campos_int(campos, chavBusca, j);
                } else if(chavBusca[j].tipo == 1) {
                    igual = compara_campos_str(campos, chavBusca, j);
                }

                if(igual == 0) {
                    break;
                }
            }
        }

        // registro encontrado:
        if(igual == 1) {
            numEncontrados++;
            campos->numEncontrados++;

            if(numEncontrados > 0) {
                byteOffsets[busca] = (long long int *) realloc(byteOffsets[busca], (numEncontrados+1) * sizeof(long long int));
            }

            // Guarda o byteOffset do registro encontrado:
            byteOffsets[busca][numEncontrados-1] = indCampos[i].byteOffset;

            if(func == 4) {                     // print de registro se for apenas busca
                print_de_arquivo_bin(campos);
            }
        }
    }

    if(numEncontrados == 0) {
        if(func == 4) {
            erro_reg_inexistente();
        }
    }

    return byteOffsets;
}


// Realiza busca binaria pelo indice buscado:
int busca_binaria(IndexCampos *indexCampos, int quantInd, ChavesBusca *chavesBusca, int index, Campos *campos, int func) {
    int esq = 0;
    int dir = quantInd - 1;
    int meio;
    int encontrouIndice = 0;

    if(chavesBusca[index].tipo == 0) {  // tipo 0 = inteiro
        while(esq <= dir) {
            meio = (esq + dir) / 2;

            // verificando chaveInt
            if(chavesBusca[index].chaveInt == indexCampos[meio].chaveInt) {
                encontrouIndice = 1;
                break;
            }

            // se o meio for maior, o da direita se torna meio -1
            if(chavesBusca[index].chaveInt < indexCampos[meio].chaveInt) {
                dir = meio - 1;
            
            // se o meio for menor, o da direita se torna meio +1
            } else if(chavesBusca[index].chaveInt > indexCampos[meio].chaveInt) {
                esq = meio + 1;
            }
        }
    } else if(chavesBusca[index].tipo == 1) {   // tipo 2 = str
        preenche_cifrao_chav_str(chavesBusca, index);

        while(esq <= dir) {
            meio = (esq + dir) / 2;

            // verificando meio
            if(strcmp(chavesBusca[index].chaveStr, indexCampos[meio].chaveStr) == 0) {
                encontrouIndice = 1;
                break;
            }

            // verifica se menor, e atualiza dir
            if(strcmp(chavesBusca[index].chaveStr, indexCampos[meio].chaveStr) < 0) {
                dir = meio - 1;

            // verifica se maior, e atualiza esq
            } else if(strcmp(chavesBusca[index].chaveStr, indexCampos[meio].chaveStr) > 0) {
                esq = meio + 1;
            }
        }
    }

    if(encontrouIndice == 0) {
        if(func == 4) {
            erro_reg_inexistente();
        }
        
        meio = -1;
    }

    return meio;
}

// Insere no fim do arquivo:
void insere_arq_dados(FILE *arqBin, Campos *insercao, Cabecalho *cab) {
    fseek(arqBin, cab->proxByteOffset, SEEK_SET);  // Pula para o ultimo byte.

    escreve_arquivo_bin(arqBin, insercao, cab);
}


// Completa o registro escrito com cifrao, se a atualizacao for menor que o registro antigo:
void completa_reg_cifrao(int tamRegAtual, int tamRegNovo, FILE *arqBin){
    int quant_cifrao = tamRegAtual - tamRegNovo;

    char cifrao = '$';
    char hashtag = '#';

    fseek(arqBin, -1, SEEK_CUR);

    for(int i = 0; i < quant_cifrao; i++) {
        fwrite(&cifrao, sizeof(char), 1, arqBin);
    }

    fwrite(&hashtag, sizeof(char), 1, arqBin);
} 
