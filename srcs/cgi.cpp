
//besoin de : structure contenant les infos du fichier de config (cgi_path) + classe avec les infos de la requete
char *request; // on a la request dans la classe Request

if (request.py) // si la requete se termine par .py -> script
{
    int child_pid;
    int fd[2];

    if (pipe(fd) == -1)
        return (error);

    child_pid = fork();

    if (child_pid == -1)
        return (error);
    
    if (!child_pid) //on gere l'execve
    {
        //create_env function qui prend en parametre un objet requete et cree 
        //un char** pour stocker toutes les vars d'env necessaires a l'execution du script

    //server_name, server_protocl, gateway_interface, request_method, content_type
    //content_length , query_string, script_filename,, request_uri

        /*
        On veut send le contenu genere par execve
        Le child process termine apres execve
        On a donc genere le contenu txt html mais on peut pas le send au serveur
        Il est simplement displayed sur STDOUT
        STDOUT pointe sur fd[1] qu'on close pour eviter les leaks
        */
        close(fd[0]);
        if (dup2(fd[1], STDOUT_FILENO)) //on fait pointer STDOUT sur fd[1]
            return (error);
        close(fd[1]);

        char *php_interpreter = structure.get(php_interpreter_path);
        char *php_script = request.get_file_name();

        //verifier avec access si les 2 fichiers sont executables

        char *args[] = {php_interpreter, php_script, NULL};

        //le 3e arg de execve contient les vars d'env necessaires a l'execution du programme qu'on execve
        execve(php_interpreter, args, env);
        perror("execve failed");
    }
    close(fd[1]);
    //fonction de Nico pour lire dans un fichier depuis fd[0] pour recuperer 
    //tout l'output et le stocker dans une string "response"
    close(fd[0]);
    send(events[i].data.fd, response, strlen(response), 0);
}