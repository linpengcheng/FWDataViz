//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "VisualizerPanel.h"

INT_PTR CALLBACK VisualizerPanel::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) {
   switch (message) {
      case WM_COMMAND:
         switch LOWORD(wParam) {
            case IDC_VIZPANEL_FILETYPE_SELECT:
               switch HIWORD(wParam) {
                  case LBN_SELCHANGE:
                     visualizeFile();
                     break;
               }
               break;

            case IDC_VIZPANEL_FILETYPE_CONFIG:
               ShowConfigDialog();
               break;

            case IDC_VIZPANEL_CLEAR_BUTTON:
               clearVisualize();
               break;

            case IDCANCEL:
            case IDCLOSE:
               setFocusOnEditor();
               ShowVisualizerPanel(false);
               break;

            case IDC_VIZPANEL_CARET_FRAMED:
               SendMessage(getCurrentScintilla(), SCI_SETCARETLINEFRAME,
                  (WPARAM)IsDlgButtonChecked(_hSelf, IDC_VIZPANEL_CARET_FRAMED) == BST_CHECKED ? 2 : 0, NULL);
               break;

            case IDC_VIZPANEL_WORDWRAP_BUTTON:
               SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_VIEW_WRAP);
               setFocusOnEditor();
               break;
         }

      break;

      case WM_LBUTTONDOWN:
      case WM_MBUTTONDOWN:
      case WM_RBUTTONDOWN:
         SetFocus(_hSelf);
         break;

      default :
         return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
   }

   return FALSE;
}

void VisualizerPanel::initPanel() {
   bool recentOS = Utils::checkBaseOS(WV_VISTA);
   wstring fontName = recentOS ? L"Consolas" : L"Courier New";
   int fontHeight = recentOS ? 10 : 8;

   Utils::setFont(_hSelf, IDC_VIZPANEL_FIELD_LABEL, fontName, fontHeight, FW_BOLD, FALSE, TRUE);
   Utils::setFont(_hSelf, IDC_VIZPANEL_FIELD_INFO, fontName, fontHeight);

   Utils::loadBitmap(_hSelf, IDC_VIZPANEL_FILETYPE_CONFIG, IDC_VIZPANEL_CONFIG_BITMAP);
   Utils::addTooltip(_hSelf, IDC_VIZPANEL_FILETYPE_CONFIG, NULL, VIZ_PANEL_TIP_CONFIG, FALSE);

   if (_gLanguage != LANG_ENGLISH) localize();
}

void VisualizerPanel::localize() {
   SetWindowText(_hSelf, FWVIZ_DIALOG_TITLE);
   SetDlgItemText(_hSelf, IDC_VIZPANEL_FILETYPE_LABEL, VIZ_PANEL_FILETYPE_LABEL);
   SetDlgItemText(_hSelf, IDC_VIZPANEL_CLEAR_BUTTON, VIZ_PANEL_CLEAR_BUTTON);
   SetDlgItemText(_hSelf, IDCLOSE, VIZ_PANEL_CLOSE);
   SetDlgItemText(_hSelf, IDC_VIZPANEL_FIELD_LABEL, VIZ_PANEL_FIELD_LABEL);
   SetDlgItemText(_hSelf, IDC_VIZPANEL_WORDWRAP_INFO, VIZ_PANEL_WORDWRAP_INFO);
   SetDlgItemText(_hSelf, IDC_VIZPANEL_CARET_FRAMED, VIZ_PANEL_CARET_FRAMED);
   SetDlgItemText(_hSelf, IDC_VIZPANEL_WORDWRAP_BUTTON, VIZ_PANEL_WORDWRAP_BUTTON);
}

void VisualizerPanel::display(bool toShow) {
   DockingDlgInterface::display(toShow);
   hFTList = GetDlgItem(_hSelf, IDC_VIZPANEL_FILETYPE_SELECT);

   if (toShow) {
      CheckDlgButton(_hSelf, IDC_VIZPANEL_CARET_FRAMED,
         SendMessage(getCurrentScintilla(), SCI_GETCARETLINEFRAME, NULL, NULL) == 0 ? BST_UNCHECKED : BST_CHECKED);

      loadFileTypes();
      SetFocus(hFTList);
      syncListFileType();
   }
};

