#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct originNode{
  char origin_word[20];
  struct originNode *next;
}originNode;

typedef struct dicNode {
  char sorted_word[20];
  int word_length;
  int score;
  struct originNode *origin;
  struct dicNode *left;
  struct dicNode *right;
}dicNode;

static dicNode *head=NULL;

void open_dictionary(int *alph) { //任意の入力文字の組み合わせで構成されている単語を抽出
  FILE *fp, *ofp;
  int i, alphcpy[27];
  char word[20];

  fp = fopen("dictionary.txt", "r");
  ofp = fopen("mydic.txt","w+");

  if (fp != NULL) {
    while (fgets(word, sizeof(word), fp)!=NULL) {
      if (word[0]!='\n') {  //改行のみの行は飛ばす
        /*大文字かどうか判定して大文字なら小文字に入れ直す*/
        for (int j = 0; word[j]!='\n'; j++) {
          if (word[j]>='A'&&word[j]<='Z') word[j] += 32;
        }

        for (int j = 0; j < 27; j++) alphcpy[j]=alph[j];
        for (i = 0; word[i]!='\n'; i++) {
          int n = word[i]-'a';
          if (alphcpy[n]>=1) alphcpy[n]--;
          else {
            /*配列の初期化*/
            for (int j = 0; j < 20; j++) {
              word[j]='\0';
            }
            break;
          }
          if (word[i]=='q'&&word[i+1]=='u') i++;
        }
        if (i==(strlen(word)-1)) fputs(word,ofp);
        /*配列の初期化*/
        for (int j = 0; j < 20; j++) {
          word[j]='\0';
        }
      }
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

int calc_score(char *word) {
  int score = 0;
  for (int i = 0; word[i]!='\n'; i++) {
    if (word[i]=='a'||word[i]=='b'||word[i]=='d'||word[i]=='e'||word[i]=='g'||word[i]=='i'||word[i]=='n'||word[i]=='o'||word[i]=='r'||word[i]=='s'||word[i]=='t'||word[i]=='u') {
      score++;
    } else if (word[i]=='c'||word[i]=='f'||word[i]=='h'||word[i]=='l'||word[i]=='m'||word[i]=='p'||word[i]=='q'||word[i]=='v'||word[i]=='w'||word[i]=='y') {
      score = score+2;
    } else {
      score = score+3;
    }
  }
  return score;
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

dicNode* NEW(char *sorted_word, char *origin_word, int score) {
  dicNode *x = malloc(sizeof *x);
  if (x == NULL) {
    fprintf(stderr, "Usage: malloc\n");
    exit(2);
  }
  strcpy(x->sorted_word, sorted_word);
  x->word_length = strlen(sorted_word)-1;
  x->score = score;
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

void insert(dicNode **node, char *sorted_word, char *origin_word) {  //単語のscoreでソート
  int score = calc_score(sorted_word);
  if ((*node)==NULL) { //新しいノードを作成
    *node = NEW(sorted_word, origin_word, score);
  } else {
    if ((*node)->score==score) {
      if (strcmp((*node)->sorted_word, sorted_word)==0) origin_insert(&(*node)->origin, origin_word);
      else if (strcmp((*node)->sorted_word, sorted_word)>0) insert(&(*node)->left, sorted_word, origin_word);
      else insert(&(*node)->right, sorted_word, origin_word);
    } else if ((*node)->score>score) insert(&(*node)->left, sorted_word, origin_word);
    else insert(&(*node)->right, sorted_word, origin_word);

  }
}

void print(dicNode *now) {
  if (now == NULL) return;

  print(now->left);
  printf("score=%d %s\n", now->score, now->origin->origin_word);
  print(now->right);
}


int main(int argc, char *argv[]) {
  int alph[26];
  char word[20], sorted_word[20];
  FILE *fp;
  for (int i = 0; i < 26; i++) alph[i] = 0;

  /*エラー処理*/
  if (argc < 2) {
    fprintf(stderr, "Usage: not exact input!\n");
    exit(2);
  }
  /*入力文字の出現回数を記録*/
  for (int i = 1; argv[i] != NULL; i++) {
    if (strcmp(argv[i],"qu")==0) {
      alph['q'-'a']++;
    } else {
      alph[*argv[i]-'a']++;
    }
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
  print(head);

  return 0;
}
