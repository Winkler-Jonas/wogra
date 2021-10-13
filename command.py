import platform
import regex as re
from pathlib import Path
from subprocess import run, CalledProcessError


def clear_folder(directory: Path = None):
    directory = Path(directory)
    for item in directory.iterdir():
        if item.is_dir():
            clear_folder(item)
        else:
            item.unlink()
    directory.rmdir()


def add_ssh_keys(server_ip: str, ssh_port: str):
    try:
        ssh_file: Path = Path.home() / '.ssh' / 'known_hosts' if \
            re.search('linux', platform.system(), re.I) else \
            Path.home() / '%USERPROFILE%' / '.ssh'
        output = run(['ssh-keyscan', '-H', f'-p {ssh_port}', f'{server_ip}'], capture_output=True, text=True, check=True)
        ssh_fingerprint = re.search(r'stdout=\'(?P<fingerprint>.+?)\\n\'', str(output), flags=re.S | re.M)
        with ssh_file.open(mode='a') as file:
            file.write(test := ssh_fingerprint.group('fingerprint'))
            print(test)
    except CalledProcessError as ce:
        # ssh-keyscan could not be executed
        raise ce
    except FileNotFoundError as fe:
        # the file specifies was not found (not sure if windows actually works)
        raise fe
    except AttributeError as at:
        # regex did not find output from subprocess
        raise at


if __name__ == '__main__':
    add_ssh_keys(server_ip='141.82.59.167', ssh_port='2222')
    #clear_folder(Path.cwd() / 'TMP')
    #sh.git('clone', 'ssh://git@r-n-d.informatik.hs-augsburg.de:2222/dva/berichte-2020/53.git', 'TMP/', _out=git_interact, _out_bufsize=0)

    # execute command in background
    # p = sleep(3, _bg=True)