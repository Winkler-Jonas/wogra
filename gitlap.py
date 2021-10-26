from exceptions import RegexException, CommandException
from subprocess import run, CalledProcessError, Popen, PIPE
from datetime import datetime, timedelta, date
from typing import List, Union, Callable
from repository import Repository
from data_types import POutput
import multiprocessing as mp
from shutil import rmtree
from pathlib import Path
import gitlab as git_api
from os import linesep
import regex as re
import platform
import logging
logger = logging.getLogger('root')

repo_pattern = re.compile("""
            id                              
            \W+                             # tag and value seperated by none 'norm' characters
                (?P<id>                    
                    \d+?                    # capture id consisting of one or more numbers
                )                           
            \,.*?name\W+
                (?P<name>                   
                    [\w\-\s]+?              # capture name consisting of [a-zA-Z_- and spacing] 
                )\'                         # non-greedy ends with apostrophe
            .*?ssh_url_to_repo\W+
                (?P<url>                    
                    .*?                     # capture url non-greedy 
                )                           
            \'.*?last_activity_at\W+        # group capture ends with apostrophe
                (?P<date>                   
                    \d{4}-\d{2}-\d{2}       # capture date format YYYY-MM-DD
                )                 
            """, flags=re.M | re.S | re.VERBOSE)

server_port_pattern = re.compile("""
            \@                   
                (?P<server>          
                    .*                  # capture anything greedy from @ to : 
                )                    
                    (?=:                # look ahead for :
                (?P<port>            
                    \d+                 # any digit greedy
                )                    
            )                   
            """, flags=re.M | re.S | re.VERBOSE)

# todo config file needed
TMP_REPO_FOLDER: str = 'tmp_repo'


