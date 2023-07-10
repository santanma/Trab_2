/*Leitura de Arquivos - https://www.tutorialspoint.com/c_standard_library/c_function_fgets.htm 
Splitar uma String em C - https://www.geeksforgeeks.org/how-to-split-a-string-in-cc-python-and-java/
Compartilhar Variáveis pelo Programa - https://www.quora.com/How-do-you-share-a-global-variable-between-C-files*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>

#include "lista.h"

FILE *arquivoLog;

#define MAX_LINHA_ARQUIVO 50

Fila *filaProntos;
Fila *filaBloqueados;
Lista *listaCPU;

void inicializarArquivoLog (char *nomeArquivoLog);
int pegarTamanhoProcessoArquivo(char *linhaArquivo);
void inicializarFilas (char *nomeArquivoProcesso);
void exibirResumoExecucao(NoProcesso *processo,Fila *filaProntos,Fila *filaBloqueados,int tempoDecorrido);
void iniciarProcessamentoCPU();

void inicializarArquivoLog (char *nomeArquivoLog)
{
	arquivoLog = fopen(nomeArquivoLog,"w");
	
	if(arquivoLog == NULL)
	{
		printf("Erro na Abertura do Arquivo de Log para Escrita\n");
		exit(0);
	}
	
	fprintf(arquivoLog,"Arquivo de Log Criado com Sucesso ---> %s\n",nomeArquivoLog);
}

int pegarTamanhoProcessoArquivo(char *linhaArquivo)
{
	char auxLinhaArquivo[MAX_LINHA_ARQUIVO];
	int tamanhoProcessoArquivo;

	strcpy(auxLinhaArquivo,linhaArquivo);

	char *token = strtok(auxLinhaArquivo," ");

	while(token != NULL)
	{
		if(strstr(token,"Kb"))
		{
			char *ultimaStringLinhaProcesso = (char*) malloc (strlen(token)+1);
			memcpy(ultimaStringLinhaProcesso,token,strlen(token)-2); //Subtrai os 2 caracteres de Kb
			tamanhoProcessoArquivo = atoi(ultimaStringLinhaProcesso);
			break;
		}
		token = strtok(NULL," ");
	}
	return tamanhoProcessoArquivo;
}

void inicializarFilas (char *nomeArquivoProcesso)
{
	FILE *arquivoProcessos;
	char linha[MAX_LINHA_ARQUIVO];
	int idLinha;
	int quantidadeProcessos;
	int idProcesso;
	int tamanhoProcesso;
	int quantidadeExecIo;
	char tipoExecIo[5];
	int tempoExecIo;
		
	arquivoProcessos = fopen(nomeArquivoProcesso,"r");
	
	if(arquivoProcessos == NULL)
	{
		printf("Erro na Abertura do Arquivo de Processos para Leitura\n");
		exit(0);
	}
	
	fprintf(arquivoLog,"Iniciando Leitura dos Processos ---> %s\n",nomeArquivoProcesso);
	
	idLinha = 1;
	
	while(fgets(linha,MAX_LINHA_ARQUIVO,arquivoProcessos) != NULL)
	{
		if(idLinha == 1)
		{
			quantidadeProcessos = atoi(linha);
			
			fprintf(arquivoLog,"Quantidade de Processos ---> %d",quantidadeProcessos);
	
			filaProntos = criarFila();
			filaBloqueados = criarFila();	
				
			idLinha++;
		}
		else if(strstr(linha,"Processo"))
		{
			//tamanhoProcesso = (int)(linha[strlen(linha)-4]) - '0';
			tamanhoProcesso = pegarTamanhoProcessoArquivo(linha);

			char *token = strtok(linha," ");
			
			while(token != NULL)
			{
				//Pegar o Id Do Processo de Forma Dinâmica
				if(token[0] == '#') 
				{
					idProcesso = atoi(strtok(token,"#"));
				}			
				
				token = strtok(NULL," ");
			}

			if(tamanhoProcesso > pegarTamanhoCPU())
			{
				fprintf(arquivoLog,"\nERRO.: IdProcesso Nº%d - Tamanho %dKb - Maior que CPU %dKb",
						idProcesso,tamanhoProcesso,pegarTamanhoCPU());
				fprintf(arquivoLog," ***** Processo Inválido");
				continue;
			}
					
			inserirFila(filaProntos,idProcesso,tamanhoProcesso,0,"",0);
			fprintf(arquivoLog,"\nLendo Processo Nº %d - ",idProcesso);
			fprintf(arquivoLog,"Tamanho %d - ",tamanhoProcesso);

			continue;
		}
		else if(strstr(linha,"exec") || strstr(linha,"io"))
		{
			if(tamanhoProcesso > pegarTamanhoCPU())
			{
				continue;
			}

			char *token = strtok(linha," ");
			int index = 1;
			
			//Pegar Exec/Io e seu tempo
			while (token != NULL)
    		{
    			if(index == 1)
    			{
    				strcpy(tipoExecIo,token);
    				index++;	
    			}
    			else
    			{
    				tempoExecIo = atoi(token);    			
    			}
       		 	token = strtok(NULL, " ");
    		}

			inserirFila(filaProntos,idProcesso,tamanhoProcesso,quantidadeExecIo,tipoExecIo,tempoExecIo);	
			fprintf(arquivoLog," [%s:%d:%s]",tipoExecIo,tempoExecIo,"Não Lida");				
		}
		else
		{
			if(tamanhoProcesso > pegarTamanhoCPU())
			{
				continue;
			}

			quantidadeExecIo = atoi(linha);

			inserirFila(filaProntos,idProcesso,tamanhoProcesso,quantidadeExecIo,"",0);	
			fprintf(arquivoLog,"Quantidade Exec/Io %d",quantidadeExecIo);	
		}
	}
	/*Apenas para Manter a Formatação do Arquivo de Log Legível*/
	fprintf(arquivoLog,"\n");
}

