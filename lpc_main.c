/**********************************************************************
** LPC Main 
** Designed By Luolong, @15.06,2009
** Copyright (c) LuoLong
** All Rights Reserved!
**********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "evmdm6437.h"
#include "lpc.h"


typedef struct
{
	short BufferIn1[BUFLEN];
	unsigned char BufferOut1[12];
}
LPC_STATE1;

LPC_STATE1 lpcstate1;


typedef struct
{
	unsigned char BufferIn2[12];
	short BufferOut2[LPC_SAMPLES_PER_FRAME];
}
LPC_STATE2;
	
LPC_STATE2 lpcstate2;
	 
lpc_encoder_state *state1;
lpc_decoder_state *state2;


//main 
void main()
{		
	FILE *fp1, *fp2, *fp3, *fp4, *fp5, *fp6;

	long LEN; //文件长度
	int i = 0;
	long length=0;

	unsigned char ss;
	unsigned char temp1;
	unsigned char temp2;
	short temp;

	short *in1;
	unsigned char *out1;

    unsigned char *in2;
	short *out2;

	EVMDM6437_init(); 

	in1 = lpcstate1.BufferIn1;
	out1 = lpcstate1.BufferOut1;

	in2 = lpcstate2.BufferIn2;
	out2 = lpcstate2.BufferOut2;

	init_lpc_encoder_state(state1);
	init_lpc_decoder_state(state2);
	
	fp1 = fopen("c:\\CCStudio_v3.3\\MyProjects\\lpc9\\encode_txt.txt","w");//打开文本文件，写入压缩后的数据,文本形式
	if (fp1 == NULL)
	{
		printf("无法打开编码文本文件!\n");
		exit(0);
	}

	fp2 = fopen("c:\\CCStudio_v3.3\\MyProjects\\lpc9\\encode_bit.txt","wb");//打开文本文件，写入压缩后的数据.二进制形式
	if (fp2 == NULL)
	{
		printf("无法打开编码比特流文件!\n");
		exit(0);
	}

	fp3 = fopen("c:\\CCStudio_v3.3\\MyProjects\\lpc9\\lpc.pcm","rb"); //打开原始语音文件 ,二进制方式 
	if (fp3 == NULL)   
	{   
		printf("无法打开PCM语音文件!\n");    
		exit(0);   
	}
	else   
	{
		printf("*****下面是编码*****\n");
	}

	
	fseek(fp3, 0L, SEEK_END);
	LEN = ftell(fp3);
	fseek(fp3, 0L, SEEK_SET);

	printf("从PCM语音文件取得数据!\n");

    while (length <= LEN)
    {
    	for (i=0; i < BUFLEN; i++)
		{
			temp=0x0000;
			temp1=fgetc(fp3);//低位
			temp2=fgetc(fp3);//高位
			temp|=temp2;
			temp=(temp<<8);
			temp|=temp1;
			length+=2;
			in1[i]=temp;
			printf("%d\n",in1[i]);
		}

		lpc_encode(in1,out1,state1);

		for (i=0; i<12; i++)
		{
			fprintf(fp1,"%u",out1[i]);//写入文本文件
			fprintf(fp1," ");
			fwrite(&out1[i],1,1,fp2);//写入二进制文件
		}
		fprintf(fp1,"\n");
	}

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	
	fp4 = fopen("c:\\CCStudio_v3.3\\MyProjects\\lpc9\\encode_txt.txt","r");//打开编码文件,按文本读入
	if (fp4 == NULL)
	{
		printf("无法打开编码文本文件!\n");
		exit(0);
	}
	
	fp5 = fopen("c:\\CCStudio_v3.3\\MyProjects\\lpc9\\encode_bit.txt","rb");//打开编码文件,按二进制读入
	if(fp5==NULL)
	{
		printf("无法打开编码比特流文件!\n");
		exit(0);
	}
	
	fp6=fopen("c:\\CCStudio_v3.3\\MyProjects\\lpc9\\result_voice.pcm","wb");//输出解压后的语音
	if (fp6 == NULL)
	{ 
		printf("无法打开解码语音文件!\n"); 
		exit(0); 
	} 
	else
	{
		printf("*****下面是解码*****\n");
		while (1)
		{
			for (i=0; i<12; i++)//按二进制读入压缩后的数据
			{
				fread(&in2[i],1,1,fp5);
			}

			if (feof(fp5)) 
			{
				break;
			}
		
			lpc_decode(in2,out2,state2);

			for (i=0; i<LPC_SAMPLES_PER_FRAME; i++)
			{
				ss=((unsigned char)((unsigned short)out2[i]&0x00ff));
				fwrite(&ss,1,1,fp6);
				ss=((unsigned char)((unsigned short)(out2[i])>>8));
				fwrite(&ss,1,1,fp6);
			}			
		}		
	}

	_wait(500000);
	printf("解码完成!\n");
	SW_BREAKPOINT;
	
	return;
}
