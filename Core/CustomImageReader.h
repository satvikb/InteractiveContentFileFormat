#include <wx/image.h>
#include "reader.h"

#define IC_IMAGE_METADATA_START 0x1
#define IC_IMAGE_METADATA_END 0x2
class CustomImageReader : public wxImageHandler {
public:
	const wxString& GetMimeType() const;
	//bool CanRead(wxInputStream& stream) override;
	virtual bool DoCanRead(wxInputStream& stream) override;
	virtual bool LoadFile(wxImage* image, wxInputStream& stream, bool verbose = true, int index = -1) override;
};