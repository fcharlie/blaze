#include <iostream>
#include <iomanip>
#include <time.h>
#include <cmath>
#include <windows.h>
#include <string>

using namespace std;
string printProg(int);

int main()
{
    srand(time(0));
    int x = 0;
    cout << "Working ..." << endl;
    for(int i = 0; i<100; i++){
        int r = rand() % 1000;
        x++;
        cout << "\r" << setw(-20) << printProg(x) << " " << x << "% completed." << flush;
        if(i < 43){
           Sleep(r/6);
        }else if(i > 43 && i < 74){
           Sleep(r/8);
        }else if(i < 98){
           Sleep(r/5);
        }else if(i > 97 && i != 99){
           Sleep(1000);
        }
    }

    cout << endl << endl << "Operation completed successfully.\n" << flush;
    return 0;
}

string printProg(int x){
    string s;
    s="[";
    for (int i=1;i<=(100/2);i++){
        if (i<=(x/2) || x==100)
            s+="=";
        else if (i==(x/2))
            s+=">";
        else
            s+=" ";
    }

    s+="]";
    return s;
}