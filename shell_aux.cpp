#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <typeinfo>

#define GRN "\e[0;32m"
#define NC "\e[0m"

using namespace std;

string reminderMessage;

const int READ = 0;  // Variable de lectura para pipe
const int WRITE = 1; // Variable de escritura para pipe
void comando_sin_pipe(const vector<vector<string>> &comandos);

string getCurrentDir()
{
    char buff[FILENAME_MAX];

    // Obtener el directorio de trabajo actual
    if (getcwd(buff, FILENAME_MAX) == nullptr)
    {
        return ""; // Si getcwd falla, retorna una cadena vacía
    }

    string currentDir(buff);

    // Obtener la ruta del directorio HOME
    const char *homePath = getenv("HOME");
    if (homePath != nullptr)
    {
        string homeDir(homePath);
        if (currentDir.compare(0, homeDir.length(), homeDir) == 0)
        {
            currentDir.replace(0, homeDir.length(), "~");
        }
    }

    // Contar las barras '/' en el directorio actual
    int slashCount = count(currentDir.begin(), currentDir.end(), '/');
    if (slashCount <= 2)
    {
        return currentDir; // Si hay 2 o menos '/', retornamos el directorio tal cual
    }

    // Encontrar las posiciones de las últimas barras
    size_t lastSlash = currentDir.find_last_of('/');
    if (lastSlash != string::npos)
    {
        size_t secondLastSlash = currentDir.find_last_of('/', lastSlash - 1);
        if (secondLastSlash != string::npos)
        {
            // Devolver el directorio con el patrón "/..."
            return "/..." + currentDir.substr(secondLastSlash);
        }
    }

    return currentDir; // Retorna el directorio tal cual si no hay suficientes barras
}

int parser(const string &comando, long long &maxim, vector<vector<string>> &comandos)
{
    if (comando == "exit")
    {
        exit(0);
    }
    else if (comando.empty())
    { // Si el comando esta vacio, limpiar y mostrar prompt
        comandos.clear();
        cout << GRN "MyShell " << getCurrentDir() << " # " << NC;
        return 1;
    }

    vector<string> comandosTEMP;
    stringstream comandoStream(comando);
    string comandoSegmento;

    // Separar el comando por el delimitador '|'
    while (getline(comandoStream, comandoSegmento, '|'))
    {
        stringstream segmentoStream(comandoSegmento);
        string palabra;

        // Separar cada segmento por espacios
        while (getline(segmentoStream, palabra, ' '))
        {
            // Eliminar comillas de cada palabra
            palabra.erase(remove_if(palabra.begin(), palabra.end(),
                                    [](char c)
                                    { return c == '\"'; }),
                          palabra.end());
            if (!palabra.empty())
            {
                comandosTEMP.push_back(palabra);
            }
        }

        if (!comandosTEMP.empty())
        {
            // Actualizar el tamaño máximo de los comandos
            maxim = max(static_cast<long long>(comandosTEMP.size()), maxim);
            comandos.push_back(comandosTEMP);
            comandosTEMP.clear();
        }
    }

    return 0;
}

void reminderHandler(int signum)
{
    cout << endl;
    cout << "Recordatorio: " << reminderMessage << endl;
    fflush(stdout);
    cout << "\e[0;32mMyShell " << getCurrentDir() << " # \e[0m";
    fflush(stdout);
}

void reminder(int seconds, const string &message)
{
    reminderMessage = message;
    signal(SIGALRM, reminderHandler);
    alarm(seconds);
}

// Mapa para almacenar los comandos favoritos
unordered_map<int, string> favoritos;
int nextId = 1; // ID del próximo comando

void favs_crear(const string &comando)
{
    cout << comando << endl;

    ofstream archivo(comando);
    if (archivo)
    {
        archivo.close();
        cout << "Archivo de favoritos creado en: " << comando << endl;
    }
    else
    {
        cout << "Error al crear el archivo en la ruta especificada." << endl;
    }
}

void favs_mostrar()
{
    if (favoritos.empty())
    {
        cout << "No hay comandos favoritos." << endl;
    }
    else
    {
        for (const auto &pair : favoritos)
        {
            cout << pair.first << ": " << pair.second << endl;
        }
    }
}

