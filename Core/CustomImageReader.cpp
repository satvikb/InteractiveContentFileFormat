#include "CustomImageReader.h"

bool CustomImageReader::DoCanRead(wxInputStream& stream) {
	return true; // TODO
}

// the input steam buffer should start from the first byte in content
bool CustomImageReader::LoadFile(wxImage* image, wxInputStream& stream, bool verbose, int index) {
	if (stream.GetC() != IMAGE_METADATA_START) {
		return false;
	}
	uint16_t width = stream.GetC() << 8 | (unsigned char)stream.GetC();
	uint16_t height = stream.GetC() << 8 | (unsigned char)stream.GetC();
	uint8_t alphaFlag = stream.GetC();

	// skip rest of metadata
	// TODO handle eof
	while (stream.GetC() != IMAGE_METADATA_END) {
		stream.GetC();
	}
	if (stream.GetC() == IMAGE_ACTION_START) {
		// skip action table here
		while (stream.GetC() != IMAGE_ACTION_END) {
			stream.GetC();
		}
	}
	uint8_t* pixelColors = (uint8_t*)malloc(sizeof(uint8_t) * width * height * 3);
	uint8_t* pixelAlphas = (uint8_t*)malloc(sizeof(uint8_t) * width * height);

	int pixel = 0;
	while (stream.CanRead()) {
		// read each pixel
		uint8_t alpha = stream.GetC();
		if (alpha == 0x0) {
			// skip color bytes
			pixelColors[(pixel * 3)] = 0;
			pixelColors[(pixel * 3) + 1] = 0;
			pixelColors[(pixel * 3) + 2] = 0;
			pixelAlphas[pixel] = alpha;
		}
		else {
			pixelColors[(pixel * 3)] = stream.GetC();
			pixelColors[(pixel * 3) + 1] = stream.GetC();
			pixelColors[(pixel * 3) + 2] = stream.GetC();
			pixelAlphas[pixel] = alpha;
		}
		pixel += 1;
	}

	image->Create(width, height, pixelColors, pixelAlphas, false);
	return true;
}