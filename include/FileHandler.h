#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <TGFrame.h>
#include <TFile.h>
#include <TString.h>
#include "DataReader.h"

class AdvancedPlotGUI;  // Forward declaration

class FileHandler {
private:
    AdvancedPlotGUI* fMainGUI;
    TFile* fCurrentRootFile;
    ColumnData fCurrentData;
    
public:
    FileHandler(AdvancedPlotGUI* mainGUI);
    virtual ~FileHandler();
    
    // File operations
    void Browse();
    void Load();
    void LoadFromDrop(const char* filepath);
    void LoadRootFile(const char* filepath);
    void LoadCSVFile(const char* filepath);
    
    // Getters
    const ColumnData& GetCurrentData() const { return fCurrentData; }
    TFile* GetCurrentRootFile() const { return fCurrentRootFile; }
    
    // Setters
    void SetCurrentData(const ColumnData& data) { fCurrentData = data; }
    
    ClassDef(FileHandler, 0)
};

#endif // FILEHANDLER_H
