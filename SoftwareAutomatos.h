#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Estrutura para Alfabeto e Palavra
typedef struct conjunto_char{
	char letra;
	struct conjunto_char *prox;
}CADEIA;

//Estrutura para Regras
typedef struct regras{
	char estado_origem[4];
	char estado_destino[4];
	char letra;
	struct regras *prox;
}REGRA;

//Estrutura para Estados
typedef struct estados{
	char estado[4];
	struct estados *prox;
}ESTADO;

void guarda_char(CADEIA **p, char letra);
void print_char(CADEIA *p);
int vazio_char(CADEIA *p);
int char_existe(CADEIA *p, char letra); //Se a letra exista retorna 1, se nao 0
void ler_palavra(CADEIA **p);
int tam_palavra(CADEIA *p);
int palavra_valida(CADEIA *p, CADEIA *alf); //Retorna 1 se a palavra cont�m algum caracter inexistente ao Alfabeto definido
void imprime_passo(CADEIA *p, char estado[], int posi);
void ler_automato(CADEIA *palavra, REGRA *regra, ESTADO *estados_finais, char estado_inicio[]); //
char* prox_estado(REGRA *p, char estado_origem[], char letra);

int vazio_regra(REGRA *p);
void imprime_regra(REGRA *p);
void enfileira_regra(REGRA **p, char est_origem[], char letra, char est_destino[]);

void enfileira_est(ESTADO **p, char estado[]);
void imprime_est(ESTADO *p);
int vazio_est(ESTADO *p);
int valida_estado(ESTADO *est, char estado[]);
int valida_conjunto_est(ESTADO *est_princ, ESTADO *est_secun);
void filtra_estados(FILE **fp, ESTADO **p);

//Escopo das funcoes
void ler_automato(CADEIA *palavra, REGRA *regra, ESTADO *estados_finais, char estado_inicio[]){
	CADEIA *tmp = palavra;

	int tam = tam_palavra(palavra), i;
	//char palavra_tmp[tam];
	char est_tmp[4];

	printf("Palavra: ");
	print_char(palavra);
	printf("\n");

	for ( i=0; i<tam; i++ ){
		if ( prox_estado(regra,estado_inicio,palavra->letra) != NULL ){
			strcpy(est_tmp,estado_inicio);
			strcpy(estado_inicio,prox_estado(regra,estado_inicio,palavra->letra));

			palavra = palavra->prox;
		}else{
			imprime_passo(tmp,estado_inicio,i);
			printf("\nPALAVRA NAO ACEITA");
			return;
		}
		imprime_passo(tmp,est_tmp,i);
	}

	imprime_passo(tmp,estado_inicio,i);
	if ( valida_estado(estados_finais,estado_inicio) )
		printf("\nPALAVRA ACEITA");
	else
		printf("\nPALAVRA NAO ACEITA");
}

char* prox_estado(REGRA *p, char estado_origem[], char letra){
	while( !vazio_regra(p) ){
		if ( strcmp(estado_origem,p->estado_origem) == 0 && p->letra == letra)
			return p->estado_destino;
		p = p->prox;
	}
	return NULL;
}

//Imprime a transicao atual {Estado atual,letra lida}
void imprime_passo(CADEIA *p, char estado[], int posi){
	int i = 0;

	while( !vazio_char(p) ){
		if ( posi != i )
			printf("%c",p->letra);
		else
			printf("{%s,%c}",estado,p->letra);
		i++;
		p = p->prox;
	}
	if ( posi == i )
		printf("{%s}",estado);
	printf("\n");
}

int palavra_valida(CADEIA *p, CADEIA *alf){
	while ( !vazio_char(p) ){
		if ( !char_existe(alf,p->letra) ){
			return 0;
		}
		p = p->prox;
	}
	return 1;
}
//Retorna o tamanho da cadeia
int tam_palavra(CADEIA *p){
	int cont=0;
	while(!vazio_char(p)){
		cont++;
		p = p->prox;
	}
	return cont;
}

//Usado para leitura de uma palavra do buffer na cadeia
void ler_palavra(CADEIA **p){
	char letra = ' ';

	while (letra != '\n'){
		letra = getchar();
		if (letra != '\n')
			guarda_char(&*p, letra);
	}
}

