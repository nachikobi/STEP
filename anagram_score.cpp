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
  int high_score = 0;
jump:
  while (getline(ifs, word)) {
    transform (word.begin (), word.end (), word.begin (), tolower); //小文字統一
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
    /* スコア計算 */
    int score = 0;
    for (int i = 0; i < word.length(); i++) {
      switch (word[i]) {
        case 'a': case 'b': case 'd': case 'e': case 'g': case 'i': case 'n': case 'o': case 'r': case 's': case 't': case 'u':
          score += 1;
          break;
        case 'c': case 'f': case 'h': case 'l': case 'm': case 'p': case 'v': case 'w': case 'y':
          score += 2;
          break;
        case 'j': case 'k': case 'x': case 'z':
          score += 3;
          break;
        case 'q':
          score += 3;
          i++;
          break;
      }
    }
    if (score > high_score) {
      word.copy(suggest, word.length());
      high_score = score;
    }
  }

  std::cout << suggest << '\n';
  return 0;
}
