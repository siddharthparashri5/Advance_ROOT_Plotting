// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dIhomedIsiddharthdIIFIC_WorkdIMy_CodesdIAdvance_ROOT_Plotting_devdIsrcdIProjectDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/AdvancedPlotGUI.h"
#include "/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/ColumnSelector.h"
#include "/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/CSVPreviewDialog.h"
#include "/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/RootDataInspector.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void delete_AdvancedPlotGUI(void *p);
   static void deleteArray_AdvancedPlotGUI(void *p);
   static void destruct_AdvancedPlotGUI(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::AdvancedPlotGUI*)
   {
      ::AdvancedPlotGUI *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::AdvancedPlotGUI >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("AdvancedPlotGUI", ::AdvancedPlotGUI::Class_Version(), "AdvancedPlotGUI.h", 25,
                  typeid(::AdvancedPlotGUI), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::AdvancedPlotGUI::Dictionary, isa_proxy, 4,
                  sizeof(::AdvancedPlotGUI) );
      instance.SetDelete(&delete_AdvancedPlotGUI);
      instance.SetDeleteArray(&deleteArray_AdvancedPlotGUI);
      instance.SetDestructor(&destruct_AdvancedPlotGUI);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::AdvancedPlotGUI*)
   {
      return GenerateInitInstanceLocal((::AdvancedPlotGUI*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::AdvancedPlotGUI*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void delete_ColumnSelectorDialog(void *p);
   static void deleteArray_ColumnSelectorDialog(void *p);
   static void destruct_ColumnSelectorDialog(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ColumnSelectorDialog*)
   {
      ::ColumnSelectorDialog *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::ColumnSelectorDialog >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("ColumnSelectorDialog", ::ColumnSelectorDialog::Class_Version(), "ColumnSelector.h", 19,
                  typeid(::ColumnSelectorDialog), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::ColumnSelectorDialog::Dictionary, isa_proxy, 4,
                  sizeof(::ColumnSelectorDialog) );
      instance.SetDelete(&delete_ColumnSelectorDialog);
      instance.SetDeleteArray(&deleteArray_ColumnSelectorDialog);
      instance.SetDestructor(&destruct_ColumnSelectorDialog);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ColumnSelectorDialog*)
   {
      return GenerateInitInstanceLocal((::ColumnSelectorDialog*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::ColumnSelectorDialog*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void delete_CSVPreviewDialog(void *p);
   static void deleteArray_CSVPreviewDialog(void *p);
   static void destruct_CSVPreviewDialog(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::CSVPreviewDialog*)
   {
      ::CSVPreviewDialog *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::CSVPreviewDialog >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("CSVPreviewDialog", ::CSVPreviewDialog::Class_Version(), "CSVPreviewDialog.h", 37,
                  typeid(::CSVPreviewDialog), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::CSVPreviewDialog::Dictionary, isa_proxy, 4,
                  sizeof(::CSVPreviewDialog) );
      instance.SetDelete(&delete_CSVPreviewDialog);
      instance.SetDeleteArray(&deleteArray_CSVPreviewDialog);
      instance.SetDestructor(&destruct_CSVPreviewDialog);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::CSVPreviewDialog*)
   {
      return GenerateInitInstanceLocal((::CSVPreviewDialog*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::CSVPreviewDialog*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void delete_RootDataInspector(void *p);
   static void deleteArray_RootDataInspector(void *p);
   static void destruct_RootDataInspector(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RootDataInspector*)
   {
      ::RootDataInspector *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RootDataInspector >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("RootDataInspector", ::RootDataInspector::Class_Version(), "RootDataInspector.h", 19,
                  typeid(::RootDataInspector), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RootDataInspector::Dictionary, isa_proxy, 4,
                  sizeof(::RootDataInspector) );
      instance.SetDelete(&delete_RootDataInspector);
      instance.SetDeleteArray(&deleteArray_RootDataInspector);
      instance.SetDestructor(&destruct_RootDataInspector);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RootDataInspector*)
   {
      return GenerateInitInstanceLocal((::RootDataInspector*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RootDataInspector*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr AdvancedPlotGUI::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *AdvancedPlotGUI::Class_Name()
{
   return "AdvancedPlotGUI";
}

//______________________________________________________________________________
const char *AdvancedPlotGUI::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::AdvancedPlotGUI*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int AdvancedPlotGUI::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::AdvancedPlotGUI*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *AdvancedPlotGUI::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::AdvancedPlotGUI*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *AdvancedPlotGUI::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::AdvancedPlotGUI*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr ColumnSelectorDialog::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *ColumnSelectorDialog::Class_Name()
{
   return "ColumnSelectorDialog";
}

//______________________________________________________________________________
const char *ColumnSelectorDialog::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ColumnSelectorDialog*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int ColumnSelectorDialog::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ColumnSelectorDialog*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *ColumnSelectorDialog::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ColumnSelectorDialog*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *ColumnSelectorDialog::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ColumnSelectorDialog*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr CSVPreviewDialog::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *CSVPreviewDialog::Class_Name()
{
   return "CSVPreviewDialog";
}

//______________________________________________________________________________
const char *CSVPreviewDialog::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::CSVPreviewDialog*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int CSVPreviewDialog::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::CSVPreviewDialog*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *CSVPreviewDialog::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::CSVPreviewDialog*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *CSVPreviewDialog::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::CSVPreviewDialog*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr RootDataInspector::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *RootDataInspector::Class_Name()
{
   return "RootDataInspector";
}

//______________________________________________________________________________
const char *RootDataInspector::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RootDataInspector*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int RootDataInspector::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RootDataInspector*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RootDataInspector::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RootDataInspector*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RootDataInspector::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RootDataInspector*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void AdvancedPlotGUI::Streamer(TBuffer &R__b)
{
   // Stream an object of class AdvancedPlotGUI.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(AdvancedPlotGUI::Class(),this);
   } else {
      R__b.WriteClassBuffer(AdvancedPlotGUI::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_AdvancedPlotGUI(void *p) {
      delete ((::AdvancedPlotGUI*)p);
   }
   static void deleteArray_AdvancedPlotGUI(void *p) {
      delete [] ((::AdvancedPlotGUI*)p);
   }
   static void destruct_AdvancedPlotGUI(void *p) {
      typedef ::AdvancedPlotGUI current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::AdvancedPlotGUI

//______________________________________________________________________________
void ColumnSelectorDialog::Streamer(TBuffer &R__b)
{
   // Stream an object of class ColumnSelectorDialog.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(ColumnSelectorDialog::Class(),this);
   } else {
      R__b.WriteClassBuffer(ColumnSelectorDialog::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_ColumnSelectorDialog(void *p) {
      delete ((::ColumnSelectorDialog*)p);
   }
   static void deleteArray_ColumnSelectorDialog(void *p) {
      delete [] ((::ColumnSelectorDialog*)p);
   }
   static void destruct_ColumnSelectorDialog(void *p) {
      typedef ::ColumnSelectorDialog current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::ColumnSelectorDialog

//______________________________________________________________________________
void CSVPreviewDialog::Streamer(TBuffer &R__b)
{
   // Stream an object of class CSVPreviewDialog.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(CSVPreviewDialog::Class(),this);
   } else {
      R__b.WriteClassBuffer(CSVPreviewDialog::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_CSVPreviewDialog(void *p) {
      delete ((::CSVPreviewDialog*)p);
   }
   static void deleteArray_CSVPreviewDialog(void *p) {
      delete [] ((::CSVPreviewDialog*)p);
   }
   static void destruct_CSVPreviewDialog(void *p) {
      typedef ::CSVPreviewDialog current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::CSVPreviewDialog

//______________________________________________________________________________
void RootDataInspector::Streamer(TBuffer &R__b)
{
   // Stream an object of class RootDataInspector.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(RootDataInspector::Class(),this);
   } else {
      R__b.WriteClassBuffer(RootDataInspector::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_RootDataInspector(void *p) {
      delete ((::RootDataInspector*)p);
   }
   static void deleteArray_RootDataInspector(void *p) {
      delete [] ((::RootDataInspector*)p);
   }
   static void destruct_RootDataInspector(void *p) {
      typedef ::RootDataInspector current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::RootDataInspector

namespace ROOT {
   static TClass *vectorlEstringgR_Dictionary();
   static void vectorlEstringgR_TClassManip(TClass*);
   static void *new_vectorlEstringgR(void *p = nullptr);
   static void *newArray_vectorlEstringgR(Long_t size, void *p);
   static void delete_vectorlEstringgR(void *p);
   static void deleteArray_vectorlEstringgR(void *p);
   static void destruct_vectorlEstringgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<string>*)
   {
      vector<string> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<string>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<string>", -2, "vector", 389,
                  typeid(vector<string>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEstringgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<string>) );
      instance.SetNew(&new_vectorlEstringgR);
      instance.SetNewArray(&newArray_vectorlEstringgR);
      instance.SetDelete(&delete_vectorlEstringgR);
      instance.SetDeleteArray(&deleteArray_vectorlEstringgR);
      instance.SetDestructor(&destruct_vectorlEstringgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<string> >()));

      ::ROOT::AddClassAlternate("vector<string>","std::vector<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > > >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<string>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEstringgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<string>*)nullptr)->GetClass();
      vectorlEstringgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEstringgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEstringgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<string> : new vector<string>;
   }
   static void *newArray_vectorlEstringgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<string>[nElements] : new vector<string>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEstringgR(void *p) {
      delete ((vector<string>*)p);
   }
   static void deleteArray_vectorlEstringgR(void *p) {
      delete [] ((vector<string>*)p);
   }
   static void destruct_vectorlEstringgR(void *p) {
      typedef vector<string> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<string>

namespace {
  void TriggerDictionaryInitialization_ProjectDict_Impl() {
    static const char* headers[] = {
"/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/AdvancedPlotGUI.h",
"/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/ColumnSelector.h",
"/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/CSVPreviewDialog.h",
"/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/RootDataInspector.h",
nullptr
    };
    static const char* includePaths[] = {
"/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include",
"/opt/root/root-6.26.06-install/include/",
"/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/build/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "ProjectDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/AdvancedPlotGUI.h")))  AdvancedPlotGUI;
class __attribute__((annotate("$clingAutoload$/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/ColumnSelector.h")))  ColumnSelectorDialog;
class __attribute__((annotate("$clingAutoload$/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/CSVPreviewDialog.h")))  CSVPreviewDialog;
class __attribute__((annotate("$clingAutoload$/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/RootDataInspector.h")))  RootDataInspector;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "ProjectDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/AdvancedPlotGUI.h"
#include "/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/ColumnSelector.h"
#include "/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/CSVPreviewDialog.h"
#include "/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting_dev/include/RootDataInspector.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"AdvancedPlotGUI", payloadCode, "@",
"CSVPreviewDialog", payloadCode, "@",
"ColumnSelectorDialog", payloadCode, "@",
"RootDataInspector", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("ProjectDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_ProjectDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_ProjectDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_ProjectDict() {
  TriggerDictionaryInitialization_ProjectDict_Impl();
}
