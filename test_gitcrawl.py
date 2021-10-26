# Imports
from global_logger import setup_logger
logger = setup_logger('root')
from gitlap import Gitlab

url_hsa: str = 'https://r-n-d.informatik.hs-augsburg.de:8080/'
p_token_hsa: str = 'bsrg4w9xqTxotvxzVLGD'
url_wogra: str = 'https://gitlab.wogra.com'
p_token_wogra: str = 'Lq1z1hMxG_yKeyTLaAXD'


if __name__ == '__main__':
    try:
        gitlab_obj: Gitlab = Gitlab(url_wogra, p_token_wogra)

        for repo in gitlab_obj.get_repositories('Renovate Bot', 10):
            print(repo)
            for key, value in repo.get_pretty_commits():
                print(f'{key}: Sicherheitsupdate wurde installiert. Neue version {value}')
    except:
        raise




    # time_frame: int = 3
    # user: str = 'Hubert Hoegl'
    # # Datatype Tuple[server, port, RepositoryObjects]
    # gitlab_info: tuple[str, str, list[Repository]] = crawl_gitlab(url=url_hsa,
    #                                                               private_token=p_token_hsa,
    #                                                               time_in_days=time_frame)
