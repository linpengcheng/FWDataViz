#include "ThemeDialog.h"
#include "EximFileTypeDialog.h"

extern HINSTANCE _gModule;
extern ThemeDialog _themeDlg;
extern EximFileTypeDialog _eximDlg;

void ThemeDialog::doDialog(HINSTANCE hInst) {
   if (!isCreated()) {
      Window::init(hInst, nppData._nppHandle);
      create(IDD_THEME_DEFINER_DIALOG);
   }

   hThemesLB = GetDlgItem(_hSelf, IDC_THEME_DEF_LIST_BOX);
   hStylesLB = GetDlgItem(_hSelf, IDC_THEME_STYLE_LIST_BOX);

   SendDlgItemMessage(_hSelf, IDC_THEME_DEF_DESC_EDIT, EM_LIMITTEXT, (WPARAM)MAX_PATH, NULL);

   Utils::loadBitmap(_hSelf, IDC_THEME_DEF_DOWN_BUTTON, IDC_FWVIZ_DEF_MOVE_DOWN_BITMAP);
   Utils::addTooltip(_hSelf, IDC_THEME_DEF_DOWN_BUTTON, NULL, THEME_DEF_MOVE_DOWN, FALSE);

   Utils::loadBitmap(_hSelf, IDC_THEME_DEF_UP_BUTTON, IDC_FWVIZ_DEF_MOVE_UP_BITMAP);
   Utils::addTooltip(_hSelf, IDC_THEME_DEF_UP_BUTTON, NULL, THEME_DEF_MOVE_UP, FALSE);

   Utils::loadBitmap(_hSelf, IDC_THEME_STYLE_DOWN_BUTTON, IDC_FWVIZ_DEF_MOVE_DOWN_BITMAP);
   Utils::addTooltip(_hSelf, IDC_THEME_STYLE_DOWN_BUTTON, NULL, THEME_STYLE_MOVE_DOWN, FALSE);

   Utils::loadBitmap(_hSelf, IDC_THEME_STYLE_UP_BUTTON, IDC_FWVIZ_DEF_MOVE_UP_BITMAP);
   Utils::addTooltip(_hSelf, IDC_THEME_STYLE_UP_BUTTON, NULL, THEME_STYLE_MOVE_UP, FALSE);

   if (_gLanguage != LANG_ENGLISH) localize();
   goToCenter();

   SendMessage(_hParent, NPPM_DMMSHOW, 0, (LPARAM)_hSelf);

   loadConfigInfo();
   fillThemes();
}

