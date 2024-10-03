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
//Zmienne potrzebne do stężeń
int p;
int d;
int s;
int py;
int c;
int n;
int co2;
int hcl;
int hf;
int nh;
int v; 
char nameElement[5];

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
            if(strcmp(atts[i], "nazwa")==0){
                fprintf(fptr, "%s %s\t", bufferT, bufferH);
                fprintf(fptr, "%" XML_FMT_STR, atts[i+1]);
                strcpy(nameElement, atts[i+1]);                 
            }

            //Status automatyczny
            if(strcmp(name, "auto")==0){    
                fprintf(fptr, ".Status_Automatyczny\t""%"XML_FMT_STR"\n", atts[i + 1]);
            }                    

            //Status ręczny
           if(strcmp(name, "reka")==0){
                fprintf(fptr, "%s %s\t", bufferT, bufferH);
                fprintf(fptr, "%s", nameElement);
                fprintf(fptr, ".Status_Reczny\t""%"XML_FMT_STR"\n", atts[i + 1]);
           }

           //Nazwa parametru
           if(strcmp(name, "parametr")==0){                

                if(strcmp(atts[i+1], "VSS")==0){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Parametr.%"XML_FMT_STR".Wartosc",nameElement, atts[i+1]);
                    d=1;
                    v=1;
                }
                else if(strcmp(atts[i], "typ")==0){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Parametr.%"XML_FMT_STR, nameElement, atts[i+1]);
                    d=1;
                }          
           }

            //Wartość parametru (wartości stężeń poniżej)
           if(strcmp(name, "wartosc")==0){
                if(strcmp(atts[i], "mnoznik")==0){
                    if(d==0){
                        continue;
                    }
                    fprintf(fptr, "\t%"XML_FMT_STR"\n", atts[i-1]);
                }
            }

            //Sprawdzenie nazwy stężenia
           if(strcmp(name, "stezenie")==0){
                p=1;
                d=0;
                if(strcmp(atts[i+1], "PYL")==0)
                    py=1;

                else if(strcmp(atts[i+1], "SO2")==0)
                    s=1;
                
                else if(strcmp(atts[i+1], "CO")==0)
                    c=1;

                else if(strcmp(atts[i+1], "NOX")==0)
                    n=1;

                else if(strcmp(atts[i+1], "CO2")==0)
                    co2=1;

                else if(strcmp(atts[i+1], "HCL")==0)
                    hcl=1;

                else if(strcmp(atts[i+1], "HF")==0)
                    hf=1;

                else if(strcmp(atts[i+1], "NH3")==0)
                    nh=1;
           }
           
           //Wartości stężeń
           if(p==1 && strcmp(atts[i], "pkt")==0){

                if(py==1){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.PYL", nameElement);
                    fprintf(fptr, ".Wartosc\t%"XML_FMT_STR"\n", atts[i+1]);
                    py=2;
                    p=0;
                    continue;
                }

                if(s==1){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.SO2"".Wartosc\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                    s=2;
                }

                if(c==1){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.CO"".Wartosc\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                    c=2;
                }

                if(n==1){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.NOx"".Wartosc\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                    n=2;
                }

                if(co2==1){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.CO2"".Wartosc\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                    co2=2;
                    continue;
                }

                if(hcl==1){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.HCL"".Wartosc\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                    hcl=2;
                    continue;
                }

                if(hf==1){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.HF"".Wartosc\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                    hf=2;
                    continue;
                }

                if(nh==1){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.NH3"".Wartosc\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                    nh=2;
                    continue;
                }
                
            }

            //Statusy stężeń
            if(strcmp(name, "status")==0){
                    if(py==2){
                        fprintf(fptr, "%s %s\t", bufferT, bufferH);
                        fprintf(fptr, "%s.Stezenie.PYL"".Status\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                        py=3;
                        continue;
                    }
                    if(s==2){
                        fprintf(fptr, "%s %s\t", bufferT, bufferH);
                        fprintf(fptr, "%s.Stezenie.SO2"".Status\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                        s=3;
                        continue;
                    }
                    if(c==2){
                        fprintf(fptr, "%s %s\t", bufferT, bufferH);
                        fprintf(fptr, "%s.Stezenie.CO"".Status\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                        c=3;
                        continue;
                    }

                    if(n==2){
                        fprintf(fptr, "%s %s\t", bufferT, bufferH);
                        fprintf(fptr, "%s.Stezenie.NOx"".Status\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                        n=3;
                        continue;
                    }

                    if(co2==2){
                        fprintf(fptr, "%s %s\t", bufferT, bufferH);
                        fprintf(fptr, "%s.Stezenie.CO2"".Status\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                        co2=0;
                        continue;;
                    }

                    if(hcl==2){
                        fprintf(fptr, "%s %s\t", bufferT, bufferH);
                        fprintf(fptr, "%s.Stezenie.HCL"".Status\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                        hcl=3;
                        continue;
                    }

                    if(hf==2){
                        fprintf(fptr, "%s %s\t", bufferT, bufferH);
                        fprintf(fptr, "%s.Stezenie.HF"".Status\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                        hf=3;
                        continue;
                    }

                    if(nh==2){
                        fprintf(fptr, "%s %s\t", bufferT, bufferH);
                        fprintf(fptr, "%s.Stezenie.NH3"".Status\t%" XML_FMT_STR "\n", nameElement, atts[i+1]);
                        nh=3;
                        continue;
                    }

                    if(v==1){
                        fprintf(fptr, "%s %s\t", bufferT, bufferH);
                        fprintf(fptr, "%s.Parametr.VSS"".Status\t%" XML_FMT_STR "\n", nameElement, atts[i+1]); 
                        v=0; 
                    }        
           }

            //Niepewności stężeń
            if(strcmp(name, "niepewnosc")==0){

                if(py==3){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.PYL", nameElement);
                    fprintf(fptr, ".Niepewnosc\t%"XML_FMT_STR"\n", atts[i+1]);
                    py=4;
                    continue;
                }

                if(s==3){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.SO2"".Niepewnosc\t%" XML_FMT_STR "\n" , nameElement, atts[i+1]);
                    s=4;
                    continue;
                }

                if(c==3){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.CO"".Niepewnosc\t%" XML_FMT_STR "\n",nameElement, atts[i+1]);
                    c=4;
                    continue;
                }

                if(n==3){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.NOx"".Niepewnosc\t%" XML_FMT_STR "\n",nameElement, atts[i+1]);
                    n=4;
                    continue;
                }

                if(hcl==3){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.HCL"".Niepewnosc\t%" XML_FMT_STR "\n",nameElement, atts[i+1]);
                    hcl=4;
                    continue;
                }

                if(hf==3){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.HF"".Niepewnosc\t%" XML_FMT_STR "\n",nameElement, atts[i+1]);
                    hf=4;
                    continue;
                }

                if(nh==3){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.NH3"".Niepewnosc\t%" XML_FMT_STR "\n",nameElement, atts[i+1]);
                    nh=4;
                    continue;
                }   
            }

            //Standardy stężeń
            if(strcmp(atts[i+1], "H")==0){

                if(py==4){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.PYL", nameElement);
                    fprintf(fptr, ".Standard\t%"XML_FMT_STR"\n", atts[i-1]);
                    py=0;
                    continue;
                }

                if(s==4){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.SO2"".Standard\t%" XML_FMT_STR "\n", nameElement, atts[i-1]);
                    s=0;
                    continue;
                }

                if(c==4){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.CO"".Standard\t%" XML_FMT_STR "\n", nameElement, atts[i-1]);
                    c=0;
                    continue;
                }

                if(n==4){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.NOx"".Standard\t%" XML_FMT_STR "\n", nameElement, atts[i-1]);
                    n=0;
                    continue;
                }

                if(hcl==4){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.HCL"".Standard\t%" XML_FMT_STR "\n", nameElement, atts[i-1]);
                    hcl=0;
                    continue;
                }

                if(hf==4){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.HF"".Standard\t%" XML_FMT_STR "\n", nameElement, atts[i-1]);
                    hf=0;
                    continue;
                }

                 if(nh==4){
                    fprintf(fptr, "%s %s\t", bufferT, bufferH);
                    fprintf(fptr, "%s.Stezenie.NH3"".Standard\t%" XML_FMT_STR "\n", nameElement, atts[i-1]);
                    nh=0;
                    continue;
                }   
            }

        }
}

void XMLCALL endElement(void *userData, const XML_Char *name){

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