void favs_guardar(const string &ruta)
{
    ofstream archivo(ruta);
    cout << ruta << endl;

    if (archivo.is_open())
    {
        // Escribe en el archivo
        for (const auto &pair : favoritos)
        {
            archivo << pair.second << endl;
        }
        // Cierra el archivo
        archivo.close();
        cout << "Comandos guardados en el archivo." << endl;
    }
    else
    {
        cout << "Error al abrir el archivo para guardar." << endl;
    }
}

void favs_eliminar(const string &numeros)
{
    int int1, int2;
    stringstream ss(numeros);
    char coma;
    // Extraemos los dos enteros, separados por la coma
    ss >> int1 >> coma >> int2;
    

    vector<pair<int, string>> elementos_restantes;
// Eliminar los elementos en el rango especificado
    
    for (auto it = favoritos.begin(); it != favoritos.end();)
    {
        if (it->first >= int1 && it->first <= int2)
        {
            it = favoritos.erase(it); // Eliminar elemento y avanzar
        }
        else
        {
            elementos_restantes.push_back(*it); // Guardar el elemento restante
            ++it; // Avanzar al siguiente elemento
        }
    }

    // Limpiar el unordered_map original
    favoritos.clear();

    // Volver a insertar los elementos en el unordered_map con nuevas claves
    for (size_t i = 0; i < elementos_restantes.size(); ++i)
    {
        // Asignar nueva clave empezando desde 1
        favoritos[i + 1] = elementos_restantes[i].second; // Reinserta el valor con nueva clave
    }
}

void favs_buscar(const string &Objetivo)
{
    cout << "Resultados de búsqueda:" << endl;
    for (const auto &pair : favoritos)
    {
        if (pair.second.find(Objetivo) != string::npos)
        {
            cout << pair.first << ": " << pair.second << endl;
        }
    }
}

void favs_borrar()
{
    favoritos.clear();
    cout << "Todos los comandos favoritos han sido borrados." << endl;
}

void favs_cargar(const string &ruta)
{
    ifstream archivo(ruta);
    if (!archivo)
    {
        cout << "Error al abrir el archivo." << endl;
        return;
    }
    nextId = 1;
    favoritos.clear();
    string comando;
    while (getline(archivo, comando))
    {
        for (const auto &pair : favoritos)
        {
            if (pair.second == comando)
            {
                return; // Ya existe en favoritos
            }
        }
        favoritos[nextId++] = comando; // Agrega nuevo favorito
    }
    archivo.close();
    cout << "Comandos cargados desde el archivo." << endl;
}

void favs_guardarAuto(const string &comando)
{
    if (comando.empty())
    {
        return;
    }
    else
    {
        // Agrega el comando ingresado como favorito si no es uno de favoritos
        for (const auto &pair : favoritos)
        {
            if (pair.second == comando)
            {
                return; // Ya existe en favoritos
            }
        }
        favoritos[nextId++] = comando; // Agrega nuevo favorito
    }
}

void favs_ejecutar(const string &comando)
{
    string comandoEjecutar;

    int num = stoi(comando);
    for (const auto &pair : favoritos)
    {
        if (pair.first == num)
        {
            cout << "Quieres ejecutar:" + pair.second << endl;
            comandoEjecutar = pair.second;
        
        }
    }

    vector<vector<string>> comandos; // Matriz para almacenar los vectores de palabras
    stringstream ss(comandoEjecutar);
    string palabra;
    vector<string> palabras;

    // Dividir por espacios
    while (ss >> palabra)
    {
        palabras.push_back(palabra); // Añadir cada palabra al vector
    }
    comandos.push_back(palabras); // Añadir el vector a la matriz de comandos
    
    comando_sin_pipe(comandos);
}