INT_PTR CALLBACK ThemeDialog::run_dlgProc(UINT message, WPARAM wParam, LPARAM) {
   switch (message) {
      case WM_COMMAND:
         switch LOWORD(wParam) {
            case IDC_THEME_DEF_LIST_BOX:
               switch HIWORD(wParam) {
                  case LBN_SELCHANGE:
                     onThemeSelect();
                     break;
               }
               break;

            case IDC_THEME_DEF_DOWN_BUTTON:
               moveThemeType(MOVE_DOWN);
               break;

            case IDC_THEME_DEF_UP_BUTTON:
               moveThemeType(MOVE_UP);
               break;

            case IDC_THEME_DEF_DESC_EDIT:
               switch HIWORD(wParam) {
                  case EN_CHANGE:
                     if (!loadingEdits) {
                        cleanThemeVals = FALSE;
                        enableThemeSelection();
                     }
                     break;
                  }
               break;

            case IDC_THEME_DEF_ACCEPT_BTN:
               themeEditAccept();
               break;

            case IDC_THEME_DEF_NEW_BTN:
               themeEditNew();
               break;

            case IDC_THEME_DEF_DEL_BTN:
               themeEditDelete();
               break;

            case IDC_THEME_STYLE_LIST_BOX:
               switch HIWORD(wParam) {
                  case LBN_SELCHANGE:
                     onStyleSelect();
                     break;
               }
               break;

            case IDC_THEME_STYLE_DOWN_BUTTON:
               moveStyleType(MOVE_DOWN);
               break;

            case IDC_THEME_STYLE_UP_BUTTON:
               moveStyleType(MOVE_UP);
               break;

            case IDC_THEME_STYLE_NEW_BTN:
               styleEditNew();
               break;

            case IDC_THEME_STYLE_DEL_BTN:
               styleEditDelete();
               break;

            case IDC_THEME_STYLE_DEF_ACCEPT_BTN:
               styleDefsAccept();
               break;

            case IDC_THEME_STYLE_DEF_RESET_BTN:
               fillStyleDefs();
               break;

            case IDCANCEL:
            case IDCLOSE:
               if (promptDiscardChangesNo()) return TRUE;

               display(FALSE);
               return TRUE;

            case IDC_THEME_DEF_SAVE_CONFIG_BTN:
               //SetCursor(LoadCursor(NULL, IDC_WAIT));
               //saveConfigInfo();
               //SetCursor(LoadCursor(NULL, IDC_ARROW));
               return TRUE;

            case IDC_THEME_DEF_RESET_BTN:
               //if (!promptDiscardChangesNo()) {
               //   configFile = L"";
               //   loadConfigInfo();
               //   fillThemes();
               //}
               break;

            case IDC_THEME_DEF_BACKUP_LOAD_BTN:
               //if (!promptDiscardChangesNo()) {
               //   wstring sBackupConfigFile;

               //   if (_configIO.queryConfigFileName(_hSelf, TRUE, TRUE, TRUE, sBackupConfigFile)) {
               //      configFile = sBackupConfigFile;
               //      loadConfigInfo();
               //      fillThemes();
               //      cleanConfigFile = FALSE;
               //      enableThemeSelection();
               //   }
               //}
               break;

            case IDC_THEME_DEF_BACKUP_VIEW_BTN:
               _configIO.viewBackupFolder();
               break;

            case IDC_THEME_DEF_CLONE_BTN:
               //cloneConfigInfo();
               break;

            case IDC_THEME_DEF_EXTRACT_BTN:
               //showEximDialog(TRUE);
               break;

            case IDC_THEME_DEF_APPEND_BTN:
               //showEximDialog(FALSE);
               break;
         }
         break;
   }

   return FALSE;
}

void ThemeDialog::localize() {
   SetWindowText(_hSelf, THEME_DIALOG_TITLE);
   SetDlgItemText(_hSelf, IDC_THEME_DEF_GROUP_BOX, THEME_DEF_GROUP_BOX);
   SetDlgItemText(_hSelf, IDC_THEME_DEF_DESC_LABEL, THEME_DEF_DESC_LABEL);
   SetDlgItemText(_hSelf, IDC_THEME_DEF_NEW_BTN, THEME_DEF_NEW_BTN);
   SetDlgItemText(_hSelf, IDC_THEME_DEF_ACCEPT_BTN, THEME_DEF_ACCEPT_BTN);
   SetDlgItemText(_hSelf, IDC_THEME_DEF_DEL_BTN, THEME_DEF_DEL_BTN);
   SetDlgItemText(_hSelf, IDC_THEME_STYLE_GROUP_BOX, THEME_STYLE_GROUP_BOX);
   SetDlgItemText(_hSelf, IDC_THEME_STYLE_NEW_BTN, THEME_STYLE_NEW_BTN);
   SetDlgItemText(_hSelf, IDC_THEME_STYLE_DEL_BTN, THEME_STYLE_DEL_BTN);
   SetDlgItemText(_hSelf, IDC_THEME_STYLE_DEF_GROUP_BOX, THEME_STYLE_DEF_GROUP_BOX);
   SetDlgItemText(_hSelf, IDC_THEME_STYLE_DEF_ACCEPT_BTN, THEME_STYLE_DEF_ACCEPT_BTN);
   SetDlgItemText(_hSelf, IDC_THEME_STYLE_DEF_RESET_BTN, THEME_STYLE_DEF_RESET_BTN);
   SetDlgItemText(_hSelf, IDC_THEME_DEF_SAVE_CONFIG_BTN, THEME_DIALOG_SAVE_BTN);
   SetDlgItemText(_hSelf, IDC_THEME_DEF_RESET_BTN, THEME_DIALOG_RESET_BTN);
   SetDlgItemText(_hSelf, IDC_THEME_DEF_BACKUP_LOAD_BTN, THEME_DIALOG_BKUP_LOAD_BTN);
   SetDlgItemText(_hSelf, IDC_THEME_DEF_BACKUP_VIEW_BTN, THEME_DIALOG_BKUP_VIEW_BTN);
   SetDlgItemText(_hSelf, IDC_THEME_DEF_CLONE_BTN, THEME_DIALOG_CLONE_BTN);
   SetDlgItemText(_hSelf, IDC_THEME_DEF_EXTRACT_BTN, THEME_DIALOG_EXTRACT_BTN);
   SetDlgItemText(_hSelf, IDC_THEME_DEF_APPEND_BTN, THEME_DIALOG_APPEND_BTN);
   SetDlgItemText(_hSelf, IDCLOSE, THEME_DIALOG_CLOSE_BTN);
}

