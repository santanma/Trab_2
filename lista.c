#include "lista.h"

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#define TAMANHO_CPU 16
#define TIME_SLICE 4

extern FILE *arquivoLog;

struct lista
{
	NoMemoria *inicio;
	int tamanhoCPU;
};

struct noMemoria
{
	int enderecoInicial;
	int tamanhoParticao;
	bool preenchido;
	
	NoProcesso *processo; /*Informações do Processo em Execução*/		
	NoMemoria *proximo;
};

Lista* criarListaCPU(int tamanhoCPU)
{
	Lista *listaCPU = (Lista *) malloc(sizeof(Lista));
	NoMemoria *memoria = (NoMemoria *) malloc (sizeof(NoMemoria));

	memoria->enderecoInicial = 0;
	memoria->tamanhoParticao = tamanhoCPU; /**Maximo Implementado sem as Partições conforme permitido no enunciado*/

	memoria->preenchido = false;

	listaCPU->inicio = memoria;
	listaCPU->tamanhoCPU = tamanhoCPU;

	return listaCPU;
}

bool listaCPUVazia(Lista *listaCPU)
{
	//*Conceito -> vazia se não houver ninguém na CPU Executando
	NoMemoria *ptr = listaCPU->inicio;

	for(;ptr!=NULL;ptr = ptr->proximo)
	{
		if(ptr->preenchido)
			return false;
	}
	return true;
}

int pegarTamanhoCPU()
{
	return TAMANHO_CPU;
}

int pegarTimeSliceCPU()
{
	return TIME_SLICE;
}

NoMemoria* localizarParticaoExecutandoNaCPU(Lista *listaCPU)
{
	NoMemoria *ptr = listaCPU->inicio;
	for(;ptr!=NULL;ptr = ptr->proximo)
	{
		if(ptr->preenchido) // Pela Especificação Apenas UM Prcesso ocupa a CPU
			return ptr;
	}

 	return NULL;
}

//Considera que, conforme o enunciado, apenas um Processo estará executando
//por vez na CPU
void insereListaCPUOrdenada(Lista *listaCPU,NoMemoria *particaoNova)
{
	NoMemoria *particaoVazia = (NoMemoria *) malloc (sizeof(NoMemoria));
	NoMemoria *particaoCorrente = localizarParticaoExecutandoNaCPU(listaCPU); 

	//Caso 1.: CPU Vazia
	//Caso 2.: CPU Não Vazia + Endereço Inicial do Processo Novo < Endereço Inicial do Processo em Exec
	if(listaCPUVazia(listaCPU) || 
		particaoNova->enderecoInicial <= particaoCorrente->enderecoInicial)
	{
		listaCPU->inicio = particaoNova;

		particaoVazia->enderecoInicial = particaoNova->tamanhoParticao;
		particaoVazia->tamanhoParticao = listaCPU->tamanhoCPU - particaoVazia->enderecoInicial;
		particaoVazia->preenchido = false;
		particaoNova->proximo = particaoVazia;
	}
	else
	{
		particaoVazia->enderecoInicial = 0;
		particaoVazia->tamanhoParticao = particaoCorrente->enderecoInicial + particaoCorrente->tamanhoParticao;
		particaoVazia->preenchido = false;
		particaoVazia->proximo = particaoNova;

		listaCPU->inicio = particaoVazia;

		if(particaoNova->enderecoInicial + particaoNova->tamanhoParticao < listaCPU->tamanhoCPU)
		{
			NoMemoria *particaoVaziaFinal = (NoMemoria *) malloc (sizeof(NoMemoria));
			particaoVaziaFinal->enderecoInicial = particaoNova->enderecoInicial + particaoNova->tamanhoParticao;
			particaoVaziaFinal->tamanhoParticao = listaCPU->tamanhoCPU - particaoVaziaFinal->enderecoInicial;
			particaoVaziaFinal->preenchido = false;

			particaoNova->proximo = particaoVaziaFinal;
		}
	}
}

