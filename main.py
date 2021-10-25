import gitlab
import regex as re
import logging as log
from typing import List
import logging.handlers
from pathlib import Path
from os import linesep, environ
from repository import Repository
from collections import defaultdict
from datetime import date, datetime, timedelta

logging_handler = logging.handlers.WatchedFileHandler(environ.get('LOGFILE', f'{(Path.cwd() / "basic.log").resolve()}'))
logging_formatter = logging.Formatter(logging.BASIC_FORMAT)
logging_handler.setFormatter(logging_formatter)

root = logging.getLogger()
root.setLevel(environ.get('LOGLEVEL', 'INFO'))
root.addHandler(logging_handler)


class RegexException(Exception):
    def __init__(self, errno: int, msg: str):
        self._msg: str = msg
        self._errno: int = errno
        super(RegexException, self).__init__('msg: {}, errno: {}'.format(msg, errno))

    def __reduce__(self):
        return RegexException, (self._msg, self._errno)

    @property
    def msg(self) -> str:
        return self._msg

    @property
    def errno(self) -> int:
        return self._errno


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

url_wogra: str = 'https://gitlab.wogra.com'
p_token_wogra: str = 'Lq1z1hMxG_yKeyTLaAXD'
url_hsa: str = 'https://r-n-d.informatik.hs-augsburg.de:8080/'
p_token_hsa: str = 'bsrg4w9xqTxotvxzVLGD'


def __get_project_list(url: str, token: str) -> list:
    """
    Connect to GitLab and retrieve project information

    :param url: The GitLab Server url
    :type url: str
    :param token: Private Access Token from GitLab
    :type token: str
    :return: List of project-objects
    :rtype: List
    """
    with gitlab.Gitlab(url=url, private_token=token) as gl:
        try:
            return gl.projects.list(all=True)
        except:
            log.exception('Error! Cannot log in to Gitlab, please verify credentials')
            raise


def __get_project_info(proj_list: list, date_y_m_d: datetime) -> tuple[str, str, List[Repository]]:
    """
    Convert list of projects into strings and filter out
    projects that were not edited in the provided timeframe

    :raises AttributeError: If input parameter corrupt (See log-file for error message)
    :raises RegexException: If provided strings don't match regex expressions
    :raises ValueError: In case of Unknown-Error occurs

    :param date_y_m_d: The date the repository had to be edited the very least
    :type date_y_m_d: date
    :param proj_list: The list of projects on the server
    :type proj_list: list
    :return: The server-address, the ssh-port of the server and a dictionary
    containing relevant information about the repo
    :rtype: tuple[str, str, defaultdict[Any, dict]]
    """
    # confirm date
    try:
        datetime(year=date_y_m_d.year, month=date_y_m_d.month, day=date_y_m_d.day, hour=date_y_m_d.hour)
        if not proj_list:
            raise IndexError
    except ValueError as val_err:  # Date not allowed
        log.error(f'Error occurred! Illegal timestamp: {date_y_m_d}')
        raise AttributeError()
    except IndexError as idx_err:  # Project-list empty
        log.error(f'Error occurred! Provided gitlab-project-list empty')
        raise AttributeError()

    repository_lst: List[Repository] = []
    try:
        for repository_pattern_match in (repo_pattern.search(str(repo_str)) for repo_str in proj_list):
            repo_id, repo_name, repo_url, repo_date_str = repository_pattern_match.group('id', 'name', 'url', 'date')
            if (repo_date := datetime.strptime(f'{repo_date_str}', '%Y-%m-%d')) > date_y_m_d:
                repository_lst.append(Repository(repo_id=repo_id,
                                                 name=repo_name,
                                                 url=repo_url,
                                                 date_edited=repo_date))
    except AttributeError:
        err_msg: str = f'Error occurred grouping either id/name/url or date! Str-Dump: {linesep}{proj_list}{linesep}'
        log.error(err_msg)
        raise RegexException(-10, err_msg)
    except ValueError as val_err:
        log.error(f'Unknown Error!! {val_err}')
        raise val_err
    try:  # Server and port are port are in ssh_url_to_repo string
        any_ssh_url: re.Match = server_port_pattern.search(repository_lst[-1].url)
        server, port = any_ssh_url.group('server', 'port')
    except AttributeError:
        err_msg: str = f'Error occurred grouping either server or port! Str-Dump: {linesep}{repository_lst[-1].url}{linesep}'
        log.error(err_msg)
        raise RegexException(-10, err_msg)
    except IndexError:
        exc_msg: str = f'Exception occurred! Provided string did not match any internal search pattern' \
                       f'Please provided string containing ID/Name/URL..'
        log.exception(exc_msg)
        exit(0)
    return server, port, repository_lst


def convert_date(time_in_days: int = None) -> datetime:
    """
    Used to determine which time to use for data retrieval.

    If a time is provided, today - days(time) provided is the new timeframe
    Else The time-frame is set from the first of the month till today

    :param time_in_days: The time in days subtracted from today (timeframe)
    :type time_in_days: int
    :return: The actual date used for further references
    :rtype: date
    """
    today: str = date.today().strftime("%Y-%m-%d")
    if time_in_days:
        date_y_m_d: datetime = datetime.strptime(f"{today}", "%Y-%m-%d") - timedelta(days=time_in_days)
    else:
        date_y_m_d: datetime = datetime.strptime(f"{today[:-2]}01", "%Y-%m-%d")
    return date_y_m_d


def get_gitlab_info(url: str, private_token: str, time_in_days: int = None) -> tuple[str, str, list[Repository]]:
    """
    Get repository urls edited in either provided time-frame or by default from
    the last month

    :param url: The gitlab server url
    :type url: str
    :param private_token: Private access token, can be acquired on gitlab web interface
    :type private_token: str
    :param time_in_days: Time in days to look back (default is a time-frame from the first of the month until today)
    :type time_in_days: int
    :return: None
    :rtype: None
    """
    return __get_project_info(__get_project_list(url, private_token), convert_date(time_in_days))


if __name__ == '__main__':
    get_gitlab_info(url=url_hsa, private_token=p_token_hsa, time_in_days=100)