void ThemeDialog::indicateCleanStatus() {
   if (cleanConfigFile) {
      SetDlgItemText(_hSelf, IDC_THEME_DEF_SAVE_CONFIG_BTN, THEME_DIALOG_SAVE_BTN);
      Utils::setFontRegular(_hSelf, IDC_THEME_DEF_SAVE_CONFIG_BTN);
   }
   else {
      SetDlgItemText(_hSelf, IDC_THEME_DEF_SAVE_CONFIG_BTN, (wstring(THEME_DIALOG_SAVE_BTN) + L"*").c_str());
      Utils::setFontBold(_hSelf, IDC_THEME_DEF_SAVE_CONFIG_BTN);
   }

   if (cleanThemeVals) {
      SetDlgItemText(_hSelf, IDC_THEME_DEF_ACCEPT_BTN, THEME_DEF_ACCEPT_BTN);
      Utils::setFontRegular(_hSelf, IDC_THEME_DEF_ACCEPT_BTN);
   }
   else {
      SetDlgItemText(_hSelf, IDC_THEME_DEF_ACCEPT_BTN, (wstring(THEME_DEF_ACCEPT_BTN) + L"*").c_str());
      Utils::setFontBold(_hSelf, IDC_THEME_DEF_ACCEPT_BTN);
   }

   if (cleanStyleDefs) {
      SetDlgItemText(_hSelf, IDC_THEME_STYLE_DEF_ACCEPT_BTN, THEME_STYLE_DEF_ACCEPT_BTN);
      Utils::setFontRegular(_hSelf, IDC_THEME_STYLE_DEF_ACCEPT_BTN);
   }
   else {
      SetDlgItemText(_hSelf, IDC_THEME_STYLE_DEF_ACCEPT_BTN, (wstring(THEME_STYLE_DEF_ACCEPT_BTN) + L"*").c_str());
      Utils::setFontBold(_hSelf, IDC_THEME_STYLE_DEF_ACCEPT_BTN);
   }
}

int ThemeDialog::addConfigInfo(int vIndex, const wstring& themeType) {
   ThemeType& TT = vThemeTypes[vIndex];

   TT.label = themeType;
   _configIO.getFullStyle(themeType, L"EOL", TT.eolStyle);

   wchar_t styleIndex[8];
   int styleCount;

   styleCount = stoi(_configIO.getStyleValue(themeType, L"Count"));

   TT.vStyleInfo.clear();
   TT.vStyleInfo.resize(styleCount);

   for (int i{}; i < styleCount; i++) {
      swprintf(styleIndex, 8, L"BFBI_%02i", i);
      _configIO.getFullStyle(themeType, styleIndex, TT.vStyleInfo[i]);
   }

   return styleCount;
}

int ThemeDialog::loadConfigInfo() {
   vector<wstring> themesList;
   wstring sThemes;
   int themesCount;

   sThemes = _configIO.getStyleValue(L"Base", L"Themes");
   themesCount = _configIO.Tokenize(sThemes, themesList);

   vThemeTypes.clear();
   vThemeTypes.resize(themesCount);

   for (int i{}; i < themesCount; i++) {
      wstring &themeType = themesList[i];
      addConfigInfo(i, themeType);
   }

   return static_cast<int>(vThemeTypes.size());
}

