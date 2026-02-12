#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <TGFrame.h>
#include <TGLayout.h>
#include <TFile.h>
#include <TString.h>
#include <TSystem.h>
#include "DataReader.h"

class AdvancedPlotGUI;  // Forward declaration

// ============================================================================
// FileHandler — manages file browsing, loading, and drag-and-drop.
// Plain C++ class (no TObject inheritance, no ClassDef).
// ============================================================================
class FileHandler {
private:
    AdvancedPlotGUI* fMainGUI;
    TFile*           fCurrentRootFile;
    ColumnData       fCurrentData;

public:
    FileHandler(AdvancedPlotGUI* mainGUI);
    ~FileHandler();

    // FIXED: Single Browse() that returns the selected filepath
    std::string Browse();
    
    void Load(const std::string& filepath);
    void LoadFromDrop(const char* filepath);
    void LoadRootFile(const char* filepath);
    void LoadCSVFile(const char* filepath);
    void LoadCSVWithSettings(const char* filepath, char delim, 
                             Int_t skipRows, Bool_t useHeader);

    const ColumnData& GetCurrentData()     const { return fCurrentData;    }
    TFile*            GetCurrentRootFile() const { return fCurrentRootFile; }
    void              SetCurrentData(const ColumnData& data) { fCurrentData = data; }
};

#endif // FILEHANDLER_H