void VisualizerPanel::setParent(HWND parent2set) {
   _hParent = parent2set;
};

void VisualizerPanel::loadFileTypes() {
   vector<wstring> fileTypes;
   wstring fileTypeList;

   fileTypeList = _configIO.getConfigString(L"Base", L"FileTypes");
   _configIO.Tokenize(fileTypeList, fileTypes);

   mapFileDescToType.clear();
   mapFileTypeToDesc.clear();

   SendMessage(hFTList, CB_RESETCONTENT, NULL, NULL);
   SendMessage(hFTList, CB_ADDSTRING, NULL, (LPARAM)L"-");

   for (auto fType : fileTypes) {
      wstring fileLabel;

      fileLabel = _configIO.getConfigString(fType, L"FileLabel");

      mapFileDescToType[fileLabel] = fType;
      mapFileTypeToDesc[fType] = fileLabel;
      SendMessage(hFTList, CB_ADDSTRING, NULL, (LPARAM)fileLabel.c_str());
   }
}

void VisualizerPanel::syncListFileType() {
   HWND hScintilla{ getCurrentScintilla() };
   if (!hScintilla) return;

   wstring fileType;
   wstring fDesc;

   if (!getDocFileType(hScintilla, fileType)) {
      SendMessage(hFTList, CB_SETCURSEL, (WPARAM)0, NULL);
      return;
   }

   fDesc = mapFileTypeToDesc[fileType];

   SendMessage(hFTList, CB_SETCURSEL, (WPARAM)
      SendMessage(hFTList, CB_FINDSTRING, (WPARAM)-1, (LPARAM)fDesc.c_str()), NULL);
}

void VisualizerPanel::visualizeFile() {
   HWND hScintilla{ getCurrentScintilla() };
   if (!hScintilla) return;

   wchar_t fDesc[MAX_PATH];
   wstring sDesc;

   SendMessage(hFTList, WM_GETTEXT, MAX_PATH, (LPARAM)fDesc);
   sDesc =  mapFileDescToType[fDesc];

   if (sDesc.length() < 2) {
      clearVisualize();
      return;
   }

   SendMessage(hScintilla, SCI_SETCARETLINEFRAME, (WPARAM)2, NULL);
   CheckDlgButton(_hSelf, IDC_VIZPANEL_CARET_FRAMED, BST_CHECKED);

   clearVisualize(FALSE);
   setDocFileType(hScintilla, sDesc);

   loadStyles();
   applyStyles();

   loadLexer();
   updateCurrentPage();
   setFocusOnEditor();

   //size_t startLine{ static_cast<size_t>(SendMessage(hScintilla, SCI_GETFIRSTVISIBLELINE, NULL, NULL)) };
   //applyLexer(startLine, startLine + 10);
}

void VisualizerPanel::clearVisualize(bool sync) {
   HWND hScintilla{ getCurrentScintilla() };
   if (!hScintilla) return;

   SendMessage(hScintilla, SCI_STYLECLEARALL, NULL, NULL);
   SendMessage(hScintilla, SCI_STARTSTYLING, 0, NULL);
   SendMessage(hScintilla, SCI_SETSTYLING,
      SendMessage(hScintilla, SCI_GETLENGTH, NULL, NULL), STYLE_DEFAULT);

   setDocFileType(hScintilla, L"");
   clearLexer();

   if (sync) syncListFileType();
}