void ThemeDialog::fillThemes() {
   SendMessage(hThemesLB, LB_RESETCONTENT, NULL, NULL);

   for (const auto TT : vThemeTypes) {
      SendMessage(hThemesLB, LB_ADDSTRING, NULL, (LPARAM)TT.label.c_str());
   }

   if (vThemeTypes.size() > 0)
      SendMessage(hThemesLB, LB_SETCURSEL, 0, NULL);

   cleanConfigFile = TRUE;
   cleanThemeVals = TRUE;
   onThemeSelect();
}

int ThemeDialog::getCurrentThemeIndex() {
   int idxFT;

   idxFT = static_cast<int>(SendMessage(hThemesLB, LB_GETCURSEL, NULL, NULL));
   if (idxFT == LB_ERR) return LB_ERR;

   return idxFT;
}

int ThemeDialog::getCurrentStyleIndex() {
   int idxRec;

   idxRec = static_cast<int>(SendMessage(hStylesLB, LB_GETCURSEL, NULL, NULL));
   if (idxRec == LB_ERR) return LB_ERR;

   return idxRec;
}

bool ThemeDialog::getCurrentThemeInfo(ThemeType* &themeInfo) {
   int idxFT{ getCurrentThemeIndex() };
   if (idxFT == LB_ERR) return FALSE;

   themeInfo = &vThemeTypes[idxFT];
   return TRUE;
}

ThemeDialog::ThemeType ThemeDialog::getNewTheme() {
   ThemeType newTheme;

   newTheme.label = L"";
   newTheme.vStyleInfo = vector<StyleInfo>{ getNewStyle() };

   return newTheme;
}

void ThemeDialog::getThemeConfig(size_t idxTh, bool cr_lf, wstring &ftCode, wstring &ftConfig) {
   size_t recTypeCount;
   wchar_t fileTypeCode[60], recTypeCode[10];
   wstring new_line, rawCode, recTypes{}, rtConfig{}, recTypePrefix;

   ThemeType& FT = vThemeTypes[idxTh];
   new_line = cr_lf ? L"\r\n" : L"\n";

   rawCode = regex_replace(FT.label, wregex(L"( |,|=|\\[|\\])"), L"_").substr(0, 50);
   swprintf(fileTypeCode, 60, L"FT%03d_%s", static_cast<int>(idxTh + 1), rawCode.c_str());
   _configIO.ToUpper(fileTypeCode);

   recTypeCount = (FT.vStyleInfo.size() > 999) ? 999 : FT.vStyleInfo.size();

   for (size_t j{}; j < recTypeCount; j++) {
      //StyleInfo& RT = FT.vStyleInfo[j];

      swprintf(recTypeCode, 10, L"REC%03d", static_cast<int>(j + 1));
      recTypePrefix = wstring{ recTypeCode };
      recTypes += (j == 0 ? L"RecordTypes=" : L",") + recTypePrefix;

      //rtConfig += recTypePrefix + L"_FieldLabels=" + RT.backColor;
   }

   ftCode = wstring{ fileTypeCode };
   ftConfig = L"[" + ftCode + L"]" + new_line +
      L"FileLabel=" + FT.label + new_line +
      recTypes + new_line + rtConfig;
}

bool ThemeDialog::getCurrentStyleInfo(StyleInfo*& recInfo) {
   int idxFT{ getCurrentThemeIndex() };
   if (idxFT == LB_ERR) return FALSE;

   int idxRec{ getCurrentStyleIndex() };
   if (idxRec == LB_ERR) return FALSE;

   recInfo = &vThemeTypes[idxFT].vStyleInfo[idxRec];
   return TRUE;
}

StyleInfo ThemeDialog::getNewStyle() {
   StyleInfo newStyle;

   newStyle.backColor = 0;
   newStyle.foreColor = 0;
   newStyle.bold = 0;
   newStyle.italics = 0;
   return newStyle;
}

