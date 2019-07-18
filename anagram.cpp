#include <iostream>
#include <vector>
#include <string>
#include <fstream>


int main() {
  char alph;
  std::vector<int> alph_count(26,0);
  for (int i = 1; i <= 16; i++) {
    std::cout << i << "文字目を入れてください>";
    std::cin >> alph;
    alph_count[alph-'a']++;
    /*デバッグ
    std::cout << "abcdefghijklmnopqrstuvwxyz" << '\n';
    for (int j = 0; j < 26; j++) {
      std::cout << alph_count[j];
    }
    std::cout << '\n';
    デバッグ終わり */
  }

  std::ifstream ifs("dictionary.txt");
  std::string word;
  if (ifs.fail()) {
    std::cerr << "Failed to open file." << std::endl;
    return -1;
  }
  /* ファイル読み込み */
  char suggest[20]={};
jump:
  while (getline(ifs, word)) {
    transform (word.begin (), word.end (), word.begin (), tolower); //小文字統一
    if (strlen(suggest)<word.length()) {
      /* anagram判定 */
      std::vector<int> alph_check(alph_count);
      for (int i = 0; i < word.length(); i++) {
        if (alph_check[word[i]-'a']>0) {
          if (word[i]=='q') { //Qu対応
            if (word[i+1]=='u') {
              alph_check[word[i]-'a']--;
              i++;
            } else goto jump;
          } else alph_check[word[i]-'a']--;
        } else goto jump;
      }
      word.copy(suggest, word.length());
    }
  }

  std::cout << suggest << '\n';
  return 0;
}
