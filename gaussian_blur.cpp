/*

Algoritmh:

  Gaussian Blur

Task:

  Create an algorithm using SSE or MMX instructions to blur an image by the Gaussian filter (use either
  inline assembler or intrisnsic functions). Try to make the function run faster than the reference
  implementation. Fill your code in the prepared regions below.

Functions:

  CustomInit() is a one-time initialization function called from the constructor of an instance of the CGaussianBlur class.

  CustomDeinit() is a one-time initialization function called from the destructor of an instance of the CGaussianBlur class.

  Blur(const unsigned char *image, int image_w, int image_h, unsigned char *new_image, float user_radius) calculates the
  Gaussian blur of a grayscale image. The image pointer points at the input image and the new_image pointer points at
  the output image. Size of both input and output images is image_w x image_h pixels. The filter works with radius equal
  to the value of user_radius parameter. Both pointers image and new_image are aligned to 16 bytes and memory for the
  output has been allocated.

*/

#include "gaussian_blur.h"
#ifndef _IPA_BENCH_

void CGaussianBlur::CustomInit()
{


	/*******************************************************************/
	/* TO DO: Insert your one-time initialization code here if needed. */
	/*******************************************************************/


}

void CGaussianBlur::CustomDeinit()
{


	/*************************************************************/
	/* TO DO: Insert your one-time clean up code here if needed. */
	/*************************************************************/


}