void ThemeDialog::onThemeSelect() {
   ThemeType* themeInfo;

   if (!getCurrentThemeInfo(themeInfo)) {
      ThemeType newTheme{ getNewTheme() };
      themeInfo = &newTheme;
   }

   loadingEdits = TRUE;
   SetDlgItemText(_hSelf, IDC_THEME_DEF_DESC_EDIT, themeInfo->label.c_str());
   loadingEdits = FALSE;

   enableMoveThemeButtons();
   fillStyles();
}

void ThemeDialog::enableMoveThemeButtons() {
   int idxFT{ getCurrentThemeIndex() };
   if (idxFT == LB_ERR) return;

   EnableWindow(GetDlgItem(_hSelf, IDC_THEME_DEF_DOWN_BUTTON),
      (idxFT < static_cast<int>(vThemeTypes.size()) - 1));
   EnableWindow(GetDlgItem(_hSelf, IDC_THEME_DEF_UP_BUTTON), (idxFT > 0));
}

void ThemeDialog::enableThemeSelection() {
   bool enable{ cleanThemeVals && cleanStyleVals && cleanStyleDefs };
   EnableWindow(GetDlgItem(_hSelf, IDC_THEME_DEF_LIST_BOX), enable);
   EnableWindow(GetDlgItem(_hSelf, IDC_THEME_DEF_NEW_BTN), enable);
   EnableWindow(GetDlgItem(_hSelf, IDC_THEME_DEF_CLONE_BTN), enable);
   EnableWindow(GetDlgItem(_hSelf, IDC_THEME_DEF_EXTRACT_BTN), enable);
   EnableWindow(GetDlgItem(_hSelf, IDC_THEME_DEF_APPEND_BTN), enable);

   if (enable) {
      enableMoveThemeButtons();
   }
   else {
      EnableWindow(GetDlgItem(_hSelf, IDC_THEME_DEF_DOWN_BUTTON), FALSE);
      EnableWindow(GetDlgItem(_hSelf, IDC_THEME_DEF_UP_BUTTON), FALSE);
   }

   indicateCleanStatus();
}

int ThemeDialog::moveThemeType(move_dir dir) {
   const int idxTheme{ getCurrentThemeIndex() };
   if (idxTheme == LB_ERR) return LB_ERR;

   switch(dir) {
      case MOVE_DOWN:
         if (idxTheme >= static_cast<int>(vThemeTypes.size()) - 1) return LB_ERR;
         break;

      case MOVE_UP:
         if (idxTheme == 0) return LB_ERR;
         break;

      default:
         return LB_ERR;
   }

   ThemeType currType = vThemeTypes[idxTheme];
   ThemeType &adjType = vThemeTypes[idxTheme + dir];

   vThemeTypes[idxTheme] = adjType;
   vThemeTypes[idxTheme + dir] = currType;

   SendMessage(hThemesLB, LB_DELETESTRING, (WPARAM)idxTheme, NULL);
   SendMessage(hThemesLB, LB_INSERTSTRING, (WPARAM)(idxTheme + dir),
      (LPARAM)vThemeTypes[idxTheme + dir].label.c_str());
   SendMessage(hThemesLB, LB_SETCURSEL, idxTheme + dir, NULL);

   cleanConfigFile = FALSE;
   indicateCleanStatus();
   enableMoveThemeButtons();

   return idxTheme + dir;
}

void ThemeDialog::fillStyles() {
   ThemeType* themeInfo;

   if (!getCurrentThemeInfo(themeInfo)) {
      ThemeType newTheme{ getNewTheme() };
      themeInfo = &newTheme;
   }

   wchar_t styleLabel[10];
   vector <StyleInfo>& styleList{ themeInfo->vStyleInfo };
   int styleCount{ static_cast<int>(styleList.size()) };

   SendMessage(hStylesLB, LB_RESETCONTENT, NULL, NULL);

   for (int i{}; i < styleCount; i++) {
      swprintf(styleLabel, 10, L"Style #%02i", i);
      SendMessage(hStylesLB, LB_ADDSTRING, NULL, (LPARAM)styleLabel);
   }

   SendMessage(hStylesLB, LB_ADDSTRING, NULL, (LPARAM)L"EOL Marker Style");
   SendMessage(hStylesLB, LB_SETCURSEL, 0, NULL);

   cleanStyleVals = TRUE;
   onStyleSelect();
}

