# Son Güncelleme: 27/11/2022
• Bu projede, hashing kullanarak basit bir arama motoru gerçekleştirilmiştir.<br/>
• Kelimeleri ve geçtiği web sitelerini programa ekleyebilmek için "Sample.txt" dosyası kullanılmıştır.<br/>
• Hash tablomuzda her bir satırda ilgili kelime ve geçmiş olduğu web siteleri birlikte yer almaktadır. Struct yapısı aşağıdaki gibidir.<br/>
```C
struct wordKeyNode{
	char *word; //kelimemiz
	struct webAdrNode *head; //geçmiş olduğu web sitelerinin linkli liste şeklinde tutulması
	int tryCount; //tabloya yerleştirmek için kaç defa denendi?
	int webAdrCount; //kaç web sitesinde geçti?
};

struct webAdrNode{
	char webAdr[35];
	struct webAdrNode *next;
};
```

