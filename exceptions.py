# ------------------------------------------------------------------------------------------
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


# ------------------------------------------------------------------------------------------


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


# ------------------------------------------------------------------------------------------
