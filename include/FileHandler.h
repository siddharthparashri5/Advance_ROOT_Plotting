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

    void Browse();
    void Load();
    void LoadFromDrop(const char* filepath);
    void LoadRootFile(const char* filepath);
    void LoadCSVFile (const char* filepath);

    const ColumnData& GetCurrentData()     const { return fCurrentData;    }
    TFile*            GetCurrentRootFile() const { return fCurrentRootFile; }
    void              SetCurrentData(const ColumnData& data) { fCurrentData = data; }
};

#endif // FILEHANDLER_H