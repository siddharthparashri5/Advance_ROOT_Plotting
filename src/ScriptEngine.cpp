#include "ScriptEngine.h"
#include "AdvancedPlotGUI.h"

#include <TGFileDialog.h>
#include <TGClient.h>
#include <TPython.h>
#include <TROOT.h>
#include <TSystem.h>

#include <fstream>
#include <iostream>

ClassImp(ScriptEngine);

// ============================================================================
// Constructor
// ============================================================================
ScriptEngine::ScriptEngine(AdvancedPlotGUI* mainGUI)
    : fMainGUI(mainGUI),
      fCoutBuf(nullptr),
      fCerrBuf(nullptr),
      fOldCoutBuf(nullptr),
      fOldCerrBuf(nullptr)
{
}

// ============================================================================
// Destructor
// ============================================================================
ScriptEngine::~ScriptEngine()
{
    RestoreStreams();
    
    if (fCoutBuf) delete fCoutBuf;
    if (fCerrBuf) delete fCerrBuf;
}

// ============================================================================
// Run script from editor
// ============================================================================
void ScriptEngine::RunScript(Int_t language)
{
    TGTextEdit* editor = fMainGUI->GetScriptEditor();
    TGTextView* output = fMainGUI->GetScriptOutput();
    
    if (!editor || !output) return;
    
    TString code = editor->GetText()->AsString();

    RedirectStreams();
    output->AddLine(">>> Running script...");

    if (language == 0) {  // ROOT/C++
        gROOT->ProcessLine(code.Data());
        gSystem->ProcessEvents();
    } 
    else if (language == 1) {  // Python
        if (!TPython::Exec(code.Data())) {
            output->AddLine("ERROR: Python execution failed");
        }
    }

    // Capture any new output
    FlushStreams();
    RestoreStreams();

    output->AddLine(">>> Script execution complete.");
    output->ShowBottom();
}

// ============================================================================
// Run single command
// ============================================================================
void ScriptEngine::RunCommand(Int_t language)
{
    TGTextEntry* cmdEntry = fMainGUI->GetCommandEntry();
    TGTextView* output = fMainGUI->GetScriptOutput();
    
    if (!cmdEntry || !output) return;
    
    TString cmd = cmdEntry->GetText();
    if (cmd.IsNull()) return;

    output->AddLine(Form("root [0] %s", cmd.Data()));

    RedirectStreams();

    if (language == 0) {  // ROOT/C++
        gROOT->ProcessLine(cmd.Data());
        gSystem->ProcessEvents();
    } 
    else if (language == 1) {  // Python
        TPython::Exec(cmd.Data());
    }

    FlushStreams();
    RestoreStreams();

    output->ShowBottom();
    cmdEntry->Clear();
}

// ============================================================================
// Load script from file
// ============================================================================
void ScriptEngine::LoadScript()
{
    const char* filetypes[] = {
        "C++ files", "*.C;*.cpp;*.cxx",
        "Python files", "*.py",
        "All files", "*",
        nullptr, nullptr
    };
    
    TGFileInfo fileInfo;
    fileInfo.fFileTypes = filetypes;
    
    new TGFileDialog(gClient->GetRoot(), fMainGUI, kFDOpen, &fileInfo);
    
    if (fileInfo.fFilename) {
        std::ifstream file(fileInfo.fFilename);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
            
            TGTextEdit* editor = fMainGUI->GetScriptEditor();
            if (editor) {
                editor->LoadBuffer(content.c_str());
            }
            file.close();
        }
    }
}

// ============================================================================
// Save script to file
// ============================================================================
void ScriptEngine::SaveScript()
{
    const char* filetypes[] = {
        "C++ files", "*.C;*.cpp;*.cxx",
        "Python files", "*.py",
        "All files", "*",
        nullptr, nullptr
    };
    
    TGFileInfo fileInfo;
    fileInfo.fFileTypes = filetypes;
    
    new TGFileDialog(gClient->GetRoot(), fMainGUI, kFDSave, &fileInfo);
    
    if (fileInfo.fFilename) {
        std::ofstream file(fileInfo.fFilename);
        if (file.is_open()) {
            TGTextEdit* editor = fMainGUI->GetScriptEditor();
            if (editor) {
                file << editor->GetText()->AsString().Data();
            }
            file.close();
        }
    }
}

// ============================================================================
// Clear script editor
// ============================================================================
void ScriptEngine::ClearEditor()
{
    TGTextEdit* editor = fMainGUI->GetScriptEditor();
    if (editor) {
        editor->Clear();
    }
}

// ============================================================================
// Clear output view
// ============================================================================
void ScriptEngine::ClearOutput()
{
    TGTextView* output = fMainGUI->GetScriptOutput();
    if (output) {
        output->Clear();
    }
}

// ============================================================================
// Redirect standard output streams
// ============================================================================
void ScriptEngine::RedirectStreams()
{
    if (!fCoutBuf) fCoutBuf = new std::stringstream();
    if (!fCerrBuf) fCerrBuf = new std::stringstream();

    fOldCoutBuf = std::cout.rdbuf();
    fOldCerrBuf = std::cerr.rdbuf();

    std::cout.rdbuf(fCoutBuf->rdbuf());
    std::cerr.rdbuf(fCerrBuf->rdbuf());
}

// ============================================================================
// Restore standard output streams
// ============================================================================
void ScriptEngine::RestoreStreams()
{
    if (fOldCoutBuf) std::cout.rdbuf(fOldCoutBuf);
    if (fOldCerrBuf) std::cerr.rdbuf(fOldCerrBuf);
}

// ============================================================================
// Flush captured output to GUI
// ============================================================================
void ScriptEngine::FlushStreams()
{
    TGTextView* output = fMainGUI->GetScriptOutput();
    if (!output) return;
    
    if (fCoutBuf) {
        std::string line;
        while (std::getline(*fCoutBuf, line)) {
            output->AddLine(line.c_str());
        }
        fCoutBuf->str("");
        fCoutBuf->clear();
    }
    if (fCerrBuf) {
        std::string line;
        while (std::getline(*fCerrBuf, line)) {
            output->AddLine(Form("ERROR: %s", line.c_str()));
        }
        fCerrBuf->str("");
        fCerrBuf->clear();
    }
}