void imprimirListaCPU(Lista *listaCPU)
{
	NoMemoria *ptr = listaCPU->inicio;
	
	for(;ptr!=NULL;ptr = ptr->proximo)
	{
		if(ptr->preenchido)
			printf("|Preenchido|");
		else
			printf("|Vazio|");

		printf("|Inicio = %d|",ptr->enderecoInicial);
		printf("|Tamanho = %d|",ptr->tamanhoParticao);
		
		if(ptr->preenchido)
			printf("|IdProcesso - %d|\n",pegarIdProcesso(ptr->processo));
		else	
			printf("|IdProcesso - N/A|\n");
	}
}

//Os Algoritmos BestFit,FirstFit e WorstFit consideram que o Processo que está na CPU
//no momento (Caso Exista) ja atingiu seu tempo Limite
//Essa verificação é feita previamente na Etapa Anterior pelo Relógio
NoMemoria* buscarParticaoMemoriaBestFit(Lista *listaCPU,NoProcesso *processo)
{
	NoMemoria *ptr = listaCPU->inicio;
	NoMemoria *menorParticao = NULL;
		
	int menorEspaco = listaCPU->tamanhoCPU + 1; //Força que Qualquer valor ganhe na comparação

	for(;ptr!=NULL;ptr = ptr->proximo)
	{
		if(!(ptr->preenchido)
			&& ptr->tamanhoParticao < menorEspaco
			&& ptr->tamanhoParticao >= pegarTamanhoProcesso(processo))
		{
			menorEspaco = ptr->tamanhoParticao;
			menorParticao = ptr;
		}
	}

	return menorParticao;
}

NoMemoria* buscarParticaoMemoriaFirstFit(Lista *listaCPU,NoProcesso *processo)
{
	NoMemoria *ptr = listaCPU->inicio;
	NoMemoria *primeiraParticao = NULL;

	for(;ptr!=NULL;ptr = ptr->proximo)
	{
		if(!(ptr->preenchido)
			&& ptr->tamanhoParticao >= pegarTamanhoProcesso(processo))
		{
			primeiraParticao = ptr;
			break;
		}
	}

	return primeiraParticao;
}

NoMemoria* buscarParticaoMemoriaWorstFit(Lista *listaCPU,NoProcesso *processo)
{
	NoMemoria *ptr = listaCPU->inicio;
	NoMemoria *piorParticao = NULL;

	int maiorEspaco = 0; //Força que Qualquer valor ganhe na comparação

	for(;ptr!=NULL;ptr = ptr->proximo)
	{
		if(!(ptr->preenchido)
			&& ptr->tamanhoParticao > maiorEspaco
			&& ptr->tamanhoParticao >= pegarTamanhoProcesso(processo))
		{
			maiorEspaco = ptr->tamanhoParticao;
			piorParticao = ptr;
		}
	}

	return piorParticao;
}

void alocarMemoria(Lista *listaCPU,NoProcesso *processo,char tipoAlgoritmo)
{
	printf("Inicio Algoritmo\n");
	imprimirListaCPU(listaCPU);
	
	NoMemoria *particaoEncontrada = NULL;
	
	switch(tipoAlgoritmo)
	{ 
		case 'F':
			fprintf(arquivoLog,"Executando First Fit - IdProcesso Nº%d\n",pegarIdProcesso(processo));
			particaoEncontrada = buscarParticaoMemoriaFirstFit(listaCPU,processo);
			break;
		case 'B':
			fprintf(arquivoLog,"Executando Best Fit - IdProcesso Nº%d\n",pegarIdProcesso(processo));
			particaoEncontrada = buscarParticaoMemoriaBestFit(listaCPU,processo);
			break;
		case 'W':
			fprintf(arquivoLog,"Executando Worst Fit - IdProcesso Nº%d\n",pegarIdProcesso(processo));
			particaoEncontrada = buscarParticaoMemoriaWorstFit(listaCPU,processo);
			break;		
	}
	
	if(particaoEncontrada != NULL)
	{
		particaoEncontrada->tamanhoParticao = pegarTamanhoProcesso(processo);
		particaoEncontrada->preenchido = true;
		particaoEncontrada->processo = processo;

		insereListaCPUOrdenada(listaCPU,particaoEncontrada);
	}

	printf("Final Algoritmo\n");
	imprimirListaCPU(listaCPU);
}
