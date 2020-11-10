#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <regex.h>
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

void copyFilesToSchedulerDirectory(char * schedulerCode, char * plasmaInterfaceCode) {

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
    escreverEmArquivo(schedulerCode, schedulerPath, "TestaDeadlock");
    //copy file plasmaInterface.go
    escreverEmArquivo(plasmaInterfaceCode, plasmaInterfacePath, "TestaDeadlock");

}

void executaProgramaEmProcessoFilho(char * novoPrograma) {

    //criating a child process
    pid_t programaExecutarCompileId = fork();
    if (programaExecutarCompileId < 0) {
        printf("TestaDeadlock: não foi possível executar o programa %s. Erro: impossível criar um processo filho\n", novoPrograma);
        exit(EXIT_FAILURE);
    }

    //executing compile
    if (programaExecutarCompileId == 0) { //child process
        int executeResult = 0;
        void * emptyArg;
        executeResult = execvp(novoPrograma, emptyArg);
        if (executeResult < 0) {
            printf("TestaDeadlock: não foi possível executar o programa %s. Erro: programa não pôde ser executado em processo filho\n", novoPrograma);
            exit(EXIT_FAILURE);
        }
    }

    //waiting for compile execution to finish
    int status;
    wait(&status);

}

char * recuperaNomeDoPrograma() {

    //get current directory path
    char currentDirectoryPath[200];
    getcwd(currentDirectoryPath, 200);

    //open current directory
    DIR *currentDirectory = opendir(currentDirectoryPath);
    if(currentDirectory == NULL) {
        printf("TestaDeadlock: não foi possível abrir o diretório %s.\n", currentDirectoryPath);
        exit(EXIT_FAILURE);
    }

    //find <program_name>.go
    struct dirent *sd;
    while((sd = readdir(currentDirectory)) != NULL) {
        if(sd == NULL) {
            if(errno == EBADF) {
                printf("TestaDeadlock: não foi possível ler o diretório %s.\n", currentDirectoryPath);
            }
            closedir(currentDirectory);
            exit(EXIT_FAILURE);
        }
        //get a regex for finding file .go
        regex_t reg;
        if (regcomp(&reg , ".*\.go", REG_EXTENDED|REG_NOSUB) != 0) {
            fprintf(stderr,"Erro interno [1]\n");
            exit(EXIT_FAILURE);
        }
        if ((regexec(&reg, sd->d_name, 0, (regmatch_t *) NULL, 0)) == 0) {
            //it's not parent directory, so
            break;
        }
    }
    closedir(currentDirectory);
    if (sd == NULL) {
        printf("TestaDeadlock: Não foi possível executar o arquivo .go. Erro: Arquivo não encontrado.");
        exit(EXIT_FAILURE);
    }

    //remove .go from file name
    char * programName = sd->d_name;
    for (unsigned long i = strlen(programName) - 1; i >= 0 ; i--) {
        if (programName[i] == '.') {
            programName[i] = '\0';
            break;
        }
    }

    return programName;
}

int main(int argc, char *argv[]){
//  Copia os arquivos dentro de scheduler
    copyFilesToSchedulerDirectory(
            getSchedulerCodeNoPrintInTerminal(),
            getPlasmaInterfaceCodeNoPrintInTerminal()
            );
//  Executa o compilador - arquivo compile
    executaProgramaEmProcessoFilho("./compile");
//  Executa o modelo gerado internamente em busca da mensagem de deadlock
    char * nomeDoPrograma = recuperaNomeDoPrograma();
    char programa[128];
    strcpy(programa, "./");
    strcat(programa, nomeDoPrograma);
    executaProgramaEmProcessoFilho(programa);
//  Copia outros arquivos para a pasta
    copyFilesToSchedulerDirectory(
            getSchedulerCode(),
            getPlasmaInterfaceCode()
            );
//  Reexecuta o compilador
    executaProgramaEmProcessoFilho("./compile");



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