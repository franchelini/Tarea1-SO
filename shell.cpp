#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define GRN "\e[0;32m"
#define NC "\e[0m"

using namespace std;

const int READ = 0;  // Variable de lectura para pipe
const int WRITE = 1; // Variable de escritura para pipe

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
    } else if (comando.empty()) { // Si el comando esta vacio, limpiar y mostrar prompt
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
// Mapa para almacenar los comandos favoritos
unordered_map<int, string> favoritos;
int nextId = 1; // ID del próximo comando
void favs(const string &comando) {

    stringstream ss(comando);
    stringstream ss2(comando);

    string word;

    int wordCount = 0;
    while (ss2 >> word) {
        ++wordCount;
    }
    string argumentos[wordCount];
    for (int i = 0; i < wordCount; ++i)
    {
        ss >> argumentos[i];
    }

    //string subcmd;
    //ss >> subcmd;

    if (argumentos[0] == "crear") {
        cout << "ESTOY EN LA FUNCION CREAR! " << endl;
        ofstream archivo(argumentos[1]);
        if (archivo) {
            archivo.close();
            cout << "Archivo de favoritos creado en: " << argumentos[1] << endl;
        } else {
            cout << "Error al crear el archivo en la ruta especificada." << endl;
        }
    } else if (argumentos[0] == "mostrar") {
        if (favoritos.empty()) {
            cout << "No hay comandos favoritos." << endl;
        } else {
            for (const auto &pair : favoritos) {
                cout << pair.first << ": " << pair.second << endl;
            }
        }
    } else if (argumentos[0] == "eliminar") {
        
        //string nums;
        //ss >> nums; //ARGUMENTO 1

        vector<int> numeros;
        stringstream ssNums(argumentos[1]);
        string num;
        
        while (getline(ssNums, num, ',')) {
            numeros.push_back(stoi(num));
        }
        for (int num : numeros) {
            favoritos.erase(num);
        }
        cout << "Comandos eliminados." << endl;

    } else if (argumentos[0] == "buscar") {
        string cmdBuscado;
        getline(ss, cmdBuscado); // Leer el resto de la línea
        cout << "Resultados de búsqueda:" << endl;
        for (const auto &pair : favoritos) {
            if (pair.second.find(cmdBuscado) != string::npos) {
                cout << pair.first << ": " << pair.second << endl;
            }
        }
    } else if (argumentos[0] == "borrar") {
        favoritos.clear();
        cout << "Todos los comandos favoritos han sido borrados." << endl;
    } else if (argumentos[0] == "ejecutar") {
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
    } else if (argumentos[0] == "cargar") {
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
    } else if (argumentos[0] == "guardar") {
        ofstream archivo(argumentos[1]);
        if (archivo) {
            for (const auto &pair : favoritos) {
                archivo << pair.second << endl;
            }
            archivo.close();
            cout << "Comandos guardados en el archivo." << endl;
        } else {
            cout << "Error al abrir el archivo para guardar." << endl;
        }
    //}else if(argumentos[0] == "automatico"){
      //  favoritos[nextId++] = comando;

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

void comando_sin_pipe(const vector<vector<string>>& comandos) {
    if (comandos.empty() || comandos[0].empty()) return;

    string ComandoCompleto = accumulate(comandos[0].begin() , comandos[0].end(), string(),[](const string &a, const string &b){return a.empty() ? b : a + " " + b;});

    const string& cmd = comandos[0][0];

    /* 
    if (cmd == "cd") {
        const char* newDir = nullptr;
        if (comandos[0].size() < 2) {
            newDir = getenv("HOME");
            if (newDir == nullptr) {
                cout << "Uso incorrecto del comando 'cd'. Debes especificar un directorio." << endl;
                return;
            }
        } else {
            favs(ComandoCompleto);
            newDir = comandos[0][1].c_str();
        }

        if (chdir(newDir) != 0) {
            cout << "Error al cambiar el directorio a " << newDir << endl;
        }
    */

    /*}else*/ 
    if(comandos[0][0] == "favs"){

        if (comandos[0].size() < 2) {
            cout << "Uso incorrecto del comando 'favorites'. Debes especificar un "
                    "argumento." << endl;
        }else{
            stringstream ss;
            for (size_t i = 1; i < comandos[0].size(); ++i) {
                ss << comandos[0][i] << " ";
            }
            favs(ss.str()); 
        }
    //DESDE AQUI ES EL REMINDER
    }else if(comandos[0][0] == "reminder"){

    
        favs(ComandoCompleto);

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



// Función para manejar comandos favoritos


int main(){
    vector<vector<string>> comandos; // Matriz de comandos.
    string comando;
    system("clear");
    bool prompt = true;
    cout << GRN "MyShell " << getCurrentDir() << " # ";
    cout << NC;
    long long maxim;

    while (getline(cin, comando)) {
        maxim = 0;
        int ret = parser(comando, maxim, comandos);
        if (ret == 1) {
            comando.clear();
            continue;
        }
        //-------------CASO DONDE SOLO SE TIENE UNA LINEA DE COMANDO: ls -la. NO
    // NECESITA PIPES--------------//
    if (comandos.size() <= 1) {
      comando_sin_pipe(comandos);
    } else {
      // Se buscan las pipe en la linea de comandos y se divide el comando

      char *ArgsCommand[comandos.size()][maxim + 1];
      for (int i = 0; i < comandos.size(); ++i) {
        for (int j = 0; j < comandos[i].size(); j++) {
          ArgsCommand[i][j] = strdup(comandos[i][j].c_str());
        }
        ArgsCommand[i][comandos[i].size()] = NULL;
      }

      // ----- INICIAMOS LAS PIPES-------//

      long long allpipes = comandos.size() - 1;
      int Pipes[allpipes][2];
      for (int k = 0; k < allpipes; ++k)
        pipe(Pipes[k]);

      int count = 0;

      // ------ PRIMER PROCESO ------//
      if (fork() == 0) {
        dup2(Pipes[0][WRITE], WRITE); // Abrimos el canal de escritura.
        close(Pipes[0][READ]);        // zamos el canal de lectura.
        int path = 1;
        while (path < allpipes) {
          close(Pipes[path][READ]);
          close(Pipes[path][WRITE]);
          path++;
        }

        int a = execvp(ArgsCommand[0][0], ArgsCommand[0]);
        if (a < 0) {
          cout << "Error en el comando ingresado" << endl;
          exit(0);
        }
      }

      //----- 2 PROCESOS O MAS----//
      if (allpipes > 1) {
        while (count < allpipes - 1) {
          if (fork() == 0) {
            dup2(Pipes[count][READ], READ); // Abrimos el canal de lectura.
            dup2(Pipes[count + 1][WRITE],
                 WRITE); // Abrimos el canal de escritura.
            for (int i = 0; i < allpipes; ++i) {
              if (i == count)
                close(Pipes[i][WRITE]);
              else if (i == count + 1)
                close(Pipes[i][READ]);
              else {
                close(Pipes[i][READ]);
                close(Pipes[i][WRITE]);
              }
            }
            int c = execvp(ArgsCommand[count + 1][0], ArgsCommand[count + 1]);
            if (c < 0) {
              cout << "Error en el comando ingresado" << endl;
              exit(0);
            }
          }
          count++;
        }
      }

      //------ PARTE FINAL DEL PROCESO -----//
      if (fork() == 0) {
        dup2(Pipes[count][READ], READ); // Leer la ultima pipe
        close(Pipes[count][WRITE]);     // Cerrar escritura de la ultima pipe.
        int path = 0;

        while (path < allpipes) { // Cerrar las pipes
          if (path == count) {
            path++;
            continue;
          }
          close(Pipes[path][READ]);
          close(Pipes[path][WRITE]);
          path++;
        }
        int p = execvp(ArgsCommand[count + 1][0],
                       ArgsCommand[count + 1]); // Ejecutamos el comando.
        
        //ESTA SHIET DEBE FUNCIONAR...
        //string ComandoCompleto = accumulate(comandos[0].begin() , comandos[0].end(), string(),[](const string &a, const string &b){return a.empty() ? b : a + " " + b;});
        //favs(ComandoCompleto);
                       
        if (p < 0) {
          cout << "Error en el comando ingresado" << endl;
          exit(0);
        }



      }
      for (int i = 0; i < allpipes; ++i) {
        close(Pipes[i][WRITE]);
        close(Pipes[i][READ]);
      }
      for (int l = 0; l < comandos.size(); ++l)
        wait(NULL); // Esperar a los hijos.
    }
    
    comandos.clear();
    comando.clear();
    cout << GRN "MyShell " << getCurrentDir() << " # ";
    cout << NC;
    }
    return 0;
}
