#include "DropTextEntry.h"
#include "AdvancedPlotGUI.h"
#include "FileHandler.h"

#include <TGClient.h>
#include <TString.h>
#include <TGDNDManager.h>
#include <TBufferFile.h>
#include <TVirtualX.h>
#include <TSystem.h>

#include <cstring>

// ============================================================================
// Constructor
// ============================================================================
DropTextEntry::DropTextEntry(const TGWindow* p,
                             TGTextBuffer* buffer,
                             AdvancedPlotGUI* gui)
    : TGTextEntry(p, buffer),
      fGUI(gui),
      fDNDRegistered(kFALSE),
      fAtomUri(0),
      fAtomText(0),
      fAtomRoot(0)
{
    SetToolTipText("← DROP FILE HERE (ROOT / CSV / TXT)");
    SetDNDTarget(kTRUE);
    
    // REMOVED: Don't call SetDNDAware here - window not yet mapped!
    // Will be done in DoRedraw() after window is realized
}

// ============================================================================
// Destructor
// ============================================================================
DropTextEntry::~DropTextEntry()
{
}

// ============================================================================
// ADDED: Override DoRedraw to lazily register DND after window is mapped
// ============================================================================
void DropTextEntry::DoRedraw()
{
    TGTextEntry::DoRedraw();
    
    // Register DND only once, after window ID is valid
    if (!fDNDRegistered && GetId() != 0) {
        // Cache atom values
        fAtomUri  = gVirtualX->InternAtom("text/uri-list",    kFALSE);
        fAtomText = gVirtualX->InternAtom("text/plain",       kFALSE);
        fAtomRoot = gVirtualX->InternAtom("application/root", kFALSE);
        
        // Register DND types
        Atom_t dndTypes[] = { fAtomUri, fAtomText, 0 };
        gVirtualX->SetDNDAware(GetId(), dndTypes);
        
        fDNDRegistered = kTRUE;
    }
}

// ============================================================================
// Extract file path from URI-list format
// Handles file:// URIs with percent-encoded characters
// ============================================================================
TString DropTextEntry::ExtractFilePath(const char* uriText)
{
    if (!uriText) return TString("");
    
    TString uri(uriText);
    
    // Handle file:// URIs
    if (uri.BeginsWith("file://")) {
        // Remove "file://" prefix
        uri.Remove(0, 7);
        
        // Handle percent-encoded characters (e.g., %20 for space)
        uri.ReplaceAll("%20", " ");
        uri.ReplaceAll("%21", "!");
        uri.ReplaceAll("%23", "#");
        uri.ReplaceAll("%24", "$");
        uri.ReplaceAll("%25", "%");
        uri.ReplaceAll("%26", "&");
        uri.ReplaceAll("%27", "'");
        uri.ReplaceAll("%28", "(");
        uri.ReplaceAll("%29", ")");
        uri.ReplaceAll("%2B", "+");
        uri.ReplaceAll("%2C", ",");
        uri.ReplaceAll("%3A", ":");
        uri.ReplaceAll("%3B", ";");
        uri.ReplaceAll("%3D", "=");
        uri.ReplaceAll("%3F", "?");
        uri.ReplaceAll("%40", "@");
        uri.ReplaceAll("%5B", "[");
        uri.ReplaceAll("%5D", "]");
        uri.ReplaceAll("%7B", "{");
        uri.ReplaceAll("%7D", "}");
        
        // Remove leading newlines/whitespace
        uri = uri.Strip(TString::kLeading, '\n');
        uri = uri.Strip(TString::kLeading);
        
        // Handle the case of multiple URIs (text/uri-list can have multiple lines)
        Int_t newlinePos = uri.First('\n');
        if (newlinePos > 0) {
            uri = uri(0, newlinePos);
        }
        
        return uri;
    }
    
    // If it's already a plain path
    return uri;
}

// ============================================================================
// Validate file existence and accessibility
// ============================================================================
Bool_t DropTextEntry::IsValidFile(const char* filepath)
{
    if (!filepath || strlen(filepath) == 0) {
        return kFALSE;
    }
    
    // Check if file exists
    FileStat_t buf;
    if (gSystem->GetPathInfo(filepath, buf) != 0) {
        return kFALSE;
    }
    
    // Check if it's a regular file (not a directory)
    if (R_ISREG(buf.fMode) == 0) {
        return kFALSE;
    }
    
    return kTRUE;
}

// ============================================================================
// Handle drag-and-drop entry - FIXED: Uses cached atoms
// ============================================================================
Bool_t DropTextEntry::HandleDNDDrop(TDNDData* data) 
{
    if (!data || !data->fData) {
        return kFALSE;
    }

    // Extract file path based on MIME type (using cached atoms)
    TString filePath;
    Bool_t success = kFALSE;

    if (data->fDataType == fAtomUri) {
        // Handle file URIs (most common from file managers)
        const char* uriText = reinterpret_cast<const char*>(data->fData);
        filePath = ExtractFilePath(uriText);
        
        if (IsValidFile(filePath)) {
            if (fGUI) {
                SetText(filePath);
                fGUI->LoadFromDrop(filePath);
            }
            success = kTRUE;
        } else {
            // File not found or invalid
            fprintf(stderr, "DropTextEntry: Invalid file path: %s\n", filePath.Data());
        }
    } 
    else if (data->fDataType == fAtomText) {
        // Handle plain text (fallback)
        const char* text = reinterpret_cast<const char*>(data->fData);
        filePath = TString(text).Strip();
        
        if (IsValidFile(filePath)) {
            if (fGUI) {
                SetText(filePath);
                fGUI->LoadFromDrop(filePath);
            }
            success = kTRUE;
        }
    } 
    else if (data->fDataType == fAtomRoot) {
        // Handle ROOT objects
        TBufferFile buffer(TBuffer::kRead, data->fDataLength, (void*)data->fData);
        buffer.SetReadMode();
        TObject* obj = (TObject*)buffer.ReadObjectAny(TObject::Class());
        if (obj && fGUI) {
            TString objName(obj->GetName());
            SetText(objName);
            fGUI->LoadFromDrop(objName);
            success = kTRUE;
        }
    }
    
    return success;
}