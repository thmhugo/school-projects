# Algorithms

> **Disclaimer** : These are first ideas and do not necessarily reflect real implementations.

## mkdirtar

```lua
function mkdirtar(fd, path):
    if commons_tar_find_file_from(fd, path, pheader) == FALSE
    then
        move_offset_to_end(fd)
        return commons_tar_append_dir(fd, path, TAR_DIR_F)
    else
        return ERR
```

## rmdirtar

```lua
function rmdirtar(fd, path):
    if commons_tar_find_file_from(fd, path, pheader) != FALSE
        && pheader.typeflag == TAR_DIR_F
        && is_dir_empty(fd, path)
    then
        pheader_offset = get_pheader_offset(pheader)
        return commons_tar_blockmove(fd, pheader_offset)
    else
        return ERR
```

## pwdtar

```lua
function pwdtar(void):
    return current_path_inside_tarball
```

## cptar

```lua
function cptar(fd_source, fd_target, path_source, path_target, with_option):
    if fd_source != FD_NOTDEF
        && commons_tar_find_file_from(fd_source, path_source, pheader_source) == ERR)
    then
            return ERR
    else
        if with_option then
            if fd_source != FD_NOTDEF && fd_target != FD_NOTDEF
            then
                return cptar_children(fd_source, fd_target, path_source, path_target)

            else if fd_source != FD_NOTDEF && fd_target == FD_NOTDEF
            then
                -- Pratiquement la même chose que cptar_children sauf qu'à la
                -- place de créer un nouveau header, on crée un nouveau fichier à
                -- l'extérieur du tar.
                return cptar_children_from_tar_to_outside(fd_source, path_source, path_target)
            else
                if is_dir(fd_source)
                then
                    /* Utiliser struct dirent, opendir, readdir etc.. */
                    return read_dir_and_tar_append(path_source, fd_target, path_target)
                else
                    buffer = read_content(fd_source)
                    return commons_tar_append(fd_target, buffer, path_target)
        else
            if fd_source != FD_NOTDEF && pheader_source.typeflag == TYPE_DIR
            then
                return ERR

            if fd_source != FD_NOTDEF && fd_target != FD_NOTDEF
            then
                return write_content_from_to(fd_source, fd_target, path_source, path_target)
            else if fd_source != FD_NOTDEF && fd_target == FD_NOTDEF
            then
                return read_from_tar_and_write_outside(fd_source, fd_target, path_source, path_target)
            else
                buffer = read_content(fd_source)
                return commons_tar_append(fd_target, buffer, path_source, path_target)
```

## cptar_children

```lua
function cptar_children(fd_source, fd_target, path_source, path_target):
    end_offset = get_end_offset(fd_source)
    move_offset_to_beginning(fd_source)
    actual_offset = get_actual_offset(fd_source)
    depth = commons_tar_get_depth(path_source - 1)

    while actual_offset != end_offset do:
        header = read_header(fd_source)

        if is_same_parent_path_as_src(fd_source, path_source, header.name) then
            write_content_from_to(fd_source, fd_target, header.name, path_target)

            if TAR_DIR_F == pheader.typeflag
                && strequals(path_src, commons_tar_format_name_according_max_depth(header.name, path_depth))
            then
                parent_dir_name = commons_tar_format_name_according_max_depth(path_source, depth)
                cptar_children(fd_source, fd_target, parent_dir_name, path_target, end_offset)

        actual_offset = move_to_next_header(fd_source)

    return OK
```

## rmtar

```lua
function rmtar(fd, path, with_option):
    if commons_tar_find_file_from(fd, path, pheader) == ERR then
        return ERR
    else
        if with_option then
            return rmtar_children(fd, path)
        else
            if pheader_source.typeflag == TYPE_DIR then
                return ERR
            else
                pheader_offset = get_pheader_offset(pheader)
                return commons_tar_blockmove(fd, pheader_offset)
```

### rmtar_children

```lua
function rmtar_children(fd, path):
    end_offset = get_end_offset(fd)
    move_offset_to_beginning(fd)
    actual_offset = get_actual_offset(fd)
    tmp_offset = 0

    while actual_offset != end_offset do:
        header = read_header(fd)

        if is_same_parent_path_as_src(fd, path, header.name) then
            commons_tar_blockmove(fd, actual_offset)
            actual_offset = tmp_offset
        else
            actual_offset = get_next_header_offset(fd)

        tmp_offset = actual_offset

    return OK
```

## commons_tar_blockmove

