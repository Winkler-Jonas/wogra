import re
import gitlab
from typing import List
from datetime import date, datetime, timedelta


combined = re.compile(r"\'http_url_to_repo\':\s\'(?P<url>.+?)\'.*?\'last_activity_at\':\s\'(?P<date>\d{4}-\d{2}-\d{2}).*?\'", flags=re.M | re.S)

url_wogra: str = 'https://gitlab.wogra.com'
p_token_wogra: str = 'Lq1z1hMxG_yKeyTLaAXD'


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


def __get_project_info(proj_list: list, date_y_m_d: date) -> List[str]:
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
    repository_list = []
    for proj in proj_list:
        if (proj_string := str(proj)) and (it := combined.finditer(proj_string)):
            for match in it:
                if datetime.strptime(f"{match.group('date')}", "%Y-%m-%d") > date_y_m_d:
                    repository_list += [fr"URL: {match.group('url')}, Edited: {match.group('date')}"]
        else:
            # should raise exception due to empty repository
            pass
    return repository_list


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
    for repo in repo_server:
        print(repo)


if __name__ == '__main__':
    get_gitlab_info(url=url_wogra, private_token=p_token_wogra, time_in_days=100)



