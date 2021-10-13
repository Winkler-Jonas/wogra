import gitlab
import regex as re
from typing import List, Dict, Callable
from datetime import date, datetime, timedelta
from collections import defaultdict

repo_pattern = re.compile("""
    \'id                            # Each object starts with id
    (?P<spacing>\':\s)              # Helper group for repeacing pattern
    (?P<id>                         # Group ID 
        \d+?)                       # Consists of multiple digits
    ,.*?                            # Seperator
    name(?P=spacing)\'              # Static string with repeating spacing group
    (?P<name>                       # Start name group
        [\w-]+?)                    # Match words/digits/dashes and underscores
    \'.*?                           # Seperator
    ssh_url_to_repo(?P=spacing)\'   # Static string with repeating spacing group
    (?P<url>                        # Start url group
        .*?)\'                      # Match anything till appearance of Apostrophe
    .*?                             # Seperator
    last_activity_at(?P=spacing)\'  # Static string with repeating spacing group
    (?P<date>                       # Start group date
        \d{4}-                      # Match 4 digits followed by a dash
        \d{2}-                      # Match 2 digits followed by a dash
        \d{2})                      # Match 2 digits followed by a dash
    """, flags=re.M | re.S | re.VERBOSE)

# r"\'id(?P<spacing>\':\s)(?P<id>\d+?),.*?name(?P=spacing)\'(?P<name>[\w-]+?)\'.*?ssh_url_to_repo(?P=spacing)\'(
# ?P<url>.*?)\'.*?last_activity_at(?P=spacing)\'(?P<date>\d{4}-\d{2}-\d{2})", flags=re.M | re.S)
# repo_user = re.compile(r'\'owner\'.+?\'username\':\s\'(?P<usr>[a-z]+)', flags=re.M | re.S)
port_pattern = re.compile(r'(?=:(?P<port>\d+))', flags=re.M | re.S)

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
            # well an error should probably be risen..
            pass


def __get_project_info(proj_list: list, date_y_m_d: date) -> defaultdict[str, Dict[str, str]]:
    """
    Convert list of projects into strings and filter out
    projects that were not edited in the provided timeframe

    :param date_y_m_d: The date the repository had to be edited the very least
    :type date_y_m_d: date
    :param proj_list: The list of projects on the server
    :type proj_list: list
    :return: List of remaining urls / repositories
    :rtype: List[str]
    """
    repository_dict: [Callable, dict] = defaultdict(lambda: dict())
    for proj in proj_list:
        if (proj_string := str(proj)) and \
                (repo_info := repo_pattern.search(proj_string)) and \
                (repo_last_edit := repo_info.group('date')) and \
                datetime.strptime(f"{repo_last_edit}", "%Y-%m-%d") > date_y_m_d:

            repo_id, repo_name, repo_url = repo_info.group('id', 'name', 'url')
            repo_port: str = '' if not (tmp := port_pattern.search(repo_url)) else tmp.group('port')
            repository_dict[repo_id] = {'repo_name': repo_name,
                                        'repo_url': repo_url,
                                        'repo_last_edit': repo_last_edit,
                                        'repo_port': repo_port}
        else:
            # should raise exception due to empty repository
            pass
    return repository_dict


def __convert_date(time_in_days: int = None) -> date:
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
        date_y_m_d: date = datetime.strptime(f"{today}", "%Y-%m-%d") - timedelta(days=time_in_days)
    else:
        date_y_m_d: date = datetime.strptime(f"{today[:-2]}01", "%Y-%m-%d")
    return date_y_m_d


def get_gitlab_info(url: str, private_token: str, time_in_days: int = None):
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
    repo_server = __get_project_info(__get_project_list(url, private_token), __convert_date(time_in_days))
    for key, value in repo_server.items():
        print(key, value)


if __name__ == '__main__':
    get_gitlab_info(url=url_hsa, private_token=p_token_hsa, time_in_days=100)
