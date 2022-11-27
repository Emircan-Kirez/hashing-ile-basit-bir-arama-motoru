/* Emircan Kirez - Ch3rry */
/* Son G�ncelleme: 27/11/2022 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define SIZE 255

//her bir kelimeyi linkli listede tutan yap�
struct wordNode{
	char word[20];
	char webAdr[35];
	struct wordNode *next;
};

struct fileContent{
	struct wordNode *head;
	struct wordNode *tail;
	int wordCount;
};

//her bir kelimenin ge�ti�i web adreslerini linkli listede tutan yap�
struct webAdrNode{
	char webAdr[35];
	struct webAdrNode *next;
};

//hash tablomun bir sat�r�ndaki struct yap�s�
struct wordKeyNode{
	char *word;
	struct webAdrNode *head;
	int tryCount;
	int webAdrCount;
};

struct hashTable{
	struct wordKeyNode *arr;
	int size;
};

int menu();
void findWord();
void detailedMod(struct hashTable *HashTable);
void readFile(struct fileContent *FileContent);
struct fileContent *allocateFileContent();
void insertWordNode(struct fileContent *FileContent, char *word, char *webAdr);
struct wordNode *createWordNode(char *word, char *webAdr);
int findWordNode(struct wordNode *head, char *word);
int isPrime(int n);
int nextPrime(int N);
struct hashTable *allocateHashTable(int size);
int getHornerKey(char *word);
void insertHashTable(struct wordNode *tmp, struct hashTable *HashTable);
struct wordKeyNode *search(char *word, struct hashTable *HashTable);
void oneWordSearch(char *word, struct hashTable *HashTable);
void andSearch(char *word1, char *word2, struct hashTable *HashTable);
void orSearch(char *word1, char *word2, struct hashTable *HashTable);
char *convertStringToLowercase(char *word);

int main(){
	struct fileContent *FileContent = allocateFileContent();
	readFile(FileContent);
	
	float loadFactor;
	printf("Load factor degerini giriniz: ");
	scanf("%f", &loadFactor);
	int minSize = (int)(FileContent->wordCount / loadFactor);
	struct hashTable *HashTable = allocateHashTable(nextPrime(minSize));
	
	//her bir kelimeyi hash tabloma ekliyorum ve gereksiz yere durmamas� i�in linkli listeden siliyorum
	struct wordNode *tmp;
	while(FileContent->head != NULL){
		insertHashTable(FileContent->head, HashTable);
		tmp = FileContent->head;
		FileContent->head = FileContent->head->next;
		free(tmp);
	}
	
	if(menu()){
		detailedMod(HashTable);
	}else{
		findWord(HashTable);
	}
	return 0;
}

int menu(){
	int option;
	printf("-----------------------\n");
	printf("� Normal modda calistirmak icin 0'a,\n� Detay modda calistirmak icin 1'e basiniz: ");
	scanf("%d", &option);
	return option;
}

//kullan�c�n�n girdi�i de�ere g�re arama yapan fonksiyon
void findWord(struct hashTable *HashTable){
	char line[SIZE];
	int option;
	char *word1, *word2, *andOr; 
	do{
		printf("Aramak istediginiz kelime(leri) yaziniz: ");
		fflush(stdin);
		scanf("%[^\n]", line);
		word1 = strtok(line, " ");
		andOr = strtok(NULL, " ");
		if(andOr != NULL){
			word2 = strtok(NULL, " ");
		}
		
		if(andOr == NULL){
			oneWordSearch(word1, HashTable);
		}else if(strcmp(andOr, "ve") == 0){
			andSearch(word1, word2, HashTable);
		}else if(strcmp(andOr, "veya") == 0){
			orSearch(word1, word2, HashTable);
		}else{
			printf("Hatali giris");
		}
		
		printf("Yeni kelime aramak istiyorsaniz 1'e, kapatmak icin 0'a basiniz: ");
		scanf("%d", &option);
	}while(option == 1);
	
	printf("Bye Bye :)");
}

void detailedMod(struct hashTable *HashTable){
	printf("------ Detayli Mod ------\n");
	printf("Hash tablosunun uzunlugu: %d\n\n", HashTable->size);
	
	printf("Hash Tablosunun Icerigi\n");
	int i;
	for(i = 0; i < HashTable->size; i++){
		printf("%d. siradaki kelime: %s Deneme sayisi: %d\n", i + 1, HashTable->arr[i].word, HashTable->arr[i].tryCount);
		struct webAdrNode *tmp = HashTable->arr[i].head;
		if(tmp != NULL){
			printf("%s kelimesinin bulundugu adresler:\n", HashTable->arr[i].word);
		}
		while(tmp != NULL){
			printf("%s\n", tmp->webAdr);
			tmp = tmp->next;
		}
		printf("\n");
	}
	
	findWord(HashTable);
}

//dosyay� bir kere okuyup linkli listede tutman� sa�layan fonksiyon
void readFile(struct fileContent *FileContent){
	FILE *fPtr = fopen("Sample.txt", "r");
	char line[SIZE], webAdr[35], *word;
	while(fgets(line, SIZE, fPtr)){
		line[strlen(line) - 1] = '\0'; //fgets ile okunan \n karakterini silme
		strcpy(webAdr, line); 
		fgets(line, SIZE, fPtr);
		if(line[strlen(line) - 1] == '\n') //fgets ile okunan \n karakterini silme
			line[strlen(line) - 1] = '\0';
		word = strtok(line, " \r"); //okuma yaparken \r diye bir �ey okudu�u i�in �al��m�yordu (debug ederken buldum). onu da almamas� i�in 
		while(word != NULL){
			insertWordNode(FileContent, convertStringToLowercase(word), webAdr);
			word = strtok(NULL, " \r");
		}
	}
	fclose(fPtr);
}

//file content i�in yer a��yorum
struct fileContent *allocateFileContent(){
	struct fileContent *tmp = (struct fileContent *)malloc(sizeof(struct fileContent));
	tmp->head = tmp->tail = NULL;
	tmp->wordCount = 0;
	return tmp;
}

//okudu�um kelimeyi file content i�erisine ekliyorum
void insertWordNode(struct fileContent *FileContent, char *word, char *webAdr){
	struct wordNode *newNode = createWordNode(word, webAdr);
	if(!findWordNode(FileContent->head, word)){ //kelime yoksa 
		FileContent->wordCount++;
	}
	
	if(FileContent->head == NULL){ //ilk eleman
		FileContent->head = FileContent->tail = newNode;
		return;
	}
	
	// en sona newNode'� ekle - s�ra bozulmas�n diye
	FileContent->tail->next = newNode;
	FileContent->tail = newNode;
}

//okudu�um kelime i�in node olu�turur
struct wordNode *createWordNode(char *word, char *webAdr){
	struct wordNode *tmp = (struct wordNode *)malloc(sizeof(struct wordNode));
	strcpy(tmp->word, word);
	strcpy(tmp->webAdr, webAdr);
	tmp->next = NULL;
	return tmp;
}

//kelimeyi linkli listede olup olmad���n� arar
int findWordNode(struct wordNode *head, char *word){
	struct wordNode *tmp = head;
	while(tmp != NULL && strcmp(tmp->word, word) != 0){
		tmp = tmp->next;
	}
	if(tmp == NULL){ //kelime yok, yani unique
		return 0;
	}else{
		return 1;
	}
}

//verilen say�n�n prime olup olmad���n� s�yler
int isPrime(int n){
    if (n <= 1)  
		return 0;
    if (n <= 3)  
		return 1;
   
    if (n%2 == 0 || n%3 == 0) 
		return 0;
   	
   	int i;
    for (i=5; i*i<=n; i=i+6)
        if (n%i == 0 || n%(i+2) == 0)
           return 0;
   
    return 1;
}

//bir sonraki next prime'� bulur
int nextPrime(int N){
 
    if (N <= 1)
        return 2;
 
    int prime = N;
    int found = 0;
 	
 	if(prime % 2 == 0){ //e�er �ift ise tek asal say� 2 oldu�u i�in bir fazlas�n� al�p kontrol ediyorum. Bu �ekilde while da +2 olarak ilerliyorum
 		prime++;
 		if(isPrime(prime))
 			found = 1;
	}
 	
    while (!found) {
        prime += 2;
 
        if (isPrime(prime))
            found = 1;
    }
 
    return prime;
}

//hash tablom i�in yer ay�r�r
struct hashTable *allocateHashTable(int size){
	struct hashTable *tmp = (struct hashTable *)malloc(sizeof(struct hashTable));
	tmp->arr = (struct wordKeyNode *)calloc(sizeof(struct wordKeyNode), size);
	tmp->size = size;
	return tmp;
}

//verilen kelimenin horner key de�erini d�nd�r�r
int getHornerKey(char *word){
	int hornerKey = 0, tmp, i;
	int length = strlen(word);
    
	for(i = 0; i < length; i++){
		tmp = word[i] - 'a' + 1;
		hornerKey = 31 * hornerKey + tmp;
	}
	if(hornerKey < 0) //say� int s�n�r�n� a��p negatif olduysa tekrar pozitif yapar�m
	 	hornerKey += INT_MAX;
	return hornerKey;
}

//hash tabloma kelime ekler
void insertHashTable(struct wordNode *tmp, struct hashTable *HashTable){
	int i = 0, adr;
	int key = getHornerKey(tmp->word);
	int tableSize = HashTable->size;
	adr = (key + i) % tableSize;
	while(HashTable->arr[adr].word != NULL && strcmp(HashTable->arr[adr].word, tmp->word) != 0 && i < tableSize){
		i++;
		adr = (key + i) % tableSize;
	}
	
	if(i == tableSize){
		printf("HashTable dolu");
		return;
	}
	
	HashTable->arr[adr].webAdrCount++;
	if(HashTable->arr[adr].word == NULL){
		//yeni kelimeyi ekle
		HashTable->arr[adr].word = (char *)malloc(sizeof(char) * (strlen(tmp->word) + 1));
		strcpy(HashTable->arr[adr].word, tmp->word);
		
		struct webAdrNode *newNode = (struct webAdrNode *)malloc(sizeof(struct webAdrNode));
		strcpy(newNode->webAdr, tmp->webAdr);
		newNode->next = NULL;
		
		HashTable->arr[adr].head = newNode;
		HashTable->arr[adr].tryCount = i + 1; //ayn� kelime farkl� web adreslerinde bulunsa da tryCount kelime ayn� oldu�u i�in ayn� olur
	}else{
		//olan kelimenin webadresini s�ral� ekle
		struct webAdrNode *newNode = (struct webAdrNode *)malloc(sizeof(struct webAdrNode));
		strcpy(newNode->webAdr, tmp->webAdr);
		newNode->next = NULL;
		
		//web adreslerini s�ral� bir �ekilde linkli listeye ekleme
		if(strcmp(HashTable->arr[adr].head->webAdr, newNode->webAdr) > 0){ //ba�a ekleme
			newNode->next = HashTable->arr[adr].head;
			HashTable->arr[adr].head = newNode;
			return;
		}
		
		struct webAdrNode *tmpNode = HashTable->arr[adr].head;
		while(tmpNode->next != NULL && strcmp(tmpNode->next->webAdr, newNode->webAdr) < 0){
			tmpNode = tmpNode->next;
		}
		
		newNode->next = tmpNode->next;
		tmpNode->next = newNode;
	}
	
}

// verilen kelimeyi hash tablosunda arar. E�er varsa ilgili struct'� d�nd�r�r
struct wordKeyNode *search(char *word, struct hashTable *HashTable){
	int i = 0, adr;
	char *tmpWord = convertStringToLowercase(word); //aranacak kelimeyi k���k harflere d�n��t�r�yorum
	int key = getHornerKey(tmpWord);
	int tableSize = HashTable->size;
	adr = (key + i) % tableSize;
	
	while(HashTable->arr[adr].word != NULL && strcmp(HashTable->arr[adr].word, tmpWord) != 0 && i < tableSize){
		i++;
		adr = (key + i) % tableSize;
	}
	
	if(HashTable->arr[adr].word == NULL || i == tableSize){
		return NULL;
	}
	
	free(tmpWord);
	return &(HashTable->arr[adr]);
}

//tek kelimelik arama yapar
void oneWordSearch(char *word, struct hashTable *HashTable){
	struct wordKeyNode *tmp = search(word, HashTable);
	if(tmp == NULL){
		printf("Kelime yok");
		return;
	}
		
		
	struct webAdrNode *tmpWebAdrNode = tmp->head;
	printf("%s Kelimesinin Bulundugu adres(ler): %d tane\n", word, tmp->webAdrCount);
	while(tmpWebAdrNode != NULL){
		printf("%s\n", tmpWebAdrNode->webAdr);
		tmpWebAdrNode = tmpWebAdrNode->next;
	}
	printf("\n");
}

//ve li arama yapar
void andSearch(char *word1, char *word2, struct hashTable *HashTable){
	struct wordKeyNode *wordKeyNode1 = search(word1, HashTable);
	struct wordKeyNode *wordKeyNode2 = search(word2, HashTable);
	
	if(wordKeyNode1 == NULL || wordKeyNode2 == NULL){
		printf("Kelime(ler) hash table da bulunmamaktadir.\n");
		return;
	}
	
	struct webAdrNode *webAdrNode1 = wordKeyNode1->head;
	struct webAdrNode *webAdrNode2 = wordKeyNode2->head;
	
	//ortak olan web adreslerini tutabilece�im char matris yap�m
	int size = wordKeyNode1->webAdrCount + wordKeyNode2->webAdrCount;
	char **intersectWebAdr = (char **)malloc(sizeof(char *) * size);
	int i = 0;
	while(webAdrNode1 != NULL && webAdrNode2 != NULL){
		if(strcmp(webAdrNode1->webAdr, webAdrNode2->webAdr) < 0){
			webAdrNode1 = webAdrNode1->next;
		}else if(strcmp(webAdrNode1->webAdr, webAdrNode2->webAdr) > 0){
			webAdrNode2 = webAdrNode2->next;
		}else{
			intersectWebAdr[i] = (char *)malloc(sizeof(char) * strlen(webAdrNode1->webAdr));
			strcpy(intersectWebAdr[i++], webAdrNode1->webAdr);
			webAdrNode1 = webAdrNode1->next;
			webAdrNode2 = webAdrNode2->next;
		}
	}
	
	if(i == 0){
		printf("Iki kelimenin birlikte gectigi yer yok.\n");
		return;
	}
	
	printf("%s ve %s kelimelerinin birlikte gectigi yerler: %d tane\n", word1, word2, i);
	int j;
	for(j = 0; j < i; j++){
		printf("%s\n", intersectWebAdr[j]);
	}
	printf("\n");
	
	//deallocate
	
	for(j = 0; j < size; j++){
		free(intersectWebAdr[j]);
	}
	free(intersectWebAdr);
}

// veya l� arama yapar
void orSearch(char *word1, char *word2, struct hashTable *HashTable){
	struct wordKeyNode *wordKeyNode1 = search(word1,  HashTable);
	struct wordKeyNode *wordKeyNode2 = search(word2,  HashTable);
	
	if(wordKeyNode1 == NULL || wordKeyNode2 == NULL){
		printf("Kelime(ler) hash table da bulunmamaktadir.\n");
		return;
	}
	
	struct webAdrNode *webAdrNode1 = wordKeyNode1->head;
	struct webAdrNode *webAdrNode2 = wordKeyNode2->head;
	
	//web adreslerini tutabilece�im char matris yap�m
	int size = wordKeyNode1->webAdrCount + wordKeyNode2->webAdrCount;
	char **unionWebAdr = (char **)malloc(sizeof(char *) * size);
	int i = 0;
	while(webAdrNode1 != NULL && webAdrNode2 != NULL){
		if(strcmp(webAdrNode1->webAdr, webAdrNode2->webAdr) < 0){
			unionWebAdr[i] = (char *)malloc(sizeof(char) * strlen(webAdrNode1->webAdr));
			strcpy(unionWebAdr[i++], webAdrNode1->webAdr);
			webAdrNode1 = webAdrNode1->next;
		}else if(strcmp(webAdrNode1->webAdr, webAdrNode2->webAdr) > 0){
			unionWebAdr[i] = (char *)malloc(sizeof(char) * strlen(webAdrNode2->webAdr));
			strcpy(unionWebAdr[i++], webAdrNode2->webAdr);
			webAdrNode2 = webAdrNode2->next;
		}else{
			unionWebAdr[i] = (char *)malloc(sizeof(char) * strlen(webAdrNode1->webAdr));
			strcpy(unionWebAdr[i++], webAdrNode1->webAdr);
			webAdrNode1 = webAdrNode1->next;
			webAdrNode2 = webAdrNode2->next;
		}
	}
	
	//web adresi kald�ysa geri kalanlar� ekle
	while(webAdrNode1 != NULL){
		unionWebAdr[i] = (char *)malloc(sizeof(char) * strlen(webAdrNode1->webAdr));
		strcpy(unionWebAdr[i++], webAdrNode1->webAdr);
		webAdrNode1 = webAdrNode1->next;
	}
	
	while(webAdrNode2 != NULL){
		unionWebAdr[i] = (char *)malloc(sizeof(char) * strlen(webAdrNode2->webAdr));
		strcpy(unionWebAdr[i++], webAdrNode2->webAdr);
		webAdrNode2 = webAdrNode2->next;
	}
	
	printf("%s ve %s kelimelerinin gectigi yerler: %d tane\n", word1, word2, i);
	int j;
	for(j = 0; j < i; j++){
		printf("%s\n", unionWebAdr[j]);
	}
	printf("\n");
	
	//deallocate
	for(j = 0; j < size; j++){
		free(unionWebAdr[j]);
	}
	free(unionWebAdr);
}

// verilen kelimeyi k���k harfli haline d�n��t�rme
char *convertStringToLowercase(char *word){
	char *tmpWord = (char *)malloc(sizeof(char) * (strlen(word) + 1));
	int i = 0;
	while (word[i]) {
        tmpWord[i] = tolower(word[i]);
        i++;
    }
    
  	//son harf olarak '\0' ekleme 
    tmpWord[i] = '\0';
    return tmpWord;
}
