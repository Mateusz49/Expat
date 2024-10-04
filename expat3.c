#include <stdio.h>
#include <expat.h>
#include <time.h>
#include <string.h>
#include <dirent.h>

#ifdef XML_LARGE_SIZE
#  define XML_FMT_INT_MOD "ll"
#else
#  define XML_FMT_INT_MOD "l"
#endif

#ifdef XML_UNICODE_WCHAR_T
#  define XML_FMT_STR "ls"
#else
#  define XML_FMT_STR "s"
#endif

FILE *fptr;

struct dirent *plik;
    DIR * sc;

int p; //p - dla poprawnego wyświetlania wartości/statusów/niepewności/standardów stężeń
int d; //d - dla poprawnego wyświetlania wartości parametrów
int glb=0; //glb - dla sprawdzenia czy obecnie znajdujemy się wewnątrz tagu stezenie
int std=0; //std - dla sprawdzenia czy obecnie znajdujemy się wewnątrz tagu standard
char wartosc[10];
char nazwaEmitor[15];
char nazwaStatus[10];
char nazwaSub[6];
char nazwaWar[15];

//Dzisiejsza data
void CurDate(char *bufferT, size_t t){
    time_t now=time(NULL);
    struct tm *tmp=localtime(&now);

    strftime(bufferT, t, "%Y-%m-%d", tmp);
}

//Obecna godzina
void CurHour(char *bufferH, size_t t){
    time_t now=time(NULL);
    struct tm *tmp=localtime(&now);

    strftime(bufferH, t, "%H", tmp);
}

void XMLCALL startElement(void *UserData, const XML_Char *name, const XML_Char **atts){

    char bufferT[11];
    char bufferH[3];

    //Wywołanie obecnej daty i godziny    
    CurDate(bufferT, sizeof(bufferT));
    CurHour(bufferH, sizeof(bufferH));

        for (int i = 0; atts[i]; i += 2) {
                           
            //Nazwa emitora
            if(strcmp(atts[i], "nazwa")==0)
                strcpy(nazwaEmitor, atts[i+1]);

            //Przypisanie statusu/parametru/stezenia    
            if(glb==0 && strcmp(name, "status")==0 || strcmp(name, "parametr")==0 || strcmp(name, "stezenie")==0)
                strcpy(nazwaStatus, name);                 

            //Statusy
            if(strcmp(name, "auto")==0 || strcmp(name, "reka")==0){    
                fprintf(fptr, "%s %s\t", bufferT, bufferH);
                fprintf(fptr, "%s.status.%s\t""%"XML_FMT_STR"\n", nazwaEmitor, name, atts[i + 1]);
            }                    

           //Nazwa parametru
           if(strcmp(name, "parametr")==0){   

                //Dla VSS, które ma wartosc i status
                if(strcmp(atts[i+1], "VSS")==0){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.%s.%"XML_FMT_STR".%s",nazwaEmitor, nazwaStatus, atts[i+1], nazwaWar);
                    strcpy(nazwaSub, atts[i+1]);
                    glb=1;
                    d=1;
                }
                //Dla reszty
                else if(strcmp(atts[i], "typ")==0){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.%s.%"XML_FMT_STR, nazwaEmitor, nazwaStatus, atts[i+1]);
                    strcpy(nazwaSub, atts[i+1]);
                    d=1;
                }         
           }

            //Wartość parametru (wartości stężeń poniżej)
           if(strcmp(name, "wartosc")==0){
                if(strcmp(atts[i], "mnoznik")==0){
                    if(d==0)
                        continue;
                
                    fprintf(fptr, "\t%"XML_FMT_STR"\n", atts[i-1]);
                }
            }

            //Przypisanie nazwy stężenia
           if(strcmp(name, "stezenie")==0){
                p=1;
                d=0;
                glb=1;
                strcpy(nazwaSub, atts[i+1]);    
           }

            //Przypisanie wypisywanej wartości/statusu/niepewności/standardu
           if(strcmp(name, "wartosc")==0 || strcmp(name, "status")==0 || strcmp(name, "niepewnosc")==0 || strcmp(name, "standard")==0){              
                strcpy(nazwaWar, name);
                
                if(strcmp(name, "standard")==0)
                    std=1;
           }
        
           //Wartości, statusy i niepewności stężeń
           if(p==1 && strcmp(name, "wartosc")==0 || strcmp(name, "status")==0 || strcmp(name, "niepewnosc")==0){

                fprintf(fptr, "%s %s\t", bufferT, bufferH);
                fprintf(fptr, "%s.%s.%s.%s\t%"XML_FMT_STR "\n", nazwaEmitor, nazwaStatus, nazwaSub, nazwaWar, atts[i+1]);
                p=0;
            }

            //Standard stężeń
            if(glb==0 && std==1 && strcmp(name, "wartosc")==0){
                fprintf(fptr, "%s %s\t", bufferT, bufferH);
                fprintf(fptr, "%s.%s.%s.standard\t%"XML_FMT_STR "\n", nazwaEmitor, nazwaStatus, nazwaSub, atts[i+1]);
                std=0;
            }
        }
}

