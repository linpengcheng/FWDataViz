#pragma once

#include "../Utils.h"
#include "../ConfigIO.h"
#include "../NPP/StaticDialog.h"
#include <regex>
#include <vector>

#define FILE_TYPE_LIMIT 999
#define REC_TYPE_LIMIT 999

using std::wregex;
using std::regex_replace;
using std::vector;
using Utils::showEditBalloonTip;

extern NppData nppData;
extern ConfigIO _configIO;

class ConfigureDialog : public StaticDialog {
public:
   ConfigureDialog() : StaticDialog() {};
   void doDialog(HINSTANCE hInst);

   void setFieldEditCaretOnFocus(HWND hEdit);
   void hiliteFieldEditPairedItem(HWND hThis, HWND hThat);
   void syncFieldEditScrolling(HWND hThis, HWND hThat);
   int appendFileTypeConfigs(const wstring& sConfigFile);

   HWND hFilesLB, hFileEOL, hThemesLB, hRecsLB, hRecStart, hRecRegex, hFieldLabels, hFieldWidths;

protected:
   enum move_dir {
      MOVE_DOWN = 1,
      MOVE_UP = -1
   };

   struct RecordType {
      wstring label;
      string marker;
      wstring fieldLabels;
      wstring fieldWidths;
   };

   struct FileType {
      wstring label;
      string eol;
      wstring theme;
      vector<RecordType> vRecTypes;
   };

   wstring configFile{ L"" };
   vector<FileType> vFileTypes;
   bool loadingEdits, cleanConfigFile, cleanFileVals, cleanRecVals, cleanFieldVals;

   INT_PTR CALLBACK run_dlgProc(UINT Message, WPARAM wParam, LPARAM);
   void localize();
   void indicateCleanStatus();
   int loadConfigInfo();
   int loadFileTypeInfo(int vIndex, const wstring& fileType, const wstring& sConfigFile);
   bool promptDiscardChangesNo();
   void saveConfigInfo();
   void showEximDialog(bool bExtract);
   string getOnlyStartsWith(string txt);

   int getCurrentFileTypeIndex();
   bool getCurrentFileTypeInfo(FileType* &fileInfo);
   FileType getNewFileType();
   void getFileTypeConfig(size_t idxFT, bool cr_lf, wstring& ftCode, wstring& ftConfig);

   int getCurrentRecIndex();
   bool getCurrentRecInfo(RecordType* &recInfo);
   RecordType getNewRec();

   void fillFileTypes();
   void onFileTypeSelect();
   void enableMoveFileButtons();
   void enableFileSelection();
   int moveFileType(move_dir dir);
   void fileEditAccept();
   void fileEditNew();
   void fileEditClone();
   int fileEditDelete();
   bool checkFTLimit(bool clone);

   void fillRecTypes();
   void onRecTypeSelect();
   void enableMoveRecButtons();
   void enableRecSelection();
   int moveRecType(move_dir dir);
   void onRecStartEditChange();
   void onRecRegexEditChange();
   void recEditAccept();
   void recEditNew(bool clone);
   int recEditDelete();

   void fillFieldTypes();
   void fieldEditsAccept();

   int editLabelsCaret{}, editWidthsCaret{};
};


