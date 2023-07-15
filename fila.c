#include "fila.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

extern FILE *arquivoLog;

struct noProcesso {
	int idProcesso;
	int tamanhoProcesso; /*Pela Especificação pode ser de 2Kb,3Kb,4Kb ... ou 8Kb*/
	int quantidadeExecIo;
	int quantidadeExecIoLidas; /*Auxiliar para Contarmos quantas Ops de Exec/Io 
								já foram inseridas a partir do arquivo de Processos
								Importante apenas na Inicialização*/
	TipoInstrucao *tipoInstrucao;
	
	struct timeval tempoInicialProcesso;
	struct timeval tempoFinalProcesso;
	
	NoProcesso *proximo;
};

struct fila {
	NoProcesso *inicio;
	NoProcesso *fim;
};

Fila* criarFila ()
{
	Fila *f = (Fila *) malloc(sizeof(Fila));
	f->inicio = f->fim = NULL;

	return f; 
}

int pegarTamanhoFila (Fila* fila)
{
	int tamanhoFila = 0;
	NoProcesso *ptr = fila->inicio;

	for(;ptr!=NULL;ptr = ptr->proximo)
	{
		tamanhoFila++;
	}

	return tamanhoFila;
}

void inserirFila (Fila *f,
				  int idProcesso,
				  int tamanhoProcesso,
				  int quantidadeExecIo,
				  char *tipoExecIo,
				  int tempoExecIo,
				  bool instrucaoLida,
				  struct timeval tempoInicialInstrucao,
				  struct timeval tempoFinalInstrucao)
{
	//Estratégia.: Implementar um Insert ou Update
	//Se Encontrar o IdProcesso = Insert
	//Caso contrário            = Update
	//Percorrer a Fila definirá o Tipo da Operação
	NoProcesso *ptr = f->inicio;
	bool encontrado = false;
	
	while(ptr!=NULL)
	{
		if(ptr->idProcesso == idProcesso) // Update
		{ 
			encontrado = true;
			
			ptr->quantidadeExecIo = quantidadeExecIo;
		
			if(ptr->tipoInstrucao == NULL) 
			{
				ptr->tipoInstrucao = (TipoInstrucao*) malloc (sizeof(TipoInstrucao)*quantidadeExecIo);	
			}	
				
			if(tipoExecIo[0] != '\0')
			{
				TipoInstrucao instrucaoExecIo;
			 	strcpy(instrucaoExecIo.tipoExecIo,tipoExecIo);
			 	instrucaoExecIo.tempoExecIo = tempoExecIo;
				instrucaoExecIo.instrucaoLida = instrucaoLida;
				instrucaoExecIo.tempoInicialInstrucao = tempoInicialInstrucao;
				instrucaoExecIo.tempoFinalInstrucao = tempoFinalInstrucao;
			 	
			 	ptr->tipoInstrucao[ptr->quantidadeExecIoLidas] = instrucaoExecIo;
			 	
			 	ptr->quantidadeExecIoLidas++;
			}
			break;
		} 
		
		ptr = ptr->proximo;		 	
	}
	
	if(!encontrado)
	{
		NoProcesso *processo = (NoProcesso*) malloc (sizeof(NoProcesso));
		processo->idProcesso = idProcesso;
		processo->tamanhoProcesso = tamanhoProcesso;
		processo->quantidadeExecIo = quantidadeExecIo;
		processo->quantidadeExecIoLidas = 0;

		if(quantidadeExecIo > 0)
		{
			processo->tipoInstrucao = (TipoInstrucao*) malloc (sizeof(TipoInstrucao)*quantidadeExecIo);

			if(tipoExecIo[0] != '\0')
			{
				TipoInstrucao instrucaoExecIo;
				strcpy(instrucaoExecIo.tipoExecIo,tipoExecIo);
				instrucaoExecIo.tempoExecIo = tempoExecIo;
				instrucaoExecIo.instrucaoLida = instrucaoLida;
				instrucaoExecIo.tempoInicialInstrucao = tempoInicialInstrucao;
				instrucaoExecIo.tempoFinalInstrucao = tempoFinalInstrucao;

				processo->tipoInstrucao[processo->quantidadeExecIoLidas] = instrucaoExecIo;
			 	
				processo->quantidadeExecIoLidas++;
			}
		}

		processo->proximo = NULL;
		
		if(f->fim != NULL)
			f->fim->proximo = processo;
		else
			f->inicio = processo;
			
		f->fim = processo;
	}
}

