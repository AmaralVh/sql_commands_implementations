#include <stdio.h>
#include <stdlib.h>
#include "funcionalidades.h"
#include "estrutura.h"

/*
        Disciplina:         SCC0215 - Organização de Arquivos
        Nomes:            Isaac Harim Ribeiro Rosendo - 13673235
                           Victor Hugo Trigolo Amaral - 12688487
*/


int main (){
    int opcao;
    
    // opcao de funcionalidade que o usuário quer fazer com o seu arquivo
    scanf("%d", &opcao);

    // Abertura do arquivo e chamada da funcionalidade:
    switch(opcao) {
        case 1:
            create_table();
            break;
        case 2:
            select_from();
            break;
        case 3:
            create_index();
            break;
        case 4:
            select_from_where(4);
            break;
        case 5:
            delete_from_where();
            break;
        case 6:
            insert_into();
            break;
        case 7:
            update();
            break;
        default:
            erro_processamento();
            break;
    }

    return 0;
}