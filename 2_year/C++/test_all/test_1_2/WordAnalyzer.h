#pragma once

using namespace System;
using namespace System::Collections::Generic;

public ref class WordAnalyzer {
private:
    array <System::String^>^ words;  

    bool isConsonant(wchar_t letter) {
        wchar_t lowerLetter = Char::ToLower(letter);

        String^ russianConsonants = L"בגדהזחיךכלםןנסעפץצקרש";
        String^ englishConsonants = L"bcdfghjklmnpqrstvwxyz";

        return russianConsonants->IndexOf(lowerLetter) != -1 ||
            englishConsonants->IndexOf(lowerLetter) != -1;
    }

public:
    void setTextForAnalyze(System::String^ text) {
        System::String^ delimStr = " ,.!:;?()[]{}<>\"'\t\n\r-";

        words = text->Split(delimStr->ToCharArray(), StringSplitOptions::RemoveEmptyEntries);
    }

    System::String^ getWordsEndingWithConsonant() {
        System::String^ resultText = "";

        for (int i = 0; i < words->Length; i++) {
            if (words[i]->Length > 0) {
                wchar_t lastChar = words[i][words[i]->Length - 1];
                if (isConsonant(lastChar)) {
                    resultText = resultText + " " + words[i];
                }
            }
        }

        if (resultText->Length > 0 && resultText[0] == ' ') {
            resultText = resultText->Substring(1);
        }

        return resultText;
    }

    System::String^ getWordsEndingWithConsonant(int N) {
        System::String^ resultText = "";
        int countFindWords = 0; 

        for (int i = 0; i < words->Length; i++) {
            if (countFindWords >= N) break;

            if (words[i]->Length > 0) {
                wchar_t lastChar = words[i][words[i]->Length - 1];
                if (isConsonant(lastChar)) {
                    resultText = resultText + " " + words[i];
                    countFindWords++;
                }
            }
        }

        if (resultText->Length > 0 && resultText[0] == ' ') {
            resultText = resultText->Substring(1);
        }

        return resultText;
    }
};