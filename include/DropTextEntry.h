#ifndef DROP_TEXT_ENTRY_H
#define DROP_TEXT_ENTRY_H

#include <TGTextEntry.h>
#include "FileHandler.h"

class AdvancedPlotGUI;

/**
 * @class DropTextEntry
 * @brief Enhanced TGTextEntry with drag-and-drop functionality
 * 
 * Provides reliable drag-and-drop support for:
 * - File path dropping (file:// URIs)
 * - ROOT object names
 * - Plain text paths
 * 
 * Features:
 * - Validates file before loading
 * - Properly handles URI encoding (%20 for space, etc.)
 * - Supports multiple file types (ROOT, CSV, TXT)
 * - Error feedback on invalid files
 */
class DropTextEntry : public TGTextEntry {
public:
    DropTextEntry(const TGWindow* p,
                  TGTextBuffer* buffer,
                  AdvancedPlotGUI* gui);
    
    virtual ~DropTextEntry();

    // DND event handler - only method ROOT provides for this
    virtual Bool_t HandleDNDDrop(TDNDData* data);
    
    const char* GetDroppedFilePath() const { return fDroppedPath.Data(); }

private:
    AdvancedPlotGUI* fGUI;
    TString fDroppedPath;
    
    // Helper methods
    TString ExtractFilePath(const char* uriText);
    Bool_t IsValidFile(const char* filepath);
};

#endif