void inserirProcessoFila (Fila *f,NoProcesso *processo)
{
	for(int i = 0;i < processo->quantidadeExecIo; i++)
	{
		inserirFila(f,
				processo->idProcesso,
				processo->tamanhoProcesso,
				processo->quantidadeExecIo,
				processo->tipoInstrucao[i].tipoExecIo,
				processo->tipoInstrucao[i].tempoExecIo,
				processo->tipoInstrucao[i].instrucaoLida,
				processo->tipoInstrucao[i].tempoInicialInstrucao,
				processo->tipoInstrucao[i].tempoFinalInstrucao
				);
	}
}

void imprimirFila(Fila *f)
{
	NoProcesso *ptr = f->inicio;
	
	for(;ptr!=NULL;ptr = ptr->proximo)
	{
		printf("|IdProcesso %d|",ptr->idProcesso);
		printf("|Tamanho Processo %dKb|",ptr->tamanhoProcesso);
		printf("|Quantidade Exec/Io %d|",ptr->quantidadeExecIo);
		
		for(int i = 0;i < ptr->quantidadeExecIoLidas; i++)
		{
			int tempoDecorridoInstrucao;

			tempoDecorridoInstrucao = pegarTempoDecorridoDaInstrucao(ptr->tipoInstrucao[i]);

			printf("[%s:%d de %d:%s]/n",ptr->tipoInstrucao[i].tipoExecIo,
									   ptr->tipoInstrucao[i].instrucaoLida ? tempoDecorridoInstrucao : 0,
								 	   ptr->tipoInstrucao[i].tempoExecIo,
								 	   ptr->tipoInstrucao[i].instrucaoLida ? "Lida":"Não Lida");	
		}

	}
}

void imprimirProcessoArquivoLog (NoProcesso *processo)
{
	for(int i = 0;i < processo->quantidadeExecIoLidas; i++)
	{
		int tempoDecorridoInstrucao;

		tempoDecorridoInstrucao = pegarTempoDecorridoDaInstrucao(processo->tipoInstrucao[i]);
		
		fprintf(arquivoLog," [%s:%d de %d:%s]",processo->tipoInstrucao[i].tipoExecIo,
												 processo->tipoInstrucao[i].instrucaoLida ? tempoDecorridoInstrucao : 0,
												 processo->tipoInstrucao[i].tempoExecIo,
								 				 processo->tipoInstrucao[i].instrucaoLida ? "Lida":"Não Lida");	
	}
	fprintf(arquivoLog,"\n");
}

void imprimirFilaArquivoLog (Fila *f)
{
	NoProcesso *ptr = f->inicio;

	for(;ptr!=NULL;ptr = ptr->proximo)
	{
		fprintf(arquivoLog,"|IdProcesso %d|",ptr->idProcesso);
		fprintf(arquivoLog,"|Tamanho Processo %dKb|",ptr->tamanhoProcesso);
		fprintf(arquivoLog,"|Quantidade Exec/Io %d|",ptr->quantidadeExecIo);
		
		for(int i = 0;i < ptr->quantidadeExecIoLidas; i++)
		{
			int tempoDecorridoInstrucao;

			tempoDecorridoInstrucao = pegarTempoDecorridoDaInstrucao(ptr->tipoInstrucao[i]);

			fprintf(arquivoLog," [%s:%d de %d:%s]",ptr->tipoInstrucao[i].tipoExecIo,
												   ptr->tipoInstrucao[i].instrucaoLida ? tempoDecorridoInstrucao : 0,
												   ptr->tipoInstrucao[i].tempoExecIo,
								 				   ptr->tipoInstrucao[i].instrucaoLida ? "Lida":"Não Lida");	
		}
		fprintf(arquivoLog,"\n");
	}
}

bool filaVazia(Fila *f)
{
	return (f->inicio == NULL);
}

NoProcesso* retirarProcessoDaFila(Fila *f)
{
	if(!filaVazia(f)) //Valida se de Fato tem o que tirar para não usar ponteiros inadequadamente
	{ 
		NoProcesso *retorno = f->inicio;
		f->inicio = retorno->proximo;

		if(f->inicio == NULL)
			f->fim = NULL;

		retorno->proximo = NULL; //Solta da Fila
			
		return retorno;
	}	

	return NULL;
}

int pegarTamanhoProcesso (NoProcesso *processo)
{
	return processo->tamanhoProcesso;
}

int pegarIdProcesso (NoProcesso *processo)
{
	return processo->idProcesso;
}

