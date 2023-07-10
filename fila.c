#include "fila.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>

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

void inserirFila (Fila *f,
				  int idProcesso,
				  int tamanhoProcesso,
				  int quantidadeExecIo,
				  char *tipoExecIo,
				  int tempoExecIo)
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
				ptr->tipoInstrucao = (TipoInstrucao*) malloc 	(sizeof(TipoInstrucao)*quantidadeExecIo);	
			}	
				
			if(tipoExecIo[0] != '\0')
			{
				TipoInstrucao instrucaoExecIo;
			 	strcpy(instrucaoExecIo.tipoExecIo,tipoExecIo);
			 	instrucaoExecIo.tempoExecIo = tempoExecIo;
				instrucaoExecIo.instrucaoLida = false;
			 	
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
		processo->quantidadeExecIoLidas = 0;
		
		processo->proximo = NULL;
		
		if(f->fim != NULL)
			f->fim->proximo = processo;
		else
			f->inicio = processo;
			
		f->fim = processo;
	}
}


void imprimirFila(Fila *f)
{
	NoProcesso *ptr = f->inicio;
	
	for(;ptr!=NULL;ptr = ptr->proximo)
	{
		printf("Id Processo %d - ",ptr->idProcesso);
		printf("Tamanho Processo %d Kb - ",ptr->tamanhoProcesso);
		printf("Quantidade Exec/Io %d",ptr->quantidadeExecIo);
		
		printf("\n{");
		for(int i = 0;i < ptr->quantidadeExecIoLidas; i++)
		{
			printf("[%s:%d:%s] ",ptr->tipoInstrucao->tipoExecIo,
								 ptr->tipoInstrucao[i].tempoExecIo,
								 ptr->tipoInstrucao[i].instrucaoLida ? "Lida":"Não Lida");	
		}
		printf("}\n");
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

TipoInstrucao pegarProximaInstrucaoNaoLida(NoProcesso *processo)
{
	TipoInstrucao proximaNaoLida;

	for(int i=0;i < processo->quantidadeExecIo; i++)
	{
		if(!(processo->tipoInstrucao[i].instrucaoLida))
		{
			proximaNaoLida = processo->tipoInstrucao[i];
			processo->tipoInstrucao[i].instrucaoLida = true;
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

void incrementarRelogioDoProcesso(NoProcesso *processo)
{
	gettimeofday(&processo->tempoFinalProcesso,NULL);
}

bool instrucaoExecIoTerminou(NoProcesso *processo,int tempoExecIo)
{
	int tempoDecorrido = (processo->tempoFinalProcesso.tv_sec - processo->tempoInicialProcesso.tv_sec);

	if(tempoDecorrido%tempoExecIo == 0)
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