/**
@file

Bu uygulamada verilen input dosyasindaki film ve aktor bilgisinden faydalanilarak graf olusturulmus,
olusturulan graf uzerinde BFS ve Backtracking yapilarak,
her bir aktorun Kevin Bacon sayisi hesaplanmis ve path olusturulmustur.

@author

Name 			 :		Muhammed Yasin SAGLAM
Student No		 :	 	15011804
Date 			 :		03/12/2017
E-Mail			 :		myasinsaglam1907@gmail.com
Compiler Used	 :		GCC
IDE				 :		DEV-C++(Version 5.11)
Operating System :		Windows 10 educational edition
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Act_SIZE
#define Film_Num 46//193//14129 //input dosyalarindaki film sayilari yani satir sayilari
#define LINE_BUF 3800
#define NAME_BUF 150
#define FNAME_BUF 500
#define file1 "input-1.txt" //ilgili input dosyasini tutan makro
#define F_FILMS "films1.txt" //output dosyasi filmleri ve id lerini tutar
#define F_ACTORS "actors1.txt" //output dosyasi aktorleri ve id lerini tutar
#define F_GRAPH "graph1.txt"  //input olarak verilen dosyanin id lerle olusturulmus hali string operasyonlarindan kurtulmak icin
#define delim "/"

char films[Film_Num][FNAME_BUF]; //film isimlerini tutan dizi
char **actors; //aktorleri tutan pointer
int asize=0; //toplam tekrarsiz aktor sayisi

typedef struct{
    int f_id;
    int *actors;
    int actor_num;
}PLAY;
//verilen filmde hangi aktorlerin oynadigini tutan yapi
PLAY playing[Film_Num];

//aktorlerin oynadiklari filmleri tutan yapi komsuluk olusturmada kullanilacaktir
typedef struct{
    int a_id;
    int *films;
    int film_num;
}PLAYA;
PLAYA *actor_film;

//grafin 1 node u dugum olarak aktor id si kenar olarak ise birlikte oynadiklari film id si tutulmaktadir
struct node{
    int a_id; //aktor id si
    int f_id; //film id si
    int kbnum; //kbnumarasi
    struct node* next; //graf icin sonraki dugumun adresi
};

//Tanimlanan Graph yapisinda komsuluk listeleri , dugum sayisi ve ziyaret edilen dugumler tutulmaktadir.
struct Graph{
    int numVertices; //dugum sayisi
    struct node** adjLists; //komsuluk listesi
    int *visited; //ziyaret edilen dugumler
};

//node initialize eden, olusturan fonksiyon grafa ekleme yaparken kullanilmistir.Parametre olarak aktor idsi ve film id si alir
struct node* createNode(int actor,int film){
    struct node* New = malloc(sizeof(struct node));
    New->a_id=actor;
    New->f_id=film;
    New->next=NULL;
    New->kbnum=0;
    return New;
}

//graph olusturan fonksiyon
struct Graph* createGraph(int vertices){
    int i;
    struct Graph* graph= malloc(sizeof(struct Graph));
    graph->numVertices=vertices;
    graph->adjLists=malloc(vertices * sizeof(struct node*));
    graph->visited=malloc(sizeof(int)*vertices);
    for(i=0;i<vertices;i++){
        graph->adjLists[i]=NULL;
        graph->visited[i]=0;
    }
    return graph;
}

//grafa dugum ekleyen fonksiyon
void addEdge(struct Graph* graph, int v1,int e1,int v2, int e2){
    struct node* New = createNode(v2,e2);
    New->next=graph->adjLists[v1];
    graph->adjLists[v1]=New;

    New = createNode(v1,e1);
    New->next=graph->adjLists[v2];
    graph->adjLists[v2]=New;
}



//kuyruk elemanlarinin yapisi olusturuluyor
typedef struct queue_element{
    struct node value;
    struct queue_element *next;
}queue_element;

//kuyruk yapisi
struct Queue {
    struct queue_element *front, *rear;
};
 //kuyruk icin eleman olusturup donduren fonksiyon
struct queue_element* newElement(struct node * node){
    struct queue_element *temp = (struct queue_element*)malloc(sizeof(struct queue_element));
    temp->value=*node;
    temp->next=NULL;
    return temp;
}
//kuyruk olusturan fonksiyon
struct Queue* createQueue(){
    struct Queue *q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front=q->rear=NULL;
    return q;
}
//kuyruk bos mu kontrolu yapan fonksiyon
int isEmpty(struct Queue *q){
    if(q->front==NULL){
        //printf("\nQueue is empty");
        return 1;
    }
    else{
        return 0;
    }
}
//kuyruga eleman ekleyen fonksiyon
void enqueue(struct Queue *q, struct node *value){
    struct queue_element *element= newElement(value);
    element->value=*value;
    element->next=NULL;
    if (q->rear==NULL){//kuyruk bossa
        //ilk eleman demektir
        q->front=q->rear=element;
    }
    else{ //bos degilse sona eklenecektir
        q->rear->next=element;
        q->rear=element;
    }
}
//kuyruktan eleman ceken fonksiyon
struct queue_element* dequeue(struct Queue *q){
    if(q->front==NULL){//kuyruk bossa null dondur
        return NULL;
    }
    struct queue_element *temp = q->front; //eski ilk elemanin adresini sakla
    q->front=q->front->next; //elemani kuyruktan cikar
    if(q->front== NULL) //eger kuyruk bosaldiysa rear i da null yap
        q->rear=NULL;
    return temp;
}

//BFS ve aranan eleman bulunursa backtrack ederek path i yazdiran fonksiyon
void bfSearch(struct Graph *graph, int startActorIndex, int searchIndex){
    int i,j;
    int count =0;
    int not_found=1;
    int *backtrack=malloc(sizeof(int));
    struct Queue *queue =createQueue(); //kuyruk olusturuluyor
    graph->visited[startActorIndex]=-1; //kevin bacon icin visited dizisinde -1 yazar
    //printf("\nFirst actor id > %d Name > %s",startActorIndex,actors[startActorIndex]);
    struct node first;
    first.a_id=startActorIndex;
    first.kbnum=0;
    first.next=NULL;
    enqueue(queue,&first);
    while(!isEmpty(queue)){
        //kuyrugu yazdir burada biyerde
        struct queue_element *temp = dequeue(queue); //kuyruktan elemani cek
        backtrack=realloc(backtrack, sizeof(int)*(count+1));
        backtrack[count++]=temp->value.a_id;

        //printf("\nVisited actor id > %d Name > %s ",temp->value.a_id,actors[temp->value.a_id]);
        struct node *iter = graph->adjLists[temp->value.a_id]; //ilk komsusunu al
        while(iter){
            if(graph->visited[iter->a_id]==0){ //kuyruktan cekilenin komsusu ziyaret edilmemisse
                iter->kbnum = temp->value.kbnum+1;
                graph->visited[iter->a_id]=iter->kbnum; //visited icinde kb numberlar saklanir 0 ise ziyaret edilmemis demektir
                enqueue(queue,iter); //ekle queue ya
            }
            //backtracking
            if(searchIndex==iter->a_id && graph->visited[iter->a_id]==iter->kbnum){
                //printf("\nFound...\n\nactor id > %d actor name > %s Film is > %s Kb number is %d\n\n",iter->a_id,actors[iter->a_id],films[iter->f_id],iter->kbnum);
                not_found=0;
                printf("\n%s 's Kevin Bacon number is %d\n",actors[iter->a_id],iter->kbnum);
                int a1_id = iter->a_id; //alt agda bulunan aktor id si
                int a1_kb=iter->kbnum; //alt agda bulunan aktor kb no su
                int a2_id;
                int a2_kb;
                struct node *temp;
                for(i=0;i<count;i++){
                    int found=0;
                    int tmp_id; //komsu aktor id si
                    a2_id=backtrack[count-i-1]; //queueden cekilen id ler
                    a2_kb=graph->visited[a2_id];
                    struct node *temp = graph->adjLists[a2_id]; //komsu aktorun  komsuluklari temp node un da tutulur
                    if(a2_kb ==(a1_kb-1) || a2_kb==-1){ //eger komsu aktorun kb nosu kucukse bulunan aktorun kb nosundan 1 kucukse ve
                        while(temp && !found){
                            tmp_id = temp->a_id; //id lere bak
                            if(tmp_id == a1_id){
                                printf("%s-%s : %s \n",actors[a1_id],actors[a2_id],films[temp->f_id]);
                                found=1;
                                a1_id=a2_id;
                                a1_kb=a2_kb;
                            }
                            temp=temp->next;
                        }
                    }
                }
                system("PAUSE");
            }
            //end of backtracking
            iter=iter->next;
        }
    }
    if(not_found){
        printf("\nSonsuz\n");
        system("PAUSE");
    }
    system("CLS");
    printf("RESULTS");
    int frequency[13];
//    if(graph->visited[searchIndex]==0){
//        printf("\nsonsuz");
//        system("PAUSE");
//    }
//    for(i=0;i<asize;i++){
//        printf("\n%-5d.%-30s KB:%d",i,actors[i],graph->visited[i]);
//    }
    for(i=0;i<13;i++)
        frequency[i]=0;
    for(i=0;i<13;i++){
        for(j=0;j<asize;j++){
            if(graph->visited[j]==i)
                frequency[i]++;
        }
    }
    printf("\nSonsuz olanlarin sayisi %d ",frequency[0]);
    for(i=1;i<13;i++){
        printf("\nKB = %d olanlarin sayisi %d",i,frequency[i]);
    }
}



//aktor ve filmleri dosyaya yazan ve okuyan fonksiyonlar
int create_Actors(){
    FILE *fp,*fpgr;
    char *line=calloc(LINE_BUF,sizeof(char));
    char *tok;
    int fsize=0;
    int i,k=0;
    int lc; //line counter cevrim degiskeni
    actors=(char**)malloc(sizeof(char*));
    actors[0]=(char*)malloc(sizeof(char)*NAME_BUF);
    fpgr=fopen(F_GRAPH,"w");
    if(!fpgr){
        printf("\nFile not opened. Quitting...");
        exit(0);
    }
    fp=fopen(file1,"rb");
    if(!fp){
        printf("\nFile not opened. Quitting...");
        exit(0);
    }
//    while(fgets(line, sizeof(char)*LINE_BUF,fp)!=NULL){
//       printf("%s",line);
//    }
    for(lc=0;lc<Film_Num;lc++){ //dosyanin sonuna gelene kadar
        fgets(line, sizeof(char)*LINE_BUF,fp); //satir oku
        line[strlen(line)-1]='\0';
        //printf("%s\n",line);
        //system("PAUSE");
        tok=strtok(line,delim); //ilk elemani (filmadi) / a gore parcala
        //printf("\nFilm adi : %s",tok);
        fprintf(fpgr,"%d",k++);
        strcpy(films[fsize++],tok); //ilk elemani filmler dizisine yerlestir.
        tok = strtok(NULL ,delim);
        while (tok != NULL) { //oyuncu adlari icin satir sonuna kadar / a gore parcalamaya devam et
            // printf("\nOyuncu adi : %s",tok);
            i=0;
            while(i<asize && strncmp(actors[i],tok,NAME_BUF)!=0) //actor onceden eklenmis mi
                i++;
            if(i==asize){ //eklenmemisse
                asize++; //actor dizisinin boyutunu artir
                actors=realloc(actors,asize*sizeof(char*)); //bellekte realloc ile yer ayir
                actors[asize-1]=(char*)malloc(sizeof(char)*NAME_BUF); //isim icinde yer ac
                strcpy(actors[asize-1],tok); //son goze actoru ekle
                // printf("\nActors %d : %s",asize,actors[asize-1]);
                fprintf(fpgr," %d",i);

            }
            else{
                fprintf(fpgr," %d",i);
            }
            tok = strtok(NULL ,delim); //oyuncu adini tok a al
        }
        fputc('\n',fpgr);
    }
    //printf("\nLine num %d",lc);
    fclose(fp);
    fclose(fpgr);
}

int create_files(){
    int i,j;
    FILE *fp1,*fp2;
    fp1=fopen(F_FILMS,"w");
    if(!fp1){
        printf("File not opened to write!!! Quitting...");
        exit(0);
    }
    for(i=0;i<Film_Num;i++) {
        //printf("\nFilm %d. : %s\n",i+1, films[i]);
        fprintf(fp1,"%d-%s\n",i,films[i]);
    }
    fclose(fp1);
    fp2=fopen(F_ACTORS,"w");
    if(!fp2){
        printf("File not opened to write!!! Quitting...");
        exit(0);
    }
    fprintf(fp2,"%d\n",asize);
    for(j=0;j<asize;j++){
        //printf("\t%d. %s\n",j+1,actors[j]);
        fprintf(fp2,"%d-%s\n",j,actors[j]);
    }
    fclose(fp2);
    return 0;
}

//dosyadan okuma yapar aktor ve filmleri kendi olusturdugum dosyadan
int read_from_files(){
    FILE *fp1,*fp2,*fp3;
    char *line=calloc(LINE_BUF,sizeof(char));
    char *tok;
    int i=0,j=0;
    int k;
    //aktorler okunuyor
    fp2=fopen(F_ACTORS,"r");
    if(!fp2){
        printf("File error!!! Quitting");
        exit(0);
    }
    fscanf(fp2,"%d",&asize);
    fgets(line,sizeof(char)*LINE_BUF,fp2);
    actors=(char**)(char**)malloc(sizeof(char*)*asize);
    for(i=0;i<asize;i++){
        //fgets BURADAN DEVAM ET DOSYAYA YAZIM DEGISECEK YADA TUMUYLE KAYIT ETME
        fgets(line,sizeof(char)*LINE_BUF,fp2);
        line[strlen(line)-1]='\0';
        tok=strtok(line,"-");
        tok=strtok(NULL,"-");
        while(tok!=NULL){
            actors[i]=(char*)malloc(sizeof(char)*NAME_BUF);
            strcpy(actors[i],tok);
            //printf("\n %d. actor >  %s ",i,actors[i]);
            tok=strtok(NULL,"-");
        }
    }
    fclose(fp2);
    //filmler okunuyor
    fp3=fopen(F_FILMS,"r");
    if(!fp3){
        printf("File error!!! Quitting");
        exit(0);
    }
    for(i=0;i<Film_Num;i++){
        fgets(line,sizeof(char)*LINE_BUF,fp3);
        line[strlen(line)-1]='\0';
        tok=strtok(line,"-");
        tok=strtok(NULL,"-");
        while(tok!=NULL){
            strcpy(films[i],tok);
            //printf("\n %d. film >  %s ",i,films[i]);
            tok=strtok(NULL,"-");
        }
    }
    fclose(fp3);
    //grafin inputtaki hali okunuyor
    fp1=fopen(F_GRAPH,"r");
    if(!fp1){
        printf("File error!!! Quitting");
        exit(0);
    }
    for(i=0;i<Film_Num;i++){
        fgets(line,sizeof(char)*LINE_BUF,fp1);
        line[strlen(line)-1]='\0';
        tok=strtok(line," ");
        playing[i].f_id=atoi(tok);
        //printf("\nfilm %d",playing[i].f_id);
        //printf("\nFilm id : %d Film Name : %s",playing[i].f_id,films[playing[i].f_id]);
        tok=strtok(NULL," ");
        playing[i].actors=(int*)calloc(1,sizeof(int));
        j=0;
        while(tok!=NULL){
            playing[i].actors[j]=atoi(tok);
            //printf("\n\tActor id > %d Actor Name > %s",playing[i].actors[j],actors[playing[i].actors[j]]);
            j++;
            playing[i].actors=(int*)realloc(playing[i].actors,sizeof(int)*(j+1));
            tok=strtok(NULL," ");
            //printf(" %d ",playing[i].actors[j-1]);
        }

        playing[i].actors=realloc(playing[i].actors,sizeof(int)*j);
        playing[i].actor_num=j; //filmde oynayan aktor sayisi saklaniyor
        //printf("\nTotal actor %d ",playing[i].actor_num);
        //system("PAUSE");
    }
    fclose(fp1);

    //her bir oyuncunun oynadigi filmler actor film yapisi icerisinde olusturuluyor
    actor_film=(PLAYA*)malloc(sizeof(PLAYA)*asize);
    for(i=0;i<asize;i++){
        actor_film[i].film_num=0;
        actor_film[i].a_id=i;
        actor_film[i].films=(int*)malloc(sizeof(int));
    }
    for(i=0;i<asize;i++){ //her bir aktor icin
        for(j=0;j<Film_Num;j++){//her bir filme bak
            for(k=0;k<playing[j].actor_num;k++){ //filmde oynayan aktorler arasinda
                if(playing[j].actors[k]==i){//bakilan aktor var mi varsa
                    actor_film[i].films=realloc(actor_film[i].films,sizeof(int)*(actor_film[i].film_num+1));//actorun oynadigi filmleri realloc yap
                    actor_film[i].films[actor_film[i].film_num]=j; //ilgili aktore ilgili filmin indisini ekle
                    actor_film[i].film_num++; //aktorun oynadigi film sayisini artir
                    //printf("\n\t%d. actor %d. film--res %d Total Films are %d k is %d in %d",i,j,playing[j].actors[k],actor_film[i].film_num,k,playing[j].actor_num);
                    //system("PAUSE");
                }
            }
        }
        //kontrol amacli yazdirmalar
        //printf("\nActor id %d Name > %s ",i,actors[i]);
        //printf("\n\tTotal Films are %d ",actor_film[i].film_num);
        //system("PAUSE");
    }
    //kontrol amacli yazdirmalar
//    for(i=0;i<asize;i++){
//        printf("\nActor id %d Name > %s ",i,actors[i]);
//        for(j=0;j<actor_film[i].film_num;j++){
//            printf("\n\tFilm id %d Film name > %s ",actor_film[i].films[j],films[actor_film[i].films[j]]);
//        }
//    }

}

//kevin bacon un indexini bulan fonksiyon
int find_kbacon(){
    int i=0;
    while(i<asize && strcmp("Bacon, Kevin",actors[i])!=0)
        i++;
    if(i==asize)
        return -1;
    else
        return i;
}

//adi verilen aktorun indisini bulan ve donduren fonksiyon
int find_actor(char *name){
    int i=0;
    printf("\nName is :%s",name);
    while(i<asize && strcmp(name,actors[i])!=0)
        i++;
    if(i==asize){
        printf("\nAktor bulunamadi\n");
        return -1;
    }
    else{
        return i;
    }
}

//dosyadan okuma fonksiyonlarini cagirir okuma bittikten sonra graf olusturulur, sonrasinda ise input alarak bfs yapip ekrana istenilen sonuclari dondurur.
int main(){
    int i,j,k,index,act_index;
    struct node *iter;
    int control;
    int choice=1;
    char *name=malloc(sizeof(char)*NAME_BUF);
    int name_index;
    struct Graph *graph;
    FILE *fp1,*fp2;
    //dataset ayristirip dosyaya yazan fonksiyonlar
    create_Actors();
    create_files();
    //dosyadan okuma yaparak ilgili veri yapilarina degerleri aktaran fonksiyon
    read_from_files();
    printf("\nBaslangic icin 1 cikis icin 0 giriniz \n");
    scanf("%d",&choice);
    while(choice){
        graph=createGraph(asize);
        //komsuluklar olusturuluyor yani graf olusturuluyor
        for(i=0;i<asize;i++){ //ilk aktorun id si
            for(j=0;j<actor_film[i].film_num;j++){ //oynadigi filmlerdeki
                index=actor_film[i].films[j]; //film id si
                for(k=0;k<playing[index].actor_num;k++){ //bu filmde oynayan diger oyuncu sayisi kadar don
                    act_index=playing[index].actors[k]; //bu filmde oynayan diger oyuncu indexi
                    if(act_index!=i) {//ilk oyuncunun indexine esit degilse
                        iter = graph->adjLists[i]; //komsuluk adresini iter e at
                        control=0;
                        while (iter != NULL && !control){ //eger iter null degilse ve onceden eklenmemisse
                            if(iter->a_id==act_index){ //aktor indisi komsuluklarda yoksa
                                control=1; //donguden cikis yap
                            }
                            iter=iter->next;
                        }
                        if(!control){//eger controllu bir cikis yapilmissa yani komusluklarin sonuna gelinmemisse
                            addEdge(graph, i, index, act_index, index); //grafa ekleme yap
                            //printf("Actor index > %d Actor > %s\nActor index >%d Actor > %s\nFilm index > %d Film > %s\n",i,actors[i],act_index,actors[act_index],index,films[index]);
                            // system("PAUSE");
                        }
                    }
                }
            }
        }
        system("CLS");
        fgets(name,NAME_BUF,stdin);
        printf("Oyuncu adini giriniz : ");
        scanf ("%[^\n]%*c", name);
        name_index=find_actor(name);
        printf("\nIndex of actor is %d",name_index);
        bfSearch(graph,find_kbacon(),name_index);
        printf("\nDevam icin 1 cikis icin 0 giriniz \n");
        scanf("%d",&choice);
    }


    printf("\nKBacon's index is %d\n",find_kbacon());
    for (i = 0; i < asize; i++) {
        free(actors[i]);
    }
    free(actors);
    system("PAUSE");
    return 0;
}

