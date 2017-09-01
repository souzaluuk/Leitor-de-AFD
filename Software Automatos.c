#include "SoftwareAutomatos.h"

int main()
{
	FILE *fp;
	CADEIA *alf = NULL, *palavra = NULL;
	ESTADO *est = NULL;
	ESTADO *est_finais = NULL;
	REGRA *regra = NULL;

	char letra = ' ', estado_inicio[4], estado_fim[4], caminho_arq[] = "automato.txt";
	int cont = 0, cont_virg = 0;

	fp = fopen(caminho_arq, "r");
	if ( fp == NULL ){
		printf("Arquivo '%s' nao encontrado\n",caminho_arq);
		system("pause");
		return 0;
	}
	while ( letra != ')' ){
		fscanf(fp, "%c", &letra);

		if ( letra == ',' )
			cont_virg++;

		if ( cont_virg == 3 ){
			fscanf(fp, "%s", estado_inicio);
			estado_inicio[strlen(estado_inicio)-1] = '\0'; //elimina virgula do fim da String

			if ( !valida_estado(est,estado_inicio) ){
				printf("Estado Inicial Invalido!");
				return 0;
			}
			cont_virg++;
		}
		//Conta o simbolo "{" para saber o ponto de "ignicao" para a leitura de um novo conjunto
		if (letra == '{'){
			cont++;


			//Esse switch usa a variavel cont para identificar em que conjunto estamos
			//1 - Alfabeto
			//2 - Estados
			//3 - Conjunto dos estados finais
			switch(cont){
				case 1:
					while ( letra != '}' ){
						fscanf(fp, "%c", &letra);
						if ( letra >= 'a' && letra <= 'z' || letra >= '0' && letra <= '9' ){
							// Se o retorno for 1 significa que ja existe um elemento
							// de mesma letra na estrutura, logo, a letra atual e repetida.
							// Assim o main informa que a letra e repetida e encerra o main,
							//caso contrario, enfileira a letra na estrutura "alf"
							if ( char_existe(alf,letra) ){
								printf("Elemento \'%c' Repetido", letra);
								return 0;
							}
							guarda_char(&alf,letra);
						}
					}
					break;
				case 2:
					filtra_estados(&fp,&est);
					if ( !valida_conjunto_est(est,est) ){
						return 0;
					}
					break;
				case 3:
					filtra_estados(&fp,&est_finais);
					if ( !valida_conjunto_est(est,est_finais) || !valida_conjunto_est(est_finais,est_finais)){
						return 0;
					}
					break;
				default:
					printf("ERRO!");
			}
		}
	}

	char est_origem[4], elemento[3], est_destino[4];
	while( fscanf(fp, "%s %s %s", est_origem, elemento, est_destino) != EOF ){
		//elimina a ',' (virgula) da string
		est_origem[strlen(est_origem)-1] = '\0';
		elemento[strlen(elemento)-1] = '\0';

		if ( char_existe(alf,elemento[0]) && valida_estado(est,est_origem) && valida_estado(est,est_destino) ) {
			enfileira_regra(&regra, est_origem, elemento[0], est_destino);
		}else{
			printf("Regra \'%s, %s, %s' invalida!\n",est_origem, elemento, est_destino);
			printf("\nO estado a letra ou a sintaxe da regra podem estar incorreta!");
			printf("\nReescreva como no exemplo:\n e.g.: \"qx, a, qy\"");
			return 0;
		}
	}



	//Impressão dos componentes:
	printf("Alfabeto: ");
	print_char(alf);
	printf("\nEstados: ");
	imprime_est(est);
	printf("\nEstado Inicial: %s", estado_inicio);
	printf("\nEstados Finais: ");
	imprime_est(est_finais);
	printf("\nRegras: \n");
	imprime_regra(regra);
	fclose(fp);

	printf("\nInforme a palavra: ");
	ler_palavra(&palavra);

	if ( palavra_valida(palavra,alf) )
		ler_automato(palavra,regra,est_finais,estado_inicio);
	else
		printf("PALAVRA NAO ACEITA");
	printf("\n");
	system("pause");
	return 0;
}