int VisualizerPanel::loadStyles() {
   HWND hScintilla{ getCurrentScintilla() };
   if (!hScintilla) return -1;

   wstring fileType;
   if (!getDocFileType(hScintilla, fileType)) return 0;

   wstring fileTheme;

   fileTheme = _configIO.getConfigString(fileType, L"FileTheme");
   if (fileTheme == currentStyleTheme) return 0;

   _configIO.setThemeFilePath(fileTheme);
   currentStyleTheme = fileTheme;

   _configIO.getStyleColor(L"EOL_Back", styleEOL.backColor, FALSE);
   _configIO.getStyleColor(L"EOL_Fore", styleEOL.foreColor, TRUE);
   _configIO.getStyleBool(L"EOL_Bold", styleEOL.bold);
   _configIO.getStyleBool(L"EOL_Italics", styleEOL.italics);

   int styleCount{};
   wchar_t cPre[10];
   wstring sPrefix;

   if (_configIO.getStyleValue(L"Count").length() > 0)
      styleCount = std::stoi (_configIO.getStyleValue(L"Count"));

   styleSet.clear();
   styleSet.resize(styleCount);

   for (int i{}; i < styleCount; i++) {
      swprintf(cPre, 10, L"C%02i_", i);
      sPrefix = wstring(cPre);
      _configIO.getStyleColor((sPrefix + L"Back"), styleSet[i].backColor, FALSE);
      _configIO.getStyleColor((sPrefix + L"Fore"), styleSet[i].foreColor, TRUE);
      _configIO.getStyleBool((sPrefix + L"Bold"), styleSet[i].bold);
      _configIO.getStyleBool((sPrefix + L"Italics"), styleSet[i].italics);
   }

#if FW_DEBUG_LOAD_STYLES
   wstring dbgMessage;

   for (int i{}; i < styleCount; i++) {
      swprintf(cPre, 10, L"C%02i_", i);
      sPrefix = wstring(cPre);
      dbgMessage = sPrefix + L"_Back = " + to_wstring(styleSet[i].backColor) + L"\n" +
         sPrefix + L"_Fore = " + to_wstring(styleSet[i].foreColor) + L"\n" +
         sPrefix + L"_Bold = " + to_wstring(styleSet[i].bold) + L"\n" +
         sPrefix + L"_Italics = " + to_wstring(styleSet[i].italics) + L"\n";
      MessageBox(_hSelf, dbgMessage.c_str(), L"Theme Styles", MB_OK);
   }
#endif

   return styleCount;
}

int VisualizerPanel::applyStyles() {
   PSCIFUNC_T sciFunc;
   void* sciPtr;

   if (!getDirectScintillaFunc(sciFunc, sciPtr)) return -1;

   wstring fileType;
   if (!getDocFileType(sciFunc, sciPtr, fileType)) return 0;

   if (currentStyleTheme.length() < 1) return 0;

   if (sciFunc(sciPtr, SCI_GETLEXER, NULL, NULL) == SCLEX_CONTAINER)
      return 0;

   sciFunc(sciPtr, SCI_STYLESETBACK, (WPARAM)FW_STYLE_EOL, (LPARAM)styleEOL.backColor);
   sciFunc(sciPtr, SCI_STYLESETFORE, (WPARAM)FW_STYLE_EOL, (LPARAM)styleEOL.foreColor);
   sciFunc(sciPtr, SCI_STYLESETBOLD, (WPARAM)FW_STYLE_EOL, (LPARAM)styleEOL.bold);
   sciFunc(sciPtr, SCI_STYLESETITALIC, (WPARAM)FW_STYLE_EOL, (LPARAM)styleEOL.italics);

   const int styleCount{ static_cast<int>(styleSet.size()) };

   for (int i{}; i < styleCount; i++) {
      sciFunc(sciPtr, SCI_STYLESETBACK, (WPARAM)(FW_STYLE_RANGE_START + i), (LPARAM)styleSet[i].backColor);
      sciFunc(sciPtr, SCI_STYLESETFORE, (WPARAM)(FW_STYLE_RANGE_START + i), (LPARAM)styleSet[i].foreColor);
      sciFunc(sciPtr, SCI_STYLESETBOLD, (WPARAM)(FW_STYLE_RANGE_START + i), (LPARAM)styleSet[i].bold);
      sciFunc(sciPtr, SCI_STYLESETITALIC, (WPARAM)(FW_STYLE_RANGE_START + i), (LPARAM)styleSet[i].italics);
   }

   sciFunc(sciPtr, SCI_SETLEXER, (WPARAM)SCLEX_CONTAINER, NULL);

#if FW_DEBUG_SET_STYLES
   wstring dbgMessage;
   int back, fore, bold, italics;

   for (int i{ -1 }; i < styleCount; i++) {
      back = sciFunc(sciPtr, SCI_STYLEGETBACK, (WPARAM)(FW_STYLE_RANGE_START + i), NULL);
      fore = sciFunc(sciPtr, SCI_STYLEGETFORE, (WPARAM)(FW_STYLE_RANGE_START + i), NULL);
      bold = sciFunc(sciPtr, SCI_STYLEGETBOLD, (WPARAM)(FW_STYLE_RANGE_START + i), NULL);
      italics = sciFunc(sciPtr, SCI_STYLEGETITALIC, (WPARAM)(FW_STYLE_RANGE_START + i), NULL);

      dbgMessage = L"C0" + to_wstring(i) + L"_STYLES = " +
         to_wstring(back) + L", " + to_wstring(fore) + L", " +
         to_wstring(bold) + L", " + to_wstring(italics);
      MessageBox(_hSelf, dbgMessage.c_str(), L"Theme Styles", MB_OK);
   }
#endif

   return styleCount;
}