void exibirResumoExecucao(NoProcesso *processo,Fila *filaProntos,Fila *filaBloqueados,int tempoDecorrido)
{
	fprintf(arquivoLog,"*******RESUMO EXECUÇÃO*********");
	fprintf(arquivoLog,"Processo na CPU - IdProcesso %d|",pegarIdProcesso(processo));
	//fprintf(arquivoLog,);
}

void iniciarProcessamentoCPU() 
{
	struct timeval tempoInicialRelogio;
	struct timeval tempoFinalRelogio;
	
	int tempoDecorrido;
	
	int tipoAlgoritmo;

	NoProcesso *processo;
	TipoInstrucao proximaNaoLida;

	fprintf(arquivoLog,"Iniciando CPU - Relógio será Disparado\n");
	fprintf(arquivoLog,"Inicializando Lista de Memória da CPU\n");
	
	fprintf(arquivoLog,"Capturando o Algoritmo de Alocação de Memória\n");
	
	printf("Escolha o Algoritmo de Alocação de Memória a ser Adotado:\n");
	printf("F - First Fit\n");
	printf("B - Best Fit\n");
	printf("W - Worst Fit\n");
	
	printf("Escolha.:");
	tipoAlgoritmo = toupper(getchar());

	fprintf(arquivoLog,"Algoritmo de Alocação de Memória Selecionado\n");
	
	gettimeofday(&tempoInicialRelogio,NULL);
	
	listaCPU = criarListaCPU();

	while(!filaVazia(filaProntos) || !filaVazia(filaBloqueados)) //|| !listaCPUVazia())
	{
		if(listaCPUVazia(listaCPU))
		{
			processo = retirarProcessoDaFila(filaProntos);
			proximaNaoLida = pegarProximaInstrucaoNaoLida(processo);			
		
			if(strstr(proximaNaoLida.tipoExecIo,"exec"))
			{
				iniciarRelogioDoProcesso(processo);
				alocarMemoria(listaCPU,processo,tipoAlgoritmo);
			}
		}
		else
		{
			bool terminouInstrucao = instrucaoExecIoTerminou(processo,proximaNaoLida.tempoExecIo);

			if(!terminouInstrucao)
			{
				bool tempoLimite = atingiuTempoLimite(processo,pegarTimeSliceCPU());
			}
			else
			{
				//tirarDaCPU
				//verificarseaindatemoquefazer
				//caso postivo jogarNoFiladeBloqueados
				//caso contrario jogarfora
			}
		}

		gettimeofday(&tempoFinalRelogio,NULL);
	
		tempoDecorrido = (tempoFinalRelogio.tv_sec - tempoInicialRelogio.tv_sec);
	
		if(tempoDecorrido%pegarTimeSliceCPU() == 0 || tempoDecorrido < 1) //Contemplar o Início do Programa
		{
			exibirResumoExecucao(processo,filaProntos,filaBloqueados,tempoDecorrido);
		}
	
		sleep(1);
		incrementarRelogioDoProcesso(processo);
	}		
}

int main ()
{
	inicializarArquivoLog("log_SimuladorMemoria.txt");
	inicializarFilas("Processo.txt");
	imprimirFila(filaProntos);
	iniciarProcessamentoCPU();
}