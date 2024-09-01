#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define GRN "\e[0;32m"
#define NC "\e[0m"

using namespace std;

string getCurrentDir() {
    char buff[FILENAME_MAX];
    
    // Obtener el directorio de trabajo actual
    if (getcwd(buff, FILENAME_MAX) == nullptr) {
        return ""; // Si getcwd falla, retorna una cadena vacía
    }

    string currentDir(buff);

    // Obtener la ruta del directorio HOME
    const char *homePath = getenv("HOME");
    if (homePath != nullptr) {
        string homeDir(homePath);
        if (currentDir.compare(0, homeDir.length(), homeDir) == 0) {
            currentDir.replace(0, homeDir.length(), "~");
        }
    }

    // Contar las barras '/' en el directorio actual
    int slashCount = count(currentDir.begin(), currentDir.end(), '/');
    if (slashCount <= 2) {
        return currentDir; // Si hay 2 o menos '/', retornamos el directorio tal cual
    }

    // Encontrar las posiciones de las últimas barras
    size_t lastSlash = currentDir.find_last_of('/');
    if (lastSlash != string::npos) {
        size_t secondLastSlash = currentDir.find_last_of('/', lastSlash - 1);
        if (secondLastSlash != string::npos) {
            // Devolver el directorio con el patrón "/..."
            return "/..." + currentDir.substr(secondLastSlash);
        }
    }

    return currentDir; // Retorna el directorio tal cual si no hay suficientes barras
}

int parser(const string &comando, long long &maxim, vector<vector<string>> &comandos) {
    if (comando == "exit") {
        exit(0);
    } else if (comando.empty()) { // Si el comando está vacío, limpiar y mostrar prompt
        comandos.clear();
        cout << GRN "MyShell " << getCurrentDir() << " # " << NC;
        return 1;
    }

    vector<string> comandosTEMP;
    stringstream comandoStream(comando);
    string comandoSegmento;

    // Separar el comando por el delimitador '|'
    while (getline(comandoStream, comandoSegmento, '|')) {
        stringstream segmentoStream(comandoSegmento);
        string palabra;

        // Separar cada segmento por espacios
        while (getline(segmentoStream, palabra, ' ')) {
            // Eliminar comillas de cada palabra
            palabra.erase(remove_if(palabra.begin(), palabra.end(),
                                    [](char c) { return c == '\"'; }),
                          palabra.end());
            if (!palabra.empty()) {
                comandosTEMP.push_back(palabra);
            }
        }

        if (!comandosTEMP.empty()) {
            // Actualizar el tamaño máximo de los comandos
            maxim = max(static_cast<long long>(comandosTEMP.size()), maxim);
            comandos.push_back(comandosTEMP);
            comandosTEMP.clear();
        }
    }

    return 0;
}


//pal reminder de la última parte
void reminder(int seconds, const string &message) {
    pid_t pid = fork();
    if (pid == 0) {
        sleep(seconds);
        cout << "\nRecordatorio: " << message << endl;
        exit(0);
    } else if (pid < 0) {
        cout << "Error al crear el proceso de recordatorio." << endl;
    }
}


void comando_sin_pipe(const vector<vector<string>>& comandos) {
    if (comandos.empty() || comandos[0].empty()) return;

    const string& cmd = comandos[0][0];

    if (cmd == "cd") {
        const char* newDir = nullptr;
        if (comandos[0].size() < 2) {
            newDir = getenv("HOME");
            if (newDir == nullptr) {
                cout << "Uso incorrecto del comando 'cd'. Debes especificar un directorio." << endl;
                return;
            }
        } else {
            newDir = comandos[0][1].c_str();
        }

        if (chdir(newDir) != 0) {
            cout << "Error al cambiar el directorio a " << newDir << endl;
        }
    //DESDE AQUI ES EL REMINDER
    }else if(comandos[0][0] == "reminder"){

        if (comandos[0].size() < 3) {
            cout << "Uso incorrecto del comando 'reminder'. Debes especificar un "
                    "tiempo y un mensaje." << endl;
    } else {
      try {
          int seconds = stoi(comandos[0][1]);
          string message = accumulate(comandos[0].begin() + 2, comandos[0].end(), string(),[](const string &a, const string &b) 
            {return a.empty() ? b : a + " " + b;});
          reminder(seconds, message);
      } catch (const exception &e) {
          cout << "Uso incorrecto del comando 'reminder'. Debes especificar un "
                        "tiempo como número y un mensaje como cadena." << endl;
      }
      }
      //HASTA ACÁ ES EL REMINDER
    } else {
        vector<char*> args;
        for (const auto& arg : comandos[0]) {
            args.push_back(strdup(arg.c_str()));
        }
        args.push_back(nullptr); // Finaliza el array con un puntero nulo

        int pid = fork();
        if (pid == 0) { // Proceso hijo
            if (execvp(args[0], args.data()) < 0) {
                cout << "Error en el comando ingresado" << endl;
                exit(EXIT_FAILURE);
            }

        } else if (pid < 0) {
            cout << "Error al crear el proceso hijo" << endl;

        } else { // Proceso padre
            wait(NULL);
        }

        // Liberar la memoria alocada por strdup
        for (char* arg : args) {
            free(arg);
        }
    }
}