void XMLCALL endElement(void *userData, const XML_Char *name){
    if(strcmp(name, "stezenie")==0)
        glb=0;
}

int main(){

    //NAZWA FOLDERU, Z KTÓREGO BIERZEMY PLIKI
    char folder[]="__KONFIGURACJE/old";
    char sciezka[300];

    FILE *xml;
    //Otwarcie folderu z plikami
    sc=opendir(folder);

    while(plik=readdir(sc)){
        //Pominięcie ukrytych plików
        if(strcmp(plik->d_name, ".")!=0 && strcmp(plik->d_name, "..")!=0){
            //Przypisanie do zmiennej sciezka ścieżki do pliku
            snprintf(sciezka, sizeof(sciezka), "%s/%s",folder, plik->d_name);
            xml=fopen(sciezka, "r");

            //Sprawdzenie czy plik został otwarty
            if(xml==NULL){
                printf("Nie otwarto pliku");
                continue;
            }
            else{
                printf("Otwarty plik %s\n", plik->d_name);
            }

            //Sprawdzenie czy plik zawiera jakiekolwiek rozszerzenie
            if(strchr(sciezka, '.')==NULL){
                continue;
            }

            //Przypisanie do zmiennej sciezka ścieżki i nazwy pliku csv
            snprintf(sciezka, sizeof(sciezka), "wyniki/%s",plik->d_name);
            strcpy(strrchr(sciezka, '.'), ".csv");
            fptr=fopen(sciezka, "w");

            //Sprawdzenie czy plik csv został otwarty poprawnie
            if(fptr==NULL){
                printf("Nie otwarto pliku");
                fclose(fptr);
                continue;
            }

            //Stworzenie parsera
            XML_Parser parser = XML_ParserCreate(NULL);
            int done;
            int depth=0;

            XML_SetUserData(parser, &depth);
            XML_SetElementHandler(parser, startElement, endElement);

            do{
                void *const buf = XML_GetBuffer(parser, BUFSIZ);

                if (! buf) {
                    fprintf(stderr, "Couldn't allocate memory for buffer\n");
                    XML_ParserFree(parser);
                    return 1;
                }

                const size_t len = fread(buf, 1, BUFSIZ, xml);

                done = feof(xml);

                if(XML_ParseBuffer(parser, (int)len, done) == XML_STATUS_ERROR) {

                    //Wypisanie w pliku ewentualnego błędu
                    fprintf(fptr,
                        "Parse error at line %" XML_FMT_INT_MOD "u:\n%" XML_FMT_STR "\n",
                        XML_GetCurrentLineNumber(parser),
                        XML_ErrorString(XML_GetErrorCode(parser)));
                        XML_ParserFree(parser);
                        fclose(xml);
                        continue;
                    return 1;
                }
            }while(!done);
        }
    }

    fclose(fptr);
    fclose(xml);
    closedir(sc);

    return 0;
}
