#include "funcionalidades.h"


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

        aloca_campos_variaveis(dados, 0);

        // Transferindo registros do csv para bin um de cada vez:
        while(feof(arquivo1) == 0) {
            trata_dados_csv(dados);
            if(le_arquivo_csv(arquivo1, dados)) {
                escreve_arquivo_bin(arquivo2, dados, cabecalho);
            }
        }

        desaloca_campos_variaveis(dados);
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

    
    if(!registro_inexistente(arquivo, cabecalho)) { 
        // Se existe regitro e arquivo consistente, recupera dados:
        aloca_campos_variaveis(dados, 0);

        for(int cont = 0; cont < num_registros_bin(cabecalho); cont++) {
            le_arquivo_bin(arquivo, dados);
            trata_dados_bin(dados);
            print_de_arquivo_bin(dados, 1);
        }
    }    
    
    desaloca_campos_variaveis(dados);
    free(dados);

    // Fechamento do arquivo:
    fecha_arquivo(arquivo, 3, cabecalho);

    free(cabecalho);   
}


// Escolha da terceira funcionalidade:
void create_index() {
    FILE *arquivoBin;
    char nomeArqBin[50];
    char campoIndexado[40];
    char tipoDado[30];
    char nomeArqIndex[50];
    Campos *campos;
    Cabecalho *cabecalho;
    IndexCabecalho *indexCab;
    IndexCampos *indexCampos;

    // Recebe entradas: 
    scanf("%s %s %s %s", nomeArqBin, campoIndexado, tipoDado, nomeArqIndex);

    // Aloca cabecalho e campos para leitura do arquivo binario:
    cabecalho = aloca_cabecalho();
    campos = aloca_campos();
    aloca_campos_variaveis(campos, 0);

    // Abre arquivo binario para leitura (3):
    arquivoBin = abre_arquivo(nomeArqBin, 3, cabecalho);

    indexCab = aloca_cabecalho_index();
    indexCampos = aloca_indice();

    // Preenche a variavel 'tipo' de acordo com o 'tipoDado' lido:
    int tipo = seleciona_tipo(tipoDado);

    indexCab->quant = 0;
    long long int byteOffset = 17;
    
    if(!registro_inexistente(arquivoBin, cabecalho)) { 
        // Se arquivo consistente e se existe registro, inicia a leitura:
        for(int cont = 0; cont < num_registros_bin(cabecalho); cont++) {
            // Le registro do binario:
            int tamRegBytes = le_arquivo_bin(arquivoBin, campos);  

            // Trata os dados lidos (valores nulos e cifrao):
            trata_dados_bin(campos);

            if(campos->removido == '0') {
                // Realoca memoria para mais um indice (se quant > 0):
                if(indexCab->quant > 0) {
                    indexCampos = (IndexCampos *) realloc(indexCampos, (indexCab->quant + 1) * sizeof(IndexCampos));
                }
                
                // Seleciona o campo de indexacao e o insere no array de indices (com seu byte offset):
                seleciona_index(indexCampos, campos, campoIndexado, tipo, indexCab, byteOffset);
            }

            // Incrementa o byteOffset com o tamanho do registro atual lido:
            byteOffset = byteOffset + tamRegBytes;
        }
    }

    fecha_arquivo(arquivoBin, 3, cabecalho);

    // Preenche indice com cifrao ate 12 bytes:
    if(tipo == 1) {
        preenche_cifrao_ind(indexCampos, indexCab);
    }
    

    for(int i = 0; i < indexCab->quant; i++) {
        printf("%d, %lld\n", indexCampos[i].chaveInt, indexCampos[i].byteOffset);
    }

    printf("Quantidade de nao nulos: %d.\n", indexCab->quant);

    // Cria o arquivo de indice:
    cria_arq_index(nomeArqIndex, indexCampos, indexCab, tipo);

    free(cabecalho);
    free(campos);
    free(indexCampos);
    free(indexCab);

    binarioNaTela(nomeArqIndex);
}


// Escolha da quarta funcionalidade:
void select_from_where() {
    FILE *arqBin;
    FILE *arqInd;
    char nomeArqBin[50];
    char campoIndexado[40];
    char tipoDado[30];
    char nomeArqIndex[50];
    int numBuscas;
    IndexCabecalho *indexCab;
    IndexCampos *indexCampos;
    ChavesBusca *chavesBusca;
    Cabecalho *cab;
    Campos *campos;

    scanf("%s %s %s %s %d", nomeArqBin, campoIndexado, tipoDado, nomeArqIndex, &numBuscas);

    int indexado = -1;
    int numPares;

    int tipo = seleciona_tipo(tipoDado);

    // loop para num de buscas
    for(int i = 0; i < numBuscas; i++) {
        scanf("%d", &numPares);

        chavesBusca = aloca_chaves_busca(numPares);

        for(int j = 0; j < numPares; j++){
            scanf("%s", chavesBusca[j].campoBusca);

            // Verifica se ha um campo indexado para a busca:
            if(strcmp(campoIndexado, chavesBusca[j].campoBusca) == 0) {
                indexado = j;
            }
            
            // Caso para campo inteiro:
            if(strcmp(chavesBusca[j].campoBusca, "idCrime") == 0 || strcmp(chavesBusca[j].campoBusca, "numeroArtigo") == 0) {
                chavesBusca[j].tipo = 0;
                scanf("%d", &chavesBusca[j].chaveInt);
            } else { // Caso para campo string:
                chavesBusca[j].tipo = 1;
                scan_quote_string(chavesBusca[j].chaveStr);
            }
        }

        printf("Resposta para busca %d\n", i + 1);

        if(indexado == -1) {
            // Aloca cabecalho e campos para leitura:
            cab = aloca_cabecalho();
            campos = aloca_campos();
            aloca_campos_variaveis(campos, 0);

            arqBin = abre_arquivo(nomeArqBin, 3, cab);

            busca_sequencial(arqBin, campos, cab, chavesBusca, numPares);

            fecha_arquivo(arqBin, 3, cab);
        } else {
            indexCab = aloca_cabecalho_index();
            indexCampos = aloca_indice();
            cab = aloca_cabecalho();
            campos = aloca_campos();
            aloca_campos_variaveis(campos, 0);

            arqInd = abre_arquivo_ind(nomeArqIndex, indexCab, 2);

            le_cab_indince(arqInd, indexCab);

            indexCampos = le_arq_indice(arqInd, indexCampos, indexCab, tipo);

            // printf("\n\n\nAgora, printando la fora:\n\n");
            // for(int i = 0; i < indexCab->quant; i++) {
            //     printf("%d, %lld\n", indexCampos[i].chaveInt, indexCampos[i].byteOffset);
            // }

            arqBin = abre_arquivo(nomeArqBin, 3, cab);

            busca_binaria(indexCampos, indexCab->quant, chavesBusca, indexado, arqBin, campos, numPares);

            fecha_arquivo_ind(arqInd, indexCab);
            fecha_arquivo(arqBin, 3, cab);

            free(indexCampos);
            free(indexCab);
        }
        
        free(chavesBusca);
        desaloca_campos_variaveis(campos);
        free(campos);
        free(cab);

        indexado = -1;
    }
}