void CGaussianBlur::Blur(const unsigned char *image, int image_w, int image_h, unsigned char *new_image, float user_radius)
{


	/*******************************************/
	/* TO DO: Insert your optimized code here. */
	/*******************************************/


		float *buffer = new float[(image_h > image_w)? image_h : image_w];
		float weight = 2.45f*user_radius;
		float s, w, tmp;
		float const1 = 1;
		float const2 = 2;
		float fRadius = sqrt(weight*weight + 1);
		int radius = (short)floor(fRadius);
		int x_start = radius + 1;
		int y_start = radius + 1;
		int x_end = image_w - radius;
		int y_end = image_h - radius;

		fRadius -= radius;
		weight = radius*radius + fRadius*(2*radius + 1);
		int loopCount = (x_end - x_start-1) * (y_end - y_start-1);
		int x = x_start;
		int y = y_start - 2*radius -2;
		int yy_start = y;
		int tmpInt;
		int tmpInt2;
		int yPlusRadius = y+radius;
		int yMinusRadius = y-radius+2;
		int sumConst = (y_start - radius - 1)*image_w+x+(int) image;
		float sum = image[(y_start - radius - 1)*image_w+x];
		float dif = -sum;
		float p = 0;

		FILE *f = fopen("blur.indexes", "w+");

		__asm
		{
			//TODO inicializace prvního for cyklu
			//nastavení počtu opakování vnější*vnitřní
			mov ecx, [loopCount]
			movd xmm6, [sum]
			movd xmm7, [dif]
verticalStart:

			mov eax, [yPlusRadius]
			inc eax
			mov [yPlusRadius], eax

			mov eax, [yMinusRadius]
			dec eax
			mov [yMinusRadius], eax

			mov eax, [y]
			inc eax //poslední instrukce vnitřního cyklu
			mov [y], eax

			cmp eax, [y_end] //konec vnitřního cyklu který počítá y
			je VYend

			//p = image[x + (y + radius) *  image_w]
			mov eax, [yPlusRadius]
			mov ebx, [image_w]
			mul ebx
			add eax, [x]

			add eax, image
			xor ebx, ebx
			mov bl, [eax]
			mov [p], ebx
			cvtsi2ss xmm5, [p]
			movd [p], xmm5

			//buffer[y + radius] = p
			mov ebx, [y]
			add ebx, [radius]
			mov eax, 4
			mul ebx
			add eax, buffer
			movd [eax], xmm5


			// sum = sum + dif + fRadius*p
			addss xmm6, xmm7
			movd xmm1, [fRadius]
			mulss xmm1, xmm5
			addss xmm6, xmm1
			movd [sum], xmm6 //DEBUG
		}

		// fprintf(f, "sum: %f\n", dif);

		__asm
		{

			//dif = dif + p
			addss xmm7, xmm5
			movd [dif], xmm7 //DEBUG


			// if (y >= y_start)
			mov eax, [y]
			cmp eax, [y_start]
			// pokud je y menší než y_start
			jl VYelseIf
				//s = 0, w = 0
				xorps xmm0, xmm0
				movd [s], xmm0
				movd [w], xmm0

				//dif += buffer[y - radius] - 2*buffer[y]
				// 2*buffer[y] = edx
				mov eax, [y]
				shl eax, 2
				add eax, buffer
				movd xmm0, [eax]
				mulss xmm0, [const2]

				//buffer[y-radius]
				mov eax, [y]
				sub eax, [radius]
				shl eax, 2
				add eax, buffer
				movd xmm1, [eax]
				subss xmm1, xmm0
				addss xmm7, xmm1

				//sum -= buffer[y - radius - 1]*fRadius;
				mov eax, [y]
				sub eax, [radius]
				sub eax, 1
				shl eax, 2
				add eax, buffer
				movd xmm0, [eax]
				movd xmm1, [fRadius]
				mulss xmm1, xmm0
				subss xmm6, xmm1
				// movd [sum], xmm6 //DEBUG

				//p = (float)(radius - y);
				mov eax, [radius]
				sub eax, [y]
				mov [tmp], eax
				cvtsi2ss xmm5, [tmp]
				movd [tmp], xmm5

				//if (0 < p)
				xorps xmm1, xmm1
				cmpless xmm1, xmm5 //0 menší než p
				movd eax, xmm1 //pokud true tak samé 1
				test eax, eax
				jnz VPg1
				//TODO nějak získat výsledek porovnání
				//jg VPg1 // pokud je p větší než 0


				// p = (float)(y + radius - image_h + 1);
				xorps xmm1, xmm1
				mov eax, [yPlusRadius]
				sub eax, [image_h]
				add eax, 1
				mov [tmp], eax
				cvtsi2ss xmm5, [tmp]

				//if (p > 0)
				cmpless xmm1, xmm5
				movd eax, xmm1 //pokud true tak samé 1
				test eax, eax
				jnz VPg2
				//TODO nějak získat výsledek porovnání
				//jg VPg2

				//new_image[x + y*image_w] = (unsigned char)((sum - s)/(weight - w));
				xorps xmm1, xmm1
				movd [tmp], xmm6
				movd xmm1, [tmp]
				subss xmm1, [s]
				movd [tmp], xmm1 //DEBUG sum - s
				movd xmm0, [weight]
				subss xmm0, [w]
				divss xmm1, xmm0
				movd [tmp], xmm1 //DEBUG (sum-s)/(weight - w)
				cvtss2si eax, xmm1
				xorps xmm2, xmm2
				movd xmm1, eax
				packssdw xmm1, xmm2
				packuswb xmm1, xmm2
				movd [tmp], xmm1

				mov eax, [y]
				mov ebx, [image_w]
				mul ebx
				add eax, [x]
				// mov [tmpInt], eax //DEBUG
				add eax, new_image
				mov ebx, [tmp]
				// mov bl, 0x0FF //DEBUG
				mov [eax], bl
			// }
			// fprintf(f, "%d: %d\n", tmpInt, tmpInt2);
			// __asm{
			dec ecx
			jmp verticalStart
VYend:
			mov ecx, [x_end]
			cmp ecx, [x]
			je verticalEnd

			mov eax, [x]
			inc eax
			mov [x], eax

			mov eax, [sumConst]
			add eax,4
			mov [sumConst], eax

			xor ebx, ebx
			mov bl, [eax]
			/// sum = image[y_start - radius - 1)*image_w +x]

			mov [sum], ebx
			cvtsi2ss xmm6, ebx
			// dif = -sum
			neg ebx
			mov [dif], ebx
			cvtsi2ss xmm7, ebx

			//y = y_start - 2*radius -1
			mov eax, [yy_start]
			mov [y], eax

			add eax, [radius]
			mov [yPlusRadius], eax

			sub eax, [radius]
			sub eax, [radius]
			add eax, 2
			mov [yMinusRadius], eax

			dec ecx
			jmp verticalStart

VYelseIf:	// else if (y + radius >= y_start)
			mov eax, [yPlusRadius]
			cmp eax, [y_start]
			jl VYelse
			//buffer[y]
			mov eax, [y]
			shl eax, 2
			add eax, buffer
			// -2*buffer[y]
			movd xmm0, [eax]
			mulss xmm0, [const2]
			//dif += -2*buffer[y]
			subss xmm7, xmm0


VYelse:
			dec ecx
			jmp verticalStart
			//TODO vyřešit saturace

VPg1: 		// 	if (p > 0)
			//p = p*(p-1)/2 + fRadius*p;
			movd [p], xmm5
			movd xmm1, [p]
			movd xmm0, [const1]
			subss xmm1, xmm0
			mulss xmm1, xmm5
			movd xmm0, [const2]
			divss xmm1, xmm0
			movd [p], xmm1
			movd xmm2, [p] // p*(p-1)/2
			movd xmm1, [fRadius]
			mulss xmm1, xmm5
			addss xmm2, xmm1
			movd [p], xmm2
			movd xmm5, [p]

			//s += buffer[0]*p;
			movd xmm1, [buffer]
			mulss xmm1, xmm5
			movd xmm0, [s]
			addss xmm0, xmm1
			movd [s], xmm0

			//w += p
			movd xmm0, [w]
			addss xmm0, xmm5
			movd [w], xmm0

			dec ecx
			jmp verticalStart

			// p 		xmm5
			// sum		xmm6
			// dif 		xmm7

VPg2:		// if (p > 0)

			//p = p*(p-1)/2 + fRadius*p; //TODO para 3 øádky
			movd [p], xmm5
			movd xmm1, [p]
			movd xmm0, [const1]
			subss xmm1, xmm0
			mulss xmm1, xmm5
			movd xmm0, [const2]
			divss xmm1, xmm0
			movd [p], xmm1
			movd xmm2, [p] // p*(p-1)/2
			movd xmm1, [fRadius]
			mulss xmm1, xmm5
			addss xmm2, xmm1
			movd [p], xmm2
			movd xmm5, [p]


			//s += buffer[image_h - 1]*p;
			mov eax, [image_h]
			mov ebx, 4
			dec eax
			mul ebx
			add eax, buffer
			movd xmm1, [eax]
			mulss xmm1, xmm5
			movd xmm0, [s]
			addss xmm0, xmm1
			movd [s], xmm0

			//w += p
			movd xmm0, [w]
			addss xmm0, xmm5
			movd [w], xmm0
			// }

			dec ecx
			jmp verticalStart

verticalEnd:
		}

	  //  for (x = x_start; x < x_end; ++x)     // vertical blur...
	  //  {
			// sum = image[x + (y_start - radius - 1)*image_w];
			// dif = -sum;

			// for (y = y_start - 2*radius - 1; y < y_end; ++y)
			// {													// inner vertical Radius loop
			// 	p = (float)image[x + (y + radius)*image_w];	// next pixel //TODO paralelnì s následujícím øádkem
			// 	buffer[y + radius] = p;							// buffer pixel
			// 	sum += dif + fRadius*p;							//TODO paralelnì i další øádek
			// 	dif += p;										// accumulate pixel blur



			// 	if (y >= y_start)
			// 	{
			// 		s = 0, w = 0;							// border blur correction
			// 		sum -= buffer[y - radius - 1]*fRadius;		// addition for fraction blur
			// 		dif += buffer[y - radius] - 2*buffer[y];	// sum up differences: +1, -2, +1

			// 		// cut off accumulated blur area of pixel beyond the border
			// 		// assume: added pixel values beyond border = value at border
			// 		p = (float)(radius - y);                   // top part to cut off
			// 		if (p > 0)
			// 		{
			// 			p = p*(p-1)/2 + fRadius*p; //TODO paralenì 3 øádky
			// 			s += buffer[0]*p;
			// 			w += p;
			// 		}
			// 		p = (float)(y + radius - image_h + 1);               // bottom part to cut off //TODO paralelnì
			// 		if (p > 0)
			// 		{
			// 			p = p*(p-1)/2 + fRadius*p; //TODO para 3 øádky
			// 			s += buffer[image_h - 1]*p;
			// 			w += p;
			// 		}
			// 		new_image[x + y*image_w] = (unsigned char)((sum - s)/(weight - w)); // set blurred pixel //TODO
			// 	}
			// 	else if (y + radius >= y_start)
			// 	{
			// 		dif -= 2*buffer[y];
			// 	}
			// } // for y
	  //  } // for x

	  //   for (y = y_start; y < y_end; ++y)     // horizontal blur...
	  //   {
		 //    sum = (float)new_image[x_start - radius - 1 + y*image_w]; //TODO +1
			// dif = -sum;

			// for (x = x_start - 2*radius - 1; x < x_end; ++x)
			// {                                   // inner vertical Radius loop
			// 	p = (float)new_image[x + radius + y*image_w];  // next pixel OTODO +4
			// 	buffer[x + radius] = p;								// buffer pixel
			// 	sum += dif + fRadius*p;
			// 	dif += p; // accumulate pixel blur

			// 	if (x >= x_start)
			// 	{
			// 		s = 0, w = 0;               // boarder blur correction
			// 		sum -= buffer[x - radius - 1]*fRadius; // addition for fraction blur //TODO +2
			// 		dif += buffer[x - radius] - 2*buffer[x];  // sum up differences: +1, -2, +1

			// 		// cut off accumulated blur area of pixel beyond the boarder
			// 		p = (float)(radius - x);                   // left part to cut off
			// 		if (p > 0)
			// 		{
			// 			p = p*(p-1)/2 + fRadius*p;	//TODO +3
			// 			s += buffer[0]*p;
			// 			w += p;
			// 		}
			// 		p = (float)(x + radius - image_w + 1);               // right part to cut off
			// 		if (p > 0)
			// 		{
			// 			p = p*(p-1)/2 + fRadius*p; //TODO +3
			// 			s += buffer[image_w - 1]*p;
			// 			w += p;
			// 		}
			// 		new_image[x + y*image_w] = (unsigned char)((sum - s)/(weight - w)); // set blurred pixel //TODO
	  //       }
			// else if (x + radius >= x_start)
			// {
			// 	dif -= 2*buffer[x];
			// }
	  //     } // x
	  //   } // y

		delete[] buffer;

}

#endif
