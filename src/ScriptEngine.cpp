#include "ScriptEngine.h"
#include "AdvancedPlotGUI.h"

#include <TGFileDialog.h>
#include <TGClient.h>
#include <TPython.h>
#include <TROOT.h>
#include <TSystem.h>

#include <TGMsgBox.h>

#include <fstream>
#include <iostream>


// ============================================================================
// Constructor
// ============================================================================
ScriptEngine::ScriptEngine(AdvancedPlotGUI* mainGUI)
    : fMainGUI(mainGUI),
      fCoutStream(nullptr),
      fCerrStream(nullptr),
      fOldCout(nullptr),
      fOldCerr(nullptr)
{
}

// ============================================================================
// Destructor
// ============================================================================
ScriptEngine::~ScriptEngine()
{
    RestoreStreams();
    
    if (fCoutStream) delete fCoutStream;
    if (fCerrStream) delete fCerrStream;
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
    if (code.IsNull() || code.IsWhitespace()) {
        output->AddLine("(script editor is empty)");
        return;
    }

    output->AddLine(">>> Running script...");
    output->ShowBottom();

    if (language == 0) {  // ROOT/C++
        // Write to a temp file and execute as a macro
        TString tmpFile = Form("/tmp/root_script_%d.C", gSystem->GetPid());
        
        // Wrap in a named function so Cling can compile it
        std::ofstream f(tmpFile.Data());
        f << "void _tmpScript_() {\n";
        f << code.Data();
        f << "\n}\n";
        f.close();
        
        // Redirect stdout to capture output
        RedirectStreams();
        
        Int_t err = 0;
        gROOT->ProcessLine(Form(".x %s", tmpFile.Data()), &err);
        
        if (err) {
            // Try without wrapper (some macros have their own function)
            f.open(tmpFile.Data());
            f << code.Data();
            f.close();
            gROOT->ProcessLine(Form(".x %s", tmpFile.Data()), &err);
        }
        
        gSystem->ProcessEvents();
        FlushStreams();
        RestoreStreams();
        
        // Clean up
        gSystem->Unlink(tmpFile.Data());
        
    } else if (language == 1) {  // Python
        RedirectStreams();
        if (!TPython::Exec(code.Data())) {
            output->AddLine("ERROR: Python execution failed");
        }
        FlushStreams();
        RestoreStreams();
    }

    output->AddLine(">>> Done.");
    output->ShowBottom();

    TIter nextCanvas(gROOT->GetListOfCanvases());
    TCanvas* c;
    while ((c = (TCanvas*)nextCanvas())) {
        c->Modified();
        c->Update();
    }
    gSystem->ProcessEvents();
}

// ============================================================================
// Run single command
// ============================================================================
void ScriptEngine::RunCommand(Int_t language)
{
    TGTextEntry* cmdEntry = fMainGUI->GetCommandEntry();
    TGTextView*  output   = fMainGUI->GetScriptOutput();
    if (!cmdEntry || !output) return;

    TString cmd = cmdEntry->GetText();
    cmd.Strip(TString::kBoth);
    if (cmd.IsNull()) return;

    output->AddLine(Form("root [>] %s", cmd.Data()));

    RedirectStreams();

    Int_t err = 0;
    if (language == 0) {
        gROOT->ProcessLine(cmd.Data(), &err);
        if (err) output->AddLine("  *** error above ***");
        gSystem->ProcessEvents();
    } else {
        TPython::Exec(cmd.Data());
    }

    FlushStreams();
    RestoreStreams();

    // Update canvases
    TIter nc(gROOT->GetListOfCanvases());
    TCanvas* c;
    while ((c = (TCanvas*)nc())) { c->Modified(); c->Update(); }
    gSystem->ProcessEvents();

    output->ShowBottom();
    cmdEntry->Clear();
}

