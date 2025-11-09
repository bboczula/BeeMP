#include <iostream>

#include "beemp.h"

int main(int argc, char* argv[])
{
	// Load a BMP file into memory
	auto result = LoadFileToMemory("sample.bmp", true);

	// Read its data
	ReadBmpData();

	// Modify this data a little bit, for example set the first pixel to red
	// Blur it a little, make a grayscale version, change its size, etc.
	for(int i = 0; i < output.size(); i += 3)
	{
		// Simple grayscale conversion
		unsigned char gray = static_cast<unsigned char>(0.299 * output[i] + 0.587 * output[i + 1] + 0.114 * output[i + 2]);
		output[i] = gray;       // R
		output[i + 1] = gray;   // G
		output[i + 2] = gray;   // B
	}

	// Encode image data to a BMP raw buffer
	EncodeBmpData(output, 512, 512, 24);

	// Store the data back to a new BMP file
	StoreBufferToFile("output.bmp", outputBmp, true);

	return 0;
}