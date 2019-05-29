#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct originNode{
  char origin_word[17];
  struct originNode *next;
}originNode;

typedef struct dicNode {
  char sorted_word[17];
  int word_length;
  struct originNode *origin;
  struct dicNode *left;
  struct dicNode *right;
}dicNode;

static dicNode *head=NULL;

void open_dictionary(int *alph) { //任意の入力文字の組み合わせで構成されている単語を抽出
  FILE *fp, *ofp;
  int i, alphcpy[27];
  char word[17];
  fp = fopen("https://icanhazwordz.appspot.com/dictionary.words", "r");
  ofp = fopen("mydic.txt","w+");

  if (fp != NULL) {
    while (fgets(word, sizeof(word), fp)!=NULL) {
      /*1文字目が大文字かどうか判定して大文字なら小文字に入れ直す*/
      if (word[0]>='A'&&word[0]<='Z') word[0] += 32;

      for (int j = 0; j < 27; j++) alphcpy[j]=alph[j];
      for (i = 0; word[i]!='\n'; i++) {
        int n = word[i]-'a';
        if (alphcpy[n]>=1) alphcpy[n]--;
        else break;
      }
      if (i==(strlen(word)-1)) fputs(word,ofp);
    }
  } else {  //エラー処理
    fprintf(stderr, "Usage: Can't open file!\n");
    exit(2);
  }
  fclose(fp);
  fclose(ofp);
}

void merge(char *sorted, int left, int right) {
  int i, j, k, mid;
  char work[17];  // 作業用配列
  if (left < right) {
    mid = (left + right)/2;
    merge(sorted, left, mid);
    merge(sorted, mid+1, right);
    for (i = mid; i >= left; i--) work[i] = sorted[i];
    for (j = mid+1; j <= right; j++) work[right-(j-(mid+1))] = sorted[j];
    i = left; j = right;
    for (k = left; k <= right; k++) {
      if (work[i] < work[j]) sorted[k] = work[i++];
      else                   sorted[k] = work[j--];
    }
  }
}

originNode* originNEW(char *origin_word) {
  originNode *x = malloc(sizeof *x);
  if (x == NULL) {
    fprintf(stderr, "Usage: malloc\n");
    exit(2);
  }
  strcpy(x->origin_word,origin_word);
  x->next = NULL;
  return x;
}

dicNode* NEW(char *sorted_word, char *origin_word) {
  dicNode *x = malloc(sizeof *x);
  if (x == NULL) {
    fprintf(stderr, "Usage: malloc\n");
    exit(2);
  }
  strcpy(x->sorted_word, sorted_word);
  x->word_length = strlen(sorted_word)-1;
  x->origin = originNEW(origin_word);
  x->left = NULL;
  x->right = NULL;
  return x;
}

void origin_insert(originNode **node,char *origin_word) {
  if ((*node)==NULL) {
    strcpy((*node)->origin_word,origin_word);
  }
  else origin_insert(&(*node)->next,origin_word);
}

void insert(dicNode **node, char *sorted_word, char *origin_word) {  //単語の文字数でソート
  if ((*node)==NULL) { //新しいノードを作成
    *node = NEW(sorted_word, origin_word);
  } else {
    if ((*node)->word_length==(strlen(sorted_word)-1)) {
      if (strcmp((*node)->sorted_word, sorted_word)==0) origin_insert(&(*node)->origin, origin_word);
      else if (strcmp((*node)->sorted_word, sorted_word)>0) insert(&(*node)->left, sorted_word, origin_word);
      else insert(&(*node)->right, sorted_word, origin_word);
    } else if ((*node)->word_length>(strlen(sorted_word)-1)) insert(&(*node)->left, sorted_word, origin_word);
    else insert(&(*node)->right, sorted_word, origin_word);

  }
}


int main(int argc, char *argv[]) {
  int alph[26];
  char word[17], sorted_word[17];
  FILE *fp;
  for (int i = 0; i < 26; i++) alph[i] = 0;

  /*エラー処理*/
  if (argc < 2) {
    fprintf(stderr, "Usage: not exact input!\n");
    exit(2);
  }
  /*入力文字の出現回数を記録*/
  for (int i = 1; argv[i] != NULL; i++) {
    alph[*argv[i]-'a']++;
  }
  open_dictionary(alph);

  fp = fopen("mydic.txt","r");

  while (fgets(word, sizeof(word), fp)!=NULL) {
    /*単語をソート*/
    strcpy(sorted_word,word);
    merge(sorted_word, 0, strlen(sorted_word)-2);

    /*単語を辞書木に追加*/
    insert(&head,sorted_word,word);
  }

  /*辞書木の出力*/

  return 0;
}