void ThemeDialog::onStyleSelect() {
   int idxTheme{ getCurrentThemeIndex() };
   if (idxTheme == LB_ERR) return;

   int idxStyle{ getCurrentStyleIndex() };
   if (idxStyle == LB_ERR) return;

   EnableWindow(GetDlgItem(_hSelf, IDC_THEME_STYLE_DEL_BTN),
      (idxStyle < static_cast<int>(vThemeTypes[idxTheme].vStyleInfo.size())));
   enableMoveStyleButtons();
   fillStyleDefs();
}

void ThemeDialog::enableMoveStyleButtons() {
   int idxTheme{ getCurrentThemeIndex() };
   if (idxTheme == LB_ERR) return;

   int idxStyle{ getCurrentStyleIndex() };
   if (idxStyle == LB_ERR) return;

   int styleCount{ static_cast<int>(vThemeTypes[idxTheme].vStyleInfo.size()) };

   EnableWindow(GetDlgItem(_hSelf, IDC_THEME_STYLE_DOWN_BUTTON), (idxStyle < styleCount - 1));
   EnableWindow(GetDlgItem(_hSelf, IDC_THEME_STYLE_UP_BUTTON), (idxStyle > 0 && idxStyle < styleCount));
}

void ThemeDialog::enableStyleSelection() {
   bool enable{ cleanStyleVals && cleanStyleDefs };
   EnableWindow(GetDlgItem(_hSelf, IDC_THEME_STYLE_LIST_BOX), enable);
   EnableWindow(GetDlgItem(_hSelf, IDC_THEME_STYLE_NEW_BTN), enable);

   if (enable) {
      enableMoveStyleButtons();
   }
   else {
      EnableWindow(GetDlgItem(_hSelf, IDC_THEME_STYLE_DOWN_BUTTON), FALSE);
      EnableWindow(GetDlgItem(_hSelf, IDC_THEME_STYLE_UP_BUTTON), FALSE);
   }

   enableThemeSelection();
}

int ThemeDialog::moveStyleType(move_dir dir) {
   const int idxTheme{ getCurrentThemeIndex() };
   if (idxTheme == LB_ERR) return LB_ERR;

   const int idxStyle{ getCurrentStyleIndex() };
   if (idxStyle == LB_ERR) return LB_ERR;

   vector<StyleInfo>& styleList = vThemeTypes[idxTheme].vStyleInfo;

   switch (dir) {
   case MOVE_DOWN:
      if (idxStyle >= static_cast<int>(styleList.size()) - 1) return LB_ERR;
      break;

   case MOVE_UP:
      if (idxStyle == 0) return LB_ERR;
      break;

   default:
      return LB_ERR;
   }

   StyleInfo currType = styleList[idxStyle];
   StyleInfo& adjType = styleList[idxStyle + dir];

   styleList[idxStyle] = adjType;
   styleList[idxStyle + dir] = currType;

   TCHAR itemText[100];

   SendMessage(hStylesLB, LB_GETTEXT, (WPARAM)idxStyle, (LPARAM)itemText);
   SendMessage(hStylesLB, LB_DELETESTRING, (WPARAM)idxStyle, NULL);
   SendMessage(hStylesLB, LB_INSERTSTRING, (WPARAM)(idxStyle + dir), (LPARAM)itemText);
   SendMessage(hStylesLB, LB_SETCURSEL, idxStyle + dir, NULL);

   cleanConfigFile = FALSE;
   indicateCleanStatus();
   enableMoveStyleButtons();

   return idxStyle + dir;
}

void ThemeDialog::fillStyleDefs() {
   StyleInfo *recInfo;

   if (!getCurrentStyleInfo(recInfo)) {
      StyleInfo newRec{ getNewStyle() };
      recInfo = &newRec;
   }

   cleanStyleDefs = TRUE;
   enableStyleSelection();
}

void ThemeDialog::styleDefsAccept() {
   if (cleanStyleDefs) return;

   StyleInfo *recInfo;
   if (!getCurrentStyleInfo(recInfo)) return;


   cleanConfigFile = FALSE;
   cleanStyleDefs = TRUE;
   enableStyleSelection();
}

