#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <thread>
#include <libconfig.h++>
#include <future>
using namespace std;
using namespace filesystem;
using namespace libconfig;
unsigned short int *INTERVAL = new unsigned short int;
int *CPULOAD = new int;
int *MIN = new int;
int *MAX = new int;
int *CORE_UP = new int;
int *CORE_DOWN = new int;
void config(unsigned short int *i,int *min,int *max,int *up,int *down){ //Читает параметры из конфига
    Config conf;conf.readFile("/etc/cpuctl.conf");
    *i = stoi(conf.lookup("INTERVAL"));
    if (stoi(conf.lookup("STOPPING_CORES")) == 1){
        *min = stoi(conf.lookup("MIN"));
        *max = stoi(conf.lookup("MAX"));
        *up = stoi(conf.lookup("CORE_UP"));
        *down = stoi(conf.lookup("CORE_DOWN"));
        if(*up < 1000){*up = 1000;}
        if(*down < 1000){*down = 1000;}
    }
    //if (istringstream(conf.lookup("TERMAL_CONTROL"))){}
}
int corescan(){ //Считает ядра
    int cores = 2;
    while (exists("/sys/devices/system/cpu/cpu" + to_string(cores) + "/online")){
    cores=(cores + 1);}
    return cores;
}
void cpuload(int *cpu){ //Вычисляет загрузку CPU
    static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
    double percent;
    FILE* file;
    for (;;){
        unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;
        file = fopen("/proc/stat", "r");
        fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
            &totalSys, &totalIdle);
        fclose(file);
        total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) + (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        percent *= 100;
        *cpu = percent;
        lastTotalUser = totalUser;
        lastTotalUserLow = totalUserLow;
        lastTotalSys = totalSys;
        lastTotalIdle = totalIdle;
        //cout << *cpu << endl;
        this_thread::sleep_for(chrono::milliseconds(*INTERVAL));
    }
}
void core(int n, bool o,int t){ //Меняет состояние ядра
    ofstream core;
    core.open("/sys/devices/system/cpu/cpu" + to_string(n) + "/online");
    core << o;core.close();
    this_thread::sleep_for(chrono::milliseconds(t));
}
void corectl(int *cpu,int *max,int *min,int *up,int *down){ //Меняет состояние ядер в зависимости от нагрузки на процессор
    int cores = corescan();
    int n = 1;
    for (;n < cores;n++){core(n,true,0);}chrono::milliseconds(1000);
    for (;;){
        this_thread::sleep_for(chrono::milliseconds(*INTERVAL));
        if (n != cores and *cpu >= *max){
             core(n,true,*up);
             n++;
             continue;
        }
        if (n != 0 and *cpu <= *min){
             core(n,false,*down);
             n--;
        }
    }
}
int main(){
    config(INTERVAL,MIN,MAX,CORE_UP,CORE_DOWN);
    if(*MAX){
         auto l = async(cpuload,CPULOAD);
         auto c = async(corectl,CPULOAD,MAX,MIN,CORE_UP,CORE_DOWN);
    }
    return 0;
}
