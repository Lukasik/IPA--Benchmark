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
		int x = x_start;
		int y = y_start - 2*radius -2;
		int yy_start = y_start - 2*radius -2;
		int xx_start = x_start - 2*radius - 2;
		int tmpInt;
		int tmpInt2;
		int yPlusRadius = y+radius;
		int yMinusRadius = y-radius+2;
		int sumConst = (y_start - radius - 1)*image_w+x+(int) image;
		float sum = image[(y_start - radius - 1)*image_w+x];
		float dif = -sum;
		float p = 0;
		float weightMinusW = weight;
		int imageAddress = (int) &(image[(y + radius) * image_w]);
		int newImageAddress = (int) &(new_image[y*image_w]);
		int sourceIndex = (int) &(new_image[x + radius + y*image_w]);
		int destinationIndex = (int) &(new_image[x + y*image_w]);
		void * tmpPointer;
		// FILE *f = fopen("blur.mine", "w+");
		__asm
		{
			movd xmm6, [sum]
			movd xmm7, [dif]
			movd xmm5, [p]
			mov ecx, [radius]
			mov edx, [y]
			mov esi, imageAddress
			mov edi, newImageAddress
			add esi, [x]
			add edi, [x]
verticalStart:

			// inkrementace a podmínka y cyklu
			inc edx //y++
			cmp edx, [y_end] // y < y_end
			je VYend

			// zvýšení ukazatelů na data místo násobení jen přičítám
			add edi, [image_w]
			add esi, [image_w]

			//p = image[x + (y + radius) *  image_w]
			xor eax, eax
			mov al, [esi]
			cvtsi2ss xmm5, eax

			//buffer[y + radius] = p
			mov ebx, edx
			add ebx, ecx
			shl ebx, 2
			add ebx, buffer
			movd [ebx], xmm5


			// sum = sum + dif + fRadius*p
			addss xmm6, xmm7
			movd xmm1, [fRadius]
			mulss xmm1, xmm5
			addss xmm6, xmm1

			//dif = dif + p
			addss xmm7, xmm5

			cmp edx, [y_start]
			// pokud je y menší než y_start
			jl VYelseIf
				//s = 0, w = 0
				xorps xmm4, xmm4
				movd xmm3, [weight]

				//dif += buffer[y - radius] - 2*buffer[y]
				// 2*buffer[y] = edx
				mov ebx, edx
				shl ebx, 2
				add ebx, buffer
				movd xmm0, [ebx]
				mulss xmm0, [const2]

				//buffer[y-radius]
				mov ebx, edx
				sub ebx, ecx
				shl ebx, 2
				add ebx, buffer
				movd xmm1, [ebx]
				subss xmm1, xmm0
				addss xmm7, xmm1

				//sum -= buffer[y - radius - 1]*fRadius;
				mov ebx, edx
				sub ebx, ecx
				dec ebx
				shl ebx, 2
				add ebx, buffer
				movd xmm0, [ebx]
				movd xmm1, [fRadius]
				mulss xmm1, xmm0
				subss xmm6, xmm1

				//p = (float)(radius - y);
				mov ebx, ecx
				sub ebx, edx
				cvtsi2ss xmm5, ebx

				//if (p <= 0) 
				xorps xmm1, xmm1
				cmpless xmm5, xmm1 //0 menší než p
				movd eax, xmm5 //pokud true tak samé 1
				test eax, eax
				jz VPg1
		VPg1next:

				// p = (float)(y + radius - image_h + 1);
				xorps xmm1, xmm1
				mov ebx, edx
				add ebx, ecx
				sub ebx, [image_h]
				inc ebx
				cvtsi2ss xmm5, ebx

				//if (p > 0)
				cmpless xmm5, xmm1
				movd eax, xmm5 //pokud true tak samé 1
				test eax, eax
				jz VPg2
		VPg2next:
				//new_image[x + y*image_w] = (unsigned char)((sum - s)/(weight - w));
				xorps xmm1, xmm1 // vynulování registru
				movaps xmm1, xmm6
				subss xmm1, xmm4
				divss xmm1, xmm3 //(sum-s)/(weight-w)
				cvtss2si eax, xmm1 //eax = int dělení
				xorps xmm2, xmm2 //vynulování
				movd xmm1, eax
				packssdw xmm1, xmm2
				packuswb xmm1, xmm2
				movd eax, xmm1

				//přiřazení
				mov [edi], al


			jmp verticalStart
VYend:
			mov eax, [x]
			inc eax //x++
			cmp [x_end], eax //x < x_end
			je verticalEnd
			mov [x], eax



			// sum = image[y_start - radius - 1)*image_w +x]
			mov esi, imageAddress
			add esi, eax

			mov edi, newImageAddress
			add edi, eax

			mov eax, [y_start]
			sub eax, ecx
			dec eax
			mov ebx, [image_w]
			mul ebx
			add eax, [x]
			add eax, image
			xor ebx, ebx
			mov bl, [eax]
			cvtsi2ss xmm6, ebx
			// dif = -sum
			xorps xmm7, xmm7
			subss xmm7, xmm6

			//y = y_start - 2*radius -1
			mov edx, [yy_start]

			jmp verticalStart

VYelseIf:	// else if (y + radius >= y_start)
			mov eax, edx
			add eax, ecx
			cmp eax, [y_start]
			jl VYelse
			mov eax, edx
			shl eax, 2
			add eax, buffer
			movd xmm0, [eax]

			mulss xmm0, [const2]
			subss xmm7, xmm0
VYelse:
			jmp verticalStart
VPg1: 		// 	if (p > 0)
			//p = p*(p-1)/2 + fRadius*p;
			movaps xmm1, xmm5
			subss xmm1, [const1]
			mulss xmm1, xmm5
			divss xmm1, [const2]

			movd xmm2, [fRadius]
			mulss xmm2, xmm5
			addss xmm5, xmm1

			//s += buffer[0]*p;
			movd xmm1, [buffer]
			mulss xmm1, xmm5
			addss xmm4, xmm1

			subss xmm3, xmm5

			jmp VPg1next

VPg2:		// if (p > 0)

			//p = p*(p-1)/2 + fRadius*p;
			movaps xmm1, xmm5
			subss xmm1, [const1]
			mulss xmm1, xmm5
			divss xmm1, [const2]

			movd xmm2, [fRadius]
			mulss xmm2, xmm5
			addss xmm5, xmm1

			//s += buffer[image_h - 1]*p;
			mov eax, [image_h]
			dec eax
			shl eax, 2
			add eax, buffer
			movd xmm1, [eax]
			mulss xmm1, xmm5
			addss xmm4, xmm1

			subss xmm3, xmm5

			jmp VPg2next

verticalEnd:

			movd xmm6, [sum]
			movd xmm7, [dif]
			movd xmm5, [p]

			mov eax, [y_start]
			mov [y], eax

			mov ebx, [image_w]
			mul ebx
			mov esi, new_image
			add esi, [radius]
			add esi, [xx_start]

			mov edi, new_image
			mov eax, [y]
			mov ebx, [image_w]
			mul ebx
			add eax, [xx_start]
			add edi, eax


			mov ecx, [radius]
			mov edx, [xx_start]

horizontalStart:

			// inkrementace a podmínka y cyklu
			inc edx //y++
			mov [tmpInt], edx
			cmp edx, [x_end] // y < y_end
			je HXend

			// zvýšení ukazatelů na data místo násobení jen přičítám
			inc edi
			inc esi

			//p = image[x + (y + radius) *  image_w]
			xor eax, eax
			mov al, [esi]
			cvtsi2ss xmm5, eax

			//buffer[y + radius] = p
			mov ebx, edx
			add ebx, ecx
			shl ebx, 2
			add ebx, buffer
			movd [ebx], xmm5

			// sum = sum + dif + fRadius*p
			addss xmm6, xmm7
			movd xmm1, [fRadius]
			mulss xmm1, xmm5
			addss xmm6, xmm1

			//dif = dif + p
			addss xmm7, xmm5

			// if (y >= y_start)
			cmp edx, [x_start]
			// pokud je y menší než y_start
			jl HXelseIf
				//s = 0, w = 0
				xorps xmm4, xmm4
				movd xmm3, [weight]

				//dif += buffer[y - radius] - 2*buffer[y]
				// 2*buffer[y] = edx
				mov ebx, edx
				shl ebx, 2
				add ebx, buffer
				movd xmm0, [ebx]
				mulss xmm0, [const2]

				//buffer[y-radius]
				mov ebx, edx
				sub ebx, ecx
				shl ebx, 2
				add ebx, buffer
				movd xmm1, [ebx]
				subss xmm1, xmm0
				addss xmm7, xmm1


				//sum -= buffer[y - radius - 1]*fRadius;
				mov ebx, edx
				sub ebx, ecx
				dec ebx
				shl ebx, 2
				add ebx, buffer
				movd xmm0, [ebx]
				movd xmm1, [fRadius]
				mulss xmm1, xmm0
				subss xmm6, xmm1

				//p = (float)(radius - y);
				mov ebx, ecx
				sub ebx, edx
				cvtsi2ss xmm5, ebx

				//if (p <= 0) 
				xorps xmm1, xmm1
				cmpless xmm5, xmm1 //0 menší než p
				movd eax, xmm5 //pokud true tak samé 1
				test eax, eax
				jz HPg1

				// p = (float)(y + radius - image_h + 1);
				xorps xmm1, xmm1
				mov ebx, edx
				add ebx, ecx
				sub ebx, [image_w]
				inc ebx
				cvtsi2ss xmm5, ebx

				//if (p > 0)
				cmpless xmm5, xmm1
				movd eax, xmm5 //pokud true tak samé 1
				test eax, eax
				jz HPg2

				//new_image[x + y*image_w] = (unsigned char)((sum - s)/(weight - w));
				xorps xmm1, xmm1 // vynulování registru
				movaps xmm1, xmm6
				subss xmm1, xmm4
				divss xmm1, xmm3 //(sum-s)/(weight-w)
				cvtss2si eax, xmm1 //eax = int dělení
				xorps xmm2, xmm2 //vynulování
				movd xmm1, eax
				packssdw xmm1, xmm2
				packuswb xmm1, xmm2
				movd eax, xmm1

				//přiřazení
				mov [edi], al


			jmp horizontalStart
HXend:
			mov eax, [y]
			inc eax //x++
			cmp [y_end], eax //x < x_end
			je horizontalEnd
			mov [y], eax

			//new_image[x + radius + y*image_w];
			mov ebx, [image_w]
			mul ebx
			add eax, [radius]
			add eax, [x_start]
			add eax, new_image

			xor ebx, ebx
			mov bl, [eax]
			cvtsi2ss xmm6, ebx
			// dif = -sum
			xorps xmm7, xmm7
			subss xmm7, xmm6


			//new_image[x + radius + y*image_w]
			mov esi, new_image
			mov eax, [y]
			mov ebx, [image_w]
			mul ebx
			add eax, [radius]
			add eax, [xx_start]
			add esi, eax

			//new_image[xx_start + y*image_w]
			mov eax, [y]
			mov ebx, [image_w]
			mul ebx
			add eax, [xx_start]
			mov edi, image
			add edi, eax
			//y = y_start - 2*radius -1
			mov edx, [xx_start]

			jmp horizontalStart

HXelseIf:	// else if (y + radius >= y_start)
			mov eax, edx
			add eax, ecx
			cmp eax, [x_start]
			jl HXelse
			mov eax, edx
			shl eax, 2
			add eax, buffer
			movd xmm0, [eax]

			mulss xmm0, [const2]
			subss xmm7, xmm0
HXelse:
			jmp horizontalStart
HPg1: 		// 	if (p > 0)
			//p = p*(p-1)/2 + fRadius*p;
			movaps xmm1, xmm5
			subss xmm1, [const1]
			mulss xmm1, xmm5
			divss xmm1, [const2]

			movd xmm2, [fRadius]
			mulss xmm2, xmm5
			addss xmm5, xmm1

			//s += buffer[0]*p;
			movd xmm1, [buffer]
			mulss xmm1, xmm5
			addss xmm4, xmm1

			subss xmm3, xmm5

			jmp horizontalStart

HPg2:		// if (p > 0)

			//p = p*(p-1)/2 + fRadius*p; 
			movaps xmm1, xmm5
			subss xmm1, [const1]
			mulss xmm1, xmm5
			divss xmm1, [const2]

			movd xmm2, [fRadius]
			mulss xmm2, xmm5
			addss xmm5, xmm1

			//s += buffer[image_h - 1]*p;
			mov eax, [image_h]
			dec eax
			shl eax, 2
			add eax, buffer
			movd xmm1, [eax]
			mulss xmm1, xmm5
			addss xmm4, xmm1

			subss xmm3, xmm5

			jmp horizontalStart

horizontalEnd:

		}

		delete[] buffer;

}

#endif