int VisualizerPanel::loadLexer() {
   HWND hScintilla{ getCurrentScintilla() };
   if (!hScintilla) return -1;

   wstring fileType;
   wstring recTypeList;
   vector<wstring> recTypes;
   int recTypeCount;

   if (!getDocFileType(hScintilla, fileType)) {
      clearLexer();
      return 0;
   }

   if (fwVizRegexed.compare(fileType) != 0) {
      clearLexer();
   }

   if (recInfoList.size() > 0) {
      return static_cast<int>(recInfoList.size());
   }

   recTypeList = _configIO.getConfigString(fileType, L"RecordTypes");
   recTypeCount = _configIO.Tokenize(recTypeList, recTypes);

   recInfoList.resize(recTypeCount);

   for (int i{}; i < recTypeCount; i++) {
      wstring &recType = recTypes[i];
      RecordInfo &RT = recInfoList[i];

      RT.label = _configIO.getConfigString(fileType, (recType + L"_Label"), recType);
      RT.marker = _configIO.getConfigStringA(fileType, (recType + L"_Marker"), L".");
      RT.regExpr = regex{ RT.marker + ".*(\r\n|\n|\r)?" };

      wstring fieldWidthList;
      int fieldCount;

      fieldWidthList = _configIO.getConfigString(fileType, (recType + L"_FieldWidths"));
      fieldCount = _configIO.Tokenize(fieldWidthList, RT.fieldWidths);

      RT.fieldStarts.clear();
      RT.fieldStarts.resize(fieldCount);

      for (int fnum{}, startPos{}; fnum < fieldCount; fnum++) {
         RT.fieldStarts[fnum] = startPos;
         startPos += RT.fieldWidths[fnum];
      }

      wstring fieldLabelList;

      fieldLabelList = _configIO.getConfigString(fileType, (recType + L"_FieldLabels"));
      _configIO.Tokenize(fieldLabelList, RT.fieldLabels);
   }

   fwVizRegexed = fileType;

#if FW_DEBUG_LOAD_REGEX
   int fieldCount;

   for (int i{}; i < recTypeCount; i++) {
      wstring dbgMessage;
      wstring &recType = recTypes[i];
      RecordInfo &RT = recInfoList[i];

      dbgMessage = recType + L"\nRec_Label = " + RT.label +
         L"\nRec_Marker = " + _configIO.NarrowToWide(RT.marker) + L"\nFieldWidths=\n";

      fieldCount = static_cast<int>(RT.fieldWidths.size());

      for (int j{}; j < fieldCount; j++) {
         dbgMessage += L" (" + to_wstring(RT.fieldStarts[j]) + L", " + to_wstring(RT.fieldWidths[j]) + L"),";
      }

      MessageBox(_hSelf, dbgMessage.c_str(), fwVizRegexed.c_str(), MB_OK);
   }
#endif

   return recTypeCount;
}