void ThemeDialog::styleEditNew() {
   int idxFT{ getCurrentThemeIndex() };
   if (idxFT == LB_ERR) return;

   StyleInfo newRec{ getNewStyle() };
   vector<StyleInfo> &records = vThemeTypes[idxFT].vStyleInfo;

   records.push_back(newRec);

   size_t moveTo = records.size() - 1;

   //SendMessage(hStylesLB, LB_ADDSTRING, NULL, (LPARAM)newStyle.label.c_str());
   SendMessage(hStylesLB, LB_SETCURSEL, (WPARAM)moveTo, NULL);
   onStyleSelect();

   cleanConfigFile = FALSE;
   cleanStyleVals = FALSE;
   enableStyleSelection();
}

int ThemeDialog::styleEditDelete() {
   int idxFT{ getCurrentThemeIndex() };
   if (idxFT == LB_ERR) return LB_ERR;

   int idxRec{ getCurrentStyleIndex() };
   if (idxRec == LB_ERR) return LB_ERR;

   vector<StyleInfo> &records = vThemeTypes[idxFT].vStyleInfo;
   records.erase(records.begin() + idxRec);

   int lastRec = static_cast<int>(records.size()) - 1;
   int moveTo = (idxRec <= lastRec - 1) ? idxRec : lastRec;

   SendMessage(hStylesLB, LB_DELETESTRING, (WPARAM)idxRec, NULL);
   SendMessage(hStylesLB, LB_SETCURSEL, (WPARAM)moveTo, NULL);

   cleanConfigFile = FALSE;
   cleanStyleVals = TRUE;
   onStyleSelect();

   return moveTo;
}

void ThemeDialog::themeEditAccept() {
   if (cleanThemeVals) return;

   int idxFT{ getCurrentThemeIndex() };
   if (idxFT == LB_ERR) return;

   ThemeType &fileInfo = vThemeTypes[idxFT];

   wchar_t fileVal[MAX_PATH + 1];

   GetDlgItemText(_hSelf, IDC_THEME_DEF_DESC_EDIT, fileVal, MAX_PATH + 1);
   fileInfo.label = fileVal;

   SendMessage(hThemesLB, LB_DELETESTRING, (WPARAM)idxFT, NULL);
   SendMessage(hThemesLB, LB_INSERTSTRING, (WPARAM)idxFT, (LPARAM)fileInfo.label.c_str());
   SendMessage(hThemesLB, LB_SETCURSEL, idxFT, NULL);

   cleanConfigFile = FALSE;
   cleanThemeVals = TRUE;
   enableThemeSelection();
}

int ThemeDialog::appendFileTypeConfigs(const wstring& sConfigFile) {
   wstring sections{};
   int sectionCount{};

   sectionCount = _configIO.getConfigSectionList(sections, sConfigFile);

   vector<wstring> sectionList{};
   sectionCount = _configIO.Tokenize(sections, sectionList);

   wstring sectionLabel{};
   int validCount{};
   for (int i{}; i < sectionCount; i++) {
      sectionLabel = _configIO.getConfigString(sectionList[i], L"FileLabel", L"", sConfigFile);
      if (sectionLabel.length() > 0) {
         ThemeType newFile{ getNewTheme() };

         vThemeTypes.push_back(newFile);
         addConfigInfo(static_cast<int>(vThemeTypes.size() - 1), sectionList[i]);
         SendMessage(hThemesLB, LB_ADDSTRING, NULL, (LPARAM)sectionLabel.c_str());
         validCount++;
      }
   }

   SendMessage(hThemesLB, LB_SETCURSEL, (WPARAM)(vThemeTypes.size() - 1), NULL);
   onThemeSelect();

   cleanConfigFile = FALSE;
   enableThemeSelection();

   return validCount;
}