void comando_sin_pipe(const vector<vector<string>> &comandos)
{
    if (comandos.empty() || comandos[0].empty())
        return;

    string ComandoCompleto = accumulate(comandos[0].begin(), comandos[0].end(), string(), [](const string &a, const string &b)
                                        { return a.empty() ? b : a + " " + b; });

    const string &cmd = comandos[0][0];

    if (comandos[0][0] == "favs")
    {

        if (comandos[0].size() < 2)
        {
            cout << "Uso incorrecto del comando 'favorites'. Debes especificar un "
                    "argumento."
                 << endl;
        }
        /*else{
            stringstream ss;
            for (size_t i = 1; i < comandos[0].size(); ++i) {
                ss << comandos[0][i] << " ";
            }
            favs(ss.str());
        }
        */
        if (comandos[0][1] == "mostrar")
        {
            favs_mostrar();
        }
        else if (comandos[0][1] == "crear")
        {
            favs_crear(comandos[0][2]);
        }
        else if (comandos[0][1] == "guardar")
        {
            favs_guardar(comandos[0][2]);
        }
        else if (comandos[0][1] == "ejecutar")
        {
            favs_ejecutar(comandos[0][2]);
        }
        else if (comandos[0][1] == "buscar"){
            favs_buscar(comandos[0][2]);
        }
        else if (comandos[0][1] == "cargar"){
            favs_cargar(comandos[0][2]);
        }
        else if (comandos[0][1] == "borrar"){
            favs_borrar();
        }
        else if((comandos[0][1] == "eliminar")){
            favs_eliminar(comandos[0][2]);
        }


        // DESDE AQUI ES EL REMINDER
    }
    else if (comandos[0][0] == "reminder")
    {

        favs_guardarAuto(ComandoCompleto);

        if (comandos[0].size() < 3)
        {
            cout << "Uso incorrecto del comando 'reminder'. Debes especificar un "
                    "tiempo y un mensaje."
                 << endl;
        }
        else
        {
            try
            {
                int seconds = stoi(comandos[0][1]);
                string message = accumulate(comandos[0].begin() + 2, comandos[0].end(), string(), [](const string &a, const string &b)
                                            { return a.empty() ? b : a + " " + b; });
                reminder(seconds, message);
            }
            catch (const exception &e)
            {
                cout << "Uso incorrecto del comando 'reminder'. Debes especificar un "
                        "tiempo como número y un mensaje como cadena."
                     << endl;
            }
        }
        // HASTA ACÁ ES EL REMINDER
    }
    else
    {
        vector<char *> args;
        for (const auto &arg : comandos[0])
        {
            args.push_back(strdup(arg.c_str()));
        }
        args.push_back(nullptr); // Finaliza el array con un puntero nulo

        int pid = fork();
        if (pid == 0)
        { // Proceso hijo
            // Ejecutamos el comando
            if (execvp(args[0], args.data()) < 0)
            {
                cout << "Error en el comando ingresado" << endl;
                exit(EXIT_FAILURE); // Indica un error
            }
            exit(0); // Indica éxito
        }
        else if (pid < 0)
        {
            cout << "Error al crear el proceso hijo" << endl;
        }
        else
        { // Proceso padre
            int status;
            wait(&status); // Espera a que el hijo termine

            if (WIFEXITED(status))
            {                                       // Verifica si el hijo terminó normalmente
                int exitCode = WEXITSTATUS(status); // Obtiene el código de salida
                if (exitCode == 0)
                {
                    cout << "El comando se ejecutó con éxito." << endl;
                    favs_guardarAuto(ComandoCompleto);
                }
                else
                {
                    cout << "El comando falló con el código de salida: " << exitCode << endl;
                }
            }
        }

        // Liberar la memoria alocada por strdup
        for (char *arg : args)
        {
            free(arg);
        }
    }
}

bool command_exists(const string& command) {
    string cmd = "command -v " + command + " > /dev/null 2>&1";
    return (system(cmd.c_str()) == 0);
}

