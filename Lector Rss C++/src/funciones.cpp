/*
 * funciones.c
 *
 *  Created on: 13/4/2016
 *      Author: Jon
 */
#include "funciones.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Noticia.h"
#include <list>
#include "sqlite3.h"
using namespace std;
char mostrarMenu() {
	printf("MENU PRINCIPAL\n"
			"1.\tVisualizar RSS\n"
			"2.\tEditar RSS\n"
			"3.\tCrear RSS\n"
			"4.\tInformacion RSS\n"
			"5.\tSalir\n\n"
			"Introduzca numero de la funcion deseada:\n");
	fflush(stdout);
	char resultado;
	resultado = getchar();
	fflush(stdin);
	return resultado;
}
void crearRss(string nombreRSS, list<Noticia*>* noticias) {

	char fin = 'n';
	do {
		noticias->push_back(nuevaNoticia());
		printf("Quieres introudcir mas noticias? s/n \n");
		fflush(stdout);
		scanf("%c", &fin);
		fflush(stdin);
	} while (fin=='s');
	almacenarEnBD(nombreRSS, noticias);

}
Noticia* nuevaNoticia() {

	Noticia* n = new Noticia();
	printf("Introduce titulo de la noticia: \n");
	fflush(stdout);
	string titulo;
	scanf("%s", titulo.c_str());
	fflush(stdin);
	n->setTitulo(titulo);
	//////titulo//////
	printf("Introduce autor de la noticia: \n");
	fflush(stdout);
	string autor;
	scanf("%s", autor.c_str());
	fflush(stdin);
	n->setAutor(autor);
	//////autor//////
	printf("Introduce descripcion de la noticia: \n");
	fflush(stdout);
	string desc;
	scanf("%s", desc.c_str());
	fflush(stdin);
	n->setDescripcion(desc);
	//////descripcion//////
	return n;
}
void mostrarNoticia(Noticia* n) {
	printf("Titulo de la noticia: %s\n", n->getTitulo().c_str());
	fflush(stdout);
	printf("Autor de la noticia: %s\n", n->getAutor().c_str());
	fflush(stdout);
	printf("Descripcion de la noticia: %s\n", n->getDescripcion().c_str());
	fflush(stdout);
}

void conectarBD(sqlite3* db, int rc)
{
	rc = sqlite3_open("xmlbd.s3db", &db);

	if( rc ){
	  //cambiar stderr por stdout para mostrar por consola
	  fprintf(stdout, "Error al abrir BD: %s\n", sqlite3_errmsg(db));
	  exit(0);
	}else{
	  fprintf(stdout, "Base de datos abierta exitosamente\n");
	}
	//lalal
}
void cerrarBD(sqlite3* db)
{
	sqlite3_close(db);
}
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}