// ============================================================================
// Load script from file
// ============================================================================
void ScriptEngine::LoadScript()
{
    const char* filetypes[] = {
        "All script files",  "*.[Cc]*",
        "ROOT macros",       "*.C",
        "C++ source",        "*.cpp",
        "C++ source",        "*.cxx",
        "Python scripts",    "*.py",
        "All files",         "*",
        nullptr, nullptr
    };
    
    TGFileInfo fileInfo;
    fileInfo.fFileTypes = filetypes;
    fileInfo.fIniDir    = StrDup(".");   // open in current directory
    
    new TGFileDialog(gClient->GetRoot(), fMainGUI, kFDOpen, &fileInfo);
    
    if (!fileInfo.fFilename) return;   // user cancelled
    
    std::ifstream file(fileInfo.fFilename);
    if (!file.is_open()) {
        new TGMsgBox(gClient->GetRoot(), fMainGUI,
            "Error", Form("Cannot open: %s", fileInfo.fFilename),
            kMBIconStop, kMBOk);
        return;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();
    
    TGTextEdit* editor = fMainGUI->GetScriptEditor();
    if (editor) {
        editor->Clear();
        editor->LoadBuffer(content.c_str());
    }
    
    // Auto-select language from extension
    TString fname(fileInfo.fFilename);
    TGTextView* output = fMainGUI->GetScriptOutput();
    if (fname.EndsWith(".py")) {
        // fMainGUI->GetScriptLangCombo()->Select(1);  
        // Need to expose the combo box or add a SetLanguage() method
        if (output) output->AddLine(">>> Python script loaded. Select 'Python' in language dropdown.");
    } else {
        if (output) output->AddLine(Form(">>> Script loaded: %s", 
            gSystem->BaseName(fileInfo.fFilename)));
    }
    output->ShowBottom();
}

void ScriptEngine::SaveScript()
{
    const char* filetypes[] = {
        "ROOT macros",       "*.C",
        "Python scripts",    "*.py",
        "C++ source",        "*.cpp",
        "All files",         "*",
        nullptr, nullptr
    };
    
    TGFileInfo fileInfo;
    fileInfo.fFileTypes = filetypes;
    fileInfo.fIniDir    = StrDup(".");
    
    new TGFileDialog(gClient->GetRoot(), fMainGUI, kFDSave, &fileInfo);
    
    if (!fileInfo.fFilename) return;
    
    TGTextEdit* editor = fMainGUI->GetScriptEditor();
    if (!editor) return;
    
    std::ofstream file(fileInfo.fFilename);
    if (!file.is_open()) {
        new TGMsgBox(gClient->GetRoot(), fMainGUI,
            "Error", Form("Cannot write: %s", fileInfo.fFilename),
            kMBIconStop, kMBOk);
        return;
    }
    
    TString content = editor->GetText()->AsString();
    file << content.Data();
    file.close();
    
    TGTextView* output = fMainGUI->GetScriptOutput();
    if (output) {
        output->AddLine(Form(">>> Saved to: %s", fileInfo.fFilename));
        output->ShowBottom();
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
    if (!fCoutStream) fCoutStream = new std::ostringstream();
    if (!fCerrStream) fCerrStream = new std::ostringstream();
    
    // Reset
    fCoutStream->str("");
    fCerrStream->str("");
    
    fOldCout = std::cout.rdbuf(fCoutStream->rdbuf());
    fOldCerr = std::cerr.rdbuf(fCerrStream->rdbuf());
}

// ============================================================================
// Restore standard output streams
// ============================================================================
void ScriptEngine::RestoreStreams()
{
    if (fOldCout) std::cout.rdbuf(fOldCout);
    if (fOldCerr) std::cerr.rdbuf(fOldCerr);
}

// ============================================================================
// Flush captured output to GUI
// ============================================================================
void ScriptEngine::FlushStreams()
{
    std::cout.flush();
    std::cerr.flush();
    
    TGTextView* output = fMainGUI->GetScriptOutput();
    if (!output) return;
    
    if (fCoutStream) {
        std::string all = fCoutStream->str();
        std::istringstream iss(all);
        std::string line;
        while (std::getline(iss, line)) {
            if (!line.empty()) output->AddLine(line.c_str());
        }
        fCoutStream->str("");
    }
    if (fCerrStream) {
        std::string all = fCerrStream->str();
        std::istringstream iss(all);
        std::string line;
        while (std::getline(iss, line)) {
            if (!line.empty()) output->AddLine(Form("ERR: %s", line.c_str()));
        }
        fCerrStream->str("");
    }
}