// Mapa para almacenar los comandos favoritos
unordered_map<int, string> favoritos;
int nextId = 1; // ID del próximo comando

// Función para manejar comandos favoritos
void favs(const string &comando) {
    stringstream ss(comando);
    string subcmd;
    ss >> subcmd;

    if (subcmd == "crear") {
        string ruta;
        ss >> ruta;
        ofstream archivo(ruta);
        if (archivo) {
            archivo.close();
            cout << "Archivo de favoritos creado en: " << ruta << endl;
        } else {
            cout << "Error al crear el archivo en la ruta especificada." << endl;
        }
    } else if (subcmd == "mostrar") {
        if (favoritos.empty()) {
            cout << "No hay comandos favoritos." << endl;
        } else {
            for (const auto &pair : favoritos) {
                cout << pair.first << ": " << pair.second << endl;
            }
        }
    } else if (subcmd == "eliminar") {
        string nums;
        ss >> nums;
        vector<int> numeros;
        stringstream ssNums(nums);
        string num;
        while (getline(ssNums, num, ',')) {
            numeros.push_back(stoi(num));
        }
        for (int num : numeros) {
            favoritos.erase(num);
        }
        cout << "Comandos eliminados." << endl;
    } else if (subcmd == "buscar") {
        string cmdBuscado;
        getline(ss, cmdBuscado); // Leer el resto de la línea
        cout << "Resultados de búsqueda:" << endl;
        for (const auto &pair : favoritos) {
            if (pair.second.find(cmdBuscado) != string::npos) {
                cout << pair.first << ": " << pair.second << endl;
            }
        }
    } else if (subcmd == "borrar") {
        favoritos.clear();
        cout << "Todos los comandos favoritos han sido borrados." << endl;
    } else if (subcmd == "ejecutar") {
        int num;
        ss >> num;
        if (favoritos.find(num) != favoritos.end()) {
            int pid = fork();
            if (pid == 0) {
                char *args[] = {strdup(favoritos[num].c_str()), nullptr};
                execvp(args[0], args);
                perror("Error al ejecutar el comando");
                exit(EXIT_FAILURE);
            } else if (pid < 0) {
                cout << "Error al crear el proceso hijo" << endl;
            } else {
                wait(NULL);
            }
        } else {
            cout << "No existe el comando favorito con el número " << num << endl;
        }
    } else if (subcmd == "cargar") {
        string ruta;
        ss >> ruta;
        ifstream archivo(ruta);
        if (!archivo) {
            cout << "Error al abrir el archivo." << endl;
            return;
        }
        string comando;
        while (getline(archivo, comando)) {
            for (const auto &pair : favoritos) {
                if (pair.second == comando) {
                    return; // Ya existe en favoritos
                }
            }
            favoritos[nextId++] = comando; // Agrega nuevo favorito
        }
        archivo.close();
        cout << "Comandos cargados desde el archivo." << endl;
    } else if (subcmd == "guardar") {
        string ruta;
        ss >> ruta;
        ofstream archivo(ruta);
        if (archivo) {
            for (const auto &pair : favoritos) {
                archivo << pair.second << endl;
            }
            archivo.close();
            cout << "Comandos guardados en el archivo." << endl;
        } else {
            cout << "Error al abrir el archivo para guardar." << endl;
        }
    } else {
        // Agrega el comando ingresado como favorito si no es uno de favoritos
        for (const auto &pair : favoritos) {
            if (pair.second == comando) {
                return; // Ya existe en favoritos
            }
        }
        favoritos[nextId++] = comando; // Agrega nuevo favorito
    }
}

int main(){
  
}

