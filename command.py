from typing import Dict, Tuple, List

import sh
import platform
import regex as re
from pathlib import Path
import multiprocessing as mp
from collections import defaultdict
from collections.abc import Callable
from subprocess import run, CalledProcessError
from main import get_gitlab_info

url_hsa: str = 'https://r-n-d.informatik.hs-augsburg.de:8080/'
p_token_hsa: str = 'bsrg4w9xqTxotvxzVLGD'

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
        output = run(['ssh-keyscan', '-H', f'-p {ssh_port}', f'{server_ip}'], capture_output=True, text=True, check=True)
        ssh_fingerprint = re.search(r'stdout=\'(?P<fingerprint>.+?)\\n\'', str(output), flags=re.S | re.M)
        # todo check if fingerprint already in file
        with ssh_file.open(mode='a') as file:
            file.write(ssh_fingerprint.group('fingerprint'))
        return True
    except CalledProcessError as ce:
        # ssh-keyscan could not be executed
        raise ce
    except FileNotFoundError as fe:
        # the file specifies was not found (not sure if windows actually works)
        raise fe
    except AttributeError as at:
        # regex did not find output from subprocess
        raise at


def clone_repo(url: str, directory: Path):
    pass


def dl_repo(repo_url: List[str]):
    sh.git('clone', repo_url[1], tmp_path := Path.cwd() / TMP_REPO_FOLDER / repo_url[0])
    print(f'Cloning {repo_url[1]} to {tmp_path}')


def run_function(repo_server: str, ssh_port: str, repo_dict: defaultdict[str, Dict[str, str]], user_name: str, time_frame: str = None):
    """
    Function needed to build the whole process

    - Cleanup / Prep folder if necessary
    - Clone repos todo posix (folder to be called like repo)
    - Search for entries of user in given timeframe (usually from the 1st of the month till execution date)
    - Collect data
    - Return data / Or store it in a db ?!?
    """
    pool = mp.Pool(mp.cpu_count())
    try:
        prep_clean()
        # [dl_repo([key, value['repo_url']]) for key, value in repo_dict.items()]
        pool.map(dl_repo, [[key, value['repo_url']] for key, value in repo_dict.items()])

    except ValueError as e:
        pass
    finally:
        prep_clean()


if __name__ == '__main__':

    run_function(*get_gitlab_info(url=url_hsa, private_token=p_token_hsa, time_in_days=100), user_name='olaf')
    # add_ssh_keys(server_ip='r-n-d.informatik.hs-augsburg.de', ssh_port='2222')
    # clear_folder(Path.cwd() / 'TMP')
    # sh.git('clone', 'ssh://git@r-n-d.informatik.hs-augsburg.de:2222/dva/berichte-2020/53.git', 'TMP/')

    # execute command in background
    # p = sleep(3, _bg=True)