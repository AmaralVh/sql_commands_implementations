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


// Tratando do caso NULO e dos casos de sobra de bytes:
void trata_dados_bin(Campos *dados) {
    if(strcmp(dados->dataCrime, "") == 0) { 
        strcpy(dados->dataCrime, "NULO");
    }
    if(dados->numeroArtigo == (-1)) {
        dados->numeroArtigo = 0;        // Indica NULO
    }
    if(strcmp(dados->marcaCelular, "$$$$$$$$$$$$") == 0) {
        strcpy(dados->marcaCelular, "NULO");
    }

    for(int i = 0; i < strlen(dados->marcaCelular); i++) {
        if(dados->marcaCelular[i] == '$') {
            dados->marcaCelular[i] = '\0';
            break;
        }
    }
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
void print_de_arquivo_bin(Campos *dados, int quantidade){
    for(int i = 0; i < quantidade; i++) {
        if(dados[i].removido == '0') {
            printf("%d, ", dados[i].idCrime);
            printf("%s, ", dados[i].dataCrime);

            if (dados[i].numeroArtigo == 0)
                printf("NULO, ");
            else
                printf("%d, ", dados[i].numeroArtigo);

            printf("%s, ", dados[i].lugarCrime);
            printf("%s, ", dados[i].descricaoCrime);
            printf("%s", dados[i].marcaCelular);
            printf("\n");
        }
    }
}


// Retorna o total de registros do arquivo:
int num_registros_bin(Cabecalho *cabecalho) {
    return cabecalho->nroRegArq;
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


int compara_campos_int(Campos *campos, ChavesBusca *chavesBusca, int j) {
    int igual = 0;

    if(strcmp(chavesBusca[j].campoBusca, "idCrime") == 0) {
        if(chavesBusca[j].chaveInt == campos->idCrime) {
            igual = 1;
        }
    } else if(strcmp(chavesBusca[j].campoBusca, "numeroArtigo") == 0) {
        if(chavesBusca[j].chaveInt == campos->numeroArtigo) {
            igual = 1;
        }
    }

    return igual;
}


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
 

void busca_sequencial(FILE *arquivoBin, Campos *campos, Campos *encontrados, Cabecalho *cab, ChavesBusca *chavesBusca, int numPares) {
    int i = 0;
    int numEncontrados = 0;

    if(!registro_inexistente(arquivoBin, cab)) {
        while(i < cab->nroRegArq) {
            int igual = 1;

            le_arquivo_bin(arquivoBin, campos);

            trata_dados_bin(campos);

            if(campos->removido == '0') {
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
            
            if(igual == 1) {
                if(numEncontrados > 0) {
                    encontrados = (Campos *) realloc(encontrados, (numEncontrados + 1) * sizeof(Campos));
                }
                
                // Salva o registro encontrado:
                encontrados[numEncontrados].idCrime = campos->idCrime;
                encontrados[numEncontrados].numeroArtigo = campos->numeroArtigo;
                encontrados[numEncontrados].removido = campos->removido;
                strcpy(encontrados[numEncontrados].dataCrime, campos->dataCrime);
                encontrados[numEncontrados].lugarCrime = copia_campo_variavel(campos->lugarCrime);
                encontrados[numEncontrados].descricaoCrime = copia_campo_variavel(campos->descricaoCrime);
                strcpy(encontrados[numEncontrados].marcaCelular, campos->marcaCelular);
                
                numEncontrados++;
            }
            i++;
        }
    } 

    if(numEncontrados > 0) {
        print_de_arquivo_bin(encontrados, numEncontrados);
    } else {
        erro_reg_inexistente();
    }
    
}