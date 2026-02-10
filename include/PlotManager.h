#ifndef PLOTMANAGER_H
#define PLOTMANAGER_H

#include <TCanvas.h>
#include <TH1.h>
#include <TObject.h>
#include <vector>
#include "PlotTypes.h"    // defines PlotConfig and PlotCreator
#include "FitUtils.h"
#include "DataReader.h"

class AdvancedPlotGUI;  // Forward declaration

class PlotManager {
private:
    AdvancedPlotGUI* fMainGUI;
    std::vector<PlotConfig> fPlotConfigs;
    
    // Helper methods for different canvas modes
    void CreateDividedCanvas(const std::string& title, FitUtils::FitType fitType, 
                            const std::string& customFunc, const ColumnData& data);
    void CreateOverlayCanvas(const std::string& title, FitUtils::FitType fitType, 
                            const std::string& customFunc, const ColumnData& data);
    void CreateSeparateCanvases(const std::string& title, FitUtils::FitType fitType, 
                               const std::string& customFunc, const ColumnData& data);
    
    void ApplyFit(TObject* obj, FitUtils::FitType type, Int_t color, 
                 const std::string& customFunc);
    void ApplyRooFitGaussian(TH1* hist, Int_t color);
    void PrintCanvasInfo(TCanvas* canvas);
    
public:
    PlotManager(AdvancedPlotGUI* mainGUI);
    virtual ~PlotManager();
    
    // Plot configuration
    void AddPlot(const ColumnData& data);
    void RemovePlot(Int_t index);
    void ClearAll();
    
    // Plotting
    void CreatePlots(const std::string& canvasTitle, Bool_t overlayMode, 
                    Bool_t dividedMode, Int_t nRows, Int_t nCols,
                    FitUtils::FitType fitType, const std::string& customFunc,
                    const ColumnData& data);
    
    // Getters
    const std::vector<PlotConfig>& GetPlotConfigs() const { return fPlotConfigs; }
    size_t GetNumPlots() const { return fPlotConfigs.size(); }
    
    ClassDef(PlotManager, 0)
};

#endif // PLOTMANAGER_H
