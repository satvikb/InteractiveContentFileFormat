#include <wx/image.h>
#include "reader.h"

class CustomImageReader : public wxImageHandler {
public:
	const wxString& GetMimeType() const;
	//bool CanRead(wxInputStream& stream) override;
	virtual bool DoCanRead(wxInputStream& stream) override;
	virtual bool LoadFile(wxImage* image, wxInputStream& stream, bool verbose = true, int index = -1) override;
};