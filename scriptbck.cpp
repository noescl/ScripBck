#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <ctime>
#include <fstream>

using namespace std;

string getTimestamp();
string fileWithDate(string file7zip);
void writeLog(const string &msg);

int main(int argc, char *argv[]) {
    cout << "Numero de parametros "<<argc;
    printf("Numero de parametros %i\n",argc);
    if (argc < 4) {
        writeLog("ERROR: Parámetros insuficientes.");
        cerr << "Uso: " << argv[0] << " <archivo.7z> <carpeta>" << endl;
        return 1;
    }

    string dirTarget    = argv[1];
    string file7zip     = argv[2];
    string folderSource = argv[3];
    string timestamp    = getTimestamp();

    writeLog("=== INICIO DE EJECUCIÓN ===");
    writeLog("Parametros recibidos: \n dirTarget=" + dirTarget+ "\n file7zip="+file7zip+"\n folderSource=" + folderSource);
    
    dirTarget = dirTarget+fileWithDate(file7zip);
    writeLog("Nombre final del archivo con fecha: " + dirTarget);
    
    pid_t pid = fork();
    if (pid == 0) { // Proceso hijo
        writeLog("Proceso hijo iniciado.");
        const char *args[] = {
            "7z",      // ejecutable
            "a",       // acción: agregar/comprimir
            "-mx9",
            dirTarget.c_str(),
            folderSource.c_str(),
            NULL
        };
        writeLog("Ejecutando comando 7z...");
        execvp(args[0], (char * const *)args);
        perror("No se pudo ejecutar");
        return 1;
    } 
    else if (pid > 0) {
        // Proceso padre
        wait(NULL);  // Espera a que termine el hijo
        writeLog("Proceso hijo finalizó.");
    } 
    else {
        perror("Error en fork");
        writeLog("ERROR: fork() falló.");
    }
    
    writeLog("=== FIN DE EJECUCIÓN ===\n");
    return 0;
}

string getTimestamp() {
    time_t now = time(nullptr);
    tm *t = localtime(&now);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%d%m%y%H%M%S", t);
    return string(buffer);
}

string fileWithDate(string file7zip){
    string timestamp = getTimestamp();
    size_t pos = file7zip.find_last_of(".");
    string fileWithDate;

    if (pos != string::npos) {
        fileWithDate = file7zip.substr(0, pos) + timestamp + file7zip.substr(pos);
    } else {
        fileWithDate = file7zip + timestamp;
    }
    return fileWithDate;
}

void writeLog(const string &msg) {
    ofstream log("backup.log", ios::app);  // modo append
    if (!log) return;                      // Si falla, no detenemos el programa
    
    time_t now = time(nullptr);
    tm* t = localtime(&now);

    char buffer[40];
    strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y", t);

    log << "[" << buffer << "] " << msg << endl;
}

//Copiar Codigo en Linux
//rsync -av --delete /origen/ /destino/
//Example :
//rsync -av --delete /home/noescl/Workspace/CodigoC/carpetaUno/To.txt /home/noescl/Workspace/CodigoC/carpetaDos
//Comprimir una carpeta en un archivo .7z
//7z a -mx9 /home/noescl/Workspace/CodigoC/CarpetaBck/archiv.7z /home/noescl/Workspace/CodigoC/carpetaUno/
//./scriptbck /home/noescl/Workspace/CodigoC/CarpetaBck/ archiv /home/noescl/Workspace/CodigoC/carpetaUno/
//DirectoryTaget | FileName   | DirectorySource 