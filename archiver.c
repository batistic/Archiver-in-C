#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/**
* Simple Archiver
* @author Edson da Costa Vitor Junior
* @author Guilherme Atihe de Oliveira
* @author Gustavo Batistic Ribeiro
* @author Matheus Baroni
*
*
* Archiver Header Structure
* Date yy-mm-dd-hh-mm-ss
* Number of files included
* Next File Pointer (RRN)
*
* File Header
* Next File Pointer (RRN)
* Name Size
* Name
* Is Deleted?
* File Size /512 rounded up (2 bytes)
*
*
* ! Importante
* Converter numero de 2 bytes para 2 bytes independentes
*
* unsigned short number = ceil(ftell(file)/512);
  unsigned char p1;
  p1 = *((unsigned char*)(&number));
  fputc(p1, archive);
  unsigned char p2;
  p2 = *((unsigned char*)(&number) + 1);
  fputc(p2, archive);
*
* Contrario
*
* unsigned short x = p1 | p2 << 8;
*/

int delete(char *archiveName, char *fileName) {
  FILE *archive, *file;

  char *archiveFileName = malloc(strlen(archiveName)+strlen(".arc")+1);
  strcpy(archiveFileName, archiveName);
  strcat(archiveFileName, ".arc");

  int a;

  file = fopen(fileName, "w+");
  if (file == NULL) {
    puts("Impossivel abrir arquivo.");
    exit(1);
  }

  archive = fopen(archiveFileName, "rb+");
  if (archive == NULL) {
    puts("Impossivel abrir archiver.");
    fclose(file);
    exit(1);
  }
  int index = 7;
  while(1) {
    a = fgetc(archive);
    if(!a)
      break;
    else {
      fseek(archive, (a*512), SEEK_SET);
    }
  }
  a = fgetc(archive);
  fseek(archive, a, SEEK_CUR);
  fputc(1, archive);
  while ( (a = fgetc(file)) != EOF )
    fputc(a, archive);
  fclose(file);
  fclose(archive);

  return 0;
}

int list(char *name) {
  FILE *archive;
  char *fileName = malloc(strlen(name)+strlen(".arc")+1);
  strcpy(fileName, name);
  strcat(fileName, ".arc");

  int a, x;

  archive = fopen(fileName, "rb+");
  if (archive == NULL) {
    puts("Impossivel abrir archiver.");
    fclose(archive);
    exit(1);
  }

  printf("Lista de Arquivos presentes\n");
  fseek(archive, 6, SEEK_CUR);
  a = fgetc(archive);
  printf("Numero de arquivos: %d\n", a);

  x = fgetc(archive);
  if(a == 0)
    return 0;

  if (a == 1) {
    fseek(archive, 513, SEEK_SET);
    a = fgetc(archive);
    printf("Arquivo 1: ");
    for (int i = 0; i < a; ++i) {
      printf("%c", fgetc(archive));
    }
    printf("\nFim da Lista!\n");
  } else {
    fseek(archive, 0, SEEK_SET);
    fseek(archive, 513, SEEK_CUR);
    while(a > 0) {
      a = fgetc(archive);
      printf("Arquivo id %d: ", x);
      for (int i = 0; i < a; ++i) {
        printf("%c", fgetc(archive));
      }
      fseek(archive, ((a+2)*-1), SEEK_CUR);
      fseek(archive, a*512, SEEK_CUR);
    }
    printf("\nFim da Lista!\n");
  }

  fclose(archive);

  return 0;
}

int subtract(char *name) {
  return 0;
}



