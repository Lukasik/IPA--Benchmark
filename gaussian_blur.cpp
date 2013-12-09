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


		float weight = 2.45f*user_radius;
		float fRadius = sqrt(weight*weight + 1);
		int radius = (int)floor(fRadius); // integer blur radius
		int x_start = radius + 1;
		int y_start = radius + 1;
		int x_end = image_w - radius;
		int y_end = image_h - radius;
		float *buffer = new float[(image_h > image_w)? image_h : image_w];
		float sum, dif, p, s, w;
		int x, y;
		int lup;

		fRadius -= radius;                      // fraction of radius
		weight = radius*radius + fRadius*(2*radius + 1);//TODO paralelnì

		lup = x_end - x_start + y_end - y_start;

		__asm
		{
			//TODO
			//sum = image[x + (y_start - radius - 1)*image_w];
			//dif = -sum;
			//x = x_start
			//y = y_start
			mov ecx, [lup] //nastavení počtu opakování vnější * vnitřní
verticalStart: nop

			//TODO
			// p = (float)image[x + (y + radius)*image_w];	// next pixel //TODO paralelnì s následujícím øádkem
			// buffer[y + radius] = p;							// buffer pixel
			// sum += dif + fRadius*p;							//TODO paralelnì i další øádek
			// dif += p;
			cmp [y], [y_start]
			jl VYelseIf // pokud je y menší než y_start
			// if (y >= y_start)
			// {
			// 	s = 0, w = 0;							// border blur correction
			mov [s], 0
			mov [w], 0
			// 	sum -= buffer[y - radius - 1]*fRadius;		// addition for fraction blur
			// 	dif += buffer[y - radius] - 2*buffer[y];	// sum up differences: +1, -2, +1

			// 	// cut off accumulated blur area of pixel beyond the border
			// 	// assume: added pixel values beyond border = value at border
			// 	p = (float)(radius - y);                   // top part to cut off
			cmp [p], 0
			jg VPg1 // pokud je p větší než 0
			/* 
			if (p > 0)
			{
				p = p*(p-1)/2 + fRadius*p; //TODO paralenì 3 øádky
				s += buffer[0]*p;
				w += p;
			} 
			*/
			// 	p = (float)(y + radius - image_h + 1);               // bottom part to cut off //TODO paralelnì
			cmp [p], 0
			jg VPg2
			/*
			if (p > 0)
			{
				p = p*(p-1)/2 + fRadius*p; //TODO para 3 øádky
				s += buffer[image_h - 1]*p;
				w += p;
			}
			*/
			// 	new_image[x + y*image_w] = (unsigned char)((sum - s)/(weight - w)); // set blurred pixel //TODO
			// }
			/*
			else if (y + radius >= y_start)
			{
				dif -= 2*buffer[y];
			}
			*/
			inc [y] //poslední instrukce vnitřního cyklu
			cmp [y], [y_end] //konec vnitřního cyklu který počítá y
			je VYend
			loop verticalStart
			jmp verticalEnd
VYend:
			//TODO
			// pokud se rovnají nastavit y na y_start a inkrementovat x
			mov [y], [y_start]
			inc [x]
			//TODO
			//sum = image[x + (y_start - radius - 1)*image_w];
			//dif = -sum;
			dec ecx
			jmp verticalStart

VYelseIf:	//TODO možná bude potřeba kontrolovat aby se to nevykonávlao i při else
			// else if (y + radius >= y_start)
			// {
			// 	dif -= 2*buffer[y];
			// }
			dec ecx
			jmp verticalStart

VPg1: 
			//TODO
			// 	if (p > 0)
			// 	{
			// 		p = p*(p-1)/2 + fRadius*p; //TODO paralenì 3 øádky
			// 		s += buffer[0]*p;
			// 		w += p;
			// 	}
			dec ecx
			jmp verticalStart

VPg2:
			//TODO
			// if (p > 0)
			// {
			// 	p = p*(p-1)/2 + fRadius*p; //TODO para 3 øádky
			// 	s += buffer[image_h - 1]*p;
			// 	w += p;
			// }
			dec ecx
			jmp verticalStart

verticalEnd:
		}

	    for (x = x_start; x < x_end; ++x)     // vertical blur...
	    {
			sum = image[x + (y_start - radius - 1)*image_w];
			dif = -sum;

			for (y = y_start - 2*radius - 1; y < y_end; ++y)
			{													// inner vertical Radius loop			
				p = (float)image[x + (y + radius)*image_w];	// next pixel //TODO paralelnì s následujícím øádkem
				buffer[y + radius] = p;							// buffer pixel
				sum += dif + fRadius*p;							//TODO paralelnì i další øádek
				dif += p;										// accumulate pixel blur

				

				if (y >= y_start)
				{
					s = 0, w = 0;							// border blur correction
					sum -= buffer[y - radius - 1]*fRadius;		// addition for fraction blur
					dif += buffer[y - radius] - 2*buffer[y];	// sum up differences: +1, -2, +1

					// cut off accumulated blur area of pixel beyond the border
					// assume: added pixel values beyond border = value at border
					p = (float)(radius - y);                   // top part to cut off
					if (p > 0)
					{
						p = p*(p-1)/2 + fRadius*p; //TODO paralenì 3 øádky
						s += buffer[0]*p;
						w += p;
					}
					p = (float)(y + radius - image_h + 1);               // bottom part to cut off //TODO paralelnì
					if (p > 0)
					{
						p = p*(p-1)/2 + fRadius*p; //TODO para 3 øádky
						s += buffer[image_h - 1]*p;
						w += p;
					}
					new_image[x + y*image_w] = (unsigned char)((sum - s)/(weight - w)); // set blurred pixel //TODO
				}
				else if (y + radius >= y_start)
				{
					dif -= 2*buffer[y];
				}
			} // for y
	    } // for x

	    for (y = y_start; y < y_end; ++y)     // horizontal blur...
	    {
		    sum = (float)new_image[x_start - radius - 1 + y*image_w]; //TODO +1
			dif = -sum;

			for (x = x_start - 2*radius - 1; x < x_end; ++x)
			{                                   // inner vertical Radius loop
				p = (float)new_image[x + radius + y*image_w];  // next pixel OTODO +4
				buffer[x + radius] = p;								// buffer pixel
				sum += dif + fRadius*p;
				dif += p; // accumulate pixel blur

				if (x >= x_start)
				{
					s = 0, w = 0;               // boarder blur correction
					sum -= buffer[x - radius - 1]*fRadius; // addition for fraction blur //TODO +2
					dif += buffer[x - radius] - 2*buffer[x];  // sum up differences: +1, -2, +1

					// cut off accumulated blur area of pixel beyond the boarder
					p = (float)(radius - x);                   // left part to cut off
					if (p > 0)
					{
						p = p*(p-1)/2 + fRadius*p;	//TODO +3
						s += buffer[0]*p;
						w += p;
					}
					p = (float)(x + radius - image_w + 1);               // right part to cut off
					if (p > 0)
					{
						p = p*(p-1)/2 + fRadius*p; //TODO +3
						s += buffer[image_w - 1]*p;
						w += p;
					}
					new_image[x + y*image_w] = (unsigned char)((sum - s)/(weight - w)); // set blurred pixel //TODO
	        }
			else if (x + radius >= x_start)
			{
				dif -= 2*buffer[x];
			}
	      } // x
	    } // y	

		delete[] buffer;

}

#endif