void VisualizerPanel::applyLexer(const size_t startLine, const size_t endLine) {
   PSCIFUNC_T sciFunc;
   void* sciPtr;

   if (!getDirectScintillaFunc(sciFunc, sciPtr)) return;

   wstring fileType;
   if (!getDocFileType(sciFunc, sciPtr, fileType)) return;

   char lineTextCStr[FW_LINE_MAX_LENGTH];
   string recStartText{}, eolMarker;
   size_t caretLine, eolMarkerLen, eolMarkerPos, recStartLine{},
      currentPos, startPos, endPos, recStartPos{};

   const size_t regexedCount{ recInfoList.size() };
   const size_t styleCount{ styleSet.size() };
   bool newRec{ TRUE };

   eolMarker =  _configIO.getConfigStringA(fileType, L"RecordTerminator");
   eolMarkerLen = eolMarker.length();
   caretLine = sciFunc(sciPtr, SCI_LINEFROMPOSITION,
      sciFunc(sciPtr, SCI_GETCURRENTPOS, NULL, NULL), NULL);

   caretRecordRegIndex = -1;
   caretRecordStartPos = 0;
   caretRecordEndPos = 0;

   for (auto currentLine{ startLine }; currentLine < endLine; currentLine++) {
      if (sciFunc(sciPtr, SCI_LINELENGTH, currentLine, NULL) > FW_LINE_MAX_LENGTH) {
         continue;
      }

      sciFunc(sciPtr, SCI_GETLINE, (WPARAM)currentLine, (LPARAM)lineTextCStr);
      startPos = sciFunc(sciPtr, SCI_POSITIONFROMLINE, currentLine, NULL);
      endPos = sciFunc(sciPtr, SCI_GETLINEENDPOSITION, currentLine, NULL);
      string_view lineText{ lineTextCStr, endPos - startPos };

      if (newRec) {
         recStartLine = currentLine;
         recStartPos = startPos;
         recStartText = lineText;
      }

      if (newRec && lineText.length() == 0) {
         continue;
      }

      if (eolMarkerLen == 0) {
         newRec = TRUE;
         eolMarkerPos = endPos;
      }
      else if (lineText.length() > eolMarkerLen &&
         (lineText.substr(lineText.length() - eolMarkerLen) == eolMarker)) {
         newRec = TRUE;
         eolMarkerPos = endPos - eolMarkerLen;
      }
      else if (currentLine < endLine) {
         newRec = FALSE;
         continue;
      }
      else {
         eolMarkerPos = endPos;
      }

      currentPos = recStartPos;

      int colorOffset{};
      size_t regexIndex{};

      while (regexIndex < regexedCount) {
         if (regex_match(recStartText, recInfoList[regexIndex].regExpr)) {
            if (caretLine >= recStartLine && caretLine <= currentLine) {
               caretRecordRegIndex = static_cast<int>(regexIndex);
               caretRecordStartPos = static_cast<int>(recStartPos);
               caretRecordEndPos = static_cast<int>(endPos);
               caretEolMarkerPos = static_cast<int>(eolMarkerPos);
            }

            break;
         }

         regexIndex++;
         colorOffset += 5;
      }

      if (regexIndex >= regexedCount) {
         continue;
      }

      const vector<int> recFieldWidths{ recInfoList[regexIndex].fieldWidths };
      const size_t fieldCount{ recFieldWidths.size() };
      int unstyledLen{};

#if FW_DEBUG_APPLY_LEXER
      wstring dbgMessage;
      size_t dbgPos{ currentPos };

      dbgMessage = L"FieldWidths[" + to_wstring(regexIndex) + L"] = " +
         to_wstring(fieldCount) + L"\n";

      for (int i{}; i < static_cast<int>(fieldCount); i++) {
         dbgMessage += L" (" + to_wstring(dbgPos) + L", " +
            to_wstring(recFieldWidths[i]) + L", " + to_wstring(eolMarkerPos - eolMarkerLen) + L"), ";
         dbgPos += recFieldWidths[i];
      }

      MessageBox(_hSelf, dbgMessage.c_str(), fwVizRegexed.c_str(), MB_OK);
#endif

      for (size_t i{}; i < fieldCount; i++) {
         sciFunc(sciPtr, SCI_STARTSTYLING, (WPARAM)currentPos, NULL);
         unstyledLen = static_cast<int>(eolMarkerPos - currentPos);
         currentPos += recFieldWidths[i];

         if (recFieldWidths[i] < unstyledLen) {
            sciFunc(sciPtr, SCI_SETSTYLING, (WPARAM)recFieldWidths[i],
               FW_STYLE_RANGE_START + ((i + colorOffset) % styleCount));
         }
         else {
            sciFunc(sciPtr, SCI_SETSTYLING, (WPARAM)unstyledLen,
               FW_STYLE_RANGE_START + ((i + colorOffset) % styleCount));
            unstyledLen = 0;

            sciFunc(sciPtr, SCI_STARTSTYLING, (WPARAM)eolMarkerPos, NULL);
            sciFunc(sciPtr, SCI_SETSTYLING, (WPARAM)eolMarkerLen, FW_STYLE_EOL);
            break;
         }
      }

      if (fieldCount > 0 && unstyledLen > 0) {
         sciFunc(sciPtr, SCI_STARTSTYLING, (WPARAM)currentPos, NULL);
         sciFunc(sciPtr, SCI_SETSTYLING, (WPARAM)(endPos - currentPos), FW_STYLE_EOL);
      }
   }
}

