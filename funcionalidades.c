#include "funcionalidades.h"


// Ocorrencia de erro:
void erro_processamento() {
    printf("Falha no processamento do arquivo.\n");
    exit(0);
}


// Escolha da primeira funcionalidade:
void create_table() {
    FILE *arquivo1, *arquivo2;
    char nome_arquivocsv[50];
    char nome_arquivobin[50];
    Campos *dados;
    Cabecalho *cabecalho;

    scanf("%s %s", nome_arquivocsv, nome_arquivobin);

    cabecalho = aloca_cabecalho();

    // Abertura de arquivos
    arquivo1 = abre_arquivo(nome_arquivocsv, 1, cabecalho);
    arquivo2 = abre_arquivo(nome_arquivobin, 2, cabecalho);

    if(arquivo1 == NULL){   // Caso arquivo nao exista
        erro_processamento();
    }else{
        // Alocacao de campos:
        dados = aloca_campos();

        le_cabecalho_csv(arquivo1); // Descarte de cabecalho

        aloca_campos_variaveis(dados);

        // Transferindo registros do csv para bin um de cada vez:
        while(feof(arquivo1) == 0) {
            trata_dados_csv(dados);
            if(le_arquivo_csv(arquivo1, dados)) {
                escreve_arquivo_bin(arquivo2, dados, cabecalho);
            }
        }

        free(dados);
    }

    // Fechamento de arquivos:
    fecha_arquivo(arquivo1, 1, cabecalho);
    fecha_arquivo(arquivo2, 2, cabecalho);

    free(cabecalho);

    // Saida esperada do binario:
    binarioNaTela(nome_arquivobin);
}


// Escolha da segunda funcionalidade: 
void select_from() {
    FILE *arquivo;
    char nome_arquivo_bin[50];
    Campos *dados;
    Cabecalho *cabecalho;

    scanf("%s", nome_arquivo_bin);

    // Abrindo arquivos e alocando cabecalho:
    cabecalho = aloca_cabecalho();

    arquivo = abre_arquivo(nome_arquivo_bin, 3, cabecalho);

    dados = aloca_campos();

    
    if(le_cabecalho_bin(arquivo, cabecalho) == 0 || cabecalho->status != '1') { // Caso não tenha registro
        printf("Registro inexistente.\n");
    } else { // Se existe, recupera dados:
        aloca_campos_variaveis(dados);

        for(int cont = 0; cont < num_registros_bin(cabecalho); cont++) {
            le_arquivo_bin(arquivo, dados);
            trata_dados_bin(dados);
            print_de_arquivo_bin(dados);
        }
    }    
    
    free(dados);

    // Fechamento do arquivo:
    fecha_arquivo(arquivo, 3, cabecalho);

    free(cabecalho);   
}


// Escolha da terceira funcionalidade:
void create_index() {
    FILE *arquivoBin;
    char nomeArquivoBin[50];
    char campoIndexado[40];
    char tipoDado[30];
    char nomeArquivoIndex[50];
    Campos *campos;
    Cabecalho *cabecalho;
    IndexCabecalho *indexCabecalho;
    IndexCampos *indexCampos;

    // Recebe entradas: 
    scanf("%s %s %s %s", nomeArquivoBin, campoIndexado, tipoDado, nomeArquivoIndex);

    // Aloca cabecalho e campos para leitura do arquivo binario:
    cabecalho = aloca_cabecalho();
    campos = aloca_campos();
    aloca_campos_variaveis(campos);

    // Abre arquivo binario para leitura (3):
    arquivoBin = abre_arquivo(nomeArquivoBin, 3, cabecalho);

    indexCampos = aloca_indice();

    int tipo = seleciona_tipo(tipoDado);

    int quant = 0;
    if(!registro_inexistente(arquivoBin, cabecalho)) {  
        for(int cont = 0; cont < num_registros_bin(cabecalho); cont++) {
            le_arquivo_bin(arquivoBin, campos);  

            trata_dados_bin(campos);

            if(quant > 0) {
                indexCampos = (IndexCampos *) realloc(indexCampos, (quant + 1) * sizeof(IndexCampos));
            }

            seleciona_index(indexCampos, campos, campoIndexado, tipo, &quant);
        }
    }

    indexCabecalho = aloca_cabecalho_index();

    for(int i = 0; i < quant; i++) {
        printf("%s\n", indexCampos[i].chaveStr);
    }

    printf("total nao nulos: %d", quant);

    // cria_arq_index();
}



// testeeeeeee