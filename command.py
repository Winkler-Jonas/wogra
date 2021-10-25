import os
import platform
import regex as re
import logging as log
from typing import List
from pathlib import Path
import multiprocessing as mp
from repository import Repository
from collections.abc import Callable
from dataclasses import dataclass, field
from main import get_gitlab_info, convert_date, RegexException
from subprocess import run, CalledProcessError, Popen, PIPE


class CommandException(Exception):
    def __init__(self, errno: int, msg: str):
        self._msg: str = msg
        self._errno: int = errno
        super(CommandException, self).__init__('msg: {}, errno: {}'.format(msg, errno))

    def __reduce__(self):
        return CommandException, (self._msg, self._errno)

    @property
    def msg(self) -> str:
        return self._msg

    @property
    def errno(self) -> int:
        return self._errno


@dataclass
class POutput:
    stdout: str
    stderr: str

    _stdout: str = field(init=False, repr=False)
    _stderr: str = field(init=False, repr=False)

    @property
    def stdout(self) -> str:
        return self._stdout

    @stdout.setter
    def stdout(self, stdout: bytes) -> None:
        self._stdout = ''.join(map(chr, stdout))

    @property
    def stderr(self) -> str:
        return self._stderr

    @stderr.setter
    def stderr(self, stderr: bytes) -> None:
        self._stderr = ''.join(map(chr, stderr))


url_hsa: str = 'https://r-n-d.informatik.hs-augsburg.de:8080/'
p_token_hsa: str = 'bsrg4w9xqTxotvxzVLGD'
url_wogra: str = 'https://gitlab.wogra.com'
p_token_wogra: str = 'Lq1z1hMxG_yKeyTLaAXD'

TMP_REPO_FOLDER: str = 'tmp_repo'


def prep_clean(directory: Path = Path.cwd() / TMP_REPO_FOLDER):
    """
    Creates or deletes provided directory (default 'tmp_repo' dir), depending
    on whether it exists or not.

    :param directory: The directory to remove or to create
    :type directory: Path
    """
    create_folder: Callable = lambda directory_to_create: directory_to_create.mkdir(parents=True, exist_ok=False)

    def clear_folder(directory_to_clear: Path = None):
        directory_to_clear = Path(directory_to_clear)
        for item in directory_to_clear.iterdir():
            if item.is_dir():
                clear_folder(item)
            else:
                item.unlink()
        directory_to_clear.rmdir()

    if directory.exists():
        clear_folder(directory)
    else:
        create_folder(directory)


def add_ssh_keys(server_ip: str, ssh_port: str) -> bool:
    """
    Add ssh_fingerprint to known Hosts file.
    Designed to work on windows and linux

    todo raises several errors, not sure about their names yet

    :param server_ip: The ssh servers ip address
    :type server_ip: str
    :param ssh_port: The port the servers uses for ssh connection
    :type ssh_port: str
    :return: True if successful, else None
    :rtype: bool
    """
    try:
        ssh_file: Path = Path.home() / '.ssh' / 'known_hosts' if \
            re.search('linux', platform.system(), re.I) else \
            Path.home() / '%USERPROFILE%' / '.ssh'
        keyscan_cmd: List[str] = ['ssh-keyscan', '-H', f'-p {ssh_port}', '-t', 'rsa,ecdsa,ed25519', f'{server_ip}']
        keyscan_output = run(keyscan_cmd, capture_output=True, text=True, check=True)
        ssh_fingerprint = re.search(r'stdout=\'(?P<fingerprint>.+?)\\n\'\,\sstderr=', str(keyscan_output), flags=re.S | re.M)
        with ssh_file.open(mode='w') as file:
            file.write(re.sub(r'(?P<false_line_sep>\\n)', os.linesep, ssh_fingerprint.group('fingerprint'), re.MULTILINE))
        return True
    except CalledProcessError as ce:
        # ssh-keyscan could not be executed
        log.error('Error occurred while trying to get ssh-fingerprints')
        raise CommandException(f'Error occurred! Cannot execute command: {keyscan_cmd}')
    except FileNotFoundError as fe:
        # the file specifies was not found (not sure if windows actually works)
        raise AttributeError(f'Error occurred! File for trusted devices was not found')
    except AttributeError as at:
        # regex did not find output from subprocess
        raise RegexException(-12, f'Error occurred! Cannot set Gitlab-Server as trusted device')