void ThemeDialog::themeEditNew() {
   ThemeType newFile{ getNewTheme() };

   vThemeTypes.push_back(newFile);

   size_t moveTo = vThemeTypes.size() - 1;

   SendMessage(hThemesLB, LB_ADDSTRING, NULL, (LPARAM)newFile.label.c_str());
   SendMessage(hThemesLB, LB_SETCURSEL, (WPARAM)moveTo, NULL);
   onThemeSelect();

   cleanConfigFile = FALSE;
   cleanThemeVals = FALSE;
   enableThemeSelection();
}

int ThemeDialog::themeEditDelete() {
   int idxFT{ getCurrentThemeIndex() };
   if (idxFT == LB_ERR) return LB_ERR;

   vThemeTypes.erase(vThemeTypes.begin() + idxFT);

   int lastFile = static_cast<int>(vThemeTypes.size()) - 1;
   int moveTo = (idxFT <= lastFile - 1) ? idxFT : lastFile;

   SendMessage(hThemesLB, LB_DELETESTRING, (WPARAM)idxFT, NULL);
   SendMessage(hThemesLB, LB_SETCURSEL, (WPARAM)moveTo, NULL);

   cleanConfigFile = FALSE;
   cleanThemeVals = TRUE;
   onThemeSelect();

   return moveTo;
}

bool ThemeDialog::promptDiscardChangesNo() {
   if (!(cleanConfigFile && cleanThemeVals && cleanStyleVals && cleanStyleDefs)) {
      if (MessageBox(_hSelf, THEME_DISCARD_CHANGES, THEME_DIALOG_TITLE,
         MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDNO)
         return TRUE;
   }

   return false;
}

void ThemeDialog::saveConfigInfo() {
   if (!cleanStyleDefs) styleDefsAccept();
   if (!cleanThemeVals) themeEditAccept();

   size_t fileTypeCount;
   wstring fileData{}, fileTypes{}, ftCode{}, ftConfig{};

   fileTypeCount = (vThemeTypes.size() > 999) ? 999 : vThemeTypes.size();

   for (size_t i{}; i < fileTypeCount; i++) {
      getThemeConfig(i, TRUE, ftCode, ftConfig);
      fileTypes += (i == 0 ? L"" : L",") + ftCode;
      fileData += ftConfig + L"\r\n";
   }

   fileData = L"[Base]\r\nFileTypes=" + fileTypes + L"\r\n\r\n" + fileData;

   _configIO.backupConfigFile(TRUE);
   _configIO.saveConfigFile(fileData, TRUE);

   cleanConfigFile = TRUE;
   indicateCleanStatus();
   RefreshVisualizerPanel();
}

void ThemeDialog::cloneConfigInfo() {
   int idxFT{ getCurrentThemeIndex() };
   if (idxFT == LB_ERR) return;

   ThemeType& FT = vThemeTypes[idxFT];
   ThemeType NF{};

   NF.label = FT.label;

   size_t recCount = FT.vStyleInfo.size();
   NF.vStyleInfo.resize(recCount);

   for (size_t i{}; i < recCount; i++) {
      NF.vStyleInfo[i].backColor = FT.vStyleInfo[i].backColor;
      NF.vStyleInfo[i].foreColor = FT.vStyleInfo[i].foreColor;
      NF.vStyleInfo[i].bold = FT.vStyleInfo[i].bold;
      NF.vStyleInfo[i].italics = FT.vStyleInfo[i].italics;
   }

   vThemeTypes.push_back(NF);

   SendMessage(hThemesLB, LB_ADDSTRING, NULL, (LPARAM)NF.label.c_str());
   SendMessage(hThemesLB, LB_SETCURSEL, (WPARAM)(vThemeTypes.size() - 1), NULL);

   onThemeSelect();
   cleanConfigFile = FALSE;
   enableThemeSelection();
}

void ThemeDialog::showEximDialog(bool bExtract) {
   _eximDlg.doDialog((HINSTANCE)_gModule);
   _eximDlg.initDialog(bExtract);

   if (bExtract) {
      int idxFT{ getCurrentThemeIndex() };
      if (idxFT == LB_ERR) return;

      wstring ftCode{}, ftConfig{};
      getThemeConfig(idxFT, TRUE, ftCode, ftConfig);
      _eximDlg.setFileTypeData(ftConfig);
   }
}