class Gitlab:

    @staticmethod
    def days_to_date(time_in_days: Union[int, str] = None) -> datetime:
        """
        Used to determine which time to use for data retrieval.

        If a time is provided, today - days(time) provided is the new timeframe
        Else The time-frame is set from the first of the month till today

        :param time_in_days: The time in days subtracted from today (timeframe)
        :type time_in_days: int
        :return: The actual date used for further references
        :rtype: date
        """
        try:
            if isinstance(time_in_days, str):
                time_in_days = int(float(time_in_days))
        except TypeError:
            raise AttributeError('Timeframe not legal, use different timeframe')
        today: str = date.today().strftime("%Y-%m-%d")
        if time_in_days:
            date_y_m_d: datetime = datetime.strptime(f"{today}", "%Y-%m-%d") - timedelta(days=time_in_days)
        else:
            date_y_m_d: datetime = datetime.strptime(f"{today[:-2]}01", "%Y-%m-%d")
        return date_y_m_d

    @staticmethod
    def cleanup(directory: Path = Path.cwd() / TMP_REPO_FOLDER):
        """
        Creates or deletes provided directory (default 'tmp_repo' dir), depending
        on whether it exists or not.

        :param directory: The directory to remove or to create
        :type directory: Path
        """
        if directory.exists():
            rmtree(directory.resolve())
        else:
            directory.mkdir(parents=True, exist_ok=False)

    @staticmethod
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
            logger.error(f'{err_no}: {err_msg}')
            raise CommandException(err_no, err_msg)
        return POutput(*p_git.communicate())

    def __init__(self, url: str, private_token: str):
        self.__url: str = url
        self.__p_token: str = private_token
        self.__server_ip: str = ''
        self.__port: str = ''

        self.__str_project_list: List[str] = self.__get_project_list()
        self.__repository_list: List[Repository] = self.__get_project_info(Gitlab.days_to_date(99999))

    @property
    def server(self) -> str:
        return self.__server

    @property
    def port(self) -> str:
        return self.__port

    def __get_project_list(self) -> list:
        """
        Connect to GitLab and retrieve project information

        :raises ConnectionError: In case url/credentials incorrect

        :return: List of project-objects
        :rtype: List
        """
        with git_api.Gitlab(url=self.__url, private_token=self.__p_token) as gl:
            try:
                return gl.projects.list(all=True)
            except ConnectionError as conn_err:
                logger.exception(f'Error! Sever: {self.__url} not found')
                raise conn_err
            except git_api.GitlabAuthenticationError:
                logger.exception('Error! Cannot log in to Gitlab, please verify credentials')
                raise ConnectionError()

    def __get_project_info(self, date_y_m_d: datetime) -> List[Repository]:
        """
        Convert list of projects into strings and filter out
        projects that were not edited in the provided timeframe

        :raises AttributeError: If input parameter corrupt (See log-file for error message)
        :raises RegexException: If provided strings don't match regex expressions
        :raises ValueError: In case of Unknown-Error occurs

        :param date_y_m_d: The date the repository had to be edited the very least
        :type date_y_m_d: datetime
        :return: A list of repository-objects containing all interesting information
        :rtype: List[Repository]
        """
        try:  # confirm date
            datetime(year=date_y_m_d.year, month=date_y_m_d.month, day=date_y_m_d.day, hour=date_y_m_d.hour)
            if not self.__str_project_list:
                raise IndexError
        except ValueError as val_err:  # Date not allowed
            logger.error(f'Error occurred! Illegal timestamp: {date_y_m_d}')
            raise AttributeError()
        except IndexError as idx_err:  # Project-list empty
            logger.error(f'Error occurred! Provided gitlab-project-list empty')
            raise AttributeError()

        repository_lst: List[Repository] = []
        try:
            for test in self.__str_project_list:
                if not repo_pattern.search(str(test)):
                    print(str(test))
            for repository_pattern_match in (repo_pattern.search(str(repo_str)) for repo_str in self.__str_project_list):
                repo_id, repo_name, repo_url, repo_date_str = repository_pattern_match.group('id', 'name', 'url',
                                                                                                    'date')
                if (repo_date := datetime.strptime(f'{repo_date_str}', '%Y-%m-%d')) > date_y_m_d:
                    repository_lst.append(Repository(repo_id=repo_id,
                                                     name=repo_name,
                                                     url=repo_url,
                                                     date_edited=repo_date))
        except AttributeError:
            err_msg: str = f'Error occurred grouping either id/name/url or date! Str-Dump: {linesep}{"".join(str(proj) for proj in self.__str_project_list)}{linesep}'
            logger.error(err_msg)
            raise RegexException(-10, err_msg)
        except ValueError as val_err:
            logger.error(f'Unknown Error!! {val_err}')
            raise val_err
        try:  # Server and port are port are in ssh_url_to_repo string
            any_ssh_url: re.Match = server_port_pattern.search(repository_lst[-1].url)
            self.__server_ip, self.__port = any_ssh_url.group('server', 'port')
        except AttributeError:
            err_msg: str = f'Error occurred grouping either server or port! Str-Dump: {linesep}{repository_lst[-1].url}{linesep}'
            logger.error(err_msg)
            raise RegexException(-10, err_msg)
        except IndexError:
            exc_msg: str = f'Exception occurred! Provided string did not match any internal search pattern' \
                           f'Please provided string containing ID/Name/URL..'
            logger.exception(exc_msg)
            exit(0)
        return repository_lst

    def __add_ssh_keys(self) -> bool:
        """
        Add ssh_fingerprint to known Hosts file.
        Designed to work on windows and linux

        :raises CommandException: When ssh-keyscan command could not be executed
        :raises AttributeError: hosts file was not found or couldn't be edited

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
            keyscan_cmd: List[str] = ['ssh-keyscan', '-H', f'-p {self.__port}', '-t', 'rsa,ecdsa,ed25519', f'{self.__server_ip}']
            keyscan_output = run(keyscan_cmd, capture_output=True, text=True, check=True)
            ssh_fingerprint = re.search(r'stdout=\'(?P<fingerprint>.+?)\\n\'\,\sstderr=', str(keyscan_output),
                                        flags=re.S | re.M)
            with ssh_file.open(mode='w') as file:
                file.write(
                    re.sub(r'(?P<false_line_sep>\\n)', linesep, ssh_fingerprint.group('fingerprint'), re.MULTILINE))
            return True
        except CalledProcessError as ce:
            # ssh-keyscan could not be executed
            logger.error('Error occurred while trying to get ssh-fingerprints')
            raise CommandException(-10, f'Error occurred! Cannot execute command: {keyscan_cmd}')
        except FileNotFoundError:
            # the file specified was not found (not sure if windows actually works)
            raise AttributeError(f'Error occurred! File for trusted devices was not found')
        except AttributeError:
            # regex did not find output from subprocess
            raise RegexException(-12, f'Error occurred! Cannot set Gitlab-Server as trusted device')

    def get_repositories(self, author_name: str, time_frame: Union[int, str] = None) -> List[Repository]:
        """
        Filter list of repositories by author and time-frame.

        :raises CommandException: When ssh-keyscan command could not be executed
        :raises AttributeError: hosts file was not found or couldn't be edited

        :param author_name: The name of the author / committer
        :type author_name: str
        :param time_frame: The amount of days to ga back from today
        :type time_frame: Union[int, str]
        :returns: List with Repository-objects containing information about actual repositories
        :rtype: List[Repository]
        """
        # prologue
        ret_list: List[Repository] = []
        Gitlab.cleanup()
        self.__add_ssh_keys()
        repo_folder: Path = Path.cwd() / TMP_REPO_FOLDER
        pool = mp.Pool(mp.cpu_count())
        try:
            # Single-Core (10xslower) [dl_repo([key, value['repo_url']]) for key, value in repo_dict.items()]
            pool.map(Gitlab.git_run,
                     [['git', 'clone', f'{repo.url}', f'{str((repo_folder / repo.repo_id).resolve())}'] for repo in
                      self.__repository_list if repo.date_edited >= Gitlab.days_to_date(time_frame)])
            for folder in repo_folder.iterdir():
                cmd_git_log: List[str] = ['git', 'log',
                                          '-i',
                                          f'--after={Gitlab.days_to_date(time_frame).date()}T00:00:00',
                                          f'--author={author_name}',
                                          '--pretty=format:"%an;%ai;%s;%b"']
                git_log_out: POutput = Gitlab.git_run(cmd_git_log, cwd=folder)
                if git_log_out.stderr:
                    logger.error(f'Error occurred while trying to clone repositories: {git_log_out.stderr}')
                elif git_log_out.stdout and not git_log_out.stderr:
                    repo: Repository = next((x for x in self.__repository_list if x.repo_id == folder.stem), None)
                    repo.commits = git_log_out.stdout.splitlines()
                    ret_list.append(repo)
        finally:
            Gitlab.cleanup()
        return ret_list