int main()
{
    vector<vector<string>> comandos; // Matriz de comandos.
    string comando;
    system("clear");
    bool prompt = true;
    cout << GRN "MyShell " << getCurrentDir() << " # ";
    cout << NC;
    long long maxim;

    while (getline(cin, comando))
    {
        maxim = 0;
        int ret = parser(comando, maxim, comandos);
        if (ret == 1)
        {
            comando.clear();
            continue;
        }

        //ESTA SHIET HACE UN STRING DEL COMANDO
        string ComandoCompleto = accumulate(comandos[0].begin(), comandos[0].end(), string(), [](const string &a, const string &b) { return a.empty() ? b : a + " " + b; });
        

        //-------------CASO DONDE SOLO SE TIENE UNA LINEA DE COMANDO: ls -la. NO

        // NECESITA PIPES--------------//
        if (comandos.size() <= 1)
        {
            comando_sin_pipe(comandos);
        }
        else
        {
            // Se buscan las pipe en la linea de comandos y se divide el comando

            char *ArgsCommand[comandos.size()][maxim + 1];
            for (int i = 0; i < comandos.size(); ++i)
            {
                for (int j = 0; j < comandos[i].size(); j++)
                {
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
            if (fork() == 0)
            {
                dup2(Pipes[0][WRITE], WRITE); // Abrimos el canal de escritura.
                close(Pipes[0][READ]);        // zamos el canal de lectura.
                int path = 1;
                while (path < allpipes)
                {
                    close(Pipes[path][READ]);
                    close(Pipes[path][WRITE]);
                    path++;
                }

                int a = execvp(ArgsCommand[0][0], ArgsCommand[0]);
                if (a < 0)
                {
                    cout << "Error en el comando ingresado" << endl;
                    exit(0);
                }
            }

            //----- 2 PROCESOS O MAS----//
            if (allpipes > 1)
            {
                while (count < allpipes - 1)
                {
                    if (fork() == 0)
                    {
                        dup2(Pipes[count][READ], READ); // Abrimos el canal de lectura.
                        dup2(Pipes[count + 1][WRITE],
                             WRITE); // Abrimos el canal de escritura.
                        for (int i = 0; i < allpipes; ++i)
                        {
                            if (i == count)
                                close(Pipes[i][WRITE]);
                            else if (i == count + 1)
                                close(Pipes[i][READ]);
                            else
                            {
                                close(Pipes[i][READ]);
                                close(Pipes[i][WRITE]);
                            }
                        }
                        int c = execvp(ArgsCommand[count + 1][0], ArgsCommand[count + 1]);
                        if (c < 0)
                        {
                            cout << "Error en el comando ingresado" << endl;
                            exit(0);
                        }
                    }
                    count++;
                }
            }

            //------ PARTE FINAL DEL PROCESO -----//

            if (fork() == 0)
            {
                dup2(Pipes[count][READ], READ); // Leer la ultima pipe
                close(Pipes[count][WRITE]);     // Cerrar escritura de la ultima pipe.
                int path = 0;

                while (path < allpipes)
                { // Cerrar las pipes
                    if (path == count)
                    {
                        path++;
                        continue;
                    }
                    close(Pipes[path][READ]);
                    close(Pipes[path][WRITE]);
                    path++;
                }

                int p = execvp(ArgsCommand[count + 1][0],
                               ArgsCommand[count + 1]); // Ejecutamos el comando.

                if (p < 0)
                {
                    cout << "Error en el comando ingresado" << endl;
                    exit(0);
                }
            }
            for (int i = 0; i < allpipes; ++i)
            {
                close(Pipes[i][WRITE]);
                close(Pipes[i][READ]);
            }
            ostringstream oss; 
            for (int l = 0; l < comandos.size(); ++l)
                wait(NULL); // Esperar a los hijos.
        
            for (size_t i = 0; i < comandos.size(); ++i) {
                    for (const auto &parte : comandos[i]) {
                        oss << parte << " ";  // Añadir cada parte del comando con un espacio
                    }
                    if (i < comandos.size() - 1) {
                        oss << "| ";  // Añadir el separador `|` entre los comandos, excepto después del último comando
                    }
                }
                if(command_exists(oss.str()) == 1){
                    favs_guardarAuto(oss.str());
                }
        }
        comandos.clear();
        comando.clear();
        cout << GRN "MyShell " << getCurrentDir() << " # ";
        cout << NC;
    }
    return 0;
}