//Insere o uma letra na cadeia de caracteres
void guarda_char(CADEIA **p, char letra){
	CADEIA *p1 = (CADEIA*) malloc(sizeof(CADEIA)), *p2;
	p1->letra = letra;
	p1->prox = NULL;
	if ( vazio_char(*p) ){
		*p = p1;
	}else{
		p2 = *p;
		while (p2->prox != NULL)
			p2 = p2->prox;
		p2->prox = p1;
	}
}

//imprime os componentes de uma cadeia de caracteres
void print_char(CADEIA *p){
	while( !vazio_char(p) ){
		printf("%c ",p->letra);
		p = p->prox;
	}
}

//verifica se a cadeia de caracteres esta vazia
int vazio_char(CADEIA *p){
	return (p==NULL);
}

int char_existe(CADEIA *p, char letra){
	int cont = 0;
	//O laco percorre toda a fila do tipo ALFABETO
	//verificando a existencia da letra passada pra funcao atual
	//no Conjunto Alfabeto
	while( !vazio_char(p) ){
		if ( p->letra == letra )
			cont++;
		p = p->prox;
	}
	//Caso exista apenas um correspondente e retornado 1
	return ( cont == 1 )?1:0;
}

//Funcoes Estados
void enfileira_est(ESTADO **p, char estado[]){
	ESTADO *p1 = (ESTADO*)malloc(sizeof(ESTADO)), *p2;
	//p1->estado = estado;
	strcpy(p1->estado, estado);
	p1->prox = NULL;

	if ( vazio_est(*p) ){
		*p = p1;
	}else{
		p2 = *p;
		while (p2->prox != NULL)
			p2 = p2->prox;
		p2->prox = p1;
	}
}
int vazio_est(ESTADO *p){
	return(p == NULL)?1:0;
}

void imprime_est(ESTADO *p){
	while( !vazio_est(p) ){
		printf("%s ",p->estado);
		p = p->prox;
	}
}

int tam_est(char estado[]){
	int i = 0;
	while(estado[i] != '\0')
		i++;
	return i;
}

void filtra_estados(FILE **fp, ESTADO **p){
	char letra = ' ';
	while ( letra != '}' ){
		fscanf(*fp, "%c", &letra);
		if ( letra == 'q' ){
			int cont_est = 0;
			char estado_1[4] = {};
			estado_1[cont_est] = letra;

			while ( letra != ',' && letra != '}' ){
				fscanf(*fp, "%c", &letra);
				if ( letra >= '0' && letra <= '9' || letra == 'f' ){
					cont_est++;
					estado_1[cont_est] = letra;
				}
			}
			if ( tam_est(estado_1) > 1 )
				enfileira_est(&*p,estado_1);
		}
	}
}
//
int valida_conjunto_est(ESTADO *est_princ, ESTADO *est_secun){
	while ( !vazio_est(est_secun) ){
		if ( !valida_estado(est_princ,est_secun->estado) ){
			printf("Estado \'%s' Invalido", est_secun->estado);
			return 0;
		}
		est_secun = est_secun->prox;
	}
	return 1;
}
//
int valida_estado(ESTADO *est, char estado[]){
	int cont = 0;

	while ( !vazio_est(est) ){
		if ( strcmp(est->estado, estado) == 0 )
			cont++;
		est = est->prox;
	}
	return (cont == 1)?1:0;
}

//Funcoes Regras
void enfileira_regra(REGRA **p, char est_origem[], char letra, char est_destino[]){
	REGRA *p1 = (REGRA*)malloc(sizeof(REGRA)), *p2;

	strcpy(p1->estado_origem, est_origem);
	strcpy(p1->estado_destino, est_destino);
	p1->letra = letra;
	p1->prox = NULL;

	if ( vazio_regra(*p) ){
		*p = p1;
	}else{
		p2 = *p;
		while (p2->prox != NULL)
			p2 = p2->prox;
		p2->prox = p1;
	}
}
//Funcao que verifica se o ponteiro da regra passada e nulo
//Usado na funcao imprime_regra
int vazio_regra(REGRA *p){
	return(p == NULL)?1:0;
}
//Funcao que imprime as regras de transicao do automato
void imprime_regra(REGRA *p){
	while( !vazio_regra(p) ){
		printf("%s, %c, %s\n",p->estado_origem, p->letra, p->estado_destino);
		p = p->prox;
	}
}
