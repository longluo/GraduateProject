/*
  LPC voice codec
    
*/

#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "lpc.h"


void init_lpc_decoder_state(lpc_decoder_state *st)
{
    int i;
    
    st->Oldper = 0.0f;
    st->OldG = 0.0f;

    for (i = 0; i <= LPC_FILTORDER; i++) {
        st->Oldk[i] = 0.0f;
        st->bp[i] = 0.0f;
    }
    st->pitchctr = 0;
    st->exc = 0.0f;

	st->tri=0;
	st->tri_1=0;
	st->tri_2=0;
}


/* PN random number generator  */
/* returns all numbers from 1 to 65536 */
__inline int PNrandom (void)
{
    static int the_random = 2385;
    
    if((the_random & 1) == 1)
    {
        the_random = ((the_random >> 1) ^ 0xfff6) | 0x8000;
    }
    else
    { 
        the_random >>= 1;
    }
    
    return(the_random);
}


//解压方法:一般是接受本次参数,对上次参数进行合成
int lpc_decode(unsigned char *in,short *out, lpc_decoder_state *st)
{
    int i;
    register float u, f, per, G, NewG, Ginc, Newper, perinc;
    float k[LPC_FILTORDER], Newk[LPC_FILTORDER];
    float bp0, bp1, bp2, bp3, bp4, bp5, bp6, bp7, bp8, bp9, bp10;
    float b, kj;
    
	//接受上次状态参数
    bp0 = st->bp[0];
    bp1 = st->bp[1];
    bp2 = st->bp[2];
    bp3 = st->bp[3];
    bp4 = st->bp[4];
    bp5 = st->bp[5];
    bp6 = st->bp[6];
    bp7 = st->bp[7];
    bp8 = st->bp[8];
    bp9 = st->bp[9];
    bp10 = st->bp[10];
    
    per = (float)(in[0]);
    //per = (float)(per / 2);
    
    G = (float)in[1] / 256.f;
    //k[0] = 0.0;
    for (i = 0; i < LPC_FILTORDER; i++)
		//还原k
        k[i] = (float) ((signed char)in[i + 2]) * 0.0078125f; 
    
    G /= (float)sqrt(BUFLEN / ((per < FLT_EPSILON && per > -FLT_EPSILON)? 3.0f : per));
    Newper = st->Oldper;
    NewG = st->OldG;

    for (i = 0; i < LPC_FILTORDER; i++)
        Newk[i] = st->Oldk[i];
    
	//两桢都为元音,两桢采取周期递增,平滑性好
    if (st->Oldper != 0 && per != 0)
	{
        perinc = (per - st->Oldper) * (1.0f/LPC_SAMPLES_PER_FRAME);
        Ginc = (G - st->OldG) * (1.0f/LPC_SAMPLES_PER_FRAME);
    } 
	else //若有一桢不为元音,则不进行递增
	{
        perinc = 0.0f;
        Ginc = 0.0f;
    }
    
	//上一桢为辅音
    if (Newper == 0.f) 
	{
        st->pitchctr = 0;
		st->tri=0;
		st->tri_1=0;
		st->tri_2=0;
	}
    

	//int tri;//三角波参数
	//int tri_1;
	//int tri_2;

    for (i = 0; i < LPC_SAMPLES_PER_FRAME; i++) 
	{
        if (Newper == 0.f) //假如上一桢是辅音，必有perinc = 0.0f;Ginc = 0.0f;处理上一桢
		{
            u = (((PNrandom() - 32768) / 32768.0f)) * 1.5874f * NewG;//随机产生激励预案,噪声
        } 
		else //上一桢是元音
		{
			//激励源三角波
			if(st->pitchctr==0)
			{
				st->pitchctr=(int) Newper;
				st->tri=0;
				st->tri_1=(int)(3*Newper)/7;
				st->tri_2=(int)(6*Newper)/7;
			}
			if(st->tri<st->tri_1)
			{
				u=(float)(1-cos(M_PI*st->tri/st->tri_1)/2)*NewG;
				st->pitchctr--;
				st->tri++;
			}
			else
			{
				if(st->tri<st->tri_2)
				{
					u=(float)(cos(M_PI*(st->tri-st->tri_1)/(2*st->tri_2)))*NewG;
					st->pitchctr--;
					st->tri++;
				}
				else
				{
					u=0;
					st->pitchctr--;
					st->tri++;
				}
			}
			//三角波部分结束

        }
        

		//下面是解压过程,极点模型s(n)=G*u+(预测部分);begin
        f = u;
        
        b = bp9;
        kj = Newk[9];
        f -= kj * bp9;
        bp10 = bp9 + kj * f;
        
        kj = Newk[8];
        f -= kj * bp8;
        bp9 = bp8 + kj * f;
        
        kj = Newk[7];
        f -= kj * bp7;
        bp8 = bp7 + kj * f;
        
        kj = Newk[6];
        f -= kj * bp6;
        bp7 = bp6 + kj * f;
        
        kj = Newk[5];
        f -= kj * bp5;
        bp6 = bp5 + kj * f;
        
        kj = Newk[4];
        f -= kj * bp4;
        bp5 = bp4 + kj * f;
        
        kj = Newk[3];
        f -= kj * bp3;
        bp4 = bp3 + kj * f;
        
        kj = Newk[2];
        f -= kj * bp2;
        bp3 = bp2 + kj * f;
        
        kj = Newk[1];
        f -= kj * bp1;
        bp2 = bp1 + kj * f;
        
        kj = Newk[0];
        f -= kj * bp0;
        bp1 = bp0 + kj * f;
        
        bp0 = f;
        
        u = f;
        if (u  < -0.9999f)
		{
            u = -0.9999f;
        } 
		else if (u > 0.9999f) 
		{
            u = 0.9999f;
        }
        *out++ = (short)(u * 32767.0f);
		//解压过程结束 end
        Newper += perinc;
        NewG += Ginc;
    }
    st->bp[0] = bp0;
    st->bp[1] = bp1;
    st->bp[2] = bp2;
    st->bp[3] = bp3;
    st->bp[4] = bp4;
    st->bp[5] = bp5;
    st->bp[6] = bp6;
    st->bp[7] = bp7;
    st->bp[8] = bp8;
    st->bp[9] = bp9;
    st->bp[10] = bp10;
    
    st->Oldper = per;
    st->OldG = G;
    for (i = 0; i < LPC_FILTORDER; i++)
        st->Oldk[i] = k[i];
    
    return 1;
}


void destroy_lpc_decoder_state(lpc_decoder_state *st)
{
    if(st != NULL)
    {
        free(st);
        st = NULL;
    }
}

