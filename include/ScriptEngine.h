#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <TGTextEdit.h>
#include <TGTextView.h>
#include <TGTextEntry.h>
#include <sstream>

class AdvancedPlotGUI;  // Forward declaration

class ScriptEngine {
private:
    AdvancedPlotGUI* fMainGUI;
    
    // Stream redirection for capturing output
    std::stringstream* fCoutBuf;
    std::stringstream* fCerrBuf;
    std::streambuf* fOldCoutBuf;
    std::streambuf* fOldCerrBuf;
    
    void RedirectStreams();
    void RestoreStreams();
    void FlushStreams();
    
public:
    ScriptEngine(AdvancedPlotGUI* mainGUI);
    virtual ~ScriptEngine();
    
    // Script operations
    void RunScript(Int_t language);
    void RunCommand(Int_t language);
    void LoadScript();
    void SaveScript();
    void ClearEditor();
    void ClearOutput();
    
    ClassDef(ScriptEngine, 0)
};

#endif // SCRIPTENGINE_H
