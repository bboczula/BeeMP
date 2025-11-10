#include <iostream>

#include "beemp.h"

int main(int argc, char* argv[])
{
	BeeMP::Bitmap bmp;

	// Load a BMP file into memory
	bmp.LoadFileFromMemory("../sample.bmp", true);

	// Read its data
	bmp.ReadBmpData();

	// Modify this data a little bit, for example set the first pixel to red
	// Blur it a little, make a grayscale version, change its size, etc.
	for(int i = 0; i < bmp.Size(); i += 3)
	{
		// Simple grayscale conversion
		unsigned char gray = static_cast<unsigned char>(0.299 * bmp.Get(i) + 0.587 * bmp.Get(i + 1) + 0.114 * bmp.Get(i + 2));
		bmp.Set(i, gray);
		bmp.Set(i + 1, gray);
		bmp.Set(i + 2, gray);
	}

	// Encode image data to a BMP raw buffer
	bmp.EncodeBmpData();

	// Store the data back to a new BMP file
	bmp.StoreBufferToFile("output.bmp");

	return 0;
}