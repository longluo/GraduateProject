/*
  LPC Header Files
*/

#ifndef LPC_H
#define LPC_H

#ifdef __cplusplus
extern "C" {
#endif

#define LPC_SAMPLES_PER_FRAME	    160
#define LPC_ENCODED_FRAME_SIZE      12

#define LPC_FILTORDER			10
#define BUFLEN			((LPC_SAMPLES_PER_FRAME * 3) / 2)

typedef struct lpc_e_state {
	float   w_s[BUFLEN], w_y[BUFLEN], w_h[BUFLEN], w_w[BUFLEN];
    float   fa[6], u, u1, yp1, yp2;
    int     vuv;
} lpc_e_state_t;


typedef struct lpc_d_state{
		float Oldper, OldG, Oldk[LPC_FILTORDER], bp[LPC_FILTORDER + 1];
		int pitchctr;
        float exc;
		int tri;
		int tri_1;
		int tri_2;
} lpc_d_state_t;


#define bcopy(a, b, n)	  memmove(b, a, n)

#define M_PI  3.14159265358979323846

#define FS				8000.0f	/* Sampling rate */

#define DOWN			5   	/* Decimation for pitch analyzer */
#define PITCHORDER		4		/* Model order for pitch analyzer */
#define FC				600.0f	/* Pitch analyzer filter cutoff */
#define MINPIT			50.0f	/* Minimum pitch */
#define MAXPIT			300.0f	/* Maximum pitch */

#define MINPER			(int)(FS/(DOWN*MAXPIT)+.5f)		/* Minimum period */
#define MAXPER			(int)(FS/(DOWN*MINPIT)+.5f)		/* Maximum period */

#define WSCALE			1.5863f	/* Energy loss due to windowing */

/*	The LPC coder does truly awful things when driven into clipping.
If you set GAIN_ADJUST to a number less than 1.0, samples will
be scaled by that factor to avoid overdriving the coder.  */

#define GAIN_ADJUST 0.98f

typedef struct lpc_e_state lpc_encoder_state;
typedef struct lpc_d_state lpc_decoder_state;

lpc_encoder_state *create_lpc_encoder_state(void);

void init_lpc_encoder_state(lpc_encoder_state *st);

int  lpc_encode(const short *in, unsigned char *out, lpc_encoder_state *st);

void destroy_lpc_encoder_state(lpc_encoder_state *st);

lpc_decoder_state *create_lpc_decoder_state(void);

void init_lpc_decoder_state(lpc_decoder_state *st);

int  lpc_decode(unsigned char *in, short *out, lpc_decoder_state *st);

void destroy_lpc_decoder_state(lpc_decoder_state *st);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif 

