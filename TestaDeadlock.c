#include <dirent.h>
#include <unistd.h>
#include "EscreveEmArquivo.c"
#include "plasmaInterface.c"
#include "scheduler.c"

DIR * openDirectory(char * path) {
    DIR *dirOrigem = opendir(path);
    if(dirOrigem == NULL) {
        if(errno == EACCES) {
            printf("TestaDeadlock: não foi possível abrir o diretório %s. Erro: Permissão negada.\n", path);
        } else if(errno == ENFILE) {
            printf("TestaDeadlock: não foi possível abrir o diretório %s. Erro: Muitos arquivos abertos no sistema no momento.\n", path);
        } else if(errno == ENOENT) {
            printf("TestaDeadlock: não foi possível abrir o diretório %s. Erro: Diretório não existe.\n", path);
        } else if(errno == ENOMEM) {
            printf("TestaDeadlock: não foi possível abrir o diretório %s. Erro: Memória insuficiente para completar a operação.\n", path);
        } else {
            printf("TestaDeadlock: não foi possível abrir o diretório %s. Erro: %s.\n", path, strerror(errno));
        }

        closedir(dirOrigem);
        exit(EXIT_FAILURE);
    }
    return dirOrigem;
}

void copyFilesToSchedulerDirectory() {

    //scheduler file path to copy:
    //char * arquivoOrigem1 = "files/scheduler.c";

    //plasmaInterface file path to copy:
    //char * arquivoOrigem2 = "files/plasmaInterface.c";

    //get current directory path
    char currentDirectory[200];
    getcwd(currentDirectory, 200);

    //get parent directory
    char * ret = strrchr(currentDirectory, '/'); //get last '/' position

    int parentPathLength = strlen(currentDirectory) - strlen(ret);
    char parentPath[parentPathLength+1];
    for (int i = 0; i < parentPathLength; ++i) {
        parentPath[i] = currentDirectory[i];
    }
    parentPath[parentPathLength] = '\0';

    //append new directory where we want to go
    strcat(parentPath, "/scheduler");

    //save scheduler path
    char schedulerPath[200];
    strcpy(schedulerPath,parentPath);
    strcat(schedulerPath,"/scheduler.go");

    //save plasmaInterface path
    char plasmaInterfacePath[200];
    strcpy(plasmaInterfacePath,parentPath);
    strcat(plasmaInterfacePath,"/plasmaInterface.go");

    //copy file scheduler.go
    //escreverEmArquivo(arquivoOrigem1,schedulerPath, "TestaDeadlock");
    escreverEmArquivo(getSchedulerCode(), schedulerPath, "TestaDeadlock");
    //copy file plasmaInterface.go
    escreverEmArquivo(getPlasmaInterfaceCode(), plasmaInterfacePath, "TestaDeadlock");

}

int main(int argc, char *argv[]){

//  Copia os arquivos dentro de scheduler
    copyFilesToSchedulerDirectory();




//  Executa o compilador - arquivo compile
//  Executa o modelo gerado internamente em busca da mensagem de deadlock
//  Retorna ao usuário se ocorreu um deadlock e onde
//  Copia outros arquivos para a pasta
//  Reexecuta o compilador




    //open scheduler directory
//    DIR *schedulerDirectory = openDirectory(parentPath);
//
//    //read current directory
//    struct dirent *sd;
//    while((sd = readdir(dirOrigem)) != NULL) {
//        if(sd == NULL) {
//            if(errno == EBADF) {
//                printf("TestaDeadlock: não foi possível ler o diretório %s.\n", currentDirectory);
//            }
//            closedir(dirOrigem);
//            exit(EXIT_FAILURE);
//        }
//
//        //get parent directory
//        if (strcmp(sd->d_name, ".") != 0) {
//            //it's not parent directory, so
//            continue;
//        }
//
//
//    }
//    closedir(dirOrigem);


    //go to parent directory
    //go to scheduler directory

}