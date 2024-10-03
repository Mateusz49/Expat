#include <stdio.h>
#include <expat.h>
#include <time.h>
#include <string.h>

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

//p - dla poprawnego wyświetlania wartości/statusów/niepewności/standardów stężeń
//d - dla poprawnego wyświetlania wartości parametrów
//glb - dla sprawdzenia czy obecnie znajdujemy się wewnątrz tagu stezenie
//std - dla sprawdzenia czy obecnie znajdujemy się wewnątrz tagu standard
int p;
int d;
int glb=0;
int std=0;
char wartosc[5];
char nazwaEmitor[5];
char nazwaStatus[10];
char nazwaSub[4];
char nazwaWar[11];

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

    //Otwarcie plików
    fptr=fopen("results.csv", "w");

    FILE *xml;
    xml=fopen("ke_lodz_ec4_ke.cfg.xml", "r");

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
        fprintf(stderr,
                "Parse error at line %" XML_FMT_INT_MOD "u:\n%" XML_FMT_STR "\n",
                XML_GetCurrentLineNumber(parser),
                XML_ErrorString(XML_GetErrorCode(parser)));
            XML_ParserFree(parser);
         return 1;
        }
    }while(!done);

    fclose(fptr);
    fclose(xml);

    return 0;
}
