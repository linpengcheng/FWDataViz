#pragma once

#include "../Utils.h"
#include "../ConfigIO.h"
#include "../NPP/StaticDialog.h"
#include <regex>
#include <vector>

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

   HWND hFilesList, hFileEOL, hRecsList, hRecStart, hRecRegex, hFieldLabels, hFieldWidths;

protected:
   enum move_dir {
      MOVE_DOWN = 1,
      MOVE_UP = -1
   };

   struct RecordInfo {
      wstring label;
      string marker;
      wstring fieldLabels;
      wstring fieldWidths;
   };

   struct FileInfo {
      wstring label;
      string eol;
      wstring theme;
      vector<RecordInfo> records;
   };

   struct ModTrack {
      bool edited{};
      int accepted{};
   };

   wstring configFile{ L"" };
   vector<FileInfo> fileInfoList;
   ModTrack fileMods, recMods, fieldMods;

   INT_PTR CALLBACK run_dlgProc(UINT Message, WPARAM wParam, LPARAM);
   void localize();
   int loadConfigInfo();
   void reloadConfigInfo();

   int getCurrentFileIndex();
   bool getCurrentFileInfo(FileInfo* &fileInfo);
   FileInfo getNewFile();

   int getCurrentRecIndex();
   bool getCurrentRecInfo(RecordInfo* &recInfo);
   RecordInfo getNewRec();

   void fillFileTypes();
   void onFileTypeSelect();
   void enableMoveFileButtons();
   int moveFileType(move_dir dir);
   void fileEditAccept();
   void fileEditNew();
   int fileEditDelete();

   void fillRecTypes();
   void onRecTypeSelect();
   void enableMoveRecButtons();
   int moveRecType(move_dir dir);
   void onRecStartEditChange();
   void onRecRegexEditChange();
   void recEditAccept();
   void recEditNew();
   int recEditDelete();

   void fillFieldTypes();
   void fieldEditsAccept();
   void fieldEditsReset();

   int editLabelsCaret{}, editWidthsCaret{};
};