TipoInstrucao* pegarProximaInstrucaoNaoLida(NoProcesso *processo)
{
	TipoInstrucao *proximaNaoLida = (TipoInstrucao*) malloc (sizeof(TipoInstrucao));;

	strcpy(proximaNaoLida->tipoExecIo,"");
	for(int i=0;i < processo->quantidadeExecIo; i++)
	{
		if(!(processo->tipoInstrucao[i].instrucaoLida) || (processo->tipoInstrucao[i].instrucaoLida && !instrucaoExecIoTerminou(&processo->tipoInstrucao[i])))
		{
			proximaNaoLida = &processo->tipoInstrucao[i];
			strcpy(proximaNaoLida->tipoExecIo,processo->tipoInstrucao[i].tipoExecIo);
			proximaNaoLida->tempoExecIo = processo->tipoInstrucao[i].tempoExecIo;
			proximaNaoLida->instrucaoLida = true;
			break;
		}
	}
	return proximaNaoLida;
}

void iniciarRelogioDoProcesso(NoProcesso *processo)
{
	gettimeofday(&processo->tempoInicialProcesso,NULL);
	gettimeofday(&processo->tempoFinalProcesso,NULL);
}

void iniciarRelogioDaInstrucao(TipoInstrucao *tipoInstrucao)
{
	gettimeofday(&tipoInstrucao->tempoInicialInstrucao,NULL);
	gettimeofday(&tipoInstrucao->tempoFinalInstrucao,NULL);
}

void retomarRelogioDaInstrucao(TipoInstrucao *tipoInstrucao) 
{
	int tempoDecorrido = pegarTempoDecorridoDaInstrucao(*tipoInstrucao);
	iniciarRelogioDaInstrucao(tipoInstrucao);
	sleep(tempoDecorrido); // Correto Seria Usar settimeofDay() mas não consegui dar as permissões do Linux
	tipoInstrucao->tempoFinalInstrucao.tv_sec = tipoInstrucao->tempoFinalInstrucao.tv_sec + tempoDecorrido;
}

void incrementarRelogioDoProcesso(NoProcesso *processo)
{
	gettimeofday(&processo->tempoFinalProcesso,NULL);
}

void incrementarRelogioDaInstrucao(TipoInstrucao *tipoInstrucao)
{
	gettimeofday(&tipoInstrucao->tempoFinalInstrucao,NULL);
}

int pegarTempoDecorridoDaInstrucao(TipoInstrucao tipoInstrucao)
{
	int tempoDecorrido = (tipoInstrucao.tempoFinalInstrucao.tv_sec - tipoInstrucao.tempoInicialInstrucao.tv_sec);

	return tempoDecorrido;
}

bool instrucaoExecIoTerminou(TipoInstrucao *tipoInstrucao)
{
	int tempoDecorrido = (tipoInstrucao->tempoFinalInstrucao.tv_sec - tipoInstrucao->tempoInicialInstrucao.tv_sec);

	if(tempoDecorrido >= tipoInstrucao->tempoExecIo)
		return true;
	else	
		return false;
}

bool atingiuTempoLimite(NoProcesso *processo,int timeSlice)
{
	int tempoDecorrido = (processo->tempoFinalProcesso.tv_sec - processo->tempoInicialProcesso.tv_sec);

	if(tempoDecorrido%timeSlice == 0)
		return true;
	else	
		return false;
}

void atualizarProcessosAguardandoIo(Fila *filaBloqueados,Fila *filaProntos)
{
	NoProcesso *ptr = filaBloqueados->inicio;
	TipoInstrucao *tipoInstrucao;
	NoProcesso *inicioFila;

	for(;ptr!=NULL;ptr = ptr->proximo)
	{
		tipoInstrucao = ptr->tipoInstrucao;

		for(int i = 0;i < ptr->quantidadeExecIo ; i++)
		{
			if(strstr(tipoInstrucao[i].tipoExecIo,"io")
			   && tipoInstrucao[i].instrucaoLida && !instrucaoExecIoTerminou(&tipoInstrucao[i]))
			{
				if((tipoInstrucao[i].tempoFinalInstrucao.tv_sec - tipoInstrucao[i].tempoInicialInstrucao.tv_sec)==0)
				{
					iniciarRelogioDaInstrucao(&tipoInstrucao[i]);
					sleep(1);
				}

				incrementarRelogioDaInstrucao(&tipoInstrucao[i]);

				if(instrucaoExecIoTerminou(&tipoInstrucao[i]))
				{
					inicioFila = retirarProcessoDaFila(filaBloqueados);

					TipoInstrucao* proximaNaoLida = pegarProximaInstrucaoNaoLida(inicioFila);

					if(strstr(proximaNaoLida->tipoExecIo,"exec"))	
					{	
						inserirProcessoFila(filaProntos,inicioFila);
					}
				}
				else // Rodar Elemento -> Tira do Início da Fila e Coloca no Final
				{
					inicioFila = retirarProcessoDaFila(filaBloqueados);
					inserirProcessoFila(filaBloqueados,inicioFila);
				}
			}
		}
	}
}