```lua
function commons_tar_blockmove(fd, off_target):
    off_src = next_header_offset()

    if read_and_overwrite(fd, off_src, off_target) == ERR then
        -- lit len_tar_ball - off_src caractères et réecrit à off_target :
        return ERR

    clear_last_block(fd)

    return OK
```

## mvtar

```lua
function mvtar(fd_source, fd_target, path_source, path_target):
    if fd_source != FD_NOTDEF
       && commons_tar_find_file_from(fd_source, path_source, header) == ERR
    then
        return ERR
    else
        if fd_source == fd_target && fd_source != FD_NOTDEF then
            return rename_file(fd_source, header, path_target)

        else if fd_source == FD_NOTDEF && fd_target != FD_NOTDEF then
            buffer = read_content(fd_source)
            commons_tar_append(fd_target, buffer, path_target)
            return remove(path_source)

        else if fd_source != FD_NOTDEF && fd_target != FD_NOTDEF then
            header_offset = get_pheader_offset(header)
            write_content_from_to(fd_source, fd_target, header.name, path_target)
            return commons_tar_blockmove(fd_source, header_offset)

        else
            detar_element_and_write_to(fd_source, fd_target, path_source, path_target)
```

## Parsing

les fonctions de la file sont décrites [ici](https://troydhanson.github.io/uthash/utlist.html)

On utilise ces structures pour stocker une commande

```c
    typedef struct {
        char cmd[CMD_MAX_SIZE];
        int nb_args;
        char *args[];
        int  with_option;
        s_command_redirections *command_redirections;
        s_command *next;
    } s_command;

    typedef struct {
        s_tsh_path    *path;
        redirect_type type;
    } s_redirection;


    typedef struct {
        s_redirection *in_red;
        s_redirection *out_red;
        s_redirection *err_red;
    } s_command_redirections;
```

Fonction de parsing :

```c
function parse(line):
    replace(line, "\t", " ")
    nb_tokens, tokens[] := tokenize(line, '|')
    File file;

    for (i=0; i<nb_tokens; i++) {
        s_command cmd;
        nb_args, args[] := tokenize(tokens[i], ' ')
        cmd.set_cmd(args[0])
        cmd.set_nb_args(ng_args)

        for (j=1; j<nb_args; j++) {
            contains_redirection = FALSE
            if (j<nb_args-1) {
                switch (args[j]) {
                    case ">" :
                        set_redirection(STdoUT to args[j+1], truncate)
                        contains_redirection = TRUE
                    case ">>":
                        set_redirection(STdoUT to args[j+1], append)
                        contains_redirection = TRUE
                    ..... // tous les symboles de redirection
                }
                if (contains_redirection) {
                    j++
                    cmd.nb_args - 2
                    contains_redirection = FALSE
                }
            }

            if (args[j][0] = '-') {
                switch (args[j][1]) {
                    case "l" :
                        if (cmd.cmd = "ls") {
                            cmd.with_option = TRUE
                            args[j] = NULL
                            cmd.nb_args - 1
                        }
                    // ... toutes les options implémentées
                    // si ce n'est pas dans les options implémentés,
                    // ce sont les fonctions exec() de linux qui s'en occuperont
                }
            }
            if (args[j] != NULL) {
                cmd.add_arg(args[j])
            }
            file.enfile(cmd)
        }
    }
```

## Shell

### Main

> Note: Le parser devrait peut-être renvoyer EXIT_CMD si l'utilisateur exécute
> la commande "exit" et le main devrait s'arrêter directement.

```lua
function main(void):
    init_current_state(getcwd())
    while true do
        print_prompt
        line <- read_line
        commands_ll <- parse(line)

        if nil == commands_ll then
            print_err
        else if nil != (err_msg <- run(commands_ll)) then
            print_err(err_msg)

function run(commands_ll):
    in_fd   <- FD_NOTDEF
    out_fd  <- FD_NOTDEF
    err_fd  <- FD_NOTDEF
    pipe_fd <- [ FD_NOTDEF, FD_NOTDEF ]

    foreach cmd in commands_ll do
        -- if its a tar redirection should open a pipe/file
        -- to redirect STDIN_FILENO to.
        try_to_open_redirection(in_fd, cmd)
        try_to_open_redirection(out_fd, cmd)
        try_to_open_redirection(err_fd, cmd)

        if FD_NOTDEF = in_fd and FD_NOTDEF != pipe_fd then
            in_fd <- pipe_fd

        if FD_NOTDEF = out_fd and FD_NOTDEF != pipe_fd then
            out_fd <- pipe_fd

        if !fork() then
            if FD_NOTDEF != in_fd then redirect_stdin_to(in_fd)
            if FD_NOTDEF != out_fd then redirect_stdout_to(out_fd)
            if FD_NOTDEF != err_fd then redirect_stdout_to(err_fd)

            execute(cmd, state)
        else
            wait();
            if stdout_is_tar_redirect(cmd) then
                write_content_from_to(out_fd, cmd)
            if stderr_is_tar_redirect(cmd) then
                write_content_from_to(err_fd, cmd)

        reset_redirections()
        close_fds()
```

### execution d'une *s_command*

fonction `execute(s_command, s_tsh_state)` qui execute une commande.

les *exec* sont executés dans des processus fils.  les retours des appels
*exec* sont gérés dans le père (celui qui lance `execute`) et celui-ci retourne
ERR en cas d'erreur.

```c
function execute(s_command cmd, s_tsh_state state) {
    switch (cmd -> cmd) {

        // exemple pour une commande implémentée,
        case "ls" :
            for (i : 0 -> cmd -> nb_args) {
                path = cmd -> args[i]
                // cas ou on est sur que la commande ne peut être pour
                // un tar : une option mais pas une option implémentée
                // pour les tarball : on lance un appel système exec avec tous
                // les arguments de la commande
                if (cmd -> with_option && !tarball_option) {
                    fork()
                    fil : exec(ls, args)
                    pere : wait() and return OK or ERR
                }

                // cas : on est dans un tar et le chemin est dans ce même tar
                else if (state -> tar_fd
                        && (commons_tar_find_dir_from(state->tar_fd, path)
                            || commons_tar_find_file_from(state->tar_fd, path)))
                {
                    if (!cmd -> tarball_option) {
                        print("option inconnue")
                        return ERR
                    }
                    lstar(state -> tar_fd, cmd -> with_option)
                }

                // cas : on est pas dans un tarball ou bien le chemin mène hors
                // du tarball ou on est
                else if(is_dest_in_a_tarball(path)) {
                    // cette fonction ouvre le tar dans le chemin ,
                    // et retourne le fd ce celui-ci, tout en donnant le
                    // chemin du fichier voulu à l'intérieur du tar.
                    inside_path, nfd = open_the_tarball_in_path(path);
                    lstar(ndf, cmd -> with_option, inside_path);
                }

                // cas par défaut : on execute la commande avec le chemin actuel
                else {
                    exec_args = {
                        path,
                        if (cmd -> with_option) "-l" else NULL,
                        NULL };

                    fork()
                    fil : exec(ls, exec_args)
                    pere : wait() and may return ERR
                    }
                }
                return OK
            }

        default :
            fork()
            fils : execl(cmd -> cmd , args);
            pere : wait and return OK or ERR
    }
}
```

### Ancien algo de Hugo

#### Fonction de *lancement* des commandes de la file de *s_command*

Le père créé *nb_commandes*  processus fils et se synchronise à la fin.

```c
function run(s_command cmd) {
    nb_pipes = count_pipes(cmd)
    pipes_fd[2*pipes_fd] = init_pipesfd(nb_pipes)
    j = 0
    int status;

    while (cmd) {
        pid = fork()
        if (pid < 0) return ERR

        if (0 == pid) { // fils
            if (is_not_last_cmd(cmd)) {
                redirect_stdout_to(pipes_fd[j+1])
            }

            if (is_not_first_cmd(cmd) {
                redirect_stdin_to(pipes_fd[j-2])
            }

            for (i : 0 -> 2*nb_pipes) {
                close(pies_fd[i])
            }

            if (ERR = manage_redirection_and_exec(cmd))
                exit(FAILURE)
            else
                exit(SUCCESS)

        }
        cmd = cmd -> next;
        j+2;
    }

    for (i : 0 -> 2*nb_pipes) {
        close(pipes_fd[i])
    }

    for (i : 0 -> nb_pipes) {
        wait(&status)
    }
}

```

#### gestion des redirections

fonction `manage_redirection_and_exec(s_command)` qui gère les redirections
puis l'execution d'une *s_command*

pour chaque redirection d'entrée :

* pipe : redirection de stdout vers l'entrée du pipe (écrivain)
* execution (avec `execute`) de la commande (donc la sortie est écrite dans le pipe)
* tant que le pipe n'est pas vide
  * lecture de le pipe et écriture du contenu dans un buffer
  * pour chaque redirection de sortie :
  * ecriture au bout du chemin (en fonction du type de redirection) du contenu
du buffer

        default :
            fork()
            fils : execl(cmd -> cmd , args);
            pere : wait and return OK or ERR
    }
}
```

