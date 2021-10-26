from dataclasses import dataclass, field

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
