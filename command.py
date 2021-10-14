import os
import sh
import platform
import regex as re
import logging as log
from pathlib import Path
import multiprocessing as mp
from typing import Dict, List
from collections import defaultdict
from collections.abc import Callable
from subprocess import run, CalledProcessError
from main import get_gitlab_info, convert_date

url_hsa: str = 'https://r-n-d.informatik.hs-augsburg.de:8080/'
p_token_hsa: str = 'bsrg4w9xqTxotvxzVLGD'
url_wogra: str = 'https://gitlab.wogra.com'
p_token_wogra: str = 'Lq1z1hMxG_yKeyTLaAXD'

TMP_REPO_FOLDER: str = 'tmp_repo'
log.basicConfig(filename='basic.log', level=log.INFO)


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
        keyscan_output = run(['ssh-keyscan', '-H', f'-p {ssh_port}', '-t', 'rsa,ecdsa,ed25519', f'{server_ip}'], capture_output=True, text=True, check=True)
        ssh_fingerprint = re.search(r'stdout=\'(?P<fingerprint>.+?)\\n\'\,\sstderr=', str(keyscan_output), flags=re.S | re.M)
        with ssh_file.open(mode='w') as file:
            file.write(re.sub(r'(?P<false_line_sep>\\n)', os.linesep, ssh_fingerprint.group('fingerprint'), re.MULTILINE))
        return True
    except CalledProcessError as ce:
        # ssh-keyscan could not be executed
        log.error('Error occurred while trying to get ssh-fingerprints')
        raise ce
    except FileNotFoundError as fe:
        # the file specifies was not found (not sure if windows actually works)
        raise fe
    except AttributeError as at:
        # regex did not find output from subprocess
        raise at


def dl_repo(repo_url: List[str]):
    """
    Download a repository to the defined folder

    :param repo_url: The UID of the repository and its url
    :type repo_url: List[str]
    """
    sh.git('clone', repo_url[1], tmp_path := Path.cwd() / TMP_REPO_FOLDER / repo_url[0])
    log.info(f'Cloning {repo_url[1]} to {tmp_path}')


def run_function(repo_server: str, ssh_port: str, repo_dict: defaultdict[str, Dict[str, str]], author_name: str, time_frame: int = None):
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
        add_ssh_keys(server_ip=repo_server, ssh_port=ssh_port)
        # Single-Core (10xslower) [dl_repo([key, value['repo_url']]) for key, value in repo_dict.items()]
        pool.map(dl_repo, [[key, value['repo_url']] for key, value in repo_dict.items()])
        for folder in (Path.cwd() / TMP_REPO_FOLDER).iterdir():
            try:
                git = sh.git.bake('--no-pager', _cwd=f'{folder.resolve()}')
                log.info(commit_msg := git('log',
                                           f'--after={convert_date(time_frame).date()}T00:00:00',
                                           f'--author={author_name}',
                                           '--pretty=format:%an;%ai;%s;%b'
                                           ))
                print(commit_msg)
            except sh.ErrorReturnCode as e:
                if 'ErrorReturnCode_128' == e.__class__.__name__:
                    # Error occurs when no commit on branch *IGNORE*
                    continue
                print('should not be here')
                raise e
    except sh.ErrorReturnCode as e:
        # occurs when there is no commit fitting the provided filters
        log.error(e.stderr)
        print('not good')
        pass
    finally:
        prep_clean()


if __name__ == '__main__':
    time_frame: int = 100
    user: str = 'Huber Hoegl'
    run_function(*get_gitlab_info(url=url_hsa,
                                  private_token=p_token_hsa,
                                  time_in_days=time_frame),
                 author_name=user,
                 time_frame=time_frame)