void almacenarEnBD(string nombreRSS, list<Noticia*>* noticias){
	sqlite3* db;
	int rc;
	char *zErrMsg = 0;
	//conectarBD(db,rc);

	rc = sqlite3_open("xmlbd.S3db", &db);
	   if( rc ){
	      fprintf(stdout, "Can't open database: %s\n", sqlite3_errmsg(db));
	      exit(0);
	   }else{
	      fprintf(stdout, "Opened database successfully\n");
	   }

	string sql ="";
	string sql2 ="INSERT INTO RSS (NOMBRE)VALUES ( '"+nombreRSS+"');";
	char *csql2 = new char[sql2.length() + 1];
	strcpy(csql2, sql2.c_str());
	delete [] csql2;
	rc = sqlite3_exec(db, csql2, callback, 0, &zErrMsg);

	   if( rc != SQLITE_OK ){
	      fprintf(stdout, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   }else{
	      fprintf(stdout, "Records created successfully1\n");
	   }
	unsigned int i;
	for(i = 0; i<noticias->size(); i++){
		sql+="INSERT INTO NOTICIA (TITULO,AUTOR,DESCRIPCION)VALUES ( '"+get(noticias,i)->getTitulo()+"', '"+get(noticias,i)->getAutor()+"','"+get(noticias,i)->getDescripcion()+"' );";
	}
	const char *csql = sql.c_str();
	rc = sqlite3_exec(db, csql, callback, 0, &zErrMsg);
		   if( rc != SQLITE_OK ){
		      fprintf(stderr, "SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }else{
		      fprintf(stdout, "Records created successfully2\n");
		   }
	cerrarBD(db);
}

/*
 char *substring(char *string, int position, int length)
 {
 char *pointer;
 int c;
 pointer = malloc(length+1);

 if (pointer == NULL)
 {
 printf("Unable to allocate memory.\n");
 exit(1);
 }

 for (c = 0 ; c < length ; c++)
 {
 *(pointer+c) = *(string+position-1);
 string++;
 }

 *(pointer+c) = '\0';

 return pointer;
 }
 int _find(char* cadena, char* palabra){

 int i, j = 0;

 for(i = 0; i < strlen(cadena); i++){
 if(palabra[j] == cadena[i]){
 ++j;

 if(j == strlen(palabra))
 return (2 + i - j);
 }

 else j = 0;
 }

 return -1;
 }

 int abrirRss(char* nombre)
 {
 FILE *ptr_file;
 char *buf;
 int s=-1;
 ptr_file =fopen(nombre,"r");
 if (!ptr_file){
 printf("\n\nArchivo no encotrado!\n\n");
 //getch();
 return 0;
 }

 fseek(ptr_file, 0, SEEK_END);
 s = ftell(ptr_file);
 fseek(ptr_file, 0, SEEK_SET);
 int cont;cont=0;
 buf=(char*)malloc(s*sizeof(char));
 while (fgets(buf,s, ptr_file)!=NULL){
 int p1,p2;
 if (_find(buf,"<item>")!=-1) {
 printf("\n-----------------------\n");
 }else if (_find(buf,"<title>")!=-1) {
 p1=_find(buf,"<title>");
 p2=_find(buf,"]]></title>");
 if (cont==0) {
 char* str;
 str=substring(buf,p1+7,(p2-1)-(p1+6));
 printf("\n\n%s\n",str);
 cont++;
 }
 else{
 char* str;
 str=substring(buf,p1+17,(p2-1)-(p1+17));
 printf("%s\n",str);
 }

 }
 else if (_find(buf,"<pubDate>")!=-1) {
 p1=_find(buf,"<pubDate>");
 p2=_find(buf,"</pubDate>");
 char* str;
 str=substring(buf,p1+9,(p2-1)-(p1+9));
 printf("%s\n",str);
 }else if (_find(buf,"<link>")!=-1) {
 p1=_find(buf,"<link>");
 p2=_find(buf,"</link>");
 char* str;
 str=substring(buf,p1+6,(p2-1)-(p1+5));
 printf("%s\n",str);
 }else if (_find(buf,"<description>")!=-1) {
 p1=_find(buf,"<description><![CDATA[ ");
 p2=_find(buf,"]]></description>");
 char* str;
 str=substring(buf,p1+23,(p2-1)-(p1));
 printf("%s\n",str);
 printf("-------------------\n");
 }
 else if(_find(buf,"</rss>")!=-1){
 return 0;
 }
 }
 fclose(ptr_file);
 //getch();
 return 0;
 }

 Info infoRss(char* nombre)
 {
 FILE *ptr_file;
 char *buf;
 int s=-1;
 ptr_file =fopen(nombre,"r");
 if (!ptr_file){
 printf("\n\nArchivo no encotrado!\n\n");
 //getch();

 }

 fseek(ptr_file, 0, SEEK_END);
 s = ftell(ptr_file);
 fseek(ptr_file, 0, SEEK_SET);
 int contTitulo;contTitulo=0;
 int contItem;contItem=1;
 char numItem[10];
 sprintf(numItem, "%d", contItem);
 strcat(numItem, ". ");
 int contFuente;contFuente=1;
 char numFuente[10];
 sprintf(numFuente, "%d", contFuente);
 strcat(numFuente, ". ");
 buf=(char*)malloc(s*sizeof(char));
 int numCaracteres = 0;
 //		while (fgets(buf,s, ptr_file)!=NULL){
 //			numCaracteres += strlen(buf);
 //		}

 Info inform;
 inform.tituloRss = (char*)malloc(200 * sizeof(char));
 inform.autorRss = (char*)malloc(200 * sizeof(char));
 inform.items = (char*)malloc(10000 * sizeof(char));
 inform.fuentes = (char*)malloc(10000 * sizeof(char));
 while (fgets(buf,s, ptr_file)!=NULL){
 int p1, p2;
 if (_find(buf,"<title>")!=-1) {
 p1=_find(buf,"<title>");
 p2=_find(buf,"]]></title>");
 if (contTitulo==0) {
 char* str;
 str=substring(buf,p1+7,(p2-1)-(p1+6));
 strcpy(inform.tituloRss, str);
 strcat(inform.tituloRss, "\n");
 contTitulo++;
 }
 else{
 char* str;
 str=substring(buf,p1+17,(p2-1)-(p1+17));
 strcat(inform.items, &numItem);
 if(contItem==1)
 {
 strcpy(inform.items, "1. ");
 }
 strcat(inform.items, str);
 strcat(inform.items, "\n");
 contItem++;
 sprintf(numItem, "%d", contItem);
 strcat(numItem, ". ");
 }

 }else if (_find(buf,"<author>")!=-1) {
 p1=_find(buf,"<author>");
 p2=_find(buf,"</author>");
 char* str;
 str=substring(buf,p1+8,(p2-1)-(p1+7));
 strcpy(inform.autorRss, str);
 strcat(inform.autorRss, "\n");
 }else if (_find(buf,"<link>")!=-1) {
 p1=_find(buf,"<link>");
 p2=_find(buf,"</link>");
 char* str;
 str=substring(buf,p1+6,(p2-1)-(p1+5));
 strcat(inform.fuentes, &numFuente);
 if(contFuente==1)
 {
 strcpy(inform.fuentes, "1. ");
 }
 strcat(inform.fuentes, str);
 strcat(inform.fuentes, "\n");
 contFuente++;
 sprintf(numFuente, "%d", contFuente);
 strcat(numFuente, ". ");
 }

 }
 return inform;

 }
 */

Noticia* get(list<Noticia*>* _list, int _i){
    list<Noticia*>::iterator it = _list->begin();
    for(int i=0; i<_i; i++){
        ++it;
    }
    return *it;
}