void VisualizerPanel::updateCurrentPage() {
   if (loadLexer() < 1) {
      clearCaretFieldInfo();
      showWordwrapInfo(FALSE);
      return;
   }

   HWND hScintilla{ getCurrentScintilla() };
   if (!hScintilla) return;

   showWordwrapInfo(SendMessage(hScintilla, SCI_GETWRAPMODE, NULL, NULL) != SC_WRAP_NONE);

   size_t startLine, lineCount, linesOnScreen, endLine;

   startLine = static_cast<size_t>(SendMessage(hScintilla, SCI_GETFIRSTVISIBLELINE, NULL, NULL));
   lineCount = static_cast<size_t>(SendMessage(hScintilla, SCI_GETLINECOUNT, NULL, NULL));
   linesOnScreen = static_cast<size_t>(SendMessage(hScintilla, SCI_LINESONSCREEN, NULL, NULL));

   endLine = (lineCount < startLine + linesOnScreen) ? lineCount : (startLine + linesOnScreen);

   applyLexer(startLine, endLine);
   displayCaretFieldInfo(startLine, endLine);
}

void VisualizerPanel::clearCaretFieldInfo() {
   ShowWindow(GetDlgItem(_hSelf, IDC_VIZPANEL_FIELD_LABEL), SW_HIDE);
   ShowWindow(GetDlgItem(_hSelf, IDC_VIZPANEL_FIELD_INFO), SW_HIDE);
   SetDlgItemText(_hSelf, IDC_VIZPANEL_FIELD_INFO, L"");
}