// Adiciona arquivo para o archiver
int add(char *archiveName, char *fileName) {
  FILE *archive, *file;

  char *archiveFileName = malloc(strlen(archiveName)+strlen(".arc")+1);
  strcpy(archiveFileName, archiveName);
  strcat(archiveFileName, ".arc");

  int a;
  int index = 0;
  unsigned short x;
  unsigned char p1, p2;

  file = fopen(fileName, "rb");
  if (file == NULL) {
    puts("Impossivel abrir arquivo.");
    exit(1);
  }

  fseek(file, 0L, SEEK_END);
  unsigned short number = ceil(ftell(file)/512);
  fclose(file);

  file = fopen(fileName, "rb");
  if (file == NULL) {
    puts("Impossivel abrir arquivo.");
    exit(1);
  }

  archive = fopen(archiveFileName, "rb+");
  if (archive == NULL) {
    puts("Impossivel abrir archiver.");
    fclose(file);
    exit(1);
  }

  fseek(archive, 6, SEEK_SET);
  a = fgetc(archive);
  fseek(archive, -1, SEEK_CUR);
  fputc(a+1, archive);
  a = fgetc(archive);

  if(a == 0) {
    fseek(archive, -1, SEEK_CUR);
    fputc(1, archive);
    fseek(archive, 512, SEEK_SET);
    fputc(0, archive);
    fputc(strlen(fileName), archive);
    for (int i = 0; i < strlen(fileName); ++i) {
      fputc(fileName[i], archive);
    }
    fputc(0, archive);
  } else {
    //Caso este arquivo nao seja o primeiro da lista
    // Procurar o fim da lista
    while(a > 0) {
      fseek(archive, -1, SEEK_CUR);
      for (int i = 0; i < a; ++i) {
        fseek(archive, 512, SEEK_CUR);
      }
      a = fgetc(archive);
    }

    // Chegou no ultimo!

    a = fgetc(archive);
    fseek(archive, (a+1), SEEK_CUR);
    p1 = fgetc(archive);
    p2 = fgetc(archive);
    x = p1 | p2 << 8;
    fseek(archive, (x*512), SEEK_CUR);

    fputc(0, archive);
    fputc(strlen(fileName), archive);
    for (int i = 0; i < strlen(fileName); ++i) {
      fputc(fileName[i], archive);
    }
    fputc(0, archive);
  }

  // Armazenar tamanho do arquivo em dois bytes

  p1 = *((unsigned char*)(&number));
  fputc(p1, archive);

  p2 = *((unsigned char*)(&number) + 1);
  fputc(p2, archive);

  // Copiar arquivo
  while ( (a = fgetc(file)) != EOF )
    fputc(a, archive);

  fclose(file);
  fclose(archive);

  return 1;
}

// Cria um novo arquivo junto do cabeçalho
int create(char *name) {
  char *fileName = malloc(strlen(name)+strlen(".arc")+1);
  strcpy(fileName, name);
  strcat(fileName, ".arc");

  FILE *file = fopen(fileName, "wb+");

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  //Set Time in header

  fputc(tm.tm_year, file);
  fputc(tm.tm_mon + 1, file);
  fputc(tm.tm_mday, file);
  fputc(tm.tm_hour, file);
  fputc(tm.tm_min, file);
  fputc(tm.tm_sec, file);

  //set number of files included
  fputc(0, file);
  fputc(0, file);

  fclose(file);
  return 1;
}

int main(){
    int opc=-1;
    while(opc!=0){
        system("cls");
        printf("Gerenciador de Archives\n\n");
        printf("1. Criar archive\n");
        printf("2. Listar arquivos de um archive\n");
        printf("3. Extrair arquivo de um archive\n");
        printf("4. Incluir arquivo em um archive\n");
        printf("5. Excluir arquivo de um archive\n");
        printf("0. Sair\n\n-> ");
        scanf("%d",&opc);
        switch (opc) {
            case 1: //criararchive();
                    break;
            case 2: //listararchive();
                    break;
            case 3: //extrairarquivo();
                    break;
            case 4: //incluirarquivo();
                    break;
            case 5: //excluirarquivo();
                    break;
            case 0: break;
            default: printf("Digite uma opcao valida\n");
        }
    }
  printf("\nSimple C Archiver\nDesigned for ST562\n\nCreating file 'teste.arc'...\n");

  //Create teste.arc
  create("teste");

  //Add texto.txt to teste.arc
  add("teste", "chart.png");
  add("teste", "texto.txt");
  printf("Arquivo Lido\n\n");
  list("teste");
  //delete("teste", "new.png");
}