# Done
def git_run(cmd: List[str], cwd: Path = None) -> POutput:
    """
    Execute a git command (could also be used to execute any shell command)

    :raises CommandException: if the command proved could not be executed

    :param cmd: The command to be executed
    :type cmd: List[str]
    :param cwd: The directory the command ought to be executed from
    :type cwd: Path
    :return: Dataclass containing commands stdout- and stderr-mgs
    :rtype: POutput
    """
    try:
        if cwd:
            p_git = Popen(cmd, stderr=PIPE, stdout=PIPE, cwd=str(cwd.resolve()))
        else:
            # Git commands like "clone" have cwd built in
            p_git = Popen(cmd, stderr=PIPE, stdout=PIPE)
    except FileNotFoundError as e:
        err_msg, err_no = f'Command was not found / Sys-Error-msg: {e.strerror}', 1
        log.error(f'{err_no}: {err_msg}')
        raise CommandException(err_no, err_msg)
    return POutput(*p_git.communicate())


def run_function(repo_server: str, ssh_port: str, repo_lst: List[Repository], author_name: str, time_frame: int = None):
    """
    Function needed to build the whole process

    - Cleanup / Prep folder if necessary
    - add ssh fingerprints to known_hosts file
    - Clone repos
    - Search for entries of user in given timeframe (usually from the 1st of the month till execution date)
    - Collect data
    - Return data / Or store it in a db ?!?
    todo des sollte noch bissl besser verteilt werden, so dass es Sinn macht...
    """
    pool = mp.Pool(mp.cpu_count())
    try:
        prep_clean()
        repo_folder: Path = Path.cwd() / TMP_REPO_FOLDER
        add_ssh_keys(server_ip=repo_server, ssh_port=ssh_port)
        # Single-Core (10xslower) [dl_repo([key, value['repo_url']]) for key, value in repo_dict.items()]
        pool.map(git_run, [['git', 'clone', f'{repo.url}', f'{str((repo_folder / repo.repo_id).resolve())}'] for repo in repo_lst])
        for folder in repo_folder.iterdir():
            cmd_git_log: List[str] = ['git', 'log',
                                      '-i',
                                      f'--after={convert_date(time_frame).date()}T00:00:00',
                                      f'--author={author_name}',
                                      '--pretty=format:"%an;%ai;%s;%b"']
            git_log_out: POutput = git_run(cmd_git_log, cwd=folder)
            print(git_log_out)
            # todo read stderr from git_log_out POutput-Object
    finally:
        prep_clean()


def pretty_print(repo_action: List[Repository]):
    pass


if __name__ == '__main__':
    time_frame: int = 3
    user: str = 'Hubert Hoegl'
    run_function(*get_gitlab_info(url=url_hsa,
                                  private_token=p_token_hsa,
                                  time_in_days=time_frame),
                 author_name=user,
                 time_frame=time_frame)


# Grobgliederung fertig machen
# Einleitung fertigstellen
# Gab es überhaupt ausfälle / wenn ja, wie lange? / Unter 5 minuten rauswerfen / Speicherverbrauch / Diskverbrauch
# Trafic Auslastungen
# Welche weitere Informationen würden sich noch lohnen zum Monitorin
# Wie verpackt man die daten für den kunden / was muss auf das Dokument??
# Prometheus quering (10.0.70.10:9090)
# Pdf generator von wogra mit json verwenden