void VisualizerPanel::displayCaretFieldInfo(const size_t startLine, const size_t endLine) {
   HWND hScintilla{ getCurrentScintilla() };
   if (!hScintilla) return;

   wstring fileType;

   if (!getDocFileType(hScintilla, fileType)) return;

   wstring fieldInfoText{};
   int caretColumnPos;
   size_t caretLine;

   caretColumnPos = static_cast<int>(SendMessage(hScintilla, SCI_GETCURRENTPOS, NULL, NULL));
   caretLine = SendMessage(hScintilla, SCI_LINEFROMPOSITION, caretColumnPos, NULL);

   if (caretLine < startLine || caretLine > endLine) {
      clearCaretFieldInfo();
      return;
   }

   if (caretRecordRegIndex < 0) {
      if (SendMessage(hScintilla, SCI_POSITIONFROMLINE, caretLine, NULL) ==
         SendMessage(hScintilla, SCI_GETLINEENDPOSITION, caretLine, NULL)) {
         fieldInfoText = L"<Blank Line>";
      }
      else {
         fieldInfoText = L"<Unknown Record Type>";
      }
   }
   else if (caretColumnPos == caretRecordEndPos) {
      fieldInfoText = L"<Record End>";
   }
   else if (caretColumnPos >= caretEolMarkerPos) {
      fieldInfoText = L"<Record Terminator>";
   }
   else {
      RecordInfo& FLD{ recInfoList[caretRecordRegIndex] };
      int caretColumn, fieldCount, fieldLabelCount, cumulativeWidth{}, matchedField{ -1 };

      caretColumn = caretColumnPos - caretRecordStartPos;
      fieldInfoText = L" Record Type: " + FLD.label;
      fieldCount = static_cast<int>(FLD.fieldStarts.size());
      fieldLabelCount = static_cast<int>(FLD.fieldLabels.size());

      for (int i{}; i < fieldCount; i++) {
         cumulativeWidth += FLD.fieldWidths[i];
         if (caretColumn >= FLD.fieldStarts[i] && caretColumn < cumulativeWidth) {
            matchedField = i;
            break;
         }
      }

      if (matchedField < 0) {
         fieldInfoText += L"\n    Overflow!";
      }
      else {
         fieldInfoText += L"\n Field Label: ";

         if (fieldLabelCount == 0 || matchedField >= fieldLabelCount) {
            fieldInfoText += L"Field #" + to_wstring(matchedField + 1);
         }
         else {
            fieldInfoText += FLD.fieldLabels[matchedField];
         }

         fieldInfoText += L"\n Field Start: " + to_wstring(FLD.fieldStarts[matchedField] + 1);
         fieldInfoText += L"\n Field Width: " + to_wstring(FLD.fieldWidths[matchedField]);
         fieldInfoText += L"\nField Column: " + to_wstring(caretColumn - FLD.fieldStarts[matchedField] + 1);
      }
   }

   wchar_t ansiInfo[200];
   UCHAR atChar = static_cast<UCHAR>(SendMessage(hScintilla, SCI_GETCHARAT, caretColumnPos, 0));
   swprintf(ansiInfo, 200, L"0x%X [%u]", atChar, atChar);
   fieldInfoText += L"\n   ANSI Byte: " + wstring(ansiInfo);

   SetDlgItemText(_hSelf, IDC_VIZPANEL_FIELD_INFO, fieldInfoText.c_str());

   ShowWindow(GetDlgItem(_hSelf, IDC_VIZPANEL_FIELD_LABEL), SW_SHOW);
   ShowWindow(GetDlgItem(_hSelf, IDC_VIZPANEL_FIELD_INFO), SW_SHOW);
}

/// *** Private Functions: *** ///

bool VisualizerPanel::getDocFileType(HWND hScintilla, wstring& fileType) {
   char fType[MAX_PATH];

   SendMessage(hScintilla, SCI_GETPROPERTY, (WPARAM)FW_DOC_FILE_TYPE, (LPARAM)fType);
   fileType = _configIO.NarrowToWide(fType);

   return (fileType.length() > 1);
}

bool VisualizerPanel::getDocFileType(PSCIFUNC_T sciFunc, void* sciPtr, wstring& fileType) {
   char fType[MAX_PATH];

   sciFunc(sciPtr, SCI_GETPROPERTY, (WPARAM)FW_DOC_FILE_TYPE, (LPARAM)fType);
   fileType = _configIO.NarrowToWide(fType);

   return (fileType.length() > 1);
}

void VisualizerPanel::setDocFileType(HWND hScintilla, wstring fileType) {
   SendMessage(hScintilla, SCI_SETLEXER, (WPARAM)SCLEX_NULL, NULL);
   SendMessage(hScintilla, SCI_SETPROPERTY, (WPARAM)FW_DOC_FILE_TYPE,
      (LPARAM)_configIO.WideToNarrow(fileType).c_str());
}

void VisualizerPanel::setFocusOnEditor() {
   HWND hScintilla{ getCurrentScintilla() };
   if (!hScintilla) return;

   SendMessage(hScintilla, SCI_GRABFOCUS, 0, 0);
}

void VisualizerPanel::clearLexer() {
   recInfoList.clear();
   fwVizRegexed = L"";
}

void VisualizerPanel::showWordwrapInfo(bool show) {
   ShowWindow(GetDlgItem(_hSelf, IDC_VIZPANEL_WORDWRAP_INFO), show ? SW_SHOW : SW_HIDE);
   ShowWindow(GetDlgItem(_hSelf, IDC_VIZPANEL_WORDWRAP_BUTTON), show ? SW_SHOW : SW_